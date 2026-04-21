#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOTE 100
#define MIN_NOTE 0

typedef struct {
    float average;
    float median;
    float std_deviation;

    float min;
    float max;

} Out;

float median(int size, float *arr) {
    if (size % 2 == 0) {
        return (arr[size / 2 - 1] + arr[size / 2]) / 2.0;
    } else {
        return arr[size / 2];
    }
}

float std_deviation(int size, float average, float *arr) {
    float standardDeviation = 0;
    for (int i = 0; i < size; i++) {
        standardDeviation += pow(arr[i] - average, 2);
    }

    standardDeviation = sqrt(standardDeviation / size);
    return standardDeviation;
}

float ****random_data_gen(int R, int C, int A, int N) {

    float ****data = (float ****)malloc(R * sizeof(float ***));
    for (int region = 0; region < R; region++) {
        data[region] = (float ***)malloc(C * sizeof(float **));

        for (int city = 0; city < C; city++) {
            data[region][city] = (float **)malloc(A * sizeof(float *));

            for (int student = 0; student < A; student++) {
                data[region][city][student] =
                    (float *)malloc(N * sizeof(float));

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

void free_nd(void *ptr, int dim, int *sizes) {
    if (ptr == NULL)
        return;

    if (dim == 1) {
        free(ptr);
        return;
    }

    void **p = (void **)ptr;
    for (int i = 0; i < sizes[0]; i++) {
        free_nd(p[i], dim - 1, sizes + 1);
    }
    free(ptr);
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

void print_average_student(float ***average_student, int R, int C, int A) {
    for (int region = 0; region < R; region++) {
        printf("Region: %d\n", region);
        for (int city = 0; city < C; city++) {
            printf("City: %d\n", city);
            for (int student = 0; student < A; student++) {
                printf("Student %d:  ", student);
                printf("%3.1f \n", average_student[region][city][student]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

void print_out_city(Out **out_city, int R, int C) {
    for (int region = 0; region < R; region++) {
        printf("Region: %d\n", region);
        for (int city = 0; city < C; city++) {
            printf("City: %d ", city);
            printf("Min: %3.1f ", out_city[region][city].min);
            printf("Max: %3.1f ", out_city[region][city].max);
            printf("Avg: %3.1f ", out_city[region][city].average);
            printf("Med: %3.1f ", out_city[region][city].median);
            printf("Dsv: %3.1f ", out_city[region][city].std_deviation);
            printf("\n");
        }
        printf("\n");
    }
}

void print_out_region(Out *out_region, int R) {
    for (int region = 0; region < R; region++) {
        printf("Region: %d ", region);
        printf("Min: %3.1f ", out_region[region].min);
        printf("Max: %3.1f ", out_region[region].max);
        printf("Avg: %3.1f ", out_region[region].average);
        printf("Med: %3.1f ", out_region[region].median);
        printf("Dsv: %3.1f ", out_region[region].std_deviation);
        printf("\n");
    }
    printf("\n");
}

void print_out_country(Out out_country) {
    printf("Brasil: ");
    printf("Min: %3.1f ", out_country.min);
    printf("Max: %3.1f ", out_country.max);
    printf("Avg: %3.1f ", out_country.average);
    printf("Med: %3.1f ", out_country.median);
    printf("Dsv: %3.1f ", out_country.std_deviation);
    printf("\n");
}

// to qsort
int compare(const void *a, const void *b) {
    return (*(float *)a - *(float *)b);
}

int main() {
    // opening the file
    const char *filename = "test.txt";
    FILE *file = fopen(filename, "r");

    // scaning variables
    int R, C, A, N, T, seed;
    fscanf(file, "%d %d %d %d %d %d", &R, &C, &A, &N, &T, &seed);

    fclose(file);

    srand(seed);

    float ****data = random_data_gen(R, C, A, N);
    // print_data(data, R, C, A, N);

    Out **out_city = (Out **)calloc(R, sizeof(Out *));
    Out *out_region = (Out *)calloc(R, sizeof(Out));
    Out out_country;
    out_country.average = 0;

    float ***average_to_city = (float ***)calloc(R, sizeof(float **));
    float **average_to_region = (float **)calloc(R, sizeof(float *));
    float *average_to_country = (float *)calloc(R * C * A, sizeof(float));

    for (int region = 0; region < R; region++) {
        average_to_region[region] = (float *)calloc(C * A, sizeof(float));
        average_to_city[region] = (float **)calloc(C, sizeof(float *));

        out_city[region] = (Out *)calloc(C, sizeof(Out));

        for (int city = 0; city < C; city++) {
            average_to_city[region][city] = (float *)calloc(A, sizeof(float));

            for (int student = 0; student < A; student++) {

                for (int grade = 0; grade < N; grade++) {
                    average_to_city[region][city][student] +=
                        data[region][city][student][grade];
                }

                average_to_city[region][city][student] /= N;

                out_city[region][city].average +=
                    average_to_city[region][city][student];
            }

            qsort(average_to_city[region][city], A, sizeof(float), compare);
            memcpy(average_to_region[region] + A * city,
                   average_to_city[region][city], A * sizeof(float));

            out_city[region][city].average /= A;

            out_city[region][city].median =
                median(A, average_to_city[region][city]);
            out_city[region][city].std_deviation =
                std_deviation(A, out_city[region][city].average,
                              average_to_city[region][city]);

            out_city[region][city].min = average_to_city[region][city][0];
            out_city[region][city].max = average_to_city[region][city][A - 1];

            // REGION
            out_region[region].average += out_city[region][city].average;
        }

        qsort(average_to_region[region], A * C, sizeof(float), compare);
        memcpy(average_to_country + A * C * region, average_to_region[region],
               A * C * sizeof(float));

        out_region[region].average /= C;

        out_region[region].median = median(A * C, average_to_region[region]);
        out_region[region].std_deviation = std_deviation(
            A * C, out_region[region].average, average_to_region[region]);

        out_region[region].min = average_to_region[region][0];
        out_region[region].max = average_to_region[region][A * C - 1];

        // COUNTRY
        out_country.average += out_region[region].average;
    }

    qsort(average_to_country, A * C * R, sizeof(float), compare);

    out_country.average /= R;

    out_country.median = median(A * C * R, average_to_country);
    out_country.std_deviation =
        std_deviation(A * C * R, out_country.average, average_to_country);

    out_country.min = average_to_country[0];
    out_country.max = average_to_country[A * C * R - 1];

    print_average_student(average_to_city, R, C, A);
    print_out_city(out_city, R, C);
    print_out_region(out_region, R);
    print_out_country(out_country);

    free_nd(data, 4, (int[]){R, C, A, N});
    free_nd(out_city, 2, (int[]){R, C});
    free_nd(out_region, 1, (int[]){R});
    free_nd(average_to_city, 3, (int[]){R, C, A});
    free_nd(average_to_region, 2, (int[]){R, C * A});
    free_nd(average_to_country, 1, (int[]){R * C * A});

    return 0;
}
