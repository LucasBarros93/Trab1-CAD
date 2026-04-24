// =====================================================
// Trabalho 01 - Computação de Alto Desempenho (SSC0903)
// =====================================================
//
// Integrantes do grupo:
// André Vargas Villalba Codorniz
// João Pedro Machado Medeiros
// Leonardo Hannas de Carvalho Santos
// Lucas Augusto Moreira Barros
// Lucas Kimo Costa
// Samuel Rubens Souza Oliveira
//
// -----------------------------------------------------
// Descrição:
// Implementação sequencial em C para processar notas de
// alunos organizados por região, cidade, aluno e nota.
//
// Estratégia geral:
// - Gerar os dados antes da medição de tempo.
// - Medir apenas a etapa computacional.
// - Calcular estatísticas em três níveis:
//   cidade, região e Brasil.
// - Usar acumuladores em double para reduzir diferenças
//   numéricas em somas longas.
//
// -----------------------------------------------------
// Compilação:
// gcc studentsseq.c -O3 -o seq -lm -fopenmp
//
// Execução (exemplos):
// ./seq caso21_r5_c50_a2000_n10_t1.txt
// ./seq caso36_r5_c100_a5000_n12_t8.txt
// ./seq caso40_r26_c10_a15000_n12_t8.txt
// =====================================================

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <omp.h>

#define MAX_NOTE 100
#define MIN_NOTE 0

/*
Estrutura usada para armazenar as estatísticas calculadas
em cada nível de agregação: cidade, região e Brasil.

Campos:
- average: média aritmética das médias dos alunos
- median: mediana das médias dos alunos
- std_deviation: desvio padrão populacional das médias dos alunos
- min: menor média individual de aluno
- max: maior média individual de aluno
*/
typedef struct {
    float average;
    float median;
    float std_deviation;

    float min;
    float max;

} Out;

// === FUNÇÕES AUXILIARES ===

/*
Calcula a mediana de um vetor já ordenado.
O vetor deve estar previamente ordenado em ordem crescente.
*/
float median(int size, float *arr) {
    if (size % 2 == 0) {
        return (arr[size / 2 - 1] + arr[size / 2]) / 2.0;
    } else {
        return arr[size / 2];
    }
}

/*
Calcula o desvio padrão populacional de um vetor.

O acumulador é double para reduzir erros de ponto flutuante
em somas longas. A função retorna float para manter compatibilidade
com a estrutura Out e com os demais arrays do programa.
*/
float std_deviation(int size, float average, float *arr) {
    double acc = 0.0;

    for (int i = 0; i < size; i++) {
        double diff = (double)arr[i] - (double)average;
        acc += diff * diff;
    }

    return (float)sqrt(acc / size);
}

/*
Gera pseudoaleatoriamente as notas individuais dos alunos.

Estrutura alocada:
data[região][cidade][aluno][nota]

As notas são geradas entre 0.0 e 100.0, com uma casa decimal,
a partir da seed definida no arquivo de entrada.
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
                        (float)((rand() % 1001) / 10.0);
                }
            }
        }
    }

    return data;
}

/*
Libera recursivamente arrays alocados dinamicamente em múltiplas dimensões.

Parâmetros:
- ptr: ponteiro para o array
- dim: número de dimensões restantes
- sizes: vetor com os tamanhos de cada dimensão
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
Comparador usado pelo qsort para ordenar valores float.

Não se deve retornar diretamente (fa - fb), pois a subtração é float
e a função deve retornar int. Diferenças pequenas, como 0.3 ou -0.8,
poderiam ser truncadas para 0, fazendo o qsort tratar valores distintos
como se fossem iguais.
*/
int compare(const void *a, const void *b) {
    float fa = *(const float *)a;
    float fb = *(const float *)b;
    if (fa < fb) return -1;
    if (fa > fb) return 1;
    return 0;
}

// === FUNÇÕES DE IMPRESSÃO ===

/*
Imprime todos os dados gerados.
Função usada apenas para depuração.
Não é usada na medição de desempenho.
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
Imprime a média calculada para cada aluno.
Função usada apenas para depuração.
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
Imprime a tabela de resultados por cidade.
Cada linha corresponde a uma cidade dentro de uma região.
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
Imprime a tabela de resultados por região.
Cada linha corresponde a uma região.
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
Imprime a tabela de resultados agregados para o Brasil.
*/
void print_out_country(Out out_country) {
    printf("%-15s %-10s %-10s %-10s %-10s %-10s\n", "Brasil", "Min Nota", "Max Nota", "Mediana", "Média", "DsvPdr");
    printf("%-15s %10.1f %10.1f %10.1f %10.1f %10.1f\n", 
           "", out_country.min, out_country.max, 
           out_country.median, out_country.average, out_country.std_deviation);
    printf("\n");
}

/*
Imprime a tabela de premiação:
- região com maior média aritmética
- cidade com maior média aritmética
*/
void print_awards(int best_region, float max_region_avg, int best_city_r, int best_city_c, float max_city_avg) {
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
    
    // Validação do argumento de entrada: o programa espera um arquivo .txt
    if (argc < 2) {
        printf("Please execute with a text file name: input.txt\n");
        exit(-1);
    }

    FILE *inputfile;
    char *inputfilename = (char *)malloc(256*sizeof(char));
    strcpy(inputfilename, argv[1]);

    // Abertura do arquivo de entrada
    if ((inputfile=fopen(inputfilename,"r")) == 0) {
        printf("Problems opening the file\n");
        exit(-1);
    }

    /*
    Leitura dos parâmetros do problema:
    R    = número de regiões
    C    = número de cidades por região
    A    = número de alunos por cidade
    N    = número de notas por aluno
    T    = número de threads informado no arquivo
    seed = semente pseudoaleatória
    */
    int R, C, A, N, T, seed;
    if (fscanf(inputfile, "%d %d %d %d %d %d", &R, &C, &A, &N, &T, &seed) != 6) {
        printf("Error reading the input file\n");
        fclose(inputfile);
        exit(-1);
    }

    // Fechamento do arquivo após a leitura dos parâmetros
    fclose(inputfile);

    // Inicialização do gerador pseudoaleatório
    srand(seed);

    /*
    Geração dos dados de entrada.
    Esta etapa não faz parte do tempo medido, conforme solicitado no enunciado.
    */
    float ****data = random_data_gen(R, C, A, N);

    /*
    Estruturas que armazenam as estatísticas finais:
    - out_city: estatísticas de cada cidade
    - out_region: estatísticas de cada região
    - out_country: estatísticas agregadas do Brasil
    */
    Out **out_city = (Out **)calloc(R, sizeof(Out *));
    Out *out_region = (Out *)calloc(R, sizeof(Out));
    Out out_country;
    out_country.average = 0;

    /*
    Arrays auxiliares:
    - average_to_city: médias individuais dos alunos por cidade
    - average_to_region: médias dos alunos agregadas por região
    - average_to_country: médias dos alunos agregadas para o Brasil
    */
    float ***average_to_city = (float ***)calloc(R, sizeof(float **));
    float **average_to_region = (float **)calloc(R, sizeof(float *));
    float *average_to_country = (float *)calloc(R * C * A, sizeof(float));

    // Alocação das estruturas auxiliares por região e cidade
    for (int region = 0; region < R; region++) {
        average_to_region[region] = calloc(C * A, sizeof(float));
        average_to_city[region] = calloc(C, sizeof(float *));
        out_city[region] = calloc(C, sizeof(Out));

        for (int city = 0; city < C; city++) {
            average_to_city[region][city] = calloc(A, sizeof(float));
        }
    }

    /*
    Início da medição de tempo.
    A medição considera apenas a etapa computacional principal.
    */
    double start = omp_get_wtime();

    /*
    Processamento sequencial.
    A ordem de cálculo segue a hierarquia:
    aluno -> cidade -> região -> Brasil.
    */
    for (int region = 0; region < R; region++) {

        for (int city = 0; city < C; city++) {

            /*
            Soma das médias individuais dos alunos da cidade.
            O acumulador em double reduz erros numéricos.
            */
            double soma_city = 0.0;

            for (int student = 0; student < A; student++) {
                
                /*
                Cálculo da média individual do aluno a partir de suas N notas.
                */
                double soma = 0.0;

                for (int grade = 0; grade < N; grade++) {
                    soma += data[region][city][student][grade];
                }

                average_to_city[region][city][student] = (float)(soma / N);
                soma_city += average_to_city[region][city][student];
            }

            /*
            NÍVEL CIDADE:
            Calcula estatísticas sobre as médias dos alunos da cidade.
            */
            out_city[region][city].average = (float)(soma_city / A);

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

        /*
        NÍVEL REGIÃO:
        Agrega as médias dos alunos de todas as cidades da região.
        */
        double soma_region = 0.0;

        for (int i = 0; i < C * A; i++)
            soma_region += average_to_region[region][i];

        out_region[region].average = (float)(soma_region / (C * A));

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
        
        /*
        Acúmulo auxiliar mantido por compatibilidade com a estrutura original.
        O valor final de out_country.average é recalculado corretamente abaixo
        usando soma_country.
        */
        out_country.average += out_region[region].average;
    }

    /*
    NÍVEL BRASIL:
    Agrega as médias individuais de todos os alunos do país.
    */
    double soma_country = 0.0;

    for (int i = 0; i < R * C * A; i++)
        soma_country += average_to_country[i];

    out_country.average = (float)(soma_country / (R * C * A));

    qsort(average_to_country, R * C * A, sizeof(float), compare);

    out_country.median =
        median(R * C * A, average_to_country);

    out_country.std_deviation =
        std_deviation(R * C * A,
                      out_country.average,
                      average_to_country);

    out_country.min = average_to_country[0];
    out_country.max = average_to_country[R * C * A - 1];

    /*
    PREMIAÇÃO:
    Busca sequencial pela região com maior média aritmética.
    */
    int best_region = 0;
    float max_region_avg = -1.0;
    
    for (int r = 0; r < R; r++) {
        if (out_region[r].average > max_region_avg) {
            max_region_avg = out_region[r].average;
            best_region = r;
        }
    }

    /*
    Busca sequencial pela cidade com maior média aritmética.
    */
    int best_city_r = 0, best_city_c = 0;
    float max_city_avg = -1.0;

    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            if (out_city[r][c].average > max_city_avg) {
                max_city_avg = out_city[r][c].average;
                best_city_r = r;
                best_city_c = c;
            }
        }
    }

    /*
    Fim da medição de tempo.
    O tempo calculado corresponde apenas à etapa computacional principal.
    */
    double end = omp_get_wtime();
    double time = end - start;

    // Impressão dos resultados finais
    print_out_city(out_city, R, C);
    print_out_region(out_region, R);
    print_out_country(out_country);
    print_awards(best_region, max_region_avg, best_city_r, best_city_c, max_city_avg);
    printf("Tempo de resposta em segundos, sem considerar E/S: %.3f s\n", time);
    
    // DEBUG PRINT
    // print_average_student(average_to_city, R, C, A);

    // Liberação de memória alocada dinamicamente
    free_nd(data, 4, (int[]){R, C, A, N});
    free_nd(out_city, 2, (int[]){R, C});
    free_nd(out_region, 1, (int[]){R});
    free_nd(average_to_city, 3, (int[]){R, C, A});
    free_nd(average_to_region, 2, (int[]){R, C * A});
    free_nd(average_to_country, 1, (int[]){R * C * A});

    return 0;
}