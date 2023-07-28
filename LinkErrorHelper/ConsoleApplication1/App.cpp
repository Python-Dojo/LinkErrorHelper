// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>

#include "LinkErrorHelperTool.h"

int main()
{
    using namespace link_error_helper;
    using std::filesystem::path;
    const path binPath = GetBinPath(path("../"));
    const std::vector<path> allPaths = GetAllDlls(binPath);
    const std::vector<DllInfo> allExports = GetAllExports(allPaths);
    //const auto suggestions = link_error_helper::GetSuggestions(allExports);

    // TODO Link to the tool, call most of the functions in the right order and return a suggestion
    std::cout << "Hello World!\n";
    for (const auto& dll : allExports)
    {
        std::cout << dll.m_dllName;
        for (const auto& exports : dll.m_dllExports)
        {
            std::cout << exports;
        }
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
