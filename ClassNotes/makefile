CC = mpicc
LDC = mpicc
FLAGS =
LD_FLAGS =
PROG = dot_scatter.x
OBJS = dot_scatter.o
RM = /bin/rm



all: $(PROG)

$(PROG): $(OBJS)
	$(LDC) $(OBJS) -o $(PROG) $(LD_FLAGS)

%.o: %.c
	$(CC) $(FLAGS) -c $<

clean:
	$(RM) -rf *.o $(PROG)

