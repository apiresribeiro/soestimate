CC = gcc
CFLAGS = -g -c -Wall
OBJS= controller.o memory.o sensor.o server.o
src = src/

run: main
	./main.o

main: ${OBJS}
	${CC} ${CFLAGS} ${src} main.c -o main.o


controller.o:
	${CC} ${CFLAGS} ${src} controller.c -o controller.o 


memory.o:
	${CC} ${CFLAGS} ${src} memory.c -o memory.o


sensor.o:
	${CC} ${CFLAGS} ${src} sensor.c -o sensor.o


server.o:
	${CC} ${CFLAGS} ${src} server.c -o server.o

clean:
	rm -f obj\*.o main