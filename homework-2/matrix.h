#ifndef MATRIX_H
#define MATRIX_H

#include <map>
#include <tuple>
#include <utility>
#include <cstddef>

template<typename T, T DefaultValue>
class Matrix {
private:
    std::map<std::pair<int, int>, T> data_;

public:
    // Forward declarations
    class RowProxy;
    class CellProxy;

    class Iterator {
    private:
        typename std::map<std::pair<int, int>, T>::iterator it_;

    public:
        explicit Iterator(typename std::map<std::pair<int, int>, T>::iterator it) : it_(it) {}

        std::tuple<int, int, T> operator*() const {
            return std::make_tuple(it_->first.first, it_->first.second, it_->second);
        }

        Iterator& operator++() {
            ++it_;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return it_ != other.it_;
        }

        bool operator==(const Iterator& other) const {
            return it_ == other.it_;
        }
    };

    class CellProxy {
    private:
        Matrix* matrix_;
        int row_;
        int col_;

    public:
        CellProxy(Matrix* matrix, int row, int col) 
            : matrix_(matrix), row_(row), col_(col) {}

        // Assignment operator - stores value or removes if default
        CellProxy& operator=(const T& value) {
            auto key = std::make_pair(row_, col_);
            if (value == DefaultValue) {
                matrix_->data_.erase(key);
            } else {
                matrix_->data_[key] = value;
            }
            return *this;
        }

        // Implicit conversion to T for reading
        operator T() const {
            auto key = std::make_pair(row_, col_);
            auto it = matrix_->data_.find(key);
            if (it != matrix_->data_.end()) {
                return it->second;
            }
            return DefaultValue;
        }
    };

    class RowProxy {
    private:
        Matrix* matrix_;
        int row_;

    public:
        RowProxy(Matrix* matrix, int row) : matrix_(matrix), row_(row) {}

        CellProxy operator[](int col) {
            return CellProxy(matrix_, row_, col);
        }
    };

    Matrix() = default;

    std::size_t size() const {
        return data_.size();
    }

    RowProxy operator[](int row) {
        return RowProxy(this, row);
    }

    Iterator begin() {
        return Iterator(data_.begin());
    }

    Iterator end() {
        return Iterator(data_.end());
    }
};

#endif // MATRIX_H