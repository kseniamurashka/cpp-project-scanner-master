#include <iostream>
#include <fstream>
#include <iterator>

#include <string>
#include <vector>

#include <algorithm>
#include <ranges>
#include <format>
#include <filesystem>
#include <ranges>

namespace fs = std::filesystem;

typedef struct {     
    std::string name;     
    std::uintmax_t size;
    int length;
} File;

std::vector<std::string> cppExtentions = {".cpp", ".cc", ".cxx", ".h", ".hh", ".hpp", ".hxx"};

int main(int argc, char** argv) 
{
    std::string filpath = argc > 1 ? argv[1] : ".";

    if (!fs::exists(filpath)) {
        std::cerr << "Error checking directory existence: " << std::endl;
        return -1;
    }

    std::vector<File> cppFiles;
    for (const auto& entry : fs::recursive_directory_iterator(filpath)) {
        fs::path path = entry.path();
        fs::path extention = path.extension();

        if (!extention.string().length() || std::find_if(cppExtentions.begin(), cppExtentions.end(), [=](const auto& cppExt) {return extention == cppExt;}) == cppExtentions.end())
            continue;

        std::uintmax_t size = fs::file_size(path);
        std::ifstream openedFile(path);   
        if (!openedFile.is_open()) {
            std::cerr << "Error opening file" << std::endl;
            return -1;
        }
        openedFile.unsetf(std::ios_base::skipws);
        int lenght = std::count(std::istream_iterator<char>(openedFile), std::istream_iterator<char>(), '\n');
        
        if (openedFile.peek() == EOF && lenght > 0) lenght++;
        else if (openedFile.peek() != EOF) lenght++;

        File file{path.filename().string(), size, lenght};
        cppFiles.push_back(file);
    }

    if (cppFiles.size() == 0) {
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