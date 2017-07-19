all: bmi160 bmm150

bmi160: bmi160.c debug.c delay.c sensors.c spi2.c udplink.c
	gcc -g -o $@ $^

bmm150: bmm150.c debug.c delay.c thread.c i2c.c
	gcc -g -o $@ $^

clean:
	rm -rf *.o bmm150 bmi160
