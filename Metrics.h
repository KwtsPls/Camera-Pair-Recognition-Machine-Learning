#ifndef PROJECT_METRICS_H
#define PROJECT_METRICS_H

//Metrics for binary classification
typedef struct metrics{
    //Metrics for positive class
    char *pos_tag;
    double precision_pos_class;
    double recall_pos_class;
    double f1_pos_class;
    int true_positive_pos_class;
    int false_positive_pos_class;
    int false_negative_pos_class;

    //Metric for negative class
    char *neg_tag;
    double precision_neg_class;
    double recall_neg_class;
    double f1_neg_class;
    int true_positive_neg_class;
    int false_positive_neg_class;
    int false_negative_neg_class;

}LearningMetrics;

//Function to initialize the metrics for a classification problem
LearningMetrics *init_LearningMetrics(char *pos_tag,char *neg_tag);

//Function to update the counters of true positives, false positives and false negatives
LearningMetrics *update_LearningMetrics(LearningMetrics *metrics,int yi_pred,int y);

//Function to evaluate precision,recall and f1 after the training for each class
LearningMetrics *evaluate_LearningMetrics(LearningMetrics *metrics);

//Function to count the metric form the given predictions
LearningMetrics *calculate_LearningMetrics(LearningMetrics *metrics,int *y,double *pred,int n);

//Function to print the evaluations
void print_LearningMetrics(LearningMetrics *metrics);

//Function to free all memory allocated for metrics
void destroyLearningMetrics(LearningMetrics **metrics);

#endif //PROJECT_METRICS_H
