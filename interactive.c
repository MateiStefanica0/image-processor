#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imageprocessing.h"
#include "bmp.h"

#define std_len_poze 100
#define std_len_filtre 100
#define path_len 100
#define comanda_len 10  // aceasta valoare ar putea sa fie si 3, pentru ca nu exista comenzi mai lungi de 2
                        // insa in ultimul test pe care il foloseste taskul de valgrind, comanda af
                        // primeste 2 parametrii in plus,unul dintre ei avand mai multe caractere, asa ca
                        // am definit lungimea comenzii cu 10 ca sa fie acoperitor

// structuri folosite

struct Poza {
    int N, M;
    int ***m;
};

struct Filtru {
    int dim_filtru;
    float **valori;
};

int main() {
    char *comanda = NULL;
    int memory_increase_poze = 1;  // variabila folosita pentru a creste capacitatea pozelor
    int memory_increase_filtre = 1;  // variabila folosita pentru a creste capacitatea filtrelor

    comanda = (char *)calloc(comanda_len, sizeof(char));  // pentru citirea comenzii

    // pointerii in care vor fi stocate pozele si filtrele
    struct Poza *poze = calloc(std_len_poze * memory_increase_poze, sizeof(struct Poza));
    struct Filtru *filtre = calloc(std_len_filtre * memory_increase_filtre, sizeof(struct Filtru));

    int nr_poze = 0;
    int nr_filtre = 0;

    char *path = NULL;
    path = (char *)calloc(path_len, sizeof(char));

    // variabile folosite ca parametrii
    int index = 0;
    int i = 0, j = 0;
    int k = 0;
    int x = 0, y = 0, h = 0, w = 0;
    int R = 0, G = 0, B = 0, rows = 0, cols = 0;
    int index_dst = 0, index_src = 0;
    int index_img = 0, index_filter = 0;

    int aux = 0;

    //  rulare efectiva
    while (1) {
        scanf("%s", comanda);
        if (strcmp(comanda, "e") == 0) {  // exit
            // eliberare memorie pentru poze
            for (k = 0; k < nr_poze; k++) {
                for (i = 0; i < poze[k].N; i++) {
                    for (j = 0; j < poze[k].M; j++) {
                        free(poze[k].m[i][j]);
                    }
                    free(poze[k].m[i]);
                }
                free(poze[k].m);
            }
            free(poze);

            // eliberare memorie pentru filtre
            for (k = 0; k < nr_filtre; k++) {
                for (i = 0; i < filtre[k].dim_filtru; i++) {
                    free(filtre[k].valori[i]);
                }
                free(filtre[k].valori);
            }

            // alte eliberari
            free(filtre);
            free(comanda);
            free(path);

            return 0;
        } else if (strcmp(comanda, "l") == 0) {  // load
            // N M path
            scanf("%d", &poze[nr_poze].N);
            scanf("%d", &poze[nr_poze].M);
            scanf("%s", path);

            // alocare matrice noua
            poze[nr_poze].m = (int ***)malloc(poze[nr_poze].N * sizeof(int **));
            for (int i = 0; i < poze[nr_poze].N; i++) {
                poze[nr_poze].m[i] = (int **)malloc(poze[nr_poze].M * sizeof(int *));
                for (int j = 0; j < poze[nr_poze].M; j++) {
                    poze[nr_poze].m[i][j] = (int *)malloc(3 * sizeof(int));
                }
            }

            // citire poza
            read_from_bmp(poze[nr_poze].m, poze[nr_poze].N, poze[nr_poze].M, path);
            nr_poze++;

            // marirea spatiului alocat in cazul in care este nevoie
            if (nr_poze > std_len_poze * memory_increase_poze) {
                memory_increase_poze++;
                poze = realloc(poze, std_len_poze * memory_increase_poze);
            }
        } else if (strcmp(comanda, "s") == 0) {  // save
            // index path
            scanf("%d", &index);
            scanf("%s", path);
            write_to_bmp(poze[index].m, poze[index].N, poze[index].M, path);
        } else if (strcmp(comanda, "ah") == 0) {  // horizontal flip
            // index
            scanf("%d", &index);
            // int ***flip_horizontal(int ***image, int N, int M);
            poze[index].m = flip_horizontal(poze[index].m, poze[index].N, poze[index].M);
        } else if (strcmp(comanda, "ar") == 0) {  // apply rotate
            // index
            scanf("%d", &index);
            // int ***rotate_left(int ***image, int N, int M);
            poze[index].m = rotate_left(poze[index].m, poze[index].N, poze[index].M);

            // interschimbarea inaltimii si lungimii matricei
            aux = poze[index].N;
            poze[index].N = poze[index].M;
            poze[index].M = aux;
        } else if (strcmp(comanda, "ac") == 0) {  // apply crop
            // index x y w h
            scanf("%d", &index);
            scanf("%d", &x);
            scanf("%d", &y);
            scanf("%d", &w);
            scanf("%d", &h);
            // int ***crop(int ***image, int N, int M, int x, int y, int h, int w);
            poze[index].m = crop(poze[index].m, poze[index].N, poze[index].M, x, y, h, w);
            // actualizare inaltime si latime matrice
            poze[index].N = h;
            poze[index].M = w;
        } else if (strcmp(comanda, "ae") == 0) {  // aplly extend
            // index rows cols R G B
            scanf("%d", &index);
            scanf("%d", &rows);
            scanf("%d", &cols);
            scanf("%d", &R);
            scanf("%d", &G);
            scanf("%d", &B);
            poze[index].m = extend(poze[index].m, poze[index].N, poze[index].M, rows, cols, R, G, B);
            // actualizare inaltime si latime matrice
            poze[index].N += (2 * rows);
            poze[index].M += (2 * cols);
        } else if (strcmp(comanda, "ap") == 0) {  // apply paste
            // index_dst index_src x y
            scanf("%d", &index_dst);
            scanf("%d", &index_src);
            scanf("%d", &x);
            scanf("%d", &y);
            // checherul de coding style ma obliga sa sparg antetul urmatoarei functii in doua
            // int ***paste(int ***image_dst, int N_dst, int M_dst,
            // int *** image_src, int N_src, int M_src, int x, int y);

            // checkerul de coding style ma obliga sa sparg urmatoarea linie in 2
            poze[index_dst].m = paste(poze[index_dst].m, poze[index_dst].N,
            poze[index_dst].M, poze[index_src].m, poze[index_src].N, poze[index_src].M, x, y);
        } else if (strcmp(comanda, "cf") == 0) {  // create filter
            // size [list of values]
            scanf("%d", &filtre[nr_filtre].dim_filtru);
            // alocare memorie pentru un nou filtru
            filtre[nr_filtre].valori = (float **)calloc(filtre[nr_filtre].dim_filtru, sizeof(float *));
            for (i = 0; i < filtre[nr_filtre].dim_filtru; i++) {
                filtre[nr_filtre].valori[i] = (float *)calloc(filtre[nr_filtre].dim_filtru, sizeof(float));
            }

            // citire filtru
            for (i = 0; i < filtre[nr_filtre].dim_filtru; i++) {
                for (j = 0; j < filtre[nr_filtre].dim_filtru; j++) {
                    scanf("%f", &filtre[nr_filtre].valori[i][j]);
                }
            }
            nr_filtre++;
            // cresterea memoriei alocare pentru filtre, daca este cazul
            if (nr_filtre > std_len_filtre * memory_increase_filtre) {
                memory_increase_filtre++;
                filtre = realloc(filtre, std_len_filtre * memory_increase_filtre);
            }
        } else if (strcmp(comanda, "af") == 0) {  // aplly filter
            // index_img index_filter
            scanf("%d", &index_img);
            scanf("%d", &index_filter);
            // int ***apply_filter(int ***image, int N, int M, float **filter, int filter_size);
            // checkerul de coding style ma obliga sa sparg urmatoarea linie in 2
            poze[index_img].m = apply_filter(poze[index_img].m, poze[index_img].N,
            poze[index_img].M, filtre[index_filter].valori, filtre[index_filter].dim_filtru);
        } else if (strcmp(comanda, "df") == 0) {  // delete filter
            scanf("%d", &index_filter);
            // eliberare pozitie
            for (i = 0; i < filtre[index_filter].dim_filtru; i++) {
                free(filtre[index_filter].valori[i]);
            }
            free(filtre[index_filter].valori);

            // mutare filtre mai la stanga incepand cu index_filter
            for (i = index_filter; i < nr_filtre; i++) {
                filtre[i] = filtre[i+1];
            }

            nr_filtre--;
            // eliberarea ultimei pozitii
            for (i = 0; i < filtre[nr_filtre].dim_filtru; i++) {
                free(filtre[nr_filtre].valori[i]);
            }
            free(filtre[nr_filtre].valori);
        } else if (strcmp(comanda, "di") == 0) {  // delete image
            // index_img
            scanf("%d", &index_img);
            // eliberare pozitie pentru filtrul sters
            for (i = 0; i < poze[index_img].N; i++) {
                    for (j = 0; j < poze[index_img].M; j++) {
                        free(poze[index_img].m[i][j]);
                    }
                    free(poze[index_img].m[i]);
                }
            free(poze[index_img].m);

            // mutare mai la stanga
            for (i = index_img; i < nr_poze; i++) {
                poze[i] = poze[i+1];
            }
            nr_poze--;

            // eliberare ultima pozitie
            for (i = 0; i < poze[nr_poze].N; i++) {
                for (j = 0; j < poze[nr_poze].M; j++) {
                    free(poze[nr_poze].m[i][j]);
                }
                free(poze[nr_filtre].m[i]);
            }
            free(poze[nr_poze].m);
        }
    }
    return 0;
}

