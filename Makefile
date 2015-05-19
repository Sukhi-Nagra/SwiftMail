CC = gcc
FLAGS = -g
DEPS = tcp.h imap.h 
OBJ = tcp.o imap.o

all: swiftmail

# build deps
%.o: %.c $(DEPS)
	$(CC) $(FLAGS) -c -o $@ $<

# link objs
swiftmail: $(OBJ)
	$(CC) $(FLAGS) -o $@ $^

# wipeout objs and emacs files
clean:
	rm -f *.o *~ swiftmail
