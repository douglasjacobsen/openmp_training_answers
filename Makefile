CCOMP ?= icpc
CFLAGS ?= -qopenmp -O3 -g -qopt-report=5

all:
	$(CCOMP) fibonacci.c $(CFLAGS) -S
	$(CCOMP) fibonacci.c $(CFLAGS) -o FIB.x

clean:
	rm -f *.s *.o *.optrpt FIB.x
