producer: producer.c
	gcc producer.c -pthread -o producer
	./producer
test: test
	gcc test.c -pthread -o test
	./test
