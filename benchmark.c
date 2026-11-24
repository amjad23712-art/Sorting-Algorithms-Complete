#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <psapi.h>
#include "benchmark.h"

/*
 * Windows memory monitor.
 * It samples the process working set while the sorting function runs.
 * This is process-level memory, not a direct measurement of only the
 * algorithm's temporary allocations.
 */
typedef struct
{
    volatile LONG stop;
    volatile LONG64 peakBytes;
} MemoryMonitor;

static void updatePeak(MemoryMonitor *monitor, LONG64 currentBytes)
{
    LONG64 previous;

    do
    {
        previous = InterlockedCompareExchange64(
            &monitor->peakBytes, 0, 0);

        if (currentBytes <= previous)
            return;

    } while (InterlockedCompareExchange64(
                 &monitor->peakBytes, currentBytes, previous) != previous);
}

static DWORD WINAPI monitorMemory(LPVOID parameter)
{
    MemoryMonitor *monitor = (MemoryMonitor *)parameter;

    while (InterlockedCompareExchange(&monitor->stop, 0, 0) == 0)
    {
        PROCESS_MEMORY_COUNTERS counters;
        counters.cb = sizeof(counters);

        if (GetProcessMemoryInfo(GetCurrentProcess(),
                                 &counters,
                                 sizeof(counters)))
        {
            updatePeak(monitor, (LONG64)counters.WorkingSetSize);
        }

        Sleep(1);
    }

    /* Take one final sample after the sort has ended. */
    PROCESS_MEMORY_COUNTERS counters;
    counters.cb = sizeof(counters);

    if (GetProcessMemoryInfo(GetCurrentProcess(),
                             &counters,
                             sizeof(counters)))
    {
        updatePeak(monitor, (LONG64)counters.WorkingSetSize);
    }

    return 0;
}

BenchmarkResult benchmarkSort(void (*sortFunction)(int[], int),
                              const int original[], int n)
{
    BenchmarkResult result = {0.0, 0, 0};
    double totalTime = 0.0;
    SIZE_T maximumMemoryKB = 0;

    if (sortFunction == NULL || original == NULL || n <= 0)
        return result;

    for (int run = 0; run < BENCHMARK_RUNS; run++)
    {
        int *copy = malloc((size_t)n * sizeof(int));
        if (copy == NULL)
            return result;

        for (int i = 0; i < n; i++)
            copy[i] = original[i];

        MemoryMonitor monitor;
        monitor.stop = 0;
        monitor.peakBytes = 0;

        HANDLE monitorThread = CreateThread(
            NULL, 0, monitorMemory, &monitor, 0, NULL);

        clock_t start = clock();
        sortFunction(copy, n);
        clock_t end = clock();

        totalTime += (double)(end - start) / CLOCKS_PER_SEC;

        if (monitorThread != NULL)
        {
            InterlockedExchange(&monitor.stop, 1);
            WaitForSingleObject(monitorThread, INFINITE);
            CloseHandle(monitorThread);
        }

        SIZE_T runMemoryKB =
            (SIZE_T)(InterlockedCompareExchange64(
                &monitor.peakBytes, 0, 0) / 1024);

        if (runMemoryKB > maximumMemoryKB)
            maximumMemoryKB = runMemoryKB;

        free(copy);
    }

    result.averageTimeSeconds = totalTime / BENCHMARK_RUNS;
    result.peakWorkingSetKB = maximumMemoryKB;
    result.success = 1;

    return result;
}
