INCDIR   := inc
SRCDIR   := src

CC=gcc

SOURCE = main.c ./$(SRCDIR)/*.c ./$(SRCDIR)/NIST/*.c ./$(SRCDIR)/BCH/*.c 

HEADER = -I./$(INCDIR) -I./$(INCDIR)/bch -I./$(INCDIR)/NIST
CFLAGS  = -march=native -mtune=native -O3 -fomit-frame-pointer #-Wall -Wextra -Wpedantic -Werror
LDFLAGS = -lcrypto

ALL:
	$(CC)  -c  $(SOURCE) $(HEADER) $(CFLAGS)
	$(CC)  *.o -o main $(LDFLAGS) $(CFLAGS)
	rm *.o