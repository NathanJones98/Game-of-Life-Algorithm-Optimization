/*****************************************************************************
 * life.c
 * The original sequential implementation resides here.
 * Do not modify this file, but you are encouraged to borrow from it
 ****************************************************************************/
#include "life.h"
#include "util.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
#define SWAP_BOARDS( b1, b2 )  do { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)

struct data {
	int thread_id;
	char * outboard;
	char * inboard;
	int nrows;
	int ncols;
	int gens_max;
	pthread_barrier_t barrier;
	//pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
};

#include <sched.h>
#include <assert.h>

#define NUM_OF_CORES 32 
int rowsOverCores = 0;

#define BOARD( __board, __i, __j )  (__board[(__i) + LDA*(__j)])

static inline int 

/*
assignToThisCore(int core_id)
{
    //CPU_ZERO(&mask);
    //CPU_SET(core_id, &mask);
    //sched_setaffinity(0, sizeof(mask), &mask);
}*/

/*
int reduce(int a, int b) {
	
	int d = a >= b ? a % b : a;

	int e = a & (b - 1); 
	//int e = (((u_int64_t) a * (u_int64_t) b) >> 64);
	int c = a % b;

	if(e == 1023){
		e = -1;
	} else if(e == -1){
		//e = 1023;
	}

	if(e == 1023 || e ==-1){
		e = c;
	}

	if(c!=e){
		printf("\n1: %d 2: %d", c, e);
	}
	
	assert(c == e);
	//printf("\n1: %d 2: %d", d, c);

  	return e;
	
}*/

mod_ (int x, int m)
{

	//return (x < 0) ? ((x % m) + m) : (x % m);

	return (x < 0) ? ((x >= m ? x % m : x)) + m : ((x >= m ? x % m : x));

 	/*int e = x & (m - 1);
	if(e == 1023 || e ==-1){
		e = x % m;
	} 
	return (x < 0) ? (e) + m : (e);*/
}



static inline char 
alivep_ (char count, char state)
{
  return (! state && (count == (char) 3)) || (state && (count >= 2) && (count <= 3));
}

pthread_barrier_t barrier;

void * threadFunc(void * arg)
{	
	
	struct data * vals = ((struct data *) arg);
	//int thread_id =  *((int*) arg[0][0]);
	//int thread_id =  (int*) arg;
    /*printf("\n2 Struct ptr: %p Thread_id: %ld Outboard: %p Inboard: %p nrows: %d ncol: %d gens_max: %d \n",
	 vals, vals->thread_id, vals->outboard, vals->inboard, vals->nrows,
	 	vals->ncols, vals->gens_max);*/

	int curgen, i, j, start, end;

	//if(vals->thread_id != 0){
		start = vals->thread_id;
		end = vals->thread_id + 1;
	//}

	
	char* inboard_ = vals->inboard;
	char* outboard_ = vals->outboard;
	int gens_max_ = vals->gens_max;
	int nrows_ = vals->nrows;
	const int LDA = nrows_;
	int ncols_ = vals->ncols;

	//char counted[9];
	//char counted0, counted1, counted2, counted3, counted4, counted5, counted6, counted7;
	
	//assignToThisCore(vals->thread_id);
	cpu_set_t  mask;
	CPU_SET(vals->thread_id, &mask);
	sched_setaffinity(0, sizeof(mask), &mask);
	//pthread_barrier_t * barrier_ = vals->barrier;


	for (curgen = 0; curgen < gens_max_; curgen++)
    {	
		//printf("\nStart: %d", start);
		pthread_barrier_wait (&barrier);
		//printf("\npassed: %d", start);
		
        /* HINT: you'll be parallelizing these loop(s) by doing a
           geometric decomposition of the output */
        for (j = start * (rowsOverCores); j <= end * (rowsOverCores) && j < nrows_; j++)
        {	

			const int jwest = mod_ (j-1, ncols_);
            const int jeast = mod_ (j+1, ncols_);
			
			//printf("\nStart: %d", start);
			//x
            for (i = 0; i < ncols_; i++)
            {
                const int inorth = mod_ (i-1, nrows_);
                const int isouth = mod_ (i+1, nrows_);

				//printf("\nI: %d, J: %d", i,j);
				/*
				if(i == 0){
					
					counted0 = BOARD (inboard_, inorth, jeast); 
                    counted1 = BOARD (inboard_, i, jeast);
                    counted2 = BOARD (inboard_, isouth, jeast);

                    counted3 = BOARD (inboard_, inorth, j);
                    counted4 = BOARD (inboard_, isouth, j);
                    
					counted5 = BOARD (inboard_, inorth, jwest);
					counted6 = BOARD (inboard_, i, jwest);
					counted7 = BOARD (inboard_, isouth, jwest);
                    
                    
				} else {
					counted0 = counted1;
					counted1 = counted2;
					counted2 = BOARD (inboard_, isouth, jeast);

                    counted3 = BOARD (inboard_, inorth, j);
                    counted4 = BOARD (inboard_, isouth, j);

                    counted5 = counted6;
					counted6 = counted7;
					counted7 = BOARD (inboard_, isouth, jwest);
				}

				
				printf("\nalive 0: %d, old: %d", counted[0], BOARD (inboard_, inorth, jeast) );
				printf("\nalive 1: %d, old: %d", counted[1], BOARD (inboard_, i, jeast) );
				printf("\nalive 2: %d, old: %d", counted[2], BOARD (inboard_, isouth, jeast) );
				printf("\nalive 3: %d, old: %d", counted[3], BOARD (inboard_, inorth, j) );
				printf("\nalive 4: %d, old: %d", counted[4], BOARD (inboard_, isouth, j) );
				printf("\nalive 5: %d, old: %d", counted[5], BOARD (inboard_, inorth, jwest));
				printf("\nalive 6: %d, old: %d", counted[6], BOARD (inboard_, i, jwest) );
				printf("\nalive 7: %d, old: %d", counted[7], BOARD (inboard_, isouth, jwest) );
				
				const char neighbor_count = counted0 + 
				counted1 + 
				counted2 +
				counted3 + 
				counted4 + 
				counted5 + 
				counted6 + 
				counted7;	
				*/
				
                const char neighbor_count = 
                    BOARD (inboard_, inorth, jwest) + 
                    BOARD (inboard_, inorth, j) + 
                    BOARD (inboard_, inorth, jeast) + 
                    BOARD (inboard_, i, jwest) +
                    BOARD (inboard_, i, jeast) + 
                    BOARD (inboard_, isouth, jwest) +
                    BOARD (inboard_, isouth, j) + 
                    BOARD (inboard_, isouth, jeast);
				/*
				if(neighbor_count != neighbor_count_){
					
					printf("\nI: %d, J: %d, new: %d, old: %d", i,j,neighbor_count_,neighbor_count);

					//printf("\nalive 1: %d, old: %d", counted[0], BOARD (inboard_, inorth, jeast) );
					//printf("\nalive 2: %d, old: %d", counted[0], BOARD (inboard_, i, jeast) );
					//printf("\nalive 3: %d, old: %d", counted[0], BOARD (inboard_, isouth, jeast) );
					//printf("\nalive 4: %d, old: %d", counted[0], BOARD (inboard_, inorth, j) );
					//printf("\nalive 5: %d, old: %d", counted[0], BOARD (inboard_, isouth, j) );
					//printf("\nalive 6: %d, old: %d", counted[0], BOARD (inboard_, inorth, jwest));
					//printf("\nalive 7: %d, old: %d", counted[0], BOARD (inboard_, i, jwest) );
					//printf("\nalive 8: %d, old: %d", counted[0], BOARD (inboard_, i, jwest) );
					//
				}
				
				assert(neighbor_count == neighbor_count_);*/

                BOARD(outboard_, i, j) = alivep_ (neighbor_count, BOARD (inboard_, i, j));
				
				/*
				char str1[20];

				printf("\n\nEnter name: ");
				//scanf("%19s", str1);
				*/

            }
			//printf("\nStart: %d", start);
        }
		//printf("\ncurgen: %d, %d", curgen, vals->thread_id);

		/*
		char* temp = outboard_; 
		outboard_ = inboard_; 
		inboard_ = temp; 
		*/
		SWAP_BOARDS( outboard_, inboard_ );
	}
	
}

char* game_of_life (char* outboard, 
	char* inboard,
	const int nrows,
	const int ncols,
	const int gens_max)
{
		
    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
    const int LDA = nrows;
    int curgen, i, j;

	//NUM_OF_CORES threads 
	pthread_t threads[NUM_OF_CORES];
	//5 pass values
	struct data * pass_vals[NUM_OF_CORES];

	//pthread_barrier_t barrier;
	pthread_barrier_init(&barrier, NULL, NUM_OF_CORES);

	rowsOverCores = nrows/NUM_OF_CORES;

	for(int i = 0; i < NUM_OF_CORES; i++){
		pass_vals[i] = (struct data *) malloc(sizeof(struct data));
		//printf("\nTest\n");
		pass_vals[i]->thread_id = i;
		pass_vals[i]->outboard = outboard;
		pass_vals[i]->inboard = inboard;
		pass_vals[i]->nrows = nrows;
		pass_vals[i]->ncols = ncols;
		pass_vals[i]->gens_max = gens_max;
		pass_vals[i]->barrier = barrier;
		
		//printf(*(int*)pass_vals[i][0]);

		//printf("\n1 Struct ptr: %p  Thread_id: %ld Outboard: %p Inboard: %p nrows: %d ncol: %d gens_max: %d \n",
	 		//pass_vals[i], pass_vals[i]->thread_id, pass_vals[i]->outboard, pass_vals[i]->inboard, pass_vals[i]->nrows,
	 		//pass_vals[i]->ncols, pass_vals[i]->gens_max);
		//printf("\nStart 1: %d", i);
		pthread_create(&threads[i], NULL, &threadFunc, (void*) pass_vals[i]);
	}

	
	for(int i = 0; i < NUM_OF_CORES; i++){
		pthread_join(threads[i], NULL);
		free(pass_vals[i]);
	}

    /*
    for (curgen = 0; curgen < gens_max; curgen++)
    {

        /* HINT: you'll be parallelizing these loop(s) by doing a
           geometric decomposition of the output //
        for (j = 0; j < nrows; j++)
        {
            for (i = 0; i < ncols; i++)
            {
                const int inorth = mod (i-1, nrows);
                const int isouth = mod (i+1, nrows);
                const int jwest = mod (j-1, ncols);
                const int jeast = mod (j+1, ncols);

                const char neighbor_count = 
                    BOARD (inboard, inorth, jwest) + 
                    BOARD (inboard, inorth, j) + 
                    BOARD (inboard, inorth, jeast) + 
                    BOARD (inboard, i, jwest) +
                    BOARD (inboard, i, jeast) + 
                    BOARD (inboard, isouth, jwest) +
                    BOARD (inboard, isouth, j) + 
                    BOARD (inboard, isouth, jeast);

                BOARD(outboard, i, j) = alivep (neighbor_count, BOARD (inboard, i, j));

            }
        }
        SWAP_BOARDS( outboard, inboard );
    }
	*/
    /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     */
    return inboard;
}


