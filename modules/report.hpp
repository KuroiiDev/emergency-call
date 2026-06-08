#ifndef REPORT_HPP
#define REPORT_HPP

#include "../core/store.hpp"

struct ReportArg {
    Case*  cases;
    int    total;

    int    outTotal;
    int    outWaiting;
    int    outHandled;
    int    catCount[DEFAULT_CAT_COUNT + 10];
    string catNames[DEFAULT_CAT_COUNT + 10];
    int    uniqueCats;
    double avgTime;
};

void* threadCountTotal(void* arg);
void* threadCountPerCategory(void* arg);
void* threadAvgHandleTime(void* arg);
void  cmdReport();

#endif
