#include <stdio.h>
#include <stdlib.h>
#include "imageprocessing.h"
#define min 0
#define max 255

int ***flip_horizontal(int ***image, int N, int M) {
    // inversarea valorilor de pe fiecare rand cu cele de la final, pana la jumatate
    int i = 0, j = 0, k = 0, aux = 0;
    for (i = 0; i < N; i++) {
        for (j = 0; j < M / 2; j++) {
            for (k = 0; k < 3; k++) {
                aux = image[i][j][k];
                image[i][j][k] = image[i][M-j-1][k];
                image[i][M-j-1][k] = aux;
            }
        }
    }
    return image;
}

int ***rotate_left(int ***image, int N, int M) {
    // operatia dorita se poate realiza prin "horizontal flip" si transpunere
    int i = 0, j = 0, k = 0;
    image = flip_horizontal(image, N , M);
    // alocare matrice noua, cu dimensiunile de lungime si inaltime inversate fata de cele ale matricei initiale
    int ***m = (int ***)malloc(M * sizeof(int **));
    for (i = 0; i < M; i++) {
        m[i] = (int **)malloc(N * sizeof(int *));
        for (j = 0; j < N; j++) {
            m[i][j] = (int *)malloc(3 * sizeof(int));
        }
    }

    // transpunere
    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            for (k = 0; k < 3; k++) {
                m[i][j][k] = image[j][i][k];
            }
        }
    }

    // eliberare memorie pentru matricea initiala
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            free(image[i][j]);
        }
        free(image[i]);
    }
    free(image);
    return m;
}

int ***crop(int ***image, int N, int M, int x, int y, int h, int w) {
    // alocare matrice noua
    int i = 0, j = 0, k = 0;
    int ***m = (int ***)malloc(h * sizeof(int **));
    for (i = 0; i < h; i++) {
        m[i] = (int **)malloc(w * sizeof(int *));
        for (j = 0; j < w; j++) {
            m[i][j] = (int *)malloc(3 * sizeof(int));
        }
    }
    // umplere matrice noua cu partea care rezulta dupa "crop"
    for (i = y; i < y + h ; i++) {
        for (j = x; j < x + w; j++) {
            for (k = 0; k < 3; k++) {
                m[i-y][j-x][k] = image[i][j][k];
            }
        }
    }

    // eliberare matrice veche
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            free(image[i][j]);
        }
        free(image[i]);
    }
    free(image);
    return m;
}

int ***extend(int ***image, int N, int M, int rows, int cols, int new_R, int new_G, int new_B) {
    // dimensiunile noii matrice
    int nNou = N + 2 * rows;
    int mNou = M + 2 * cols;
    int i = 0, j = 0, k = 0;
    // alocare matrice noua
    int ***m = (int ***)malloc(nNou * sizeof(int **));
    for (i = 0; i < nNou; i++) {
        m[i] = (int **)malloc(mNou * sizeof(int *));
        for (j = 0; j < mNou; j++) {
            m[i][j] = (int *)malloc(3 * sizeof(int));
        }
    }

    // initializarea intregii matrici cu noile valori;
    for (i = 0; i < nNou; i++) {
        for (j = 0; j < mNou; j++) {
            m[i][j][0] = new_R;
            m[i][j][1] = new_G;
            m[i][j][2] = new_B;
        }
    }

    // suprapunerea valorilor vechii matrici peste cea noua
    // rezultatul va fi matricea noua in centru, iar pe margini valorile RGB cerute
    for (i = rows; i < N + rows; i++) {
        for (j = cols; j < M + cols; j++) {
            for (k = 0; k < 3; k++) {
            m[i][j][k] = image[i-rows][j-cols][k];
            }
        }
    }

    // eliberare memorie pentru matricea veche
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            free(image[i][j]);
        }
        free(image[i]);
    }
    free(image);
    return m;
}

int ***paste(int ***image_dst, int N_dst, int M_dst, int ***image_src, int N_src, int M_src, int x, int y) {
    // fara alocari
    int i = 0, j = 0, k = 0, stopN = 0, stopM = 0;
    // stopN si stopM sunt valorile pana la care trebuie facuta operatia de paste
    // ele sunt dupa caz, ori finalul matricei destinatie, ori finalul matricei sursa
    if (y + N_src < N_dst) {
        stopN = N_src + y;
    } else {
        stopN = N_dst;
    }
    for (i = y; i < stopN; i++) {
        if (x + M_src < M_dst) {
            stopM = M_src + x;
        } else {
            stopM = M_dst;
        }
        for (j = x; j < stopM; j++) {
            for (k = 0; k < 3; k++) {
                image_dst[i][j][k] = image_src[i-y][j-x][k];
            }
        }
    }
    return image_dst;
}

int ***apply_filter(int ***image, int N, int M, float **filter, int filter_size) {
    // alocare matrice noua
    int ***m = (int ***)malloc(N * sizeof(int **));
    for (int i = 0; i < N; i++) {
        m[i] = (int **)malloc(M * sizeof(int *));
        for (int j = 0; j < M; j++) {
            m[i][j] = (int *)malloc(3 * sizeof(int));
        }
    }

    // extindere matrice cu 0
    int n_extins = N + filter_size - 1, m_extins = M + filter_size - 1;
    image = extend(image, N, M, filter_size / 2, filter_size / 2, 0, 0, 0);
    float rosu = 0, verde = 0, albastru = 0;
    int i = 0, j = 0, p = 0, q = 0;
    for (i = filter_size / 2; i < N + filter_size / 2; i++) {
        for (j = filter_size / 2; j < M + filter_size / 2; j++) {
                // aplicarea filtrului
                rosu = 0;
                verde = 0;
                albastru = 0;
                for (p = i - filter_size / 2; p < i + filter_size / 2 + 1; p++) {
                    for (q = j - filter_size / 2; q < j + filter_size / 2 + 1; q++) {
                        rosu = rosu + (float)image[p][q][0] * filter[p-i+filter_size/2][q-j+filter_size/2];
                        verde = verde + (float)image[p][q][1] * filter[p-i+filter_size/2][q-j+filter_size/2];
                        albastru = albastru + (float)image[p][q][2] * filter[p-i+filter_size/2][q-j+filter_size/2];
                    }
                }
                // prin urmatoarele operatii ne asiguram ca valorile RGB raman in intervalul [0, 255]
                if (rosu < min) {
                    rosu = min;
                }
                if (verde < min) {
                    verde = min;
                }
                if (albastru < min) {
                    albastru = min;
                }
                if (rosu > max) {
                    rosu = max;
                }
                if (verde > max) {
                    verde = max;
                }
                if (albastru > max) {
                    albastru = max;
                }
                // cast la int a valorilor rezultate in urma aplicarii filtrului
                m[i-filter_size/2][j-filter_size/2][0] = (int)rosu;
                m[i-filter_size/2][j-filter_size/2][1] = (int)verde;
                m[i-filter_size/2][j-filter_size/2][2] = (int)albastru;
        }
    }

    // eliberare matrice veche
    for (int i = 0; i < n_extins; i++) {
        for (int j = 0; j < m_extins; j++) {
            free(image[i][j]);
        }
        free(image[i]);
    }
    free(image);
    return m;
}
