#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "../core/types.hpp"
#include <string>
using namespace std;

void   showWelcome();
void   showSeparator();
string getCurrentTime();
void   printCaseDetail(Case c);
void   printCaseRow(int no, Case c);

#endif
