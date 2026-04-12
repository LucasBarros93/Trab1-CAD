#include <stdio.h>
#include <stdlib.h>

#define MAX_NOTE 100
#define MIN_NOTE 0

typedef struct {
    int city;
    int region;

    float average;
    float median;
    float deviation;

    float lower;
    float grater;

} Out;

float ****random_data_gen(int R, int C, int A, int N) {

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

    return data;
}

void print_data(float ****data, int R, int C, int A, int N) {
    for (int region = 0; region < R; region++) {
        printf("Region: %d\n", region);
        for (int city = 0; city < C; city++) {
            printf("City: %d\n", city);
            for (int student = 0; student < A; student++) {
                printf("Student %d:  ", student);
                for (int grade = 0; grade < N; grade++) {
                    printf("%3.1f ", data[region][city][student][grade]);
                }
                printf("\n");
            }
            printf("\n");
        }
        printf("\n");
    }
}

// to qsort
int compare(const void *a, const void *b) { return (*(int *)a - *(int *)b); }

int main() {
    // opening the file
    const char *filename = "test.txt";
    FILE *file = fopen(filename, "r");

    // scaning variables
    int R, C, A, N, T, seed;
    fscanf(file, "%d %d %d %d %d %d", &R, &C, &A, &N, &T, &seed);

    srand(seed);

    float ****data = random_data_gen(R, C, A, N);
    print_data(data, R, C, A, N);

    // average notes for every student
    float ***average_student = (float ***)calloc(R * C * A, sizeof(float **));
    Out **out_city = (Out **)calloc(R * C * A, sizeof(Out *));
    for (int region = 0; region < R; region++) {
        average_student[region] = (float **)calloc(C, sizeof(float *));
        out_city[region] = (Out *)calloc(C, sizeof(Out));

        for (int city = 0; city < C; city++) {
            average_student[region][city] = (float *)calloc(C, sizeof(float));

            out_city[region][city].region = region;
            out_city[region][city].city = city;

            for (int student = 0; student < A; student++) {
                qsort(data[region][city][student], N, sizeof(float), compare);

                for (int grade = 0; grade < N; grade++) {
                    average_student[region][city][student] +=
                        data[region][city][student][grade];
                }
                average_student[region][city][student] /= N;
                out_city[region][city].average +=
                    average_student[region][city][student];
            }

            out_city[region][city].average /= A;
            out_city[region][city].median =
                average_student[region][city][((int)A / 2)];
        }
    }

    return 0;
}
