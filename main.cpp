#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <csignal>
#include <vector>
#include <bitset>

std::string humanReadableSize(size_t bytes) {
    const char *sizes[] = {"B", "KB", "MB", "GB", "TB"};
    int i = 0;
    auto dblBytes = bytes;

    while (dblBytes >= 1024 && i < (sizeof(sizes) / sizeof(*sizes)) - 1) {
        dblBytes /= 1024;
        ++i;
    }

    std::ostringstream out;
    out << dblBytes << "" << sizes[i];
    return out.str();
}

bool endsWith(const std::string &str, const std::string &suffix) {
    if (suffix.size() > str.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

//https://man7.org/linux/man-pages/man5/proc_pid_pagemap.5.html
constexpr auto kPmPresent = 63;
constexpr auto kPmSwap = 62;
constexpr auto kPmFile = 61;
constexpr auto kPmMapExclusive = 56;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: lsmaps PID\n");
        return 1;
    }
    auto pid = std::atoi(argv[1]);
    auto mapsfile = "/proc/" + std::to_string(pid) + "/maps";
    auto pagemapfile = "/proc/" + std::to_string(pid) + "/pagemap";
    std::ifstream file(mapsfile); // Open the file
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << mapsfile << std::endl;
        return 1;
    }

    std::string line;
    const auto pagesize = sysconf(_SC_PAGESIZE);
    FILE *pmf = fopen(pagemapfile.c_str(), "rb");
    if (!pmf) {
        std::cerr << "Unable to open file: " << pagemapfile << std::endl;

        return 2;
    }

    std::vector<uint64_t> mem{};

    printf("VSZ\tRAM\tSWAP\tLine\n");
    while (std::getline(file, line)) { // Read file line by line
        uint64_t start = 0;
        uint64_t end = 0;
        sscanf(line.data(), "%lx-%lx", &start, &end);
        auto start_page = start / pagesize;
        auto end_page = end / pagesize;
        auto pages = end_page - start_page;
        if (0 != fseek(pmf, start_page * 8, SEEK_SET)) {
            perror("fseek");
            return 3;
        }
        mem.clear();
        if (mem.size() != pages) {
            mem.resize(pages);
        }
        if (1 != fread(mem.data(), mem.size() * sizeof(uint64_t), 1, pmf)) {
            return 4;
        }
        size_t inRam = 0;
        size_t inSwap = 0;
        for (const auto &pagedesc: mem) {
            const std::bitset<64> bits{pagedesc};
            inRam += bits[kPmPresent];
            inSwap += bits[kPmSwap];
//                auto str_ = bits.to_string();
//                printf("\t%s\n", str_.c_str());
        }
        auto hram = humanReadableSize(inRam * pagesize);
        auto hswap = humanReadableSize(inSwap * pagesize);


        auto sz = end - start;
        auto hsz = humanReadableSize(sz);
        while (hram.length() < 4) {
            hram += " ";
        }
        while (hswap.length() < 4) {
            hswap += " ";
        }
        while (hsz.length() < 4) {
            hsz += " ";
        }
        printf("%s\t%s\t%s\t%s\n", hsz.c_str(), hram.c_str(), hswap.c_str(), line.c_str());
//        std::cout << line << std::endl; // Print each line

    }

    file.close(); // Close the file
    return 0;
}