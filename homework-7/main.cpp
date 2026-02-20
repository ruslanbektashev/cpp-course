#include <algorithm>
#include <cctype>
#include <condition_variable>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

namespace {

struct Config {
    std::size_t threads = 1;
    std::size_t top = 20;
    std::size_t minlen = 3;
    fs::path input_dir;
};

class TaskQueue {
public:
    void push(fs::path path) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(path));
        }
        cv_.notify_one();
    }

    bool pop(fs::path& out) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return closed_ || !queue_.empty(); });
        if (queue_.empty()) {
            return false;
        }
        out = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    void close() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            closed_ = true;
        }
        cv_.notify_all();
    }

private:
    std::queue<fs::path> queue_;
    bool closed_{false};
    std::mutex mutex_;
    std::condition_variable cv_;
};

bool is_word_char(unsigned char c) {
    return std::isalnum(c) != 0 || c == '_';
}

void flush_word(std::string& token, std::size_t minlen, std::unordered_map<std::string, std::uint64_t>& local_counts) {
    if (token.size() >= minlen) {
        ++local_counts[token];
    }
    token.clear();
}

void process_line(const std::string& line, std::size_t minlen, std::unordered_map<std::string, std::uint64_t>& local_counts) {
    std::string token;
    token.reserve(64);

    for (unsigned char c : line) {
        if (is_word_char(c)) {
            token.push_back(static_cast<char>(std::tolower(c)));
        } else if (!token.empty()) {
            flush_word(token, minlen, local_counts);
        }
    }

    if (!token.empty()) {
        flush_word(token, minlen, local_counts);
    }
}

void process_file(const fs::path& file_path,
                  std::size_t minlen,
                  std::unordered_map<std::string, std::uint64_t>& local_counts) {
    std::ifstream file(file_path);
    if (!file) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        process_line(line, minlen, local_counts);
    }
}

void merge_local(std::unordered_map<std::string, std::uint64_t>& global_counts,
                 std::mutex& global_mutex,
                 std::unordered_map<std::string, std::uint64_t>& local_counts) {
    std::lock_guard<std::mutex> lock(global_mutex);
    for (auto& [word, count] : local_counts) {
        global_counts[word] += count;
    }
}

Config parse_args(int argc, char* argv[]) {
    Config cfg;
    std::vector<std::string> positional;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--threads") {
            if (i + 1 >= argc) {
                throw std::runtime_error("Missing value for --threads");
            }
            cfg.threads = static_cast<std::size_t>(std::stoul(argv[++i]));
            continue;
        }
        if (arg == "--top") {
            if (i + 1 >= argc) {
                throw std::runtime_error("Missing value for --top");
            }
            cfg.top = static_cast<std::size_t>(std::stoul(argv[++i]));
            continue;
        }
        if (arg == "--minlen") {
            if (i + 1 >= argc) {
                throw std::runtime_error("Missing value for --minlen");
            }
            cfg.minlen = static_cast<std::size_t>(std::stoul(argv[++i]));
            continue;
        }

        positional.push_back(arg);
    }

    if (cfg.threads == 0 || cfg.top == 0 || cfg.minlen == 0) {
        throw std::runtime_error("--threads, --top and --minlen must be >= 1");
    }
    if (positional.size() != 1) {
        throw std::runtime_error("Usage: ./homework_7 --threads K --top M --minlen L <path>");
    }

    cfg.input_dir = fs::path(positional.front());
    return cfg;
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        const Config cfg = parse_args(argc, argv);

        if (!fs::exists(cfg.input_dir) || !fs::is_directory(cfg.input_dir)) {
            std::cerr << "Input path is not a directory: " << cfg.input_dir << '\n';
            return 1;
        }

        TaskQueue queue;
        std::unordered_map<std::string, std::uint64_t> global_counts;
        std::mutex global_mutex;

        std::thread producer([&queue, &cfg] {
            for (const auto& entry : fs::directory_iterator(cfg.input_dir)) {
                if (entry.is_regular_file()) {
                    queue.push(entry.path());
                }
            }
            queue.close();
        });

        std::vector<std::thread> workers;
        workers.reserve(cfg.threads);

        for (std::size_t i = 0; i < cfg.threads; ++i) {
            workers.emplace_back([&queue, &global_counts, &global_mutex, &cfg] {
                std::unordered_map<std::string, std::uint64_t> local_counts;
                fs::path file_path;
                while (queue.pop(file_path)) {
                    process_file(file_path, cfg.minlen, local_counts);
                }
                merge_local(global_counts, global_mutex, local_counts);
            });
        }

        producer.join();
        for (auto& worker : workers) {
            worker.join();
        }

        std::vector<std::pair<std::string, std::uint64_t>> entries;
        entries.reserve(global_counts.size());
        for (auto& [word, count] : global_counts) {
            entries.emplace_back(word, count);
        }

        std::sort(entries.begin(), entries.end(), [](const auto& lhs, const auto& rhs) {
            if (lhs.second != rhs.second) {
                return lhs.second > rhs.second;
            }
            return lhs.first < rhs.first;
        });

        const std::size_t limit = std::min(cfg.top, entries.size());
        for (std::size_t i = 0; i < limit; ++i) {
            std::cout << entries[i].first << ' ' << entries[i].second << '\n';
        }

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
