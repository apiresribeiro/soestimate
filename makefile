CC = gcc
CFLAGS = -g -c -Wall
OBJS= controller.o memory.o sensor.o server.o
src = src/
obj = obj/

run: main
	./main.o

main: ${OBJS}
	${CC} ${CFLAGS} ${src} main.c -o ${obj}main.o


controller.o:
	${CC} ${CFLAGS} ${src} controller.c -o ${obj}controller.o 


memory.o:
	${CC} ${CFLAGS} ${src} memory.c -o ${obj}memory.o


sensor.o:
	${CC} ${CFLAGS} ${src} sensor.c -o ${obj}sensor.o


server.o:
	${CC} ${CFLAGS} ${src} server.c -o ${obj}server.o

clean:
	rm -f obj\*.o main