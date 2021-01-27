#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "HashTable.h"
#include "CsvReader.h"
#include "DataPreprocess.h"
#include "JsonParser.h"
#include "BagOfWords.h"
#include "LogisticRegression.h"
#include "test_names.h"
#include "DataLoading.h"
#include "acutest.h"

typedef struct test_args{
        int *i;
        pthread_mutex_t *locker;
}test_args;

typedef struct test_args2{
        int *i;
        char *r;
        pthread_mutex_t *locker;
        pthread_cond_t *condition;
}test_args2;

test_args *create_args(int *i, pthread_mutex_t *locker){
        test_args *args = malloc(sizeof(test_args));
        args->i = i;
        args->locker = locker;
        return args;
}


test_args2 *create_args2(int *i, char *str, pthread_mutex_t *locker, pthread_cond_t *condc){
        test_args2 *args = malloc(sizeof(test_args2));
        args->i = i;
        args->locker = locker;
        args->r = str;
        args->condition = condc;
        return args;
}

//Function to check the creation of a job scheduler
void kostas_plas_plas(void *args){
        test_args *arg = (test_args *) args;
        int *i = arg->i;
        pthread_mutex_t *locker = arg->locker;
        pthread_mutex_lock(locker);
        (*i) = (*i) + 1;
        pthread_mutex_unlock(locker);
}

void kostas_plas_plasv2(void *args){
        test_args2 *arg = (test_args2 *) args;
        int *flag = arg->i;
        pthread_mutex_t *locker = arg->locker;
        pthread_mutex_lock(locker);
        (*flag)++;
        pthread_mutex_unlock(locker);

}

void read_fun(void *args){
        test_args2 *arg = (test_args2 *) args;
        int *flag = arg->i;
        char *str = arg->r;
        pthread_cond_t *condition = arg->condition;
        pthread_mutex_t *locker = arg->locker;

        pthread_mutex_lock(locker);
        
        if((*str)=='1'){
                pthread_cond_wait(condition,locker);
        }
        
        if((*str) == '2'){
                (*flag) += 1;
        }
        
                
        pthread_mutex_unlock(locker);

}


void write_fun(void *args){
        test_args2 *arg = (test_args2 *) args;
        int *flag = arg->i;
        char *str = arg->r;
        pthread_cond_t *condition = arg->condition;
        pthread_mutex_t *locker = arg->locker;

        pthread_mutex_lock(locker);
        if((*str) != '1')
                (*flag) = -10;
        else{
                (*str) = '2';
                (*flag)++;
        }
                
        pthread_cond_signal(condition);
        pthread_mutex_unlock(locker);

}


void test_job_scheduler_create(){
        JobScheduler *scheduler = initialize_scheduler(MAX_THREADS);
        int i = 0;
        for(int j=0;j<MAX_THREADS*1000;j++){
                test_args *args = create_args(&i,&(scheduler->locking_queue));
                Job *job = create_job(-12,kostas_plas_plas,args);
                schedule(scheduler,job);
        }
        TEST_ASSERT(scheduler != NULL);
        waitUntilJobsHaveFinished(scheduler);        
        threads_must_exit(scheduler);
        destroy_JobScheduler(&scheduler);
        TEST_ASSERT(i==MAX_THREADS*1000);
}

void test_threads(){
        JobScheduler *sch = initialize_scheduler(MAX_THREADS);
        char str = '1';
        int i = 0;
        printf("\n%d\n",i);
        // int string_len = i;
        for(int j=0;j<MAX_THREADS;j++){
                
                test_args2 *args = create_args2(&i,&str,&(sch->locking_queue),&(sch->session_finsihed_cond)); 
                if(j==0){
                        Job *job_s = create_job(-12,read_fun,args);
                        schedule(sch,job_s);
                }
                else if(j==1){
                        Job *job_s2 = create_job(-12,write_fun,args);
                        schedule(sch,job_s2);
                }
                else{
                        Job *job_s3 = create_job(-12,kostas_plas_plasv2,args);
                        schedule(sch,job_s3);
                }
        }
        waitUntilJobsHaveFinished(sch);
        threads_must_exit(sch);
        destroy_JobScheduler(&sch);
        TEST_ASSERT(i == MAX_THREADS);
        
}


TEST_LIST = {
        { "job_scheduler_create", test_job_scheduler_create},
        { "try_different_jobs", test_threads },
        { NULL, NULL }
};

