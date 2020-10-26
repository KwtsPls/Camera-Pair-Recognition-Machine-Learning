main: main.o JsonParser.o ErrorHandler.o
	gcc -g3 main.o JsonParser.o ErrorHandler.o -o main

main.o : main.c JsonParser.h ErrorHandler.h
	gcc -g3 -c main.c

JsonParser.o: JsonParser.c JsonParser.h ErrorHandler.h
	gcc -g3 -c JsonParser.c

ErrorHandler.o: ErrorHandler.c ErrorHandler.h
		gcc -g3 -c ErrorHandler.c

clean:
	rm main main.o JsonParser.o ErrorHandler.o