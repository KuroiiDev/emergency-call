#include "dispatch.hpp"
#include "call.hpp"
#include "history.hpp"
#include "../utils/display.hpp"
#include <iostream>
using namespace std;

void cmdDispatch() {
    cout << "\n";

    if (isQueueEmpty(callQueue)) {
        cout << "  \033[0;33mNo calls in queue.\033[0m\n\n";
        return;
    }

    cout << "  \033[1;36m=== DISPATCHING NEXT CALL ===\033[0m\n\n";

    Case handled = dequeue(callQueue);

    handled.status = "handled";
    for (int i = 0; i < totalCases; i++) {
        if (allCases[i].id == handled.id) {
            allCases[i].status = "handled";
            break;
        }
    }

    pushStack(historyStack, handled);

    cout << "  Handling: \033[1;37m#" << handled.id << " — " << handled.name
         << "\033[0m (" << handled.category << ")\n\n";
    cout << "  \033[1;32m✓ Case #" << handled.id << " moved to handled.\033[0m\n";
    cout << "  Queue remaining: \033[1;33m" << callQueue.count << "\033[0m\n\n";
}
