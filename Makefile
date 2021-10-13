CC = clang
CFLAGS = -Wall -Werror -Wpedantic -Wextra

all: banhammer

banhammer: banhammer.o bv.o bf.o ht.o ll.o node.o parser.o speck.o
	$(CC) -o banhammer banhammer.o bv.o bf.o ht.o ll.o node.o parser.o speck.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f banhammer *.o

format:
	clang-format -i -style=file *.c *.h
