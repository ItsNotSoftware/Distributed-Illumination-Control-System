#include "command_fifo.hpp"

CommandFifo::CommandFifo() { mutex_init(&mtx); }

void CommandFifo::push(Command& item) {
    // Try to aquire lock, to push to queue.
    if (mutex_enter_timeout_ms(&mtx, 1000) != 0) {
        q.push(item);
        mutex_exit(&mtx);
    } else {
        Serial.println("[Mutex Error]: Timeout pushing item");
    }
}

bool CommandFifo::pop(Command& item) {
    // Try to aquire lock, to pop from queue.
    if (mutex_enter_timeout_ms(&mtx, 1) != 0) {
        // Check if queue is empty.
        if (q.empty()) {
            return false;
        }

        // Pop item from queue.
        item = q.front();
        q.pop();

        mutex_exit(&mtx);
        return true;
    }

    return false;
}
