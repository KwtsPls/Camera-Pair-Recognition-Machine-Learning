#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include "Queue.h"
#include "JobScheduler.h"
#include "LogisticRegression.h"



//Function for initialising scheduler
JobScheduler *initialize_scheduler(int execution_threads){
    //Initialize scheduler
    JobScheduler *scheduler = malloc(sizeof(JobScheduler));

    scheduler->jobs_running=0;
    scheduler->execution_threads =execution_threads; 
    scheduler->flag_exit_threads_all = 0;
    scheduler->jobs_curr_running = 0;


    int er = pthread_mutex_init(&(scheduler->locking_queue), NULL);
    if(er!=0)
        perror("ERROR: Mutex initialising");
    
    er = pthread_mutex_init(&(scheduler->thread_locking), NULL);
    if(er!=0)
        perror("ERROR: Mutex initialising");

    er = pthread_cond_init(&(scheduler->queue_condition), NULL);
    if(er!=0)
        perror("ERROR: Condition initialising");

    er = pthread_cond_init(&(scheduler->jobs_finished_cond), NULL);
    if(er!=0)
        perror("ERROR: Condition initialising");

    er = pthread_cond_init(&(scheduler->session_finsihed_cond),NULL);
    if(er!=0)
        perror("ERROR: Condition initialising");

    //Initialize thread_array
    scheduler->threads = malloc(sizeof(pthread_t)*execution_threads);
    scheduler->Job = initQueue();
    //Initialize thread_arguments
    scheduler->t_args = malloc(sizeof(thread_train_args));
    scheduler->t_args->job_queue = (scheduler->Job);
    scheduler->t_args->locking_queue = &(scheduler->locking_queue);
    scheduler->t_args->queue_condition = &(scheduler->queue_condition);
    scheduler->t_args->jobs_finished_cond = &(scheduler->jobs_finished_cond);
    scheduler->t_args->session_cond_t = &(scheduler->session_finsihed_cond);
    scheduler->t_args->flag_exit_threads_all = &(scheduler->flag_exit_threads_all);
    scheduler->t_args->jobs_curr_running = &(scheduler->jobs_running);
    scheduler->t_args->max_exec = scheduler->execution_threads;




    for(int i=0;i<execution_threads;i++){
        scheduler->threads[i] = i;
        pthread_create(&(scheduler->threads[i]),NULL,thread_fun, (void*)  scheduler->t_args);
    }


    return scheduler;
}


void schedule(JobScheduler *scheduler,Job *job_s){
    pthread_mutex_lock(&(scheduler->locking_queue));
    pushQueue(scheduler->Job,job_s);
    pthread_cond_signal(&(scheduler->queue_condition));
    pthread_mutex_unlock(&(scheduler->locking_queue));
}


void *thread_fun(void *args) {

    //Initialising thread arguments
    thread_train_args *argptr = ((thread_train_args *) args);
    Queue *job_queue = argptr->job_queue;
    pthread_mutex_t *queue_lock = argptr->locking_queue;
    pthread_cond_t *queue_cond = argptr->queue_condition;
    pthread_cond_t *jobs_finished_cond = argptr->jobs_finished_cond;
    int *flag_exit_threads_all = argptr->flag_exit_threads_all;
    int *jobs_running_ptr = argptr->jobs_curr_running;
    
    

    //Loop until all threads must exit
    while ((*flag_exit_threads_all)==0) {
        
        Job *jobb;

        pthread_mutex_lock(queue_lock);
       
        while (emptyQueue(job_queue) == 1 && (*flag_exit_threads_all) == 0){
            pthread_cond_wait(queue_cond, queue_lock);
        }
        if ((*flag_exit_threads_all==1)){
            pthread_mutex_unlock(queue_lock);
            break;
        }
        jobb = popQueue(&(job_queue->head));       // get pointer to next job and remove it from queue
        //printf("New Job Running %p\n",jobb);
        (*jobs_running_ptr)++;
        pthread_mutex_unlock(queue_lock);


        jobb->function((train_job_args*)jobb->args);        

        pthread_mutex_lock(queue_lock);
        Job_deleter(&jobb);                     // (!) scheduler deletes scheduled objects when done but they must be allocated before they get scheduled
        (*jobs_running_ptr)--;
        if ( (*jobs_running_ptr) == 0 ){
            pthread_cond_signal(jobs_finished_cond);
        }
        pthread_mutex_unlock(queue_lock);
    }
    pthread_exit(NULL);
}

Job *create_job(int jobTag,job_fun fun,void *args){
    Job *new_job = malloc(sizeof(Job));
    new_job->job_tag = jobTag;
    new_job->function = fun;
    new_job->args = args;
    return new_job;
}   

train_job_args *create_job_args(logisticreg *model, double *new_weight, double *gradient,int *finished,int *curr_running_threads,sparseVector **X,int *y,int size,pthread_mutex_t *locking_queue,int *flag_forNextStep, pthread_cond_t *condition, pthread_mutex_t *thread_locking,int max_exec_threads, int *flag_exit_all,int *counter){
    train_job_args *arg = malloc(sizeof(train_job_args));
    arg->model = model;
    arg->new_weight = new_weight;
    arg->gradient = gradient;
    arg->finished = finished;
    arg->curr_exec_threads = curr_running_threads;
    arg->X = X;
    arg->y = y;
    arg->size = size;
    arg->locking_queue = locking_queue; 
    arg->flag_condition_forNextStep = flag_forNextStep;
    arg->threads_finished_cond = condition;
    arg->thread_locking = thread_locking;
    arg->max_exec_threads = max_exec_threads;
    arg->flag_condition_exit_all = flag_exit_all;
    arg->cnt = counter;
    return arg;

}

void Job_args_deleter(int job_tag,void **args){
    if(job_tag == TRAINING_JOB){
        train_job_args *arg = (train_job_args *)(*args);
        free(arg);
        *args = NULL;
    }

}


void Job_deleter(Job **jobb){
    Job_args_deleter((*jobb)->job_tag,&((*jobb)->args));
    free((*jobb));
    *jobb = NULL;
}


void waitUntilJobsHaveFinished(JobScheduler *scheduler){
    pthread_mutex_lock(&(scheduler->locking_queue));
    while(emptyQueue(scheduler->Job)!=1 || scheduler->jobs_curr_running>0)
    {
        pthread_cond_wait(&(scheduler->jobs_finished_cond),&(scheduler->locking_queue));
    }
}


void threads_must_exit(JobScheduler *scheduler){
    // waitUntilJobsHaveFinished(scheduler);
    // pthread_mutex_lock(&(scheduler->locking_queue));
    scheduler->flag_exit_threads_all = 1;
    pthread_cond_broadcast(&(scheduler->session_finsihed_cond));
    pthread_cond_broadcast(&(scheduler->queue_condition));
    pthread_mutex_unlock(&(scheduler->locking_queue));

    int *ret_val;
    for(int i=0;i<scheduler->execution_threads;i++){
        pthread_join(scheduler->threads[i],(void **) &(ret_val));
    }
}

void destroy_JobScheduler(JobScheduler **scheduler){

    free((*scheduler)->threads);
    pthread_cond_destroy(&((*scheduler)->queue_condition));
    pthread_cond_destroy(&((*scheduler)->session_finsihed_cond));
    pthread_cond_destroy(&((*scheduler)->jobs_finished_cond));
    pthread_mutex_destroy(&((*scheduler)->locking_queue));
    pthread_mutex_destroy(&((*scheduler)->thread_locking));

    free((*scheduler)->t_args);
    destroyQueue((*scheduler)->Job);
    free(*scheduler);

}