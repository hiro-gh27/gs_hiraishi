#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <jmorecfg.h>
#include <sys/time.h>

#define N 4
#define REPEAT 1
#define BLOCK 2

int A[N][N], B[N][N], C[N][N];

int trace = 1;
int debug = 1;
int info = 1;

void print_all_matrix() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            printf("%d ", A[i][j]);
        }
        printf(" ");
        for (int k = 0; k < N; ++k) {
            printf("%d ", B[i][k]);
        }
        printf(" ");
        for (int l = 0; l < N; ++l) {
            printf("%d ", C[i][l]);
        }
        printf("\n");
    }
    printf("\n");
}

void init_matrix(){
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
            C[i][j] = 0;
        }
    }
    if (trace){
        printf("trace: initialize matrix\n");
        print_all_matrix();
    }
}

void clear_cache(){
    unsigned long sum = 0;
    FILE *fp;

    init_matrix();
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            sum += A[i][j] + B[i][j] + C[i][j];
        }
    }

    fp = fopen("/dev/null", "w");
    fprintf(fp, "%lu\n", sum);
    fclose(fp);
}

double now(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + 0.000001*tv.tv_usec;
}

void exec_ikj(){
    for (int i = 0; i < N; ++i) {
        for (int k = 0; k < N; ++k) {
            for (int j = 0; j < N; ++j) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    if (trace) {
        printf("trace: executed ikj pattern\n");
        print_all_matrix();
    }
}

void cache_blocking(int block_size){
    int i = 0, j = 0 , k = 0;
    if (block_size < 2){
        if (debug){
            printf("block size is too small, block_size: %d", block_size);
        }
        return;
    }
    int rest = N % block_size;
    for (i = 0; i < N-rest; i+=BLOCK) {
        for (j = 0; j < N-rest; j+=BLOCK) {
            for (k = 0; k < N-rest; k+=BLOCK) {
                for (int ii = i; ii < i+BLOCK; ++ii) {
                    for (int jj = j; jj < j+BLOCK; ++jj) {
                        for (int kk = k; kk < k+BLOCK; ++kk) {
                            C[ii][jj] += A[ii][kk] * B[kk][jj];
                            if (debug){
                                printf("debug: block_size:%d, rest: %d, i:%d, j;%d, k:%d, ii:%d, jj:%d, kk:%d\n"
                                       ,block_size, rest, i, j, k, ii,jj,kk);
                                printf("debug: C[%d][%d] += A[%d][%d]:%d * B[%d][%d]:%d\n"
                                       , ii, jj, ii, kk, A[ii][kk], kk, jj, B[kk][jj]);
                                print_all_matrix();
                            }
                        }
                    }
                }
            }
        }
    }
    if (debug){
        printf("block_size= %d, rest= %d\n", block_size, rest);
    }
    if (trace){
        printf("trace: executed N broking\n");
        print_all_matrix();
    }
}

int main() {
    srand((unsigned int) time(NULL));

    init_matrix();


    double total_time = 0;
    for (int i = 0; i < REPEAT; ++i) {
        double start = now();
        //exec_ikj();
        cache_blocking(3);
        double end = now();
        double sub_time = end - start;
        total_time += sub_time;
        clear_cache();
        if (info) printf("start-end: %lf [sec]\n", sub_time);
    }

    for (int j = 2; j < 10; ++j) {
        //cache_blocking(j);
        //clear_cache();
    }
    //cache_blocking(2);

    double average_time = total_time / (double)REPEAT;
    printf("average execute time: %lf [sec]\n", average_time);
    return 0;
}

