#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
using namespace std;

const string DEFAULT_CATEGORIES[] = {
    "Kebakaran",
    "Medis",
    "Kriminal",
    "Bencana Alam"
};
const int DEFAULT_CAT_COUNT = 4;

struct Case {
    int    id;          
    string name;        
    string phone;       
    string category;    
    string time;        
    string status;    
};

struct QueueNode {
    Case      data;
    QueueNode* next;
};
struct StackNode {
    Case       data;
    StackNode* next;
};

#endif
