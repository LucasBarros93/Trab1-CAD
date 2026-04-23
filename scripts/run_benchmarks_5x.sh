#!/usr/bin/env bash

rm -f resultados_brutos.csv resumo_metricas.csv resumo_metricas_formatado.txt

echo "caso,arquivo,T,run,seq_s,par_s" > resultados_brutos.csv
echo "caso,arquivo,T,seq_medio_s,par_medio_s,speedup,eficiencia" > resumo_metricas.csv

for f in $(ls caso*.txt | sort -V); do
  caso=$(echo "$f" | sed -E 's/caso([0-9]+).*/\1/')
  T=$(sed -n '5p' "$f")

  seq_sum=0
  par_sum=0

  for run in 1 2 3 4 5; do
    seq_t=$(./seq "$f" | awk '/Tempo de resposta/ {print $(NF-1)}')
    par_t=$(./par "$f" | awk '/Tempo de resposta/ {print $(NF-1)}')

    echo "$caso,$f,$T,$run,$seq_t,$par_t" >> resultados_brutos.csv

    seq_sum=$(awk -v a="$seq_sum" -v b="$seq_t" 'BEGIN {printf "%.6f", a+b}')
    par_sum=$(awk -v a="$par_sum" -v b="$par_t" 'BEGIN {printf "%.6f", a+b}')
  done

  seq_avg=$(awk -v s="$seq_sum" 'BEGIN {printf "%.6f", s/5}')
  par_avg=$(awk -v s="$par_sum" 'BEGIN {printf "%.6f", s/5}')
  speedup=$(awk -v s="$seq_avg" -v p="$par_avg" 'BEGIN {if (p>0) printf "%.6f", s/p; else print "nan"}')
  eficiencia=$(awk -v sp="$speedup" -v t="$T" 'BEGIN {if (t>0) printf "%.6f", sp/t; else print "nan"}')

  echo "$caso,$f,$T,$seq_avg,$par_avg,$speedup,$eficiencia" >> resumo_metricas.csv
done

column -s, -t < resumo_metricas.csv > resumo_metricas_formatado.txt

echo "Arquivos gerados:"
echo " - resultados_brutos.csv"
echo " - resumo_metricas.csv"
echo " - resumo_metricas_formatado.txt"

