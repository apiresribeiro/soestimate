CC = gcc
CFLAGS = -g -c
WALL = -Wall -o
OBJS= controller.o memory.o sensor.o server.o
src = src/
obj = obj/

run: main
	./main.o

main: ${OBJS}
	${CC} ${CFLAGS} ${src}main.c ${WALL} ${obj}main.o


controller.o:
	${CC} ${CFLAGS} ${src}controller.c ${WALL} ${obj}controller.o 


memory.o:
	${CC} ${CFLAGS} ${src}memory.c ${WALL} ${obj}memory.o


sensor.o:
	${CC} ${CFLAGS} ${src}sensor.c ${WALL} ${obj}sensor.o


server.o:
	${CC} ${CFLAGS} ${src}server.c ${WALL} ${obj}server.o

clean:
	rm -f obj\*.o main

valgrind:
	valgrind --leak-check=full --track-origins=yes ./main.o