#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP

#include <memory>
#include <atomic>
#include <cassert>
#include <array>
#include <algorithm>
#include <span>

template<typename T, size_t CAPACITY>
class RingBuffer
{
public:
    // assert("CAPACITY needs to be a power of 2", CAPACITY & (CAPACITY - 1) == 0);

    RingBuffer()
    {
    }

    ~RingBuffer() = default;

    void push_block(T* src, size_t nbytes)
    {
        size_t writeHead = writeIdx.load(std::memory_order_acquire);

        std::copy(src, src + nbytes, std::begin(m_storage) + writeHead);

        if (writeHead + nbytes >= CAPACITY)
        {
            writeHead = (writeHead + nbytes) - CAPACITY;
        }
        else
        {
            writeHead += nbytes;
        }

        writeIdx.store(writeHead);
    }

    template<size_t N>
    void try_push(std::span<T, N> data)
    {
        
    }

    void pop(T* dst, size_t nbytes)
    {
        size_t readHead = readIdx.load(std::memory_order_acquire);
        std::copy(std::begin(m_storage) + readHead, std::begin(m_storage) + readHead + nbytes, dst);
        
        if (readHead + nbytes >= CAPACITY)
        {
            readHead = (readHead + nbytes) - CAPACITY;
        }
        else
        {
            readHead += nbytes;
        }

        readIdx.store(readHead);
    }

    void print_buffer() const
    {
        std::cout << "RingBuffer write i: " << writeIdx.load(std::memory_order_acquire) << std::endl;
        std::cout << "RingBuffer read I: " << readIdx.load(std::memory_order_acquire) << std::endl;
        std::cout << "RingBuffer.data(): ";
        for (int i = 0; i < CAPACITY; ++i)
        {
            std::cout << m_storage[i] << " ";
        }
        std::cout << std::endl;
    }

private:
    std::array<T, CAPACITY> m_storage{};

    std::atomic<size_t> writeIdx{0};
    std::atomic<size_t> readIdx{0};
};

#endif // RINGBUFFER_HPP