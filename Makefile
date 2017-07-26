all: main

bmi160: bmi160.c debug.c delay.c sensors.c spi2.c udplink.c
	gcc -g -o $@ $^

bmm150: bmm150.c debug.c delay.c thread.c i2c.c
	gcc -g -o $@ $^

main: main.c bmi160.c bmm150.c delay.c sensors.c debug.c thread.c i2c.c spi2.c udplink.c
	gcc -DUSE_UDP -g -o $@ $^

clean:
	rm -rf *.o bmm150 bmi160 main
