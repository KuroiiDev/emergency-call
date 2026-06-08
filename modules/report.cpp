#include "report.hpp"
#include "../utils/display.hpp"
#include <iostream>
#include <pthread.h>
#include <cstring>
using namespace std;

void* threadCountTotal(void* arg) {
    ReportArg* r = (ReportArg*)arg;
    r->outTotal   = r->total;
    r->outWaiting = 0;
    r->outHandled = 0;
    for (int i = 0; i < r->total; i++) {
        if (r->cases[i].status == "waiting") r->outWaiting++;
        else                                 r->outHandled++;
    }
    cout << "  \033[0;90m[Thread 1]\033[0m Counting total cases...           \033[1;32m✓\033[0m\n";
    return nullptr;
}

void* threadCountPerCategory(void* arg) {
    ReportArg* r = (ReportArg*)arg;
    r->uniqueCats = 0;

    for (int i = 0; i < r->total; i++) {
        string cat = r->cases[i].category;
        bool found = false;
        for (int j = 0; j < r->uniqueCats; j++) {
            if (r->catNames[j] == cat) {
                r->catCount[j]++;
                found = true;
                break;
            }
        }
        if (!found) {
            r->catNames[r->uniqueCats] = cat;
            r->catCount[r->uniqueCats] = 1;
            r->uniqueCats++;
        }
    }
    cout << "  \033[0;90m[Thread 2]\033[0m Counting per category...          \033[1;32m✓\033[0m\n";
    return nullptr;
}

void* threadAvgHandleTime(void* arg) {
    ReportArg* r = (ReportArg*)arg;
    if (r->total == 0) {
        r->avgTime = 0.0;
    } else {
        double sum = 0.0;
        for (int i = 0; i < r->total; i++) {
            sum += (double)((r->cases[i].id % 10) + 5);
        }
        r->avgTime = sum / r->total;
    }
    cout << "  \033[0;90m[Thread 3]\033[0m Calculating avg handle time...    \033[1;32m✓\033[0m\n";
    return nullptr;
}

void cmdReport() {
    cout << "\n  \033[1;36m=== DAILY REPORT ===\033[0m\n";
    cout << "  Generating report using multithreading...\n\n";

    if (totalCases == 0) {
        cout << "  \033[0;33mNo cases recorded yet.\033[0m\n\n";
        return;
    }

    ReportArg arg;
    arg.cases = allCases;
    arg.total = totalCases;
    memset(arg.catCount, 0, sizeof(arg.catCount));
    arg.uniqueCats = 0;
    arg.outTotal   = 0;
    arg.outWaiting = 0;
    arg.outHandled = 0;
    arg.avgTime    = 0.0;

    pthread_t t1, t2, t3;
    pthread_create(&t1, nullptr, threadCountTotal,       &arg);
    pthread_create(&t2, nullptr, threadCountPerCategory, &arg);
    pthread_create(&t3, nullptr, threadAvgHandleTime,    &arg);
    
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    pthread_join(t3, nullptr);

    cout << "\n  \033[1;32mAll threads complete.\033[0m\n";
    cout << "  \033[0;90m─────────────────────────────────────────\033[0m\n";
    printf("  \033[0;37mTotal calls        : \033[1;37m%d\033[0m\n",   arg.outTotal);
    printf("  \033[0;37mStill waiting      : \033[1;34m%d\033[0m\n",   arg.outWaiting);
    printf("  \033[0;37mHandled            : \033[1;32m%d\033[0m\n\n",  arg.outHandled);

    cout << "  \033[0;37mPer Category:\033[0m\n";
    for (int i = 0; i < arg.uniqueCats; i++) {
        printf("    \033[0;33m%-18s\033[0m : %d\n",
            arg.catNames[i].c_str(), arg.catCount[i]);
    }

    printf("\n  \033[0;37mAvg handle time    : \033[1;33m~%.0f min\033[0m \033[0;90m(simulated)\033[0m\n", arg.avgTime);
    cout << "  \033[0;90m─────────────────────────────────────────\033[0m\n\n";
}
