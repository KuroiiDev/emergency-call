#include "display.hpp"
#include <iostream>
#include <ctime>
#include <cstdio>
using namespace std;

void showWelcome() {
    cout << "\n";
    cout << "  \033[1;36m╔══════════════════════════════════════════════\033[0m\n";
    cout << "  \033[1;36m║\033[0m  \033[1;37mSMART EMERGENCY CALL CENTER  v1.0\033[0m\n";
    cout << "  \033[1;36m║\033[0m  \033[0;37mSistem Layanan Panggilan Darurat\033[0m\n";
    cout << "  \033[1;36m║\033[0m  \033[0;37mAlgoritma & Pemrograman — Kelompok 04\033[0m\n";
    cout << "  \033[1;36m╚══════════════════════════════════════════════\033[0m\n";
    cout << "\n";
    cout << "  \033[0;32mSystem ready.\033[0m Type \033[1;33mhelp\033[0m for available commands.\n";
    cout << "\n";
}

void showSeparator() {
    cout << "  \033[0;90m──────────────────────────────\033[0m\n";
}

string getCurrentTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[9];
    sprintf(buf, "%02d:%02d:%02d", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    return string(buf);
}

void printCaseDetail(Case c) {
    showSeparator();
    cout << "  \033[0;90mID       :\033[0m \033[1;37m#" << c.id << "\033[0m\n";
    cout << "  \033[0;90mName     :\033[0m " << c.name << "\n";
    cout << "  \033[0;90mPhone    :\033[0m " << c.phone << "\n";
    cout << "  \033[0;90mCategory :\033[0m \033[1;33m" << c.category << "\033[0m\n";
    cout << "  \033[0;90mTime     :\033[0m " << c.time << "\n";
    if (c.status == "waiting") {
        cout << "  \033[0;90mStatus   :\033[0m \033[1;34mWaiting\033[0m\n";
    } else {
        cout << "  \033[0;90mStatus   :\033[0m \033[1;32mHandled\033[0m\n";
    }
    showSeparator();
}

void printCaseRow(int no, Case c) {
    // Format: no | id | name (16 chars) | category (12 chars) | time | status
    printf("  \033[0;90m%3d  | #%-3d | %-16s | %-12s | %s | %s\033[0m\n",
        no,
        c.id,
        c.name.substr(0, 16).c_str(),
        c.category.substr(0, 12).c_str(),
        c.time.c_str(),
        c.status == "waiting" ? "\033[1;34mwaiting\033[0m" : "\033[1;32mhandled\033[0m"
    );
}
