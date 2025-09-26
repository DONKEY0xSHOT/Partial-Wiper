#pragma once
#include <string>
#include <vector>
#include <queue>
#include <Windows.h>
#include <iostream>
#include <algorithm>
#include <stdexcept>

// Struct for info about a file
struct FileInfo {
    std::string path;
    unsigned long long size;
};

// Find the paths of the N largest files in a directory recursively, where numberOfFiles is N
std::vector<FileInfo> FindLargestFiles(const std::string& rootPath, const int numberOfFiles);

// Recursively searches a directory for files
void SearchDirectory(
    const std::string& directory,
    bool isRoot,
    std::vector<FileInfo>& foundFiles,
    const FILETIME& oneYearAgo
);

// Get the time one year ago
FILETIME GetTimeOneYearAgo();