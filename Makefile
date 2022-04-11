CFLAGS= -g3 -c -Wall -lpthread
CC= gcc
SRC = src
LIB = lib
OBJ = obj
BIN = bin
OBJECTS_GRADIENT=$(OBJ)/utils.o
HEADERS_GRADIENT= $(SRC)/utils.h 
OBJECTS_SMO=$(OBJ)/sample.o $(OBJ)/svm.o
HEADERS_SMO= $(SRC)/sample.h $(SRC)/svm.h 

all : gradient_descent smo

$(OBJ)/utils.o: $(SRC)/utils.c $(HEADERS)
	$(CC) $(CFLAGS) $(SRC)/utils.c  -o $(OBJ)/utils.o -lm

$(OBJ)/gradient_descent.o: $(SRC)/gradient_descent.c $(HEADERS_GRADIENT)
	$(CC) $(CFLAGS) $(SRC)/gradient_descent.c  -o $(OBJ)/gradient_descent.o -lm

$(OBJ)/svm.o: $(SRC)/svm.c $(HEADERS_SMO)
	$(CC) $(CFLAGS) $(SRC)/svm.c -o $(OBJ)/svm.o -lm

$(OBJ)/sample.o: $(SRC)/sample.c $(HEADERS_SMO)
	$(CC) $(CFLAGS) $(SRC)/sample.c -o $(OBJ)/sample.o -lm

$(OBJ)/smo.o: $(SRC)/smo.c $(HEADERS_SMO)
	$(CC) $(CFLAGS) $(SRC)/smo.c -o $(OBJ)/smo.o -lm

gradient_descent: ${OBJECTS_GRADIENT} $(OBJ)/gradient_descent.o $(HEADERS)
	$(CC) -o $(BIN)/gradient_descent.exe ${OBJECTS_GRADIENT} $(OBJ)/gradient_descent.o -lm -lpthread

smo: ${OBJECTS_SMO} $(OBJ)/smo.o $(HEADERS_SMO)
	$(CC) -o $(BIN)/smo.exe ${OBJECTS_SMO} $(OBJ)/smo.o -lm -lpthread

clean: 
	rm -rf $(OBJ)/*.o
	rm -rf $(BIN)/*.exe

empty: 
	del /F /Q $(OBJ)\*.o
	del /F /Q $(BIN)\*.exe

run-smo: smo
	./$(BIN)/smo.exe

run-gradient_descent: gradient_descent
	./$(BIN)/gradient_descent.exe $(size) $(seq) $(thread) $(plot) 
