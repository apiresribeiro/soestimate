# Grupo 1
# Alexandre Ribeiro - 63723
# André Vilarinho - 63727
# Margarida Torres - 63751
CC = gcc
CFLAGS = -Wall -g -Iinc
OBJS= ${OBJS_DIR}controller.o ${OBJS_DIR}memory.o ${OBJS_DIR}sensor.o ${OBJS_DIR}server.o ${OBJS_DIR}random_measurement.o ${OBJS_DIR}process.o
OBJS_DIR = obj/
src = src/
obj = -o obj/
bin = -o bin/
RUN_ARGS ?= 1 1 2

.PHONY: run clean valgrind

run: SOestimate
	./bin/SOestimate $(RUN_ARGS)

SOestimate: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} ${src}main.c ${bin}SOestimate -lm


controller.o:
	${CC} ${CFLAGS} ${src}controller.c ${obj}controller.o -lm


memory.o:
	${CC} ${CFLAGS} ${src}memory.c ${obj}memory.o -lm


sensor.o:
	${CC} ${CFLAGS} ${src}sensor.c ${obj}sensor.o -lm


server.o:
	${CC} ${CFLAGS} ${src}server.c ${obj}server.o -lm

process.o:
	${CC} ${CFLAGS} ${src}process.c ${obj}process.o -lm

random_measurement.o:
	${CC} ${CFLAGS} ${src}random_measurement.c ${obj}random_measurement.o -lm

${OBJS_DIR}%.o: ${src}%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f ${OBJS_DIR}*.o bin/SOestimate

valgrind:
	valgrind --leak-check=full --track-origins=yes ./bin/SOestimate $(RUN_ARGS)
