main: main.o JsonParser.o ErrorHandler.o Dictionary.o Bucket.o HashTable.o 
	gcc -g3 main.o JsonParser.o ErrorHandler.o Dictionary.o  Bucket.o HashTable.o  -o main

main.o : main.c JsonParser.h ErrorHandler.h
	gcc -g3 -c main.c

JsonParser.o: JsonParser.c JsonParser.h ErrorHandler.h Dictionary.h
	gcc -g3 -c JsonParser.c

ErrorHandler.o: ErrorHandler.c ErrorHandler.h
	gcc -g3 -c ErrorHandler.c

Dictionary.o: Dictionary.c Dictionary.h
	gcc -g3 -c Dictionary.c

Bucket.o : Bucket.c Bucket.h Dictionary.h
	gcc -g3 -c Bucket.c

CsvReader.o : CsvReader.c CsvReader.h
	gcc -g3 -c CsvReader.c

HashTable.o : HashTable.c HashTable.h
	gcc -g3 -c HashTable.c

clean:
	rm main main.o JsonParser.o ErrorHandler.o CsvReader.o Bucket.o