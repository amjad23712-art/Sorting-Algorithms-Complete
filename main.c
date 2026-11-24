#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <direct.h>

#include "sorting.h"
#include "benchmark.h"

#define MAX_ARRAY_SIZE 1000000
#define DISPLAY_LIMIT 20

typedef void (*SortFunction)(int[], int);

typedef struct
{
    const char *name;
    SortFunction function;
} Algorithm;

static const Algorithm algorithms[] = {
    {"Bubble Sort", bubbleSort},
    {"Selection Sort", selectionSort},
    {"Insertion Sort", insertionSort},
    {"Merge Sort", mergeSort},
    {"Quick Sort", quickSort}
};

static int readInteger(const char *prompt, int minimum, int maximum)
{
    char line[128];
    char *endPointer;
    long value;

    while (1)
    {
        printf("%s", prompt);

        if (fgets(line, sizeof(line), stdin) == NULL)
            return minimum - 1;

        errno = 0;
        value = strtol(line, &endPointer, 10);

        while (*endPointer == ' ' || *endPointer == '\t' ||
               *endPointer == '\n' || *endPointer == '\r')
            endPointer++;

        if (errno == 0 && endPointer != line && *endPointer == '\0' &&
            value >= minimum && value <= maximum)
        {
            return (int)value;
        }

        printf("Invalid input. Enter a number from %d to %d.\n",
               minimum, maximum);
    }
}

static void generateRandomArray(int arr[], int n)
{
    /* Fixed seed makes the generated sequence reproducible. */
    srand(42);

    for (int i = 0; i < n; i++)
        arr[i] = (rand() % 2000001) - 1000000;
}

static void displayArray(const int arr[], int n)
{
    int limit = (n < DISPLAY_LIMIT) ? n : DISPLAY_LIMIT;

    for (int i = 0; i < limit; i++)
        printf("%d ", arr[i]);

    if (n > DISPLAY_LIMIT)
        printf("... (%d values total)", n);

    printf("\n");
}

static int isSorted(const int arr[], int n)
{
    for (int i = 1; i < n; i++)
    {
        if (arr[i - 1] > arr[i])
            return 0;
    }

    return 1;
}

static void runOneAlgorithm(int algorithmIndex)
{
    int n = readInteger("Enter array size (1 - 1000000): ",
                       1, MAX_ARRAY_SIZE);

    if (n < 1)
        return;

    if (algorithmIndex <= 2 && n > 20000)
    {
        printf("Warning: this algorithm may be very slow on large arrays.\n");
        printf("Consider using 5000 or fewer elements for this demonstration.\n");
    }

    int *data = malloc((size_t)n * sizeof(int));
    int *displayCopy = malloc((size_t)n * sizeof(int));

    if (data == NULL || displayCopy == NULL)
    {
        printf("Memory allocation failed.\n");
        free(data);
        free(displayCopy);
        return;
    }

    generateRandomArray(data, n);
    memcpy(displayCopy, data, (size_t)n * sizeof(int));

    printf("\nRunning %s on %d values (%d runs)...\n",
           algorithms[algorithmIndex].name, n, BENCHMARK_RUNS);

    BenchmarkResult result =
        benchmarkSort(algorithms[algorithmIndex].function, data, n);

    /* Sort a separate copy for displaying and checking the output. */
    algorithms[algorithmIndex].function(displayCopy, n);

    if (!result.success)
        printf("Benchmark failed (allocation or measurement error).\n");
    else
    {
        printf("Average execution time: %.6f seconds\n",
               result.averageTimeSeconds);
        printf("Peak process working set: %zu KB\n",
               (size_t)result.peakWorkingSetKB);
        printf("Sorted correctly: %s\n",
               isSorted(displayCopy, n) ? "YES" : "NO");
        printf("Sorted output: ");
        displayArray(displayCopy, n);
    }

    free(data);
    free(displayCopy);
}

static int writeCsvHeader(FILE *file)
{
    return fprintf(file,
        "algorithm,array_size,average_time_seconds,peak_process_working_set_kb,runs\n") >= 0;
}

static void compareAllAlgorithms(void)
{
    const int sizes[] = {1000, 5000, 10000, 50000, 100000};
    const int sizeCount = (int)(sizeof(sizes) / sizeof(sizes[0]));

    _mkdir("results");

    FILE *csv = fopen("results/benchmark_results.csv", "w");
    if (csv == NULL)
    {
        printf("Could not create results/benchmark_results.csv\n");
        return;
    }

    writeCsvHeader(csv);

    printf("\n========== ALGORITHM COMPARISON ==========\n");
    printf("Each measurement uses %d runs and the same input data per size.\n",
           BENCHMARK_RUNS);
    printf("Quadratic algorithms are limited to 5000 values to avoid excessive run times.\n\n");

    printf("%-17s %10s %16s %18s\n",
           "Algorithm", "Array size", "Average (sec)", "Peak memory (KB)");
    printf("--------------------------------------------------------------------------\n");

    for (int s = 0; s < sizeCount; s++)
    {
        int n = sizes[s];
        int *data = malloc((size_t)n * sizeof(int));

        if (data == NULL)
        {
            printf("Could not allocate array of size %d. Skipping.\n", n);
            continue;
        }

        generateRandomArray(data, n);

        for (int a = 0; a < 5; a++)
        {
            /* Bubble, Selection and Insertion are quadratic. */
            if (a <= 2 && n > 5000)
                continue;

            BenchmarkResult result = benchmarkSort(
                algorithms[a].function, data, n);

            if (!result.success)
            {
                printf("%-17s %10d %16s %18s\n",
                       algorithms[a].name, n, "FAILED", "FAILED");
                continue;
            }

            printf("%-17s %10d %16.6f %18zu\n",
                   algorithms[a].name, n,
                   result.averageTimeSeconds,
                   (size_t)result.peakWorkingSetKB);

            fprintf(csv, "%s,%d,%.9f,%zu,%d\n",
                    algorithms[a].name, n,
                    result.averageTimeSeconds,
                    (size_t)result.peakWorkingSetKB,
                    BENCHMARK_RUNS);
        }

        printf("--------------------------------------------------------------------------\n");
        free(data);
    }

    fclose(csv);
    printf("\nCSV results saved to: results/benchmark_results.csv\n");
}

static void showMenu(void)
{
    printf("\n========== SORTING ALGORITHMS ==========\n");
    printf("1. Bubble Sort\n");
    printf("2. Selection Sort\n");
    printf("3. Insertion Sort\n");
    printf("4. Merge Sort\n");
    printf("5. Quick Sort\n");
    printf("6. Compare all algorithms (multiple sizes)\n");
    printf("0. Exit\n");
}

int main(void)
{
    int choice;

    do
    {
        showMenu();
        choice = readInteger("Your choice: ", 0, 6);

        if (choice >= 1 && choice <= 5)
            runOneAlgorithm(choice - 1);
        else if (choice == 6)
            compareAllAlgorithms();
        else if (choice == 0)
            printf("Program terminated.\n");

    } while (choice != 0);

    return 0;
}
