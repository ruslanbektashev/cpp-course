#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <map>
#include <tuple>

template <typename T, T DefaultValue>
class Matrix {
public:
    using value_type = T;
    using index_type = std::int64_t;
    using size_type = std::size_t;

private:
    using key_type = std::pair<index_type, index_type>;
    using storage_type = std::map<key_type, value_type>;

public:
    class CellProxy;
    class RowProxy;
    class ConstRowProxy;
    class Iterator;

    Matrix() = default;

    RowProxy operator[](index_type row) {
        return RowProxy(*this, row);
    }

    ConstRowProxy operator[](index_type row) const {
        return ConstRowProxy(*this, row);
    }

    size_type size() const noexcept {
        return data_.size();
    }

    Iterator begin() const noexcept {
        return Iterator(data_.cbegin());
    }

    Iterator end() const noexcept {
        return Iterator(data_.cend());
    }

private:
    value_type get(index_type row, index_type col) const {
        const auto it = data_.find({row, col});
        return it == data_.cend() ? DefaultValue : it->second;
    }

    void set(index_type row, index_type col, const value_type& value) {
        const key_type key{row, col};
        if (value == DefaultValue) {
            data_.erase(key);
            return;
        }
        data_[key] = value;
    }

private:
    storage_type data_;

public:
    class CellProxy {
    public:
        CellProxy(Matrix& matrix, index_type row, index_type col)
            : matrix_(matrix), row_(row), col_(col) {}

        CellProxy& operator=(const value_type& value) {
            matrix_.set(row_, col_, value);
            return *this;
        }

        CellProxy& operator=(const CellProxy& other) {
            return (*this = static_cast<value_type>(other));
        }

        operator value_type() const {
            return matrix_.get(row_, col_);
        }

    private:
        Matrix& matrix_;
        index_type row_;
        index_type col_;
    };

    class RowProxy {
    public:
        RowProxy(Matrix& matrix, index_type row)
            : matrix_(matrix), row_(row) {}

        CellProxy operator[](index_type col) {
            return CellProxy(matrix_, row_, col);
        }

    private:
        Matrix& matrix_;
        index_type row_;
    };

    class ConstRowProxy {
    public:
        ConstRowProxy(const Matrix& matrix, index_type row)
            : matrix_(matrix), row_(row) {}

        value_type operator[](index_type col) const {
            return matrix_.get(row_, col);
        }

    private:
        const Matrix& matrix_;
        index_type row_;
    };

    class Iterator {
    public:
        using storage_iterator = typename storage_type::const_iterator;
        using value_type = std::tuple<index_type, index_type, T>;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = value_type;

        explicit Iterator(storage_iterator it) : it_(it) {}

        reference operator*() const {
            return {it_->first.first, it_->first.second, it_->second};
        }

        Iterator& operator++() {
            ++it_;
            return *this;
        }

        Iterator operator++(int) {
            Iterator copy(*this);
            ++(*this);
            return copy;
        }

        bool operator==(const Iterator& other) const {
            return it_ == other.it_;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

    private:
        storage_iterator it_;
    };
};
