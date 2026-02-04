CC = mpicc
LDC = mpicc
FLAGS =
LD_FLAGS =
PROG = mm.x
OBJS = mm-student.o
RM = /bin/rm



all: $(PROG)

$(PROG): $(OBJS)
	$(LDC) $(OBJS) $(LD_FLAGS) -o $(PROG)

%.o: %.c
	$(CC) $(FLAGS) -c $<

clean:
	$(RM) -rf *.o $(PROG)

