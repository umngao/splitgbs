CFLAGS = -D_GNU_SOURCE -lz -g -std=c99 -pedantic -Wall -pipe -O2 -march=native -fomit-frame-pointer

splitgbs: splitgbs.c 
	cc ${CFLAGS} splitgbs.c -o splitgbs

