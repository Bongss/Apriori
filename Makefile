CC=g++
CFLAGS=-std=c++11
all: apriori.exe

apriori.exe: main.cc apriori.h trie.h item.h
	$(CC) ${CFLAGS} $< -o $@

old_apriori.exe: main.cc apriori.h trie.h item.h
	$(CC) -std=c++0x $< -o $@

clean:
	rm -f ${all}
	rm -f *.o
