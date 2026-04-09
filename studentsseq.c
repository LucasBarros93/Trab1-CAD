#include <stdio.h>
#include <stdlib.h>

#define MAX_NOTE 100
#define MIN_NOTE 0

typedef struct {
    int student;
    int city;
    int region;
    int average;
} Student;

int main() {
    // opening the file
    const char *filename = "test.txt";
    FILE *file = fopen(filename, "r");

    // scaning variables
    int R, C, A, N, T, seed;
    fscanf(file, "%d %d %d %d %d %d", &R, &C, &A, &N, &T, &seed);

    srand(seed);

    // random data generator
    float ****data = (float ****)calloc(R, sizeof(float ***));
    for (int region = 0; region < R; region++) {
        data[region] = (float ***)calloc(C, sizeof(float **));

        for (int city = 0; city < C; city++) {
            data[region][city] = (float **)calloc(A, sizeof(float *));

            for (int student = 0; student < A; student++) {
                data[region][city][student] = (float *)calloc(N, sizeof(float));

                for (int grade = 0; grade < N; grade++) {
                    data[region][city][student][grade] =
                        (float)(rand() / (RAND_MAX + 1.0 + MIN_NOTE) *
                                MAX_NOTE);
                }
            }
        }
    }

    // average notes for every student
    float ***average_student = (float ***)calloc(R * C * A, sizeof(float **));
    for (int region = 0; region < R; region++) {
        average_student[region] = (float **)calloc(C, sizeof(float *));

        for (int city = 0; city < C; city++) {
            average_student[region][city] = (float *)calloc(C, sizeof(float));

            for (int student = 0; student < A; student++) {

                for (int grade = 0; grade < N; grade++) {
                    average_student[region][city][student] +=
                        data[region][city][student][grade];
                }
                average_student[region][city][student] /= N;
            }
        }
    }

    // prints
    for (int region = 0; region < R; region++) {
        printf("Region: %d\n", region);
        for (int city = 0; city < C; city++) {
            printf("City: %d\n", city);
            for (int student = 0; student < A; student++) {
                printf("Student %d:  ", student);
                for (int grade = 0; grade < N; grade++) {
                    printf("%3.1f ", data[region][city][student][grade]);
                }
                printf(" AG: %3.2f", average_student[region][city][student]);
                printf("\n");
            }
            printf("\n");
        }
        printf("\n");
    }

    return 0;
}
