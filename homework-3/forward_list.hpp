#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

template <typename T, typename Allocator = std::allocator<T>>
class ForwardList {
public:
    using value_type = T;
    using size_type = std::size_t;

private:
    struct Node {
        explicit Node(const T& v) : value(v), next(nullptr) {}
        explicit Node(T&& v) : value(std::move(v)), next(nullptr) {}

        T value;
        Node* next;
    };

    using NodeAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using NodeAllocTraits = std::allocator_traits<NodeAllocator>;

public:
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        explicit Iterator(Node* ptr) : current_(ptr) {}

        reference operator*() const { return current_->value; }
        pointer operator->() const { return &current_->value; }

        Iterator& operator++() {
            current_ = current_->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator copy(*this);
            ++(*this);
            return copy;
        }

        bool operator==(const Iterator& other) const { return current_ == other.current_; }
        bool operator!=(const Iterator& other) const { return !(*this == other); }

    private:
        Node* current_;
    };

    class ConstIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        explicit ConstIterator(const Node* ptr) : current_(ptr) {}

        reference operator*() const { return current_->value; }
        pointer operator->() const { return &current_->value; }

        ConstIterator& operator++() {
            current_ = current_->next;
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator copy(*this);
            ++(*this);
            return copy;
        }

        bool operator==(const ConstIterator& other) const { return current_ == other.current_; }
        bool operator!=(const ConstIterator& other) const { return !(*this == other); }

    private:
        const Node* current_;
    };

    ForwardList() = default;

    explicit ForwardList(const Allocator& allocator)
        : allocator_(allocator) {}

    ~ForwardList() {
        clear();
    }

    ForwardList(const ForwardList&) = delete;
    ForwardList& operator=(const ForwardList&) = delete;
    ForwardList(ForwardList&&) = delete;
    ForwardList& operator=(ForwardList&&) = delete;

    void push_back(const T& value) {
        Node* node = create_node(value);
        append_node(node);
    }

    void push_back(T&& value) {
        Node* node = create_node(std::move(value));
        append_node(node);
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

    size_type size() const noexcept {
        return size_;
    }

    Iterator begin() noexcept { return Iterator(head_); }
    Iterator end() noexcept { return Iterator(nullptr); }

    ConstIterator begin() const noexcept { return ConstIterator(head_); }
    ConstIterator end() const noexcept { return ConstIterator(nullptr); }
    ConstIterator cbegin() const noexcept { return ConstIterator(head_); }
    ConstIterator cend() const noexcept { return ConstIterator(nullptr); }

    void clear() noexcept {
        Node* current = head_;
        while (current != nullptr) {
            Node* next = current->next;
            NodeAllocTraits::destroy(allocator_, current);
            NodeAllocTraits::deallocate(allocator_, current, 1);
            current = next;
        }

        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
    }

private:
    template <typename U>
    Node* create_node(U&& value) {
        Node* node = NodeAllocTraits::allocate(allocator_, 1);
        try {
            NodeAllocTraits::construct(allocator_, node, std::forward<U>(value));
        } catch (...) {
            NodeAllocTraits::deallocate(allocator_, node, 1);
            throw;
        }
        return node;
    }

    void append_node(Node* node) noexcept {
        if (tail_ == nullptr) {
            head_ = node;
            tail_ = node;
        } else {
            tail_->next = node;
            tail_ = node;
        }
        ++size_;
    }

private:
    Node* head_{nullptr};
    Node* tail_{nullptr};
    size_type size_{0};
    NodeAllocator allocator_{};
};
