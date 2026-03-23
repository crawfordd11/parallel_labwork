CC = mpicc
LDC = mpicc
FLAGS =
LD_FLAGS = -lm
PROG = LA5.x
OBJS = LA5.o
RM = /bin/rm



all: $(PROG)

$(PROG): $(OBJS)
	$(LDC) $(OBJS) -o $(PROG) $(LD_FLAGS)

%.o: %.c
	$(CC) $(FLAGS) -c $<

clean:
	$(RM) -rf *.o $(PROG)

