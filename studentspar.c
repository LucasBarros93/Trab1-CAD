// compile: gcc studentspar.c -o studentspar -fopenmp
// run: studentspar Trab01-AvalEstudantes-ExemploArqEntrada0-v2.txt

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

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

// para o qsort
int compare(const void *a, const void *b) {
    return (*(float *)a - *(float *)b);
}

int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Por favor execute com um nome de arquivo texto, i.e., arquivo_texto.txt\n");
        exit(-1);
    }

    FILE *inputfile;
    char *inputfilename = (char *)malloc(256*sizeof(char));
    strcpy(inputfilename, argv[1]);

    if ((inputfile=fopen(inputfilename,"r")) == 0) {
        printf("Erro ao abrir o arquivo.\n");
        exit(-1);
    }

    int R, C, A, N, T, seed;
    fscanf(inputfile, "%d %d %d %d %d %d", &R, &C, &A, &N, &T, &seed);

    fclose(inputfile);

    srand(seed);

    float ****data = random_data_gen(R, C, A, N);

    Out **out_city = (Out **)calloc(R, sizeof(Out *));
    Out *out_region = (Out *)calloc(R, sizeof(Out));
    Out out_country;

    float ***average_to_city = (float ***)calloc(R, sizeof(float **));
    float **average_to_region = (float **)calloc(R, sizeof(float *));
    float *average_to_country = (float *)calloc(R * C * A, sizeof(float));

    for (int region = 0; region < R; region++) {
        out_city[region] = (Out *)calloc(C, sizeof(Out));
        average_to_city[region] = (float **)calloc(C, sizeof(float *));
        average_to_region[region] = (float *)calloc(C * A, sizeof(float));

        for (int city = 0; city < C; city++) {
            average_to_city[region][city] = (float *)calloc(A, sizeof(float));
        }
    }

    // medição do tempo inicial
    double start = omp_get_wtime();

    // cálculo da média de das notas de cada aluno 
    #pragma omp parallel for collapse(3) num_threads(T)
    for (int region = 0; region < R; region++) {
        for (int city = 0; city < C; city++) {
            for (int student = 0; student < A; student++) {

                float soma = 0.0;

                for (int grade = 0; grade < N; grade++)
                    soma += data[region][city][student][grade];

                average_to_city[region][city][student] = soma / N;
            }
        }
    }

    // cálculo da nota média do aluno, mediana, min e max notas e desvio padrão por cidade para cada região
    #pragma omp parallel for collapse(2) num_threads(T)
    for (int region = 0; region < R; region++) {
        for (int city = 0; city < C; city++) {

            float soma = 0.0;

            for (int student = 0; student < A; student++)
                soma += average_to_city[region][city][student];

            out_city[region][city].average = soma / A;

            qsort(average_to_city[region][city], A, sizeof(float), compare);

            out_city[region][city].median = median(A, average_to_city[region][city]);
            out_city[region][city].std_deviation =
                std_deviation(A, out_city[region][city].average, average_to_city[region][city]);

            out_city[region][city].min = average_to_city[region][city][0];
            out_city[region][city].max = average_to_city[region][city][A - 1];

            memcpy(average_to_region[region] + city * A,
                   average_to_city[region][city],
                   A * sizeof(float));
        }
    }

    // cálculo da nota média do aluno, mediana, min e man notas e desvio padrão por região
    #pragma omp parallel for num_threads(T)
    for (int region = 0; region < R; region++) {

        float soma = 0.0;

        for (int i = 0; i < C * A; i++)
            soma += average_to_region[region][i];

        out_region[region].average = soma / (C * A);

        qsort(average_to_region[region], C * A, sizeof(float), compare);

        out_region[region].median = median(C * A, average_to_region[region]);
        out_region[region].std_deviation =
            std_deviation(C * A, out_region[region].average, average_to_region[region]);

        out_region[region].min = average_to_region[region][0];
        out_region[region].max = average_to_region[region][C * A - 1];

        memcpy(average_to_country + region * C * A,
               average_to_region[region],
               C * A * sizeof(float));
    }

    // cálculo da nota média do aluno, mediana, min e man notas e desvio padrão do Brasil
    float soma = 0.0;

    #pragma omp parallel for num_threads(T) reduction(+:soma) 
    for (int i = 0; i < R * C * A; i++)
        soma += average_to_country[i];

    out_country.average = soma / (R * C * A);

    qsort(average_to_country, R * C * A, sizeof(float), compare);

    out_country.median = median(R * C * A, average_to_country);
    out_country.std_deviation =
        std_deviation(R * C * A, out_country.average, average_to_country);

    out_country.min = average_to_country[0];
    out_country.max = average_to_country[R * C * A - 1];

    double end = omp_get_wtime();
    double time = end - start; // cálculo do tempo de execução total

    print_average_student(average_to_city, R, C, A);
    print_out_city(out_city, R, C);
    print_out_region(out_region, R);
    print_out_country(out_country);

    // printf("Tempo: %.6f s\n", time);

    return 0;
}