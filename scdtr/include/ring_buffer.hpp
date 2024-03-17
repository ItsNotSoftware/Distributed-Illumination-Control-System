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

    /*
     * Pushes an item to the ring buffer. If the buffer is full, the oldest item is overwritten.
     *
     * @param item: The item to be pushed to the buffer
     */
    inline void push(T item) {
        buffer[head++] = item;
        head %= Size;
    }

    /*
     * Pops an item from the ring buffer. If the buffer is empty, the last item is returned.
     *
     * @return: The item popped from the buffer
     */
    inline T pop() {
        uint16_t i = head == 0 ? Size - 1 : head - 1;  // Get the last item
        return buffer[i];
    }

    /*
     * Converts the ring buffer to a string representation.
     *
     * @return: String representation of the ring buffer.
     */
    std::string to_str() {
        std::string str = "";
        uint16_t count = 0;
        uint16_t i = head;

        // Iterate through the buffer and append each element to the string
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
