CC = mpicc
CPP = mpic++
LDC = mpicc
LD_FLAGS = -L/opt/sw/opencv/lib -lm -fopenmp -lstdc++ -lopencv_core -lopencv_imgproc -lopencv_imgcodecs
FLAGS = -fopenmp -g
CPPFLAGS= -I/opt/sw/opencv/include/opencv4 $(FLAGS)
PROG = final.x
RM= /bin/rm


#all rule

OBJS=$(PROG:%.x=%.o)
TOOLS=imageTools.o

all: $(PROG)

$(PROG): $(OBJS) $(TOOLS)
	$(LDC)  $^ $(LD_FLAGS) -o $@

%.o: %.c
	$(CC) $(FLAGS) -c $^ -o $@
%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $^ -o $@

#clean rule
clean:
	$(RM) -rf *.o *.x *.cx *.mod
