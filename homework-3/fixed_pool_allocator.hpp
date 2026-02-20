    #pragma once

#include <cstddef>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <typename T, std::size_t Capacity>
class FixedPoolAllocator {
    static_assert(Capacity > 0, "Capacity must be greater than zero");

public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <typename U>
    struct rebind {
        using other = FixedPoolAllocator<U, Capacity>;
    };

    FixedPoolAllocator() : state_(std::make_shared<State>()) {}

    template <typename U>
    FixedPoolAllocator(const FixedPoolAllocator<U, Capacity>&) : state_(std::make_shared<State>()) {}

    T* allocate(size_type n) {
        if (n == 0) {
            return nullptr;
        }
        if (n > Capacity - state_->used) {
            throw std::bad_alloc();
        }

        T* const result = state_->buffer + state_->used;
        state_->used += n;
        return result;
    }

    void deallocate(T*, size_type) noexcept {
    // This allocator is monotonic.
    // Individual deallocation is intentionally not supported.
    // All memory is released when allocator state is destroyed.
}

    template <typename U>
    bool operator==(const FixedPoolAllocator<U, Capacity>&) const noexcept {
        return std::is_same_v<T, U>;
    }

    template <typename U>
    bool operator!=(const FixedPoolAllocator<U, Capacity>& other) const noexcept {
        return !(*this == other);
    }

private:
    struct State {
        State() : buffer(static_cast<T*>(::operator new(sizeof(T) * Capacity))), used(0) {}

        ~State() {
            ::operator delete(buffer);
        }

        T* buffer;
        size_type used;
    };

    std::shared_ptr<State> state_;

    template <typename U, std::size_t OtherCapacity>
    friend class FixedPoolAllocator;
};
