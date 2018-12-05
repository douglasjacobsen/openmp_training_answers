CCOMP ?= icpc
CFLAGS ?= -qopenmp -O3 -g -qopt-report=5

all:
	$(CCOMP) search.cpp $(CFLAGS) -S
	$(CCOMP) search.cpp $(CFLAGS) -o SRCH.x

clean:
	rm -f *.s *.o *.optrpt SRCH.x
