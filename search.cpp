#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <sys/time.h>
#include "omp.h"

std::vector< std::vector<int> > table;
std::vector<int> searched;

double get_time() {
        struct timeval tp;
        struct timezone tzp;
        int i;

        i = gettimeofday(&tp,&tzp);
        return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}

void setup_table( const int n_values, const int n_bins, const int search_val ){

    // Initialize the table sizes
    table.clear();
    table.resize(n_bins);
    searched.clear();

    for ( int i = 0; i < n_bins; i++ ){
        table[i].clear();
        searched.push_back(0);
    }

    // Fill the table with a bunch of random numbers
#pragma omp parallel
    {
#pragma omp master
        {
#pragma omp taskgroup
            for ( int i = 0; i < n_bins; i++ ){

#pragma omp task
                for ( int j = 0; j < n_values / n_bins; j++ ) {
                    int val = ( i * j ) % n_values;

                    if ( val == search_val ) {
                        val--;
                    }
                    table[i].push_back( val );
                }
            }
        }
    }
    //for ( int i = 0; i < n_values; i++ ){
        //int val = rand();
        //int bin = val % n_bins;

        //table[bin].push_back( val );
    //}

    // Now, we're going to insert our search value
    // in a random bin, at a random index within the bin.
    // This insert causes this table to not be a hash table anymore.
    int bin = rand() % n_bins;
    int idx = rand() % table[bin].size();
    table[bin][idx] = search_val;
}

void search_table( const int search_val ) {

#pragma omp parallel
    {
#pragma omp master
        {
#pragma omp taskgroup
            for ( int i = 0; i < table.size(); i++ ) {
#pragma omp task firstprivate(search_val)
                {
#pragma omp cancellation point taskgroup
                    for ( int j = 0; j < table[i].size(); j++ ){
                        if ( table[i][j] == search_val ) {
                            printf("Value found in bin %d at index %d\n", i, j);
#pragma omp cancel taskgroup
                        }
                    }
                    searched[i] = 1;
                }
            }
        }
    }

    int searched_bins = 0;
    for ( int i = 0; i < searched.size(); i++ ) {
        searched_bins += searched[i];
    }
    printf("Search looked in %d bins\n", searched_bins);
}


int main ( int argc, char **argv ){
    if (argc != 4) {
        printf("Usage Error!\n");
        printf("Usage:  ./SRCH.x V B S\n");
        printf("\tV: Number of values to insert into table prior to searching\n");
        printf("\tB: Number of bins to split up the table\n");
        printf("\tS: Value to insert and search for\n");
        return(1);
    }

    srand( 10000 );

    // If you want run-to-run variation
    /*srand( time(NULL) );*/

    int number_of_values = atoi(argv[1]);
    int number_of_bins = atoi(argv[2]);
    int search_val = atoi(argv[3]);
    int nthreads = 1;
#pragma omp parallel
    {
#pragma omp master
        {
            nthreads = omp_get_num_threads();
        }
    }

    printf("Table search example\n");
    printf("   Table has %d bins\n", number_of_bins);
    printf("   Inserting %d values into table\n", number_of_values);
    printf("   Searching for value %d\n", search_val);
    printf("   Using %d thread(s)\n", nthreads);

    double start_time, end_time;
    double setup_time, search_time;

    start_time = get_time();
    setup_table(number_of_values, number_of_bins, search_val);
    end_time = get_time();
    setup_time = end_time - start_time;

    start_time = get_time();
    search_table( search_val );
    end_time = get_time();
    search_time = end_time - start_time;

    printf("\n\n");
    printf("Setup took %lf seconds\n", setup_time);
    printf("Search took %lf seconds\n", search_time);
}
