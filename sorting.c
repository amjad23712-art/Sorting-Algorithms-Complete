#include <stdlib.h>
#include "sorting.h"

/* Bubble Sort: stops early if a pass makes no swaps. */
void bubbleSort(int arr[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        int swapped = 0;

        for (int j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = 1;
            }
        }

        if (!swapped)
            break;
    }
}

/* Selection Sort: selects the minimum remaining element. */
void selectionSort(int arr[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        int minIndex = i;

        for (int j = i + 1; j < n; j++)
        {
            if (arr[j] < arr[minIndex])
                minIndex = j;
        }

        if (minIndex != i)
        {
            int temp = arr[i];
            arr[i] = arr[minIndex];
            arr[minIndex] = temp;
        }
    }
}

/* Insertion Sort: inserts each key into the sorted left portion. */
void insertionSort(int arr[], int n)
{
    for (int i = 1; i < n; i++)
    {
        int key = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = key;
    }
}

/* Merge Sort helpers. */
static void merge(int arr[], int temp[], int left, int mid, int right)
{
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right)
    {
        if (arr[i] <= arr[j])
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }

    while (i <= mid)
        temp[k++] = arr[i++];

    while (j <= right)
        temp[k++] = arr[j++];

    for (i = left; i <= right; i++)
        arr[i] = temp[i];
}

static void mergeSortRecursive(int arr[], int temp[], int left, int right)
{
    if (left >= right)
        return;

    int mid = left + (right - left) / 2;

    mergeSortRecursive(arr, temp, left, mid);
    mergeSortRecursive(arr, temp, mid + 1, right);
    merge(arr, temp, left, mid, right);
}

void mergeSort(int arr[], int n)
{
    if (n <= 1)
        return;

    int *temp = malloc((size_t)n * sizeof(int));
    if (temp == NULL)
        return;

    mergeSortRecursive(arr, temp, 0, n - 1);
    free(temp);
}

/*
 * Quick Sort uses Lomuto partitioning with the last element as pivot.
 * The smaller partition is processed recursively and the larger one
 * iteratively, limiting recursion-stack growth.
 */
static int partition(int arr[], int low, int high)
{
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++)
    {
        if (arr[j] <= pivot)
        {
            i++;
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }

    int temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;

    return i + 1;
}

static void quickSortRecursive(int arr[], int low, int high)
{
    while (low < high)
    {
        int pivotIndex = partition(arr, low, high);

        if (pivotIndex - low < high - pivotIndex)
        {
            quickSortRecursive(arr, low, pivotIndex - 1);
            low = pivotIndex + 1;
        }
        else
        {
            quickSortRecursive(arr, pivotIndex + 1, high);
            high = pivotIndex - 1;
        }
    }
}

void quickSort(int arr[], int n)
{
    if (n > 1)
        quickSortRecursive(arr, 0, n - 1);
}
