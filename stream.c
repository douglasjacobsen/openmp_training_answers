#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "omp.h"

#define DATA_T float

DATA_T *a;
DATA_T *b;
DATA_T *c;

double get_time() {
        struct timeval tp;
        struct timezone tzp;
        int i;

        i = gettimeofday(&tp,&tzp);
        return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}

void triad( const int N, const DATA_T scalar) {
    int i;


/*#pragma omp target map(tofrom:a[0:N],b[0:N],c[0:N],scalar)*/
#pragma omp target map(to:b[0:N],c[0:N],scalar) map(tofrom:a[0:N])
    for ( i = 0; i < N; i++ ) {
        a[i] = b[i] * scalar + c[i];
    }
}

int main(int argc, char **argv) {
    int i, nthreads;
    unsigned long long N, ITRS;
    double start, end, time;
    DATA_T *swp;


    if ( argc != 3 ) {
        printf("Usage: ./stream [N] [ITRS]\n");
        printf("    N helps select the size of the arrays.\n");
        printf("       Arrays will be the closest multiple of 16 greater or equal to the input value of N\n");
        exit(1);
    }

    N = atol(argv[1]);
    if ( N % 16 != 0 ) N = N + ( 16 - ( N % 16 ) );
    ITRS = atol(argv[2]);

    // Determine the number of threads
    nthreads = 1;

    printf("Stream triad, N=%llu  ITRS=%llu THREADS=%d\n", N, ITRS, nthreads);

    a = (DATA_T *)malloc( sizeof(DATA_T) * N );
    b = (DATA_T *)malloc( sizeof(DATA_T) * N );
    c = (DATA_T *)malloc( sizeof(DATA_T) * N );

    for (i = 0; i < N; i++) {
        a[i] = i;
        b[i] = i*i;
        c[i] = i*i*i;
    }

    // Warm up
    for ( i = 0; i < 10; i++ ) {
        triad(N, 8*i);
    }

    double time_per_triad = 0.0;
    double best_time_per_triad = 999999999.00;
    for ( i = 0; i < ITRS; i++ ) {
        start = get_time();
        triad(N, 9*i);
        end = get_time();
        double triad_time = (double)(end - start);
        time_per_triad += triad_time;
        if ( triad_time < best_time_per_triad ) best_time_per_triad = triad_time;
    }

    FILE* null = fopen("/dev/null", "wb");
    fwrite(a, sizeof(DATA_T), N, null);
    fwrite(b, sizeof(DATA_T), N, null);
    fwrite(c, sizeof(DATA_T), N, null);
    fclose(null);

    time_per_triad /= ITRS;

    double read_data = (2 * (double)N * (double)sizeof(DATA_T)) / 1e9;
    double write_data = (1 * (double)N * (double)sizeof(DATA_T)) / 1e9;
    printf(" Size of DATA_T: %lf\n", (double)sizeof(DATA_T));
    printf(" Instructions per triad: %llu\n", 8 * N);
    printf(" Read data: %lf GB\n", read_data );
    printf(" Write data: %lf GB\n", write_data );
    printf(" Ave. Time per triad: %le s\n", time_per_triad );
    printf(" Ave. Read BW: %lf GB/s\n", read_data / time_per_triad );
    printf(" Ave. Write BW: %lf GB/s\n", write_data / time_per_triad );
    printf(" Best Time per triad: %le s\n", best_time_per_triad );
    printf(" Best Read BW: %lf GB/s\n", read_data / best_time_per_triad );
    printf(" Best Write BW: %lf GB/s\n", write_data / best_time_per_triad );
}
