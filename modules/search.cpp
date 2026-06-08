#include "search.hpp"
#include "../utils/display.hpp"
#include <iostream>
using namespace std;

void merge(Case arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Case* L = new Case[n1];
    Case* R = new Case[n2];

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].id <= R[j].id) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    delete[] L;
    delete[] R;
}

void mergeSort(Case arr[], int left, int right) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

int binarySearch(Case arr[], int size, int targetId) {
    int left = 0, right = size - 1;
    while (left <= right) {
        int mid = (left + right) / 2;
        if (arr[mid].id == targetId) return mid;
        else if (arr[mid].id < targetId) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

void cmdSearch(int id) {
    cout << "\n  \033[1;36m=== SEARCH RESULT ===\033[0m\n\n";

    if (totalCases == 0) {
        cout << "  \033[0;33mNo cases registered yet.\033[0m\n\n";
        return;
    }

    Case sortedCases[MAX_CASES];
    for (int i = 0; i < totalCases; i++) sortedCases[i] = allCases[i];

    cout << "  Sorting cases (merge sort)... \033[1;32mdone.\033[0m\n";
    mergeSort(sortedCases, 0, totalCases - 1);

    cout << "  Searching ID #" << id << " (binary search)... ";
    int idx = binarySearch(sortedCases, totalCases, id);

    if (idx == -1) {
        cout << "\033[1;31mnot found.\033[0m\n\n";
        cout << "  \033[0;33mCase #" << id << " not found.\033[0m\n\n";
    } else {
        cout << "\033[1;32mfound.\033[0m\n";
        cout << "\n";
        printCaseDetail(sortedCases[idx]);
        cout << "\n";
    }
}
