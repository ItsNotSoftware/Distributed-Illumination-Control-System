#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP

#include <Arduino.h>

#include <array>
#include <cstddef>
#include <string>

template <typename T, size_t Size>
class RingBuffer {
   public:
    std::array<T, Size> buffer = {0};  // Storage for the ring buffer elements

    inline void push(T item) {
        buffer[head++] = item;
        head %= Size;
    }

    inline T pop() {
        uint16_t i = head == 0 ? Size - 1 : head - 1;
        return buffer[i];
    }

    std::string to_str() {
        std::string str = "";
        uint16_t count = 0;
        uint16_t i = head;

        while (count < Size) {
            str += std::to_string(buffer[i++]) + " ";

            i %= Size;
            count++;
        }
        return str;
    }

   private:
    size_t head = 0;  // Points to the next write position
};

#endif
