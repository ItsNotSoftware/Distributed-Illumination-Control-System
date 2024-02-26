#ifndef THREAD_SAFE_FIFO_HPP
#define THREAD_SAFE_FIFO_HPP

#include <Arduino.h>

#include <queue>

#include "command.hpp"

/* Wrapper arround std::queue for safe IPC */
class ThreadSafeFifo {
   public:
    ThreadSafeFifo();
    /*
     * Push item to queue.
     *
     * @param item Item to push.
     */
    void push(const Command& item);

    /*
     * Pop item from queue.
     *
     * @param item Item to pop.
     *
     * @return Commandrue if item was popped, false otherwise.
     */
    bool pop(Command& item);

   private:
    mutex_t mtx;
    std::queue<Command> q;
};

#endif
