all:
	gcc -g -o bmi160 bmi160.c sensors.c spi2.c debug.c delay.c

clean:
	rm -rf *.o bmi160
