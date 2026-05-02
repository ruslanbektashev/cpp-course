#pragma once

#include <cstddef>
#include <iterator>
#include <map>
#include <type_traits>
#include <utility>

template <typename T, T DefaultValue>
class Matrix final {
public:
  using value_type = T;
  using size_type = std::size_t;
  using key_type = std::pair<size_type, size_type>;
  using storage_type = std::map<key_type, value_type>;
  struct Cell final {
    size_type row{};
    size_type col{};
    value_type value{};
  };

private:
  storage_type cells_;

  value_type get_at(size_type row, size_type col) const
  {
    const auto it = cells_.find({row, col});
    if (it == cells_.end()) {
      return DefaultValue;
    }
    return it->second;
  }

  void set_at(size_type row, size_type col, const value_type& value)
  {
    if (value == DefaultValue) {
      cells_.erase({row, col});
      return;
    }
    cells_[{row, col}] = value;
  }

public:
  Matrix() = default;

  [[nodiscard]] size_type size() const noexcept { return cells_.size(); }

  class CellProxy final {
    Matrix* owner_{nullptr};
    size_type row_{0};
    size_type col_{0};

  public:
    CellProxy(Matrix& owner, size_type row, size_type col) : owner_(&owner), row_(row), col_(col) {}

    CellProxy& operator=(const value_type& value)
    {
      owner_->set_at(row_, col_, value);
      return *this;
    }

    CellProxy& operator=(value_type&& value)
    {
      owner_->set_at(row_, col_, value);
      return *this;
    }

    operator value_type() const { return owner_->get_at(row_, col_); }
  };

  class ConstCell final {
    const Matrix* owner_{nullptr};
    size_type row_{0};
    size_type col_{0};

  public:
    ConstCell(const Matrix& owner, size_type row, size_type col) : owner_(&owner), row_(row), col_(col) {}
    operator value_type() const { return owner_->get_at(row_, col_); }
  };

  class RowProxy final {
    Matrix* owner_{nullptr};
    size_type row_{0};

  public:
    RowProxy(Matrix& owner, size_type row) : owner_(&owner), row_(row) {}
    CellProxy operator[](size_type col) { return CellProxy(*owner_, row_, col); }
  };

  class ConstRowProxy final {
    const Matrix* owner_{nullptr};
    size_type row_{0};

  public:
    ConstRowProxy(const Matrix& owner, size_type row) : owner_(&owner), row_(row) {}
    ConstCell operator[](size_type col) const { return ConstCell(*owner_, row_, col); }
  };

  RowProxy operator[](size_type row) { return RowProxy(*this, row); }
  ConstRowProxy operator[](size_type row) const { return ConstRowProxy(*this, row); }

  class const_iterator final {
    typename storage_type::const_iterator it_{};

  public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = Cell;

    explicit const_iterator(typename storage_type::const_iterator it) : it_(it) {}

    value_type operator*() const { return {it_->first.first, it_->first.second, it_->second}; }

    const_iterator& operator++()
    {
      ++it_;
      return *this;
    }

    const_iterator operator++(int)
    {
      const auto tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const const_iterator& a, const const_iterator& b) { return a.it_ == b.it_; }
    friend bool operator!=(const const_iterator& a, const const_iterator& b) { return !(a == b); }
  };

  const_iterator begin() const { return const_iterator(cells_.cbegin()); }
  const_iterator end() const { return const_iterator(cells_.cend()); }
};
