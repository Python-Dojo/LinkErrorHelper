// LinkErrorHelperTool.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "LinkErrorHelperTool.h"

#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <array>
#include <execution>
#include <algorithm>
#include <vector>
#include <filesystem>

#define MY_LOG_LEVEL 100
#define MY_LOG(a_level, a_message) if(MY_LOG_LEVEL > (int)a_level) std::cout << a_message <<"\n"

std::string ExecuteProcess(const char* a_cmd) 
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(a_cmd, "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("failed to run command");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

[[nodiscard]] bool IsDir(const std::filesystem::path& a_path)
{
    if (!std::filesystem::exists(a_path))
    {
        return false;
    }
    return std::filesystem::is_directory(a_path);
}


// This is an example of an exported variable
LINKERRORHELPERTOOL_API [[nodiscard]] 
std::vector<std::filesystem::path> GetAllDlls(const std::filesystem::path& a_binPath)
{
    using std::filesystem::path;
    using std::filesystem::directory_iterator;
    std::vector<path> result;
    // must have at least 2 binaries to be getting link errors 
    // TODO - Find a more sensible way of estimating this value
    result.reserve(2);
    for (const auto& dll : directory_iterator(a_binPath))
    {
        result.push_back(dll.path());
    }
    return result;
}

LINKERRORHELPERTOOL_API [[nodiscard]] 
std::filesystem::path GetBinPath(const std::filesystem::path& a_rootDir)
{
    using std::filesystem::path;
    using std::filesystem::exists;

    const auto BinSuggestion = [](const std::string& a_folderName) -> int
    {
        char result = 0;
        // in a source folder
        result += /*static_cast<char>*/(a_folderName.find("src") != std::string::npos);
        result += (a_folderName.find("Src") != std::string::npos);
        result += (a_folderName.find("source") != std::string::npos);
        result += (a_folderName.find("Source") != std::string::npos);
        // implicate cast from char to bool, 0 is false, everything else is true
        if(result)
        {
            // if in a source folder, exit early
            return -1;
        }

        // we are in a second party or thrid party folder
        result += (a_folderName.find("party") != std::string::npos);
        result += (a_folderName.find("Party") != std::string::npos);
        result += (a_folderName.find("bin") != std::string::npos);
        result += (a_folderName.find("Debug") != std::string::npos);
        result += (a_folderName.find("Release") != std::string::npos);
        result += (a_folderName.find("x64") != std::string::npos);
        return result;
    };

    std::filesystem::path childFolder;
    for (const auto& item : std::filesystem::directory_iterator(a_rootDir))
    {
        int confidence = 0;
        if(!item.is_directory())
        {
            continue;
        }

        confidence += BinSuggestion(item.path().filename().string()) * 5;
        
        for(const auto& subItem : std::filesystem::recursive_directory_iterator(item))
        {
            if(subItem.is_directory())
            {
                confidence += BinSuggestion(subItem.path().filename().string()) * 3;
                continue;
            }
            const std::string& extenstion = subItem.path().extension().string();
            // if items that are normally in the bin folder are in this folder, we are probably in it.
            confidence += (extenstion == "dll" || extenstion == "pdb" || extenstion == "exe") ? +2 : -3;

            if (confidence > 10)
            {
                childFolder = subItem;
                break;
            }
            
            if(confidence < -10)
            {
                break;
            }
        }
        
        if (confidence > 10)
        {
            break;
        }

    }
        
    return childFolder;
}

LINKERRORHELPERTOOL_API [[nodiscard]]
std::vector<std::string> GetAllExports(const std::vector<std::filesystem::path>& a_allBinDirs)
{

}

LINKERRORHELPERTOOL_API [[nodiscard]]
std::vector<std::string> GetSuggestions(const std::vector<std::string>& a_allExports)
{
    std::string visualStudioPath = ExecuteProcess("\"C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe\" -property installationPath");
    MY_LOG(90, visualStudioPath);
    std::string dumpBinPath = "\"" + visualStudioPath + "\\VC\\Tools\\MSVC\\14.29.30133\\bin\\Hostx64\\x64\\dumpbin.exe\"";

    std::vector<std::string> allDlls = {"1","2", "3", "4"};

    const auto func = [](std::string a_dll){std::cout << a_dll;};

    std::for_each(std::execution::par_unseq, allDlls.begin(), allDlls.end(), func);
}