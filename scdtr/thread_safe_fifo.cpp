#include "thread_safe_fifo.hpp"

ThreadSafeFifo::ThreadSafeFifo() { mutex_init(&mtx); }

void ThreadSafeFifo::push(const Command& item) {
    // Try to aquire lock, to push to queue.
    if (mutex_enter_timeout_ms(&mtx, 1000) != 0) {
        q.push(item);
        mutex_exit(&mtx);
    } else {
        Serial.println("[Mutex Error]: Timeout pushing item");
    }
}

bool ThreadSafeFifo::pop(Command& item) {
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
    } else {
        Serial.println("[Mutex Error]: Timeout popping item");
        return false;
    }

    return true;
}
