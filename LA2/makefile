CC = mpicc
LDC = mpicc
FLAGS =
LD_FLAGS =
PROG = LA2.x
OBJS = LA2.o
RM = /bin/rm



all: $(PROG)

$(PROG): $(OBJS)
	$(LDC) $(OBJS) -o $(PROG) $(LD_FLAGS)

%.o: %.c
	$(CC) $(FLAGS) -c $<

clean:
	$(RM) -rf *.o $(PROG)

