#include <stdio.h>
#include <stdlib.h>
#include "imageprocessing.h"
#include "bmp.h"

// Aceasta e un fisier exemplu pentru a vedea
// cum folositi functiile read_from_bmp si write_to_bmp

// puteti folosi acest fisier si pentru testarea manuala
// a functiilor pe care le implementati in imageprocessing.c

int main()
{
	// alocam o matrice
	int N = 789, M=1024;
	int ***m = (int ***)malloc(N * sizeof(int **));
    for (int i = 0; i < N; i++) {
        m[i] = (int **)malloc(M * sizeof(int *));
        for (int j = 0; j < M; j++) {
            m[i][j] = (int *)malloc(3 * sizeof(int));
        }
    }

    read_from_bmp(m, N, M, "./4964064451_25410b9741_b.bmp");

	// mica procesare pe imagine ca exemplu
	
	// adaugam o banda orizontala rosie
    for (int i=20; i<40; i++) {
        for (int j=0; j<M; j++) {
            m[i][j][0] = 255;
        }
    }

	// adaugam o banda verticala verde
    for (int j=100; j<200; j++) {
        for (int i=0; i<N; i++) {
            m[i][j][1] = 255;
        }
    }

    // // teste 
    // m = flip_horizontal(m, N, M);
	// // salvam imaginea sub numele output.bmp
    // write_to_bmp(m, N, M, "./rez.bmp");

	// free matrice
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            free(m[i][j]);
        }
		free(m[i]);
    }
	free(m);

    printf("Done\n");
    return 0;
}
