#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "../core/store.hpp"

void mergeSort(Case arr[], int left, int right);
void merge(Case arr[], int left, int mid, int right);
int  binarySearch(Case arr[], int size, int targetId);
void cmdSearch(int id);

#endif
