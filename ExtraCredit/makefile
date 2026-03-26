CC = mpicc
CPP = mpic++
LDC = mpicc
LD_FLAGS = -L/opt/sw/opencv/lib -lm -fopenmp -lstdc++ -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs
FLAGS = -fopenmp
CPPFLAGS= -I/opt/sw/opencv/include/opencv4 $(FLAGS)
PROG = mandelbrot.cx
RM= /bin/rm


#all rule

OBJS=$(PROG:%.cx=%.o)
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
