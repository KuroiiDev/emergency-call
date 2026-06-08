#include <iostream>
#include <string>
#include "core/store.hpp"
#include "modules/call.hpp"
#include "modules/dispatch.hpp"
#include "modules/history.hpp"
#include "modules/search.hpp"
#include "modules/report.hpp"
#include "modules/help.hpp"
#include "utils/display.hpp"

using namespace std;

int main() {
    initStore();
    showWelcome();

    string cmd;
    while (true) {
        cout << "\033[1;37m>\033[0m ";
        cin >> cmd;

        if (cmd == "call")          cmdCall();
        else if (cmd == "queue")    cmdQueue();
        else if (cmd == "dispatch") cmdDispatch();
        else if (cmd == "history")  cmdHistory();
        else if (cmd == "search") {
            if (cin.peek() == '\n' || cin.peek() == '\r') {
                cout << "\n  Usage  : search <id>\n";
                cout << "  Example: search 5\n\n";
            } else {
                int id;
                if (cin >> id) {
                    cmdSearch(id);
                } else {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    cout << "\n  \033[1;31mInvalid ID.\033[0m Usage: search <id>\n\n";
                }
            }
        }
        else if (cmd == "report")  cmdReport();
        else if (cmd == "help")    cmdHelp();
        else if (cmd == "exit") {
            cout << "\n  \033[1;36mSystem shutdown. Goodbye.\033[0m\n\n";
            break;
        }
        else {
            cout << "\n  \033[1;31mCommand not found:\033[0m " << cmd
                 << ". Type \033[1;33mhelp\033[0m\n\n";
        }
    }
    return 0;
}
