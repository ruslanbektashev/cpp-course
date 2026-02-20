#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdint>
#include <memory>

class BinaryGraph {
public:
    BinaryGraph(int rows, int cols) : n(rows), m(cols) {
        row_width = (m + 63) / 64; 
        data.resize(n, std::vector<uint64_t>(row_width, 0));
    }

    void setEdge(int row, int col) {
        if (row < n && col < m) {
            data[row][col / 64] |= (1ULL << (col % 64));
        }
    }

    int getRows() const { return n; }
    size_t getRowWidth() const { return row_width; }
    const std::vector<uint64_t>& getRow(int index) const { return data[index]; }

private:
    int n, m;
    size_t row_width;
    std::vector<std::vector<uint64_t>> data;
};


class CycleDetector {
public:
    explicit CycleDetector(const BinaryGraph& graph) : targetGraph(graph) {}

    bool hasCycle4() const {
        int n = targetGraph.getRows();
        size_t width = targetGraph.getRowWidth();

        for (int i = 0; i < n; ++i) {
            const auto& rowI = targetGraph.getRow(i);
            for (int j = i + 1; j < n; ++j) {
                const auto& rowJ = targetGraph.getRow(j);
                
                if (checkIntersection(rowI, rowJ, width)) {
                    return true;
                }
            }
        }
        return false;
    }

private:
    const BinaryGraph& targetGraph;

    bool checkIntersection(const std::vector<uint64_t>& r1, 
                           const std::vector<uint64_t>& r2, 
                           size_t width) const {
        int count = 0;
        for (size_t k = 0; k < width; ++k) {
            uint64_t intersect = r1[k] & r2[k];
            if (intersect) {
                count += __builtin_popcountll(intersect);
                if (count >= 2) return true;
            }
        }
        return false;
    }
};

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::istream* input = &std::cin;
    std::ifstream file;

    if (argc > 1) {
        file.open(argv[1]);
        if (file.is_open()) input = &file;
    }

    int n, m;
    if (!(*input >> n >> m)) return 0;

    auto graph = std::make_unique<BinaryGraph>(n, m);

    for (int i = 0; i < n; ++i) {
        std::string line;
        *input >> line;
        for (int j = 0; j < m; ++j) {
            if (line[j] == '1') {
                graph->setEdge(i, j);
            }
        }
    }

    CycleDetector detector(*graph);
    std::cout << (detector.hasCycle4() ? 1 : 0) << std::endl;

    return 0;
}

