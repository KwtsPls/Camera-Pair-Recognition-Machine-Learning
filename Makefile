#Make part for the main function of the program
main: Queue.o  BinaryHeap.o Inference.o main.o JsonParser.o ErrorHandler.o Dictionary.o Bucket.o HashTable.o CsvReader.o BagOfWords.o  SecTable.o DataPreprocess.o LogisticRegression.o Metrics.o DataLoading.o Transitivity.o 
	gcc -g3 main.o JsonParser.o ErrorHandler.o Dictionary.o  Bucket.o HashTable.o CsvReader.o BagOfWords.o  SecTable.o DataPreprocess.o LogisticRegression.o Metrics.o DataLoading.o  Transitivity.o Queue.o BinaryHeap.o -o main -lm
	gcc -g3 Inference.o JsonParser.o ErrorHandler.o Dictionary.o Bucket.o HashTable.o CsvReader.o BagOfWords.o  SecTable.o DataPreprocess.o LogisticRegression.o Metrics.o DataLoading.o  Transitivity.o BinaryHeap.o Queue.o -o inference -lm


#Make part for the test part of the program using acutest library
test: test_check.o test_machine_learning.o JsonParser.o ErrorHandler.o Dictionary.o Bucket.o HashTable.o CsvReader.o BagOfWords.o  SecTable.o DataPreprocess.o LogisticRegression.o Metrics.o DataLoading.o Transitivity.o BinaryHeap.o Queue.o  
	gcc -g3 -lm test_check.o JsonParser.o ErrorHandler.o Dictionary.o Bucket.o HashTable.o BagOfWords.o  CsvReader.o SecTable.o DataPreprocess.o LogisticRegression.o Metrics.o DataLoading.o BinaryHeap.o Queue.o Transitivity.o  -o test -lm
	gcc -g3 -lm test_machine_learning.o JsonParser.o ErrorHandler.o Dictionary.o Bucket.o HashTable.o BagOfWords.o  CsvReader.o SecTable.o DataPreprocess.o LogisticRegression.o Metrics.o DataLoading.o  BinaryHeap.o Queue.o Transitivity.o  -o learning_test -lm

test_check.o: test_check.c HashTable.h ErrorHandler.h Test_Units/acutest-master/include/acutest.h Test_Units/Helper_Files/test_names.h
	gcc -g3 -ITest_Units/acutest-master/include/ -ITest_Units/Helper_Files/ -c -Wall test_check.c

test_machine_learning.o: test_machine_learning.c HashTable.h ErrorHandler.h Test_Units/acutest-master/include/acutest.h Test_Units/Helper_Files/test_names.h
	gcc -g3 -ITest_Units/acutest-master/include/ -ITest_Units/Helper_Files/ -c -Wall test_machine_learning.c

main.o : main.c JsonParser.h ErrorHandler.h
	gcc -g3 -c -Wall main.c

SecTable.o: SecTable.c SecTable.h
	gcc -g3 -c -Wall SecTable.c

JsonParser.o: JsonParser.c JsonParser.h ErrorHandler.h Dictionary.h
	gcc -g3 -c -Wall JsonParser.c

ErrorHandler.o: ErrorHandler.c ErrorHandler.h
	gcc -g3 -c -Wall ErrorHandler.c

Dictionary.o: Dictionary.c Dictionary.h
	gcc -g3 -c -Wall  Dictionary.c

Bucket.o : Bucket.c Bucket.h Dictionary.h
	gcc -g3 -c -Wall Bucket.c

CsvReader.o : CsvReader.c CsvReader.h
	gcc -g3 -c -Wall  CsvReader.c

HashTable.o : HashTable.c HashTable.h SecTable.h
	gcc -g3 -c -Wall HashTable.c

DataPreprocess.o : DataPreprocess.c DataPreprocess.h
	gcc -g3 -c -Wall DataPreprocess.c

BagOfWords.o : BagOfWords.c BagOfWords.h
	gcc -g3 -c -Wall BagOfWords.c

LogisticRegression.o : LogisticRegression.c LogisticRegression.h
	gcc -g3 -c -Wall LogisticRegression.c

Metrics.o : Metrics.c Metrics.h
	gcc -g3 -c -Wall Metrics.c

DataLoading.o : DataLoading.c DataLoading.h
	gcc -g3 -c -Wall DataLoading.c

Inference.o : Inference.c
	gcc -g3 -c -Wall Inference.c

Transitivity.o : Transitivity.c Transitivity.h
	gcc -g3 -c -Wall Transitivity.c

Queue.o : Queue.c Queue.h BinaryHeap.h
	gcc -g3 -c -Wall Queue.c

BinaryHeap.o : BinaryHeap.c BinaryHeap.h Transitivity.h
	gcc -g3 -c -Wall BinaryHeap.c


clean:
	rm main test main.o JsonParser.o ErrorHandler.o CsvReader.o Bucket.o HashTable.o Dictionary.o test_check.o SecTable.o DataPreprocess.o BagOfWords.o cliques.csv LogisticRegression.o Metrics.o test_machine_learning.o Inference.o DataLoading.o Transitivity.o  neg_cliques.csv predictions.csv inference learning_test