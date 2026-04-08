#include <stdio.h>
#include <stdlib.h>

#define MAX_NOTE 100
#define MIN_NOTE 0

int main() {
    // opening the file
    const char *filename = "test.txt";
    FILE *file = fopen(filename, "r");

    // scaning variables
    int R, C, A, N, T, seed;
    fscanf(file, "%d %d %d %d %d %d", &R, &C, &A, &N, &T, &seed);

    srand(seed);

    // random data generator
    float **data = (float **)calloc(R * C * A, sizeof(float *));
    for (int i = 0; i < R * C * A; i++) {
        data[i] = (float *)calloc(N, sizeof(float));
        for (int j = 0; j < N; j++) {
            data[i][j] =
                (float)(rand() / (RAND_MAX + 1.0 + MIN_NOTE) * MAX_NOTE);
        }
    }

    // average notes for every student
    float *average_student = (float *)calloc(R * C * A, sizeof(float));
    for (int i = 0; i < R * C * A; i++) {
        float average = 0;
        for (int j = 0; j < N; j++) {
            average += data[i][j];
        }
        average_student[i] = average / N;
    }

    // prints
    for (int i = 0; i < R * C * A; i++) {
        for (int j = 0; j < N; j++) {
            printf("%.1f ", data[i][j]);
        }
        printf("\n");
    }

    printf("\n");
    printf("\n");

    for (int i = 0; i < R * C * A; i++) {
        printf("%.1f ", average_student[i]);
    }

    return 0;
}
