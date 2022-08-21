OBJS	= MatrixT.o
SOURCE	= MatrixT.c
HEADER	= 
OUT	= MatrixT
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lpthread

default: cleanAll all clean run 
2: all clean

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

MatrixT.o: MatrixT.c
	$(CC) $(FLAGS) MatrixT.c 

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