main: main.o JsonParser.o ErrorHandler.o Dictionary.o
	gcc -g3 main.o JsonParser.o ErrorHandler.o Dictionary.o -o main

main.o : main.c JsonParser.h ErrorHandler.h
	gcc -g3 -c main.c

JsonParser.o: JsonParser.c JsonParser.h ErrorHandler.h Dictionary.h
	gcc -g3 -c JsonParser.c

ErrorHandler.o: ErrorHandler.c ErrorHandler.h
	gcc -g3 -c ErrorHandler.c

Dictionary.o: Dictionary.c Dictionary.h
	gcc -g3 -c Dictionary.c

clean:
	rm main main.o JsonParser.o ErrorHandler.o