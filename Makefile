PROJ=ifj20
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic 
CC=gcc
LIBS=-lm
RM=rm -f
FILES=main.c

$(PROJ) : $(FILES)
	$(CC) $(CFLAGS) -o $(PROJ) $(FILES) $(LIBS)
	
clean:
	$(RM) *.o $(PROJ)
