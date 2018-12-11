CCOMP ?= xlc_r
CFLAGS ?= -qsmp=omp -qoffload -O3

all:
	$(CCOMP) stream.c $(CFLAGS) -o STREAM.x

clean:
	rm -f *.s *.o *.optrpt STREAM.x
