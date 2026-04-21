// to compile: gcc arrumando_print.c -o print -fopenmp -lm
// to run: ./print test.txt


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MAX_NOTE 100
#define MIN_NOTE 0

/*
Struct to all the outputs and their datas
They are City, Region, Country
*/
typedef struct {
    float average;
    float median;
    float std_deviation;

    float min;
    float max;

} Out;

// === AUXILIAR FUNCTIONS ===

/*
Function to calculate the median of an array
Needs the size of the array
*/
float median(int size, float *arr) {
    if (size % 2 == 0) {
        return (arr[size / 2 - 1] + arr[size / 2]) / 2.0;
    } else {
        return arr[size / 2];
    }
}

/*
Function to calculate the standard deviation of an array
Needs the size and the average of the array
*/
float std_deviation(int size, float average, float *arr) {
    float standardDeviation = 0;
    for (int i = 0; i < size; i++) {
        standardDeviation += pow(arr[i] - average, 2);
    }

    standardDeviation = sqrt(standardDeviation / size);
    return standardDeviation;
}

/*
To generate the data to use
*/
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

/*
Free n dimentional array
*/
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

/*
To use qsort
*/
int compare(const void *a, const void *b) {
    return (*(float *)a - *(float *)b);
}

// === OUTPUT PRINTS ===

/*
Print the initial data
Just debug
*/
void print_data(float ****data, int R, int C, int A, int N) {
    for (int region = 0; region < R; region++) {
        printf("Regiao: %d\n", region);
        for (int city = 0; city < C; city++) {
            printf("Cidade: %d\n", city);
            for (int student = 0; student < A; student++) {
                printf("Aluno %d:  ", student);
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

/*
Print the student data
Just debug
*/
void print_average_student(float ***average_student, int R, int C, int A) {
    for (int region = 0; region < R; region++) {
        printf("Regiao: %d\n", region);
        for (int city = 0; city < C; city++) {
            printf("Cidade: %d\n", city);
            for (int student = 0; student < A; student++) {
                printf("Aluno %d:  ", student);
                printf("%3.1f \n", average_student[region][city][student]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

/*
Print the output to a city formatted as a table
*/
void print_out_city(Out **out_city, int R, int C) {
    printf("%-15s %-10s %-10s %-10s %-10s %-10s\n", "Cidades", "Min Nota", "Max Nota", "Mediana", "Média", "DsvPdr");
    for (int region = 0; region < R; region++) {
        for (int city = 0; city < C; city++) {
            char label[50];
            sprintf(label, "R=%d, C=%d", region, city);
            printf("%-15s %10.1f %10.1f %10.1f %10.1f %10.1f\n", 
                   label, out_city[region][city].min, out_city[region][city].max, 
                   out_city[region][city].median, out_city[region][city].average, out_city[region][city].std_deviation);
        }
    }
    printf("\n");
}

/*
Print the output to a region formatted as a table
*/
void print_out_region(Out *out_region, int R) {
    printf("%-15s %-10s %-10s %-10s %-10s %-10s\n", "Regiões", "Min Nota", "Max Nota", "Mediana", "Média", "DsvPdr");
    for (int region = 0; region < R; region++) {
        char label[50];
        sprintf(label, "R=%d", region);
        printf("%-15s %10.1f %10.1f %10.1f %10.1f %10.1f\n", 
               label, out_region[region].min, out_region[region].max, 
               out_region[region].median, out_region[region].average, out_region[region].std_deviation);
    }
    printf("\n");
}

/*
Print the output to a country formatted as a table
*/
void print_out_country(Out out_country) {
    printf("%-15s %-10s %-10s %-10s %-10s %-10s\n", "Brasil", "Min Nota", "Max Nota", "Mediana", "Média", "DsvPdr");
    printf("%-15s %10.1f %10.1f %10.1f %10.1f %10.1f\n", 
           "", out_country.min, out_country.max, 
           out_country.median, out_country.average, out_country.std_deviation);
    printf("\n");
}

/*
Calculate and print the Awards (Premiação) table
*/
void print_awards(Out *out_region, Out **out_city, int R, int C) {
    int best_region = 0;
    float max_region_avg = -1.0;
    
    // Encontrar a região com a maior média
    for (int r = 0; r < R; r++) {
        if (out_region[r].average > max_region_avg) {
            max_region_avg = out_region[r].average;
            best_region = r;
        }
    }

    int best_city_r = 0, best_city_c = 0;
    float max_city_avg = -1.0;
    
    // Encontrar a cidade com a maior média
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            if (out_city[r][c].average > max_city_avg) {
                max_city_avg = out_city[r][c].average;
                best_city_r = r;
                best_city_c = c;
            }
        }
    }

    printf("%-15s %-10s %-10s\n", "Premiação", "Reg/Cid", "Media Arit");
    
    char label_reg[50];
    sprintf(label_reg, "R%d", best_region);
    printf("%-15s %-10s %10.1f\n", "Melhor região:", label_reg, max_region_avg);
    
    char label_cit[50];
    sprintf(label_cit, "R%d-C%d", best_city_r, best_city_c);
    printf("%-15s %-10s %10.1f\n", "Melhor cidade:", label_cit, max_city_avg);
    printf("\n");
}

int main(int argc, char **argv) {
    
    // dealing with the input file and opening it
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

    // scaning variables
    int R, C, A, N, T, seed;
    fscanf(inputfile, "%d %d %d %d %d %d", &R, &C, &A, &N, &T, &seed);

    // closing the file
    fclose(inputfile);

    // setting the seed for the random generator
    srand(seed);

    // generating the data to use (student grades)
    float ****data = random_data_gen(R, C, A, N);
    // print_data(data, R, C, A, N);

    // The outputs arrays
    Out **out_city = (Out **)calloc(R, sizeof(Out *));
    Out *out_region = (Out *)calloc(R, sizeof(Out));
    Out out_country;
    out_country.average = 0;

    // The auxiliar arrays
    float ***average_to_city = (float ***)calloc(R, sizeof(float **));
    float **average_to_region = (float **)calloc(R, sizeof(float *));
    float *average_to_country = (float *)calloc(R * C * A, sizeof(float));

    // more allocation
    for (int region = 0; region < R; region++) {
        average_to_region[region] = calloc(C * A, sizeof(float));
        average_to_city[region] = calloc(C, sizeof(float *));
        out_city[region] = calloc(C, sizeof(Out));

        for (int city = 0; city < C; city++) {
            average_to_city[region][city] = calloc(A, sizeof(float));
        }
    }

    // time measurement and beginning of the calculations
    double start = omp_get_wtime();

    for (int region = 0; region < R; region++) {

        for (int city = 0; city < C; city++) {

            float soma_city = 0.0;

            for (int student = 0; student < A; student++) {

                float soma = 0.0;

                // STUDENT CALCULATION
                for (int grade = 0; grade < N; grade++) {
                    soma += data[region][city][student][grade];
                }

                average_to_city[region][city][student] = soma / N;
                soma_city += average_to_city[region][city][student];
            }

            out_city[region][city].average = soma_city / A;

            // CITY CALCULATION
            qsort(average_to_city[region][city], A, sizeof(float), compare);
            memcpy(average_to_region[region] + city * A,
                   average_to_city[region][city],
                   A * sizeof(float));

            out_city[region][city].median =
                median(A, average_to_city[region][city]);

            out_city[region][city].std_deviation =
                std_deviation(A,
                              out_city[region][city].average,
                              average_to_city[region][city]);

            out_city[region][city].min = average_to_city[region][city][0];
            out_city[region][city].max = average_to_city[region][city][A - 1];
        }

        float soma_region = 0.0;

        for (int i = 0; i < C * A; i++)
            soma_region += average_to_region[region][i];

        // REGION CALCULATION
        out_region[region].average = soma_region / (C * A);

        qsort(average_to_region[region], C * A, sizeof(float), compare);

        memcpy(average_to_country + region * C * A,
               average_to_region[region],
               C * A * sizeof(float));

        out_region[region].median =
            median(C * A, average_to_region[region]);

        out_region[region].std_deviation =
            std_deviation(C * A,
                          out_region[region].average,
                          average_to_region[region]);

        out_region[region].min = average_to_region[region][0];
        out_region[region].max = average_to_region[region][C * A - 1];
        
        // COUNTRY CALCULATION
        out_country.average += out_region[region].average;
    }

    float soma_country = 0.0;

    for (int i = 0; i < R * C * A; i++)
        soma_country += average_to_country[i];

    out_country.average = soma_country / (R * C * A);

    qsort(average_to_country, R * C * A, sizeof(float), compare);

    out_country.median =
        median(R * C * A, average_to_country);

    out_country.std_deviation =
        std_deviation(R * C * A,
                      out_country.average,
                      average_to_country);

    out_country.min = average_to_country[0];
    out_country.max = average_to_country[R * C * A - 1];

    double time = omp_get_wtime() - start;

    // PRINTS
    print_out_city(out_city, R, C);
    print_out_region(out_region, R);
    print_out_country(out_country);
    print_awards(out_region, out_city, R, C);
    printf("Tempo de resposta em segundos, sem considerar E/S: %.6f s\n", time);
    
    // DEBUG PRINT
    // print_average_student(average_to_city, R, C, A);

    // Frees
    free_nd(data, 4, (int[]){R, C, A, N});
    free_nd(out_city, 2, (int[]){R, C});
    free_nd(out_region, 1, (int[]){R});
    free_nd(average_to_city, 3, (int[]){R, C, A});
    free_nd(average_to_region, 2, (int[]){R, C * A});
    free_nd(average_to_country, 1, (int[]){R * C * A});

    return 0;
}