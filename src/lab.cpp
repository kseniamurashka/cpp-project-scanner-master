#include <iostream>
#include <fstream>
#include <iterator>

#include <string>
#include <vector>
#include <tuple>

#include <algorithm>
#include <ranges>
#include <optional>
#include <format>
#include <filesystem>
#include <ranges>

namespace fs = std::filesystem;

typedef struct {     
    std::string name;     
    int size;
    int length;
} File;

std::vector<std::string> cppExtentions = {".cpp", ".cc", ".cxx", ".h", ".hh", ".hpp", ".hxx"};

std::optional<std::tuple<std::string, int, int>> getFileInfo(fs::directory_entry entry) {
    fs::path path = entry.path();
    fs::path extention = path.extension();

    if (!extention.string().length() || std::find_if(cppExtentions.begin(), cppExtentions.end(), [=](const auto& cppExt) {return extention == cppExt;}) == cppExtentions.end())
        return std::nullopt;

    std::ifstream openedFile(path);   
    if (!openedFile.is_open()) {
        return std::nullopt;
    }
    openedFile.unsetf(std::ios_base::skipws);

    int size = fs::file_size(path);
    int length = std::count(std::istream_iterator<char>(openedFile), std::istream_iterator<char>(), '\n');
    if (openedFile.peek() == EOF && length > 0) length++;
    else if (openedFile.peek() != EOF) length++;

    return std::make_tuple(path.filename().string(), size, length);
}

int main(int argc, char** argv) {
    std::string filpath = argc > 1 ? argv[1] : ".";

    if (!fs::exists(filpath)) {
        std::cerr << "Error checking directory existence: " << std::endl;
        return -1;
    }

    std::vector<File> cppFiles;
    for (const auto& entry : fs::recursive_directory_iterator(filpath)) {
        std::optional<std::tuple<std::string, int, int>> info = getFileInfo(entry);
        if (info.has_value()) {
            auto& t = info.value();
            File file{std::get<0>(t), std::get<1>(t), std::get<2>(t)};
            cppFiles.push_back(file);
        }
    }

    if (cppFiles.empty()) {
        std::cout << "Total C++ files: 0\nTotal lines of code: 0\nTotal size: 0 bytes\n\nLargest file: - (0 bytes)\nLongest file: - (0 lines)\n";
        return 0;
    }

    std::uintmax_t totalSize = 0;
    int totalLenght = 0;
    std::ranges::for_each(cppFiles, [&](File file) {
        totalSize += file.size;
        totalLenght += file.length;
    });

    auto largestFile = std::ranges::max_element(cppFiles, [&](File& file1, File& file2) { return file1.size < file2.size; });
    auto longestFile = std::ranges::max_element(cppFiles, [&](File& file1, File& file2) { return file1.length < file2.length; });

    std::cout << std::format("Total C++ files: {}\nTotal lines of code: {}\nTotal size: {} bytes\n\n", cppFiles.size(), totalLenght, totalSize);
    std::cout << std::format("Largest file: {} ({} bytes)\nLongest file: {} ({} lines)\n", largestFile->name, largestFile->size, longestFile->name, longestFile->length);
   
    return 0;
}