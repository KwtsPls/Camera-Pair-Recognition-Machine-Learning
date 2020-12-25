#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Metrics.h"


//Function to initialize the metrics for a classification problem
LearningMetrics *init_LearningMetrics(char *pos_tag,char *neg_tag){
    LearningMetrics *metrics = malloc(sizeof(LearningMetrics));
    metrics->precision_pos_class=0.0;
    metrics->recall_pos_class=0.0;
    metrics->f1_pos_class=0.0;
    metrics->true_positive_pos_class=0;
    metrics->false_positive_pos_class=0;
    metrics->false_negative_pos_class=0;

    metrics->precision_neg_class=0.0;
    metrics->recall_neg_class=0.0;
    metrics->f1_neg_class=0.0;
    metrics->true_positive_neg_class=0;
    metrics->false_positive_neg_class=0;
    metrics->false_negative_neg_class=0;

    metrics->pos_tag = malloc(strlen(pos_tag)+1);
    strcpy(metrics->pos_tag,pos_tag);

    metrics->neg_tag = malloc(strlen(neg_tag)+1);
    strcpy(metrics->neg_tag,neg_tag);

    return metrics;
}

//Function to update the counters of true positives, false positives and false negatives
LearningMetrics *update_LearningMetrics(LearningMetrics *metrics,int yi_pred,int y){

    if(y==1 && yi_pred==1)
        metrics->true_positive_pos_class++;
    if(y==0 && yi_pred==1)
        metrics->false_positive_pos_class++;
    if(y==1 && yi_pred==0)
        metrics->false_negative_pos_class++;

    if(y==0 && yi_pred==0)
        metrics->true_positive_neg_class++;
    if(y==1 && yi_pred==0)
        metrics->false_positive_neg_class++;
    if(y==0 && yi_pred==1)
        metrics->false_negative_neg_class++;

    return metrics;
}

//Function to evaluate precision,recall and f1 after the training for each class
LearningMetrics *evaluate_LearningMetrics(LearningMetrics *metrics){
    metrics->precision_pos_class = (double)metrics->true_positive_pos_class/((double)metrics->true_positive_pos_class + (double)metrics->false_positive_pos_class);
    metrics->recall_pos_class = (double)metrics->true_positive_pos_class/((double)metrics->true_positive_pos_class + (double)metrics->false_negative_pos_class);
    metrics->f1_pos_class = 2*(metrics->precision_pos_class*metrics->recall_pos_class /(metrics->precision_pos_class+metrics->recall_pos_class ));

    metrics->precision_neg_class = (double)metrics->true_positive_neg_class/((double)metrics->true_positive_neg_class + (double)metrics->false_positive_neg_class);
    metrics->recall_neg_class = (double)metrics->true_positive_neg_class/((double)metrics->true_positive_neg_class + (double)metrics->false_negative_neg_class);
    metrics->f1_neg_class = 2*(metrics->precision_neg_class*metrics->recall_neg_class /(metrics->precision_neg_class+metrics->recall_neg_class ));

    return metrics;
}

//Function to print the evaluations
void print_LearningMetrics(LearningMetrics *metrics){
    printf("\n\nFor %s class:\n\nPrecision: %f\n\nRecall: %f\n\nf1 score: %f\n\n",metrics->pos_tag,metrics->precision_pos_class,metrics->recall_pos_class,metrics->f1_pos_class);
    printf("\n\nFor %s class:\n\nPrecision: %f\n\nRecall: %f\n\nf1 score: %f\n\n",metrics->neg_tag,metrics->precision_neg_class,metrics->recall_neg_class,metrics->f1_neg_class);
}

//Function to free all memory allocated for metrics
void destroyLearningMetrics(LearningMetrics **metrics){
    free((*metrics)->pos_tag);
    free((*metrics)->neg_tag);
    free(*metrics);
}

//Function to count the metric form the given predictions
LearningMetrics *calculate_LearningMetrics(LearningMetrics *metrics,int *y,double *pred,int size,int n){
    for(int i=0;i<(n-size);i++){
        int yi_pred = round(pred[i]);
        metrics = update_LearningMetrics(metrics,yi_pred,y[size+i]);
    }
    return metrics;
}