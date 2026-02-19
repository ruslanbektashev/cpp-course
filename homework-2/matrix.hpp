#pragma once

#include <map>
#include <tuple>
#include <utility>
#include <iterator>
#include <type_traits>

template <class T, T DefaultValue, class Index = int>
class Matrix {
private:
    using Key = std::pair<Index, Index>;
    std::map<Key, T> data_;

   
    T get(Index i, Index j) const {
        auto it = data_.find({i, j});
        return (it == data_.end()) ? DefaultValue : it->second;
    }

    void set(Index i, Index j, const T& value) {
        if (value == DefaultValue) {
            data_.erase({i, j});  
        } else {
            data_[{i, j}] = value; 
        }
    }

public:
    Matrix() = default;

    std::size_t size() const noexcept {
        return data_.size();
    }

    
    class CellProxy {
        Matrix* owner_ = nullptr;
        Index i_{};
        Index j_{};

    public:
        CellProxy(Matrix* owner, Index i, Index j) : owner_(owner), i_(i), j_(j) {}

        
        operator T() const {
            return owner_->get(i_, j_);
        }

                
      
        CellProxy& operator=(const T& value) {
            owner_->set(i_, j_, value);
            return *this;
        }
    };

    class RowProxy {
        Matrix* owner_ = nullptr;
        Index i_{};

    public:
        RowProxy(Matrix* owner, Index i) : owner_(owner), i_(i) {}

        CellProxy operator[](Index j) {
            return CellProxy(owner_, i_, j);
        }


        T operator[](Index j) const {
            return owner_->get(i_, j_);
        }
    };

    // matrix[i] -> RowProxy
    RowProxy operator[](Index i) {
        return RowProxy(this, i);
    }

    // const matrix[i] -> const RowProxy (для чтения)
    const RowProxy operator[](Index i) const {
        return RowProxy(const_cast<Matrix*>(this), i);
    }

    
    class Iterator {
        using InnerIt = typename std::map<Key, T>::const_iterator;
        InnerIt it_;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::tuple<Index, Index, T>;
        using difference_type = std::ptrdiff_t;

        explicit Iterator(InnerIt it) : it_(it) {}

        Iterator& operator++() { ++it_; return *this; }
        Iterator operator++(int) { Iterator tmp(*this); ++it_; return tmp; }

        bool operator==(const Iterator& other) const { return it_ == other.it_; }
        bool operator!=(const Iterator& other) const { return it_ != other.it_; }

        value_type operator*() const {
            return std::make_tuple(it_->first.first, it_->first.second, it_->second);
        }
    };

    Iterator begin() const { return Iterator(data_.cbegin()); }
    Iterator end() const { return Iterator(data_.cend()); }
};
