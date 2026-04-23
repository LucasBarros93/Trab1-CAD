## Benchmark script (5 runs per case)

The file `scripts/run_benchmarks_5x.sh` automates the benchmark execution for all input files named `caso*.txt`.

### What the script does

- removes old benchmark result files, if they exist;
- runs both executables, `./seq` and `./par`, for every `caso*.txt` input file;
- executes each case 5 times for the sequential version and 5 times for the parallel version;
- stores the raw execution times of each run;
- computes:
  - average sequential time,
  - average parallel time,
  - speedup,
  - efficiency;
- generates CSV files and a formatted text table.

### Generated files

#### 1. `resultados_brutos.csv`
Stores the raw execution times for every run.

Columns:
- `caso`: case number
- `arquivo`: input file name
- `T`: number of threads read from the input file
- `run`: repetition index (1 to 5)
- `seq_s`: sequential execution time in seconds
- `par_s`: parallel execution time in seconds

#### 2. `resumo_metricas.csv`
Stores the average times and performance metrics.

Columns:
- `caso`
- `arquivo`
- `T`
- `seq_medio_s`
- `par_medio_s`
- `speedup`
- `eficiencia`

#### 3. `resumo_metricas_formatado.txt`
Formatted version of `resumo_metricas.csv`, generated with `column`, easier to read in the terminal.

### How to compile

```bash
gcc studentsseq.c -O3 -o seq -lm -fopenmp
gcc studentspar.c -O3 -o par -lm -fopenmp
