#pragma once
#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <fstream>

// Patrially wipe the file
bool PartialOverwrite(const wchar_t* filePath, int fileSize);