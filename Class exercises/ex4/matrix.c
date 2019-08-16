#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define rows 4
#define cols 4

struct payload{
    int* value;
    int row;
    int column;
    int matrixA[4][4];
    int matrixB[4][4];
};

void* calcCell(void*);
void printMatrix(int mat[4][4]);
void freeArray(struct payload** p, int len);

int main(int argc, const char* argv[])
{
    int first[rows][cols] = {0};
    int second[rows][cols] = {0};
    int result[rows][cols] = {0};
    pthread_t threads[16] = {0};

    struct payload** arr = (struct payload**)malloc(rows*cols*sizeof(struct payload*));
    memset(arr, 0, rows*cols*sizeof(struct payload*));

    srand(time(0));

    int i = 0;
    int j = 0;

    for(i = 0; i < rows; ++i)
    {
        for(j = 0; j < cols; ++j)
        {
            first[i][j] = rand()%100;
            second[i][j] = rand()%100;
        }
    }


    printf("\n\n");
    printf("First Matrix\n");
    printMatrix(first);
    printf("\n\n");
    printf("Second Matrix\n");
    printMatrix(second);
    printf("\n\n");
    printf("\n\n");

    int c = 0;
    
    for(i = 0; i < rows; ++i)
    {
        for(j = 0; j < cols; ++j)
        {
            arr[c] = (struct payload*)malloc(1*sizeof(struct payload));
            arr[c]->value = &result[i][j];
            arr[c]->row = i;
            arr[c]->column = j;

            memcpy(arr[c]->matrixA,first,rows*cols*sizeof(int));
            memcpy(arr[c]->matrixB,second,rows*cols*sizeof(int));

            if(pthread_create(&threads[i*4+j],NULL,calcCell,arr[c++]))
                return -1;
        }
    }

    for(i = 0; i < 16; ++i)
    {
        pthread_join(threads[i],NULL);
    }

    printf("First x Second\n");
    printMatrix(result);
    freeArray(arr, rows*cols);
    free(arr);

    return 0;
}

void freeArray(struct payload** p, int len)
{   
    int i = 0;
    for(i = 0; i < len; ++i)
    {
        free(p[i]);
    }
}

void printMatrix(int mat[4][4])
{
    int i = 0;
    int j = 0;
    
    for(i = 0; i < rows; ++i)
    {
        for(j = 0; j < cols; ++j)
        {
            printf("%d\t",mat[i][j]);
        }
        printf("\n");
    }
}

void* calcCell(void* arg)
{
    int i = 0;
    int x = 0;
    int y = 0;
    int res = 0;

    struct payload* p = (struct payload*)arg;

    for( i = 0; i < rows; ++i)
    {
        *(p->value) = *(p->value) + (p->matrixA[p->row][i] * p->matrixB[i][p->column]);
    }   

    return NULL;
}