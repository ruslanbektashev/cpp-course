#ifndef COUNTER_HPP
#define COUNTER_HPP


#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <map>


template <typename T> class SparseMatrixNew;

template <typename T>
class MatrixN_1DimProxy
{
    SparseMatrixNew<T> &matrix;
    std::vector<uint64_t> coords;

public:
    MatrixN_1DimProxy(SparseMatrixNew<T> &matrix, std::vector<uint64_t> coords) : matrix(matrix), coords(coords) {}

    MatrixN_1DimProxy<T> operator[](uint64_t n_1_coord)
    {
        coords.push_back(n_1_coord);
        return MatrixN_1DimProxy<T>(matrix, coords);
    }
    MatrixN_1DimProxy<T> &operator=(T value)
    {
        matrix.addValue(value, coords);
        return *this;
    }
    operator T() const
    {
        return matrix.getValue(coords);
    }
};

template <typename T>
class SparseMatrixNew
{
    T default_value;
    std::map<std::vector<uint64_t>, T> values;

public:
    SparseMatrixNew(T default_value) : default_value(default_value) {}

    MatrixN_1DimProxy<T> operator[](uint64_t n_coord)
    {
        std::vector<uint64_t> coord;
        coord.push_back(n_coord);
        return MatrixN_1DimProxy<T>(*this, coord);
    }

    void addValue(T value, std::vector<uint64_t> coords)
    {
        if (value == default_value)
        {
            if (values.find(coords) != values.end())
                values.erase(coords);
        }
        else
        {
            values[coords] = value;
        }
    }

    T getValue(std::vector<uint64_t> coords) const
    {
        auto it = values.find(coords);
        if (it != values.end())
            return it->second;
        return default_value;
    }

    void showMatrix() const
    {
        for (const auto &raw_data : values)
        {
            std::cout << raw_data.second << " ";
            for (const auto &coord : raw_data.first)
                std::cout << coord << " ";
            std::cout << std::endl;
        }
        std::cout << "\n";
    }

    int countNonDefault() const
    {
        return values.size();
    }
};


#endif