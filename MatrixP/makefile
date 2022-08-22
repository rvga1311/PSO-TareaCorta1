OBJS	= MatrixP.o
SOURCE	= MatrixP.c
HEADER	= 
OUT	= MatrixP
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lpthread

default: cleanAll all clean run 
2: all clean

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

MatrixP.o: MatrixP.c
	$(CC) $(FLAGS) MatrixP.c 

clean:
	clear
	@echo "\nMake: Cleaning up objects files..."
	rm -f $(OBJS) *.out

cleanAll:
	clear
	@echo "\nMake: Cleaning up objects files, executable and text files..."
	rm -f $(OBJS) *.out Matrices/*.txt *.txt $(OUT)

run: $(OUT)
	clear
	@echo "\nMake: Running $(OUT)...\n"
	./$(OUT)