#ifndef THREAD_SAFE_FIFO_HPP
#define THREAD_SAFE_FIFO_HPP

#include <Arduino.h>

#include <queue>

#include "command.hpp"
#include "communication.hpp"

/* Wrapper arround std::queue for safe IPC */
template <typename T>
class ThreadSafeFifo {
   public:
    ThreadSafeFifo() { mutex_init(&mtx); };
    /*
     * Push item to queue.
     *
     * @param item Item to push.
     */
    void push(T& item) {
        // Try to aquire lock, to push to queue.
        if (mutex_enter_timeout_ms(&mtx, 2)) {
            q.push(item);
            mutex_exit(&mtx);
        } else {
            SEND_ERR();
            LOGGER_SEND_ERROR("Failed to aquire lock, push");
        }
    }

    /*
     * Pop item from queue.
     *
     * @param item Item to pop.
     *
     * @return true if item was popped, false otherwise.
     */
    bool pop(T& item) {
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

   private:
    mutex_t mtx;
    std::queue<T> q;
};

#endif
