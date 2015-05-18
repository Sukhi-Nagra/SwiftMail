CC = gcc
FLAGS = -g
DEPS = tcp.h imap.h server.h
OBJ = tcp.o imap.o server.o

# build deps
%.o: %.c $(DEPS)
	$(CC) $(FLAGS) -c -o $@ $<

# link objs
swiftmail: $(OBJ)
	$(CC) $(FLAGS) -o $@ $^

# wipeout objs and emacs files
clean:
	rm -f *.o *~
