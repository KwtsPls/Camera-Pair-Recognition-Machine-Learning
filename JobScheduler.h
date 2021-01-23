#ifndef JOBSCHEDULER_H
#define JOBSCHEDULER_H

#include <pthread.h>
#include "LogisticRegression.h"
#include "Queue.h"
#include "sparseVector.h"
#define MAX_THREADS 6
#define TRAINING_JOB 1
#define TESTING_JOB 2


typedef struct queue Queue;
typedef struct logisticreg logisticreg;

typedef struct thread_train_args{
    
    // logisticreg *model;
    pthread_mutex_t *locking_queue;
    pthread_cond_t *queue_condition;     //Condition for the queue
    pthread_cond_t *jobs_finished_cond;  
    pthread_cond_t *session_cond_t;
    int *flag_exit_threads_all;
    int *jobs_curr_running;
    int max_exec;
    Queue *job_queue;
    // double **X;
    // int *y;
    // int *size;
}thread_train_args;

//Generic type for thread function
typedef void (*job_fun)(void *args);

//Structure for jobs
typedef struct job{

    int job_tag;  //Job type 
    job_fun function; //the function that this job that will perform
    void *args;   //Arguments for the given function

}Job;


typedef struct train_job_args{
    logisticreg *model;
    double *new_weight;
    double *gradient;
    int *finished;
    int *curr_exec_threads;
    int max_exec_threads;
    int size;
    sparseVector **X;
    int *y;
    int *flag_condition_forNextStep;
    int *flag_condition_exit_all;
    int *cnt;

    pthread_mutex_t *locking_queue;
    pthread_mutex_t *thread_locking;
    pthread_cond_t *threads_finished_cond;
}train_job_args;



typedef struct job_scheduler
{   
    
    pthread_t *threads;     //Array of threads;
    int execution_threads; //number of execution threads
    int flag_exit_threads_all;  //Flag for knowing if threads must exit


    int jobs_curr_running;          //Counter for jobs_curr_running
    thread_train_args *t_args;

    Queue *Job;           //a queue that holds the submitted jobs/tasks
    pthread_mutex_t locking_queue;      //Mutex for locking the queue
    pthread_mutex_t thread_locking;
    pthread_cond_t queue_condition;     //Condition for the queue


    pthread_cond_t session_finsihed_cond;
    pthread_cond_t jobs_finished_cond;  
    int jobs_running;               //Protected by queue lock

    /*
    int numOfN;
    double *new_weight;
    double *gradient;
    int *finished;
    */

}JobScheduler;


JobScheduler *initialize_scheduler(int execution_threads);
void schedule(JobScheduler *scheduler,Job *job_s);
void waitUntilJobsHaveFinished(JobScheduler *scheduler);
void threads_must_exit(JobScheduler *scheduler);
void destroy_JobScheduler(JobScheduler **scheduler);



train_job_args *create_job_args(logisticreg *model, double *new_weight, double *gradient,int *finished,int *curr_running_threads,sparseVector **X,int *y,int size,pthread_mutex_t *locking_que,int *flag_forNextStep, pthread_cond_t *condition, pthread_mutex_t *thread_locking,int max_ex_threads, int *flag_exit_all, int *cnt);
Job *create_job(int jobTag,job_fun fun,void *args);
void Job_deleter(Job **jobb);
void Job_args_deleter(int job_tag,void **args);

//Function for threads
void *thread_fun(void *args);

#endif // !JOBSCHEDULER_H
