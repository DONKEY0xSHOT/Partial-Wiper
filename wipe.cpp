#include "wipe.h"

bool PartialOverwrite(const wchar_t* filePath, int fileSize) {
    if (fileSize < 8) {
        std::cerr << "File size is too small to divide into eighth" << std::endl;
        return false;
    }

    // Handles for file, mapping, and the memory view
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMapFile = NULL;
    LPVOID pMapView = NULL;

    bool bSuccess = false;

    // Open the existing file with read and write access
    hFile = CreateFileW(
        filePath,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateFileW failed. Error: " << GetLastError() << std::endl;
        return false;
    }

    // Create a file mapping object for the file
    hMapFile = CreateFileMappingW(
        hFile,
        NULL,
        PAGE_READWRITE,
        0,
        fileSize,
        NULL
    );

    if (hMapFile == NULL) {
        std::cerr << "CreateFileMappingW failed. Error: " << GetLastError() << std::endl;
        CloseHandle(hFile);
        return false;
    }

    // Map the file into the process's address space
    pMapView = MapViewOfFile(
        hMapFile,
        FILE_MAP_WRITE,
        0,
        0,
        fileSize
    );

    if (pMapView == NULL) {
        std::cerr << "MapViewOfFile failed. Error: " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        CloseHandle(hFile);
        return false;
    }

    // Overwriting with null bytes
    try {

        // Calculate the starting offset and the length to overwrite
        size_t eighthSize = fileSize / 8;
        size_t startOffset = eighthSize * 2;
        size_t bytesToOverwrite = eighthSize*3;


        // Get a pointer to the starting position in the mapped memory
        char* pData = static_cast<char*>(pMapView);
        char* pStartOverwrite = pData + startOffset;

        // Overwrite the memory region with null bytes
        RtlSecureZeroMemory(pStartOverwrite, bytesToOverwrite);

        std::cout << "Memory overwrite operation completed." << std::endl;
        bSuccess = true;
    }
    catch (const std::exception& e) {
        std::cerr << "An exception occurred during the memory operation: " << e.what() << std::endl;
        bSuccess = false;
    }


    // Cleanup
    // Unmap the file from the process's address space
    if (!UnmapViewOfFile(pMapView)) {
        std::cerr << "UnmapViewOfFile failed. Error: " << GetLastError() << std::endl;
    }

    // Close the file mapping object handle
    if (!CloseHandle(hMapFile)) {
        std::cerr << "CloseHandle for hMapFile failed. Error: " << GetLastError() << std::endl;
    }

    // Close the file handle
    if (!CloseHandle(hFile)) {
        std::cerr << "CloseHandle for hFile failed. Error: " << GetLastError() << std::endl;
    }

    return bSuccess;
}