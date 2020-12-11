#Make part for the main function of the program
main: main.o JsonParser.o ErrorHandler.o Dictionary.o Bucket.o HashTable.o CsvReader.o SecTable.o DataPreprocess.o
	gcc -g3 main.o JsonParser.o ErrorHandler.o Dictionary.o  Bucket.o HashTable.o CsvReader.o SecTable.o DataPreprocess.o -o main

#Make part for the test part of the program using acutest library
test: test_check.o JsonParser.o ErrorHandler.o Dictionary.o Bucket.o HashTable.o CsvReader.o SecTable.o DataPreprocess.o
	gcc -g3 test_check.o JsonParser.o ErrorHandler.o Dictionary.o Bucket.o HashTable.o CsvReader.o SecTable.o DataPreprocess.o -o test

test_check.o: test_check.c HashTable.h ErrorHandler.h Test_Units/acutest-master/include/acutest.h Test_Units/Helper_Files/test_names.h
	gcc -g3 -ITest_Units/acutest-master/include/ -ITest_Units/Helper_Files/ -c -Wall test_check.c

main.o : main.c JsonParser.h ErrorHandler.h
	gcc -g3 -c -Wall main.c

SecTable.o: SecTable.c SecTable.h
	gcc -g3 -c -Wall SecTable.c

JsonParser.o: JsonParser.c JsonParser.h ErrorHandler.h Dictionary.h
	gcc -g3 -c -Wall JsonParser.c

ErrorHandler.o: ErrorHandler.c ErrorHandler.h
	gcc -g3 -c -Wall ErrorHandler.c

Dictionary.o: Dictionary.c Dictionary.h
	gcc -g3 -c -Wall Dictionary.c

Bucket.o : Bucket.c Bucket.h Dictionary.h
	gcc -g3 -c -Wall Bucket.c

CsvReader.o : CsvReader.c CsvReader.h
	gcc -g3 -c -Wall CsvReader.c

HashTable.o : HashTable.c HashTable.h SecTable.h
	gcc -g3 -c -Wall HashTable.c

DataPreprocess.o : DataPreprocess.c DataPreprocess.h
	gcc -g3 -c -Wall DataPreprocess.c

clean:
	rm main test main.o JsonParser.o ErrorHandler.o CsvReader.o Bucket.o HashTable.o Dictionary.o test_check.o SecTable.o DataPreprocess.o