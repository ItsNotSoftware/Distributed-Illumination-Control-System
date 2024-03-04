#include "command_fifo.hpp"

#include "communication.hpp"

CommandFifo::CommandFifo() { mutex_init(&mtx); }

void CommandFifo::push(Command& item) {
    // Try to aquire lock, to push to queue.
    if (mutex_enter_timeout_ms(&mtx, 2)) {
        q.push(item);
        mutex_exit(&mtx);
    } else {
        SEND_ERR();
        LOGGER_SEND_ERROR("Failed to aquire lock, push");
    }
}

bool CommandFifo::pop(Command& item) {
    // Try to aquire lock, to pop from queue.
    if (mutex_enter_timeout_ms(&mtx, 2)) {
        // Check if queue is empty.
        if (q.empty()) {
            mutex_exit(&mtx);
            return false;
        }

        // Pop item from queue.
        item = q.front();
        q.pop();

        mutex_exit(&mtx);
        return true;

    } else {
        LOGGER_SEND_ERROR("Failed to aquire lock, pop");
        return false;
    }
}
