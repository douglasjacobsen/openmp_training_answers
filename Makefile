CCOMP ?= icpc
CFLAGS ?= -qopenmp -O3 -g -qopt-report=5

all:
	$(CCOMP) count.cpp $(CFLAGS) -S
	$(CCOMP) count.cpp $(CFLAGS) -o COUNT.x

clean:
	rm -f *.s *.o *.optrpt COUNT.x
