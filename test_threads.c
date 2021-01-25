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

//Function to check the creation of a job scheduler
void test_job_scheduler_create(){

}

TEST_LIST = {
        { "job_scheduler_create", test_job_scheduler_create},
        { NULL, NULL }
};

