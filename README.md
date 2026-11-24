# Sorting Algorithms — Implementation, Measurement and Comparison

**Author:** azri amdjed
**Academic year:** 2026–2027  
**Module:** Algorithms and Data Structures  
**Language:** C (GCC / MinGW-w64)

## 1. Project overview

This project implements five sorting algorithms from scratch:

1. Bubble Sort (with early-exit optimization)
2. Selection Sort
3. Insertion Sort
4. Merge Sort (recursive divide and conquer)
5. Quick Sort (Lomuto partition scheme; last element is the pivot)

The program provides an interactive menu, allows the user to select an array size, benchmarks each algorithm using five runs, and compares algorithms over several input sizes.

No built-in sorting function is used.

## 2. Project files

- `main.c`: menu, input validation, random array generation, comparison mode and CSV export.
- `sorting.c` / `sorting.h`: sorting algorithm implementations.
- `benchmark.c` / `benchmark.h`: execution-time and memory measurement.
- `results/benchmark_results.csv`: generated comparison data.

## 3. Requirements and compilation

The supplied implementation targets Windows with GCC (for example, MSYS2 UCRT64).

Compile from the project directory:

```powershell
gcc -std=c11 -O2 -Wall -Wextra main.c sorting.c benchmark.c -o sorting.exe -lpsapi
```

Run:

```powershell
.\sorting.exe
```

The `-lpsapi` option links the Windows Process Status API used by the memory monitor.

## 4. Measurement method

- Each algorithm receives its own copy of the same original array.
- Array generation and copying are outside the timed sorting section.
- Execution time uses the C standard library `clock()` function.
- Each benchmark is repeated five times and the average is reported.
- On Windows, the program samples the process working set using `GetProcessMemoryInfo()` while sorting is in progress.

**Memory measurement limitation:** the Windows working set is process-level memory. It includes the program, its data and the monitor thread; it is not an exact count of only the algorithm's temporary allocations. Sampling every millisecond can also miss very brief peaks. This limitation should be considered when interpreting memory results.

The random generator uses a fixed seed (`42`) so the generated input is reproducible.

## 5. Algorithm complexity

| Algorithm | Best time | Average time | Worst time | Auxiliary space |
|---|---:|---:|---:|---:|
| Bubble Sort (early exit) | O(n) | O(n²) | O(n²) | O(1) |
| Selection Sort | O(n²) | O(n²) | O(n²) | O(1) |
| Insertion Sort | O(n) | O(n²) | O(n²) | O(1) |
| Merge Sort | O(n log n) | O(n log n) | O(n log n) | O(n) |
| Quick Sort (last pivot) | O(n log n) | O(n log n) | O(n²) | O(log n) average recursion stack; O(n) worst-case time does not imply linear stack here because the implementation recurses on the smaller partition |

Quick Sort uses the last element as pivot (Lomuto partition). The implementation recursively processes the smaller partition and iterates over the larger one to keep recursion-stack usage bounded.

## 6. Running the program

Menu options 1–5 run one algorithm on a randomly generated array. Enter the desired array size when prompted.

Option 6 runs a comparison for array sizes 1,000, 5,000, 10,000, 50,000 and 100,000. Bubble, Selection and Insertion Sort are limited to 5,000 elements because their quadratic running times can become excessive. Merge Sort and Quick Sort are tested at all listed sizes.

The comparison results are written to:

```text
results/benchmark_results.csv
```

## 7. Experimental results

The following results were obtained by running the comparison mode.
Each measurement represents the average execution time over five runs.

| Algorithm | Array size | Average time (seconds) | Peak working set (KB) |
|---|---:|---:|---:|
| Bubble Sort | 1,000 | 0.0012 | 3,968 |
| Selection Sort | 1,000 | 0.0014 | 4,004 |
| Insertion Sort | 1,000 | 0.0002 | 4,004 |
| Merge Sort | 1,000 | 0.0000 | 4,020 |
| Quick Sort | 1,000 | 0.0002 | 4,020 |
| Bubble Sort | 5,000 | 0.0354 | 4,052 |
| Selection Sort | 5,000 | 0.0338 | 4,052 |
| Insertion Sort | 5,000 | 0.0040 | 4,052 |
| Merge Sort | 5,000 | 0.0006 | 4,072 |
| Quick Sort | 5,000 | 0.0004 | 4,072 |
| Merge Sort | 10,000 | 0.0010 | 4,100 |
| Quick Sort | 10,000 | 0.0010 | 4,092 |
| Merge Sort | 50,000 | 0.0052 | 4,596 |
| Quick Sort | 50,000 | 0.0048 | 4,404 |
| Merge Sort | 100,000 | 0.0118 | 5,188 |
| Quick Sort | 100,000 | 0.0098 | 4,800 |

### Machine specifications

- CPU: Intel Core i5-7300U @ 2.60 GHz
- RAM: 15.88 GB (16 GB installed)
- Operating system: Windows 10
- Compiler: GCC 16.2.0 (MinGW-w64, MSYS2 UCRT64)

### Critical analysis

The experimental results show clear differences between the sorting algorithms.

Insertion Sort performed better than Bubble Sort and Selection Sort on the smaller input sizes. However, its quadratic time complexity makes it unsuitable for larger datasets.

Merge Sort and Quick Sort performed significantly better on larger arrays. Both algorithms handled 100,000 elements efficiently, with Quick Sort achieving a slightly lower average execution time in this experiment.

The results generally agree with theoretical time complexity. The quadratic algorithms become slower as the input size increases, while Merge Sort and Quick Sort scale more efficiently.

Bubble Sort, Selection Sort and Insertion Sort were limited to 5,000 elements in comparison mode to avoid excessive execution time.

The memory measurements represent the process working set rather than the exact memory allocated by each sorting algorithm. Therefore, they should be interpreted as approximate process-level measurements.
