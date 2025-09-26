#include "findFiles.h"


std::vector<FileInfo> FindLargestFiles(const std::string& rootPath, const int numberOfFiles) {
    if (numberOfFiles <= 0) {
        return {};
    }

    std::vector<FileInfo> Files;
    const FILETIME oneYearAgo = GetTimeOneYearAgo();

    // Recursively find all files thatwere accessed in the last year
    SearchDirectory(rootPath, true, Files, oneYearAgo);

    // Sort the files by size
    std::sort(Files.begin(), Files.end(), [](const FileInfo& a, const FileInfo& b) {
        return a.size > b.size;
        });

    // Resize the files vector to the desired number of files
    if (Files.size() > static_cast<size_t>(numberOfFiles)) {
        Files.resize(numberOfFiles);
    }

    return Files;
}

void SearchDirectory(
    const std::string& directory,
    bool isRoot,
    std::vector<FileInfo>& foundFiles,
    const FILETIME& oneYearAgo)
{
    std::string searchPath = directory + "\\*";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        const std::string fileName = findData.cFileName;
        if (fileName == "." || fileName == "..") {
            continue;
        }

        std::string fullPath = directory + "\\" + fileName;

        // If it's a directory, search it recursively
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            SearchDirectory(fullPath, false, foundFiles, oneYearAgo);
        }
        // If it's a file and not in the immediate root directory
        else if (!isRoot) {

            // Check if the last access time is within the last year
            if (CompareFileTime(&findData.ftLastAccessTime, &oneYearAgo) >= 0) {
                ULARGE_INTEGER fileSize;
                fileSize.LowPart = findData.nFileSizeLow;
                fileSize.HighPart = findData.nFileSizeHigh;

                // Add the file info to our vector
                foundFiles.push_back({ fullPath, fileSize.QuadPart });
            }
        }
    } while (FindNextFileA(hFind, &findData) != 0);

    FindClose(hFind);
}

FILETIME GetTimeOneYearAgo() {
    FILETIME ftNow;
    GetSystemTimeAsFileTime(&ftNow);

    ULARGE_INTEGER uliNow;
    uliNow.LowPart = ftNow.dwLowDateTime;
    uliNow.HighPart = ftNow.dwHighDateTime;

    // A yearin 100-nanosec intervals
    const unsigned long long intervalsPerYear = 365ULL * 24ULL * 60ULL * 60ULL * 10000000ULL;

    uliNow.QuadPart -= intervalsPerYear;

    FILETIME oneYearAgo;
    oneYearAgo.dwLowDateTime = uliNow.LowPart;
    oneYearAgo.dwHighDateTime = uliNow.HighPart;

    return oneYearAgo;
}