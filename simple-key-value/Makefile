CC = g++
CFLAGS = -O2 -g -Wall

target: myredis

myredis: redis-server redis-client
.PHONY: myredis

redis-server: server.cpp hashtable.o heap.o ThreadPool.o zset.o
	$(CC) $(CFLAGS) $^ -o $@

# avl.o: avl.cpp
# 	$(CC) -c $(CFLAGS) $^ -o $@

client.o: client.cpp
	$(CC) -c $(CFLAGS) $^ -o $@

hashtable.o: hashtable.cpp
	$(CC) -c $(CFLAGS) $^ -o $@

heap.o: heap.cpp
	$(CC) -c $(CFLAGS) $^ -o $@

ThreadPool.o: ThreadPool.cpp
	$(CC) -c $(CFLAGS) $^ -o $@

zset.o: zset.cpp
	$(CC) -c $(CFLAGS) $^ -o $@

redis-client: client.cpp
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm *.o
.PHONY: clean