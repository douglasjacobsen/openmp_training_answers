#include <stdio.h>
#include <stdlib.h>
#include "omp.h"

int main(int argc, char **argv) {
    if ( argc != 3 ) {
        printf("ERROR: Incorrect usage\n");
        printf("Usage statement -- ./COUNT.x N M\n");
        printf("\tN: Number of tasks to spawn\n");
        printf("\tM: Target number to stop counting at\n");
        return(1);
    }

    int ntasks = atoi(argv[1]);
    int count_target = atoi(argv[2]);

    int nthreads = 1;

    printf("Task counter\n\tntasks = %d\n\tcount_target = %d\n", ntasks, count_target);
    printf("\tnthreads = %d\n", nthreads);

    int started = 0, completed = 0;

    for (int n = 0; n < ntasks; n++ ) {
        started++;
        completed++;
    }

    printf(" %d tasks were started\n", started);
    printf(" %d tasks were completed\n", completed);
    printf(" The target was %d\n", count_target);

    if ( completed != count_target ) {
        printf(" This test FAILED\n");
    } else {
        printf(" This test PASSED\n");
    }

}
