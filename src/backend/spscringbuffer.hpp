#ifndef SPSCRINGBUFFER_HPP
#define SPSCRINGBUFFER_HPP

#include <memory>
#include <atomic>
#include <cassert>
#include <array>
#include <span>

template<typename T, size_t CAPACITY>
class SPSCRingBuffer
{
public:
    assert("CAPACITY needs to be a power of 2", CAPACITY & (CAPACITY - 1) == 0);

    SPSCRingBuffer()
    {
        m_storage = std::make_unique<T>(CAPACITY);
    }

    ~SPSCRingBuffer() = default;

    void try_push(T* src, size_t nbytes)
    {

    }

    template<size_t N>
    void try_push(std::span<T, N> data)
    {

    }

    void pop(T* dst, size_t nbytes)
    {
        
    }

private:
    std::unique_ptr<T> m_storage;
};

#endif // SPSCRINGBUFFER_HPP