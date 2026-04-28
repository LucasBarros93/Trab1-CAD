# 🚀 Trab1-CAD — High Performance Computing (OpenMP)

> Performance analysis of sequential vs parallel implementations using OpenMP  
> Universidade de São Paulo — SSC0903 (Computação de Alto Desempenho)

---

## 📌 Overview

This project evaluates the performance of a **parallel implementation (OpenMP)** against a **sequential version** for processing large-scale educational data.

The problem simulates student evaluations across:

- Regions (R)
- Cities (C)
- Students (A)
- Grades per student (N)

The goal is to analyze:

- ⚡ Speedup
- 📈 Efficiency
- ⏱ Execution time scalability

---

## 👥 Team

- André Vargas Villalba Codorniz  
- João Pedro Machado Medeiros  
- Leonardo Hannas de Carvalho Santos  
- Lucas Augusto Moreira Barros  
- Lucas Kimo Costa  
- Samuel Rubens Souza Oliveira  

---

## 🧠 Key Concepts

- Parallel computing with **OpenMP**
- Loop parallelization (`#pragma omp for`, `collapse`)
- Reduction operations
- Synchronization (`critical`, `single`)
- Performance metrics:
  - Speedup
  - Efficiency

---

## 📂 Project Structure

```bash
.
├── studentsseq.c        # Sequential implementation
├── studentspar.c        # Parallel implementation (OpenMP)
├── scripts/
│   └── run_benchmarks_5x.sh
├── spec/
│   └── report.pdf       # Full performance analysis
├── test.txt             # Editable test input
├── README.md
```

---

## ⚙️ Compilation

Both implementations are compiled with aggressive optimization:

```bash
gcc studentsseq.c -O3 -o seq -lm -fopenmp
gcc studentspar.c -O3 -o par -lm -fopenmp
```

### Why `-O3`?

- Loop optimizations
- Better cache usage
- Possible auto-vectorization
- Fair comparison between sequential and parallel versions

---

## ▶️ Execution

Run with any input file:

```bash
./seq test.txt
./par test.txt
```

---

## 🧪 Input Format

Each test file follows:

```txt
R C A N T seed
```

Where:

- `R` → number of regions  
- `C` → cities per region  
- `A` → students per city  
- `N` → grades per student  
- `T` → number of threads  
- `seed` → random seed  

### Example

```txt
5 50 2000 10 4 7
```

---

## 🧪 Creating Custom Test Cases

You can easily create new experiments by editing:

```bash
nano test.txt
```

Then run:

```bash
./seq test.txt
./par test.txt
```

---

## 📊 Benchmarking

Run automated experiments:

```bash
./scripts/run_benchmarks_5x.sh
```

This script:

- Executes each case **5 times**
- Computes:
  - Mean execution time
  - Speedup
  - Efficiency
- Generates:
  - `resultados_brutos.csv`
  - `resumo_metricas.csv`

---

## 📈 Performance Results

Full analysis available in:

```bash
spec/report.pdf
```

### Highlights

- ✔ Significant speedup for large workloads  
- ✔ Stable results for high computational loads  
- ✔ Expected efficiency drop with increasing threads  
- ✔ Near-linear gains up to moderate parallelism  

---

## 🧪 Test Campaign

Three test batteries were designed:

### 🟢 Cases 1–20 (Exploratory)

- Small workloads  
- Execution times ≈ 0  
- Parallel overhead dominates  

### 🟡 Cases 21–36 (Main Analysis)

- Medium/high workloads  
- Reliable measurements  
- Clear speedup trends  

### 🔵 Cases 37–48 (High Scale)

- **R = 26 (Brazilian states model)**  
- High data volume  
- Best representation of scalability  

---

## 📊 Generated Graphs

- 📈 Speedup vs Threads  
- 📉 Efficiency vs Threads  
- ⏱ Sequential vs Parallel Time  

---

## 🔍 Output Validation

Compare outputs:

```bash
diff <(./seq test.txt) <(./par test.txt)
```

Ignore timing differences:

```bash
diff <(./seq test.txt | grep -v "Tempo") \
     <(./par test.txt | grep -v "Tempo")
```

---

## ☁️ Cluster Execution

Experiments were executed on a remote cluster:

```bash
ssh ssc903-ta-g02@andromeda.lasdpc.icmc.usp.br -p 2270
```

---

## ⚠️ Important Notes

- Large test cases are **not included** in this repository  
- They were generated and executed directly on the cluster  
- You can recreate them using `test.txt`  

---

## 🏁 Conclusion

- Parallelization is **not beneficial for small workloads**
- For larger inputs:
  - ✔ Significant speedup achieved  
  - ✔ Stable performance improvements  
- Efficiency decreases as expected with higher thread counts  

---

## ⭐ Takeaways

This project demonstrates:

- Practical OpenMP usage
- Real-world performance trade-offs
- Importance of workload size in parallel computing

---

## 📄 Report

👉 Full technical report available here:

```
spec/report.pdf
```

---

## 🧑‍💻 Author Note

This project was developed as part of a High Performance Computing course, with a strong focus on:

- Code correctness
- Performance evaluation
- Experimental rigor