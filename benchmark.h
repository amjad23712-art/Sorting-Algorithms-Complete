#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <windows.h>

#define BENCHMARK_RUNS 5

typedef struct
{
    double averageTimeSeconds;
    SIZE_T peakWorkingSetKB;
    int success;
} BenchmarkResult;

BenchmarkResult benchmarkSort(void (*sortFunction)(int[], int),
                              const int original[], int n);

#endif
