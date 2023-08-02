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
#include <sstream>

#define MY_LOG_LEVEL 100
#define MY_LOG(a_level) if(MY_LOG_LEVEL > (int)a_level) std::cout <<"\n"


namespace link_error_helper 
{
namespace /*anonymous*/ {

[[nodiscard]] std::stringstream ExecuteProcess(const std::string& a_cmd) 
{
    std::array<char, 128> buffer;
    std::stringstream result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(a_cmd.c_str(), "r"), _pclose);
    // if we got a nullptr, return (or throw) instead of crashing
    if (!pipe) 
    {
        result << "Failed to get exports";
        return result;
        // We don't expect this to fail currently so no point in throwing and catching
        // throw std::runtime_error("failed to run command");
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) 
    {
        result << buffer.data();
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

} // end anonymous namespace

LINKERRORHELPERTOOL_API [[nodiscard]] 
std::vector<std::filesystem::path> GetAllDlls(const std::filesystem::path& a_binPath)
{
    using std::filesystem::path;
    using std::filesystem::recursive_directory_iterator;
    std::vector<path> result;
    // TODO - Find a more sensible way of estimating this value (must have at least 2 binaries to be getting link errors)
    result.reserve(2);
    for (const auto& dll : recursive_directory_iterator(a_binPath))
    {
        if (!dll.is_regular_file())
        {
            continue; 
        }

        const path& extenstion = dll.path().filename().extension();
        if (extenstion != ".dll" && extenstion != ".lib")
        {
            // Skip things that don't export functions (and badly named dlls)
            // TODO Find if user wants something else (.so/.a on linux, .pyd files in python)
            continue;
        }

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

    std::filesystem::directory_entry childFolder;
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
            confidence += 
                (extenstion == ".dll" || extenstion == ".pdb" || extenstion == ".exe" || extenstion == ".pdb" || extenstion == ".lib") ? +5 : -3;

            if (confidence > 10)
            {
                childFolder = subItem;
                break;
            }
            
            if(confidence < -10)
            {
                std::cout << subItem << " was not the bin dir\n";
                break;
            }
        }
        
        if (confidence > 10)
        {
            break;
        }

    }

    std::cout << childFolder << "\n";

    if(childFolder.is_directory())
    {
        return path(childFolder.path());
    }

    return path(childFolder.path().parent_path());

}


// TODO return vector of DllInfo instead of strings
[[nodiscard]] LINKERRORHELPERTOOL_API
std::vector<DllInfo> GetAllExports(const std::vector<std::filesystem::path>& a_allDlls)
{
#if defined(_WIN32) || defined(_WIN64)
    // TODO Store vs and dumpbin path in an exported struct so we don't have to get it every time
    // TODO Use windows hackery to find the program files path (may not be on C drive)
    std::string visualStudioPath = ExecuteProcess("\"C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe\" -property installationPath").str();
    // remove the \n
    visualStudioPath = visualStudioPath.substr(0, visualStudioPath.length()-1);
    MY_LOG(90) << visualStudioPath;
    std::string dumpBinCommand = "\"\"" + visualStudioPath + "\\VC\\Tools\\MSVC\\14.29.30133\\bin\\Hostx64\\x64\\dumpbin.exe\" /exports ";
    MY_LOG(60) << dumpBinCommand; 
#else
    throw NotImplemented("Function is not yet implemented on non windows Systems, consider adding it at https://github.com/Python-Dojo/LinkErrorHelper/");
#endif
    using std::filesystem::path;

    std::vector<DllInfo> dlls;
    dlls.reserve(a_allDlls.size());

    const auto RunDumpBin = [&dumpBinCommand, &dlls](path a_dllPath) -> void
    {
        if (!std::filesystem::exists(a_dllPath))
        {
            MY_LOG(0) << "ERROR Could not find bin : " << a_dllPath << "\nPlease make sure it exists";
            // Skip this item since it doesn't exist
            return;
        }
        std::string command = dumpBinCommand + "\"" + a_dllPath.string() + "\"";
        std::stringstream exports = ExecuteProcess(command);
        DllInfo dllInfo{a_dllPath.filename().string(), {} }; 
        std::string anExport;
        dllInfo.m_dllExports.reserve(10); // Number of exported items in a dll, ten seems a good lower bound
        // easiest thing to search for before dlls, if RVA gets long dumpbin may add more spaces, needs testing
            // We currently shorten the string until we get somewhere to avoid this
        std::string startString = "ordinal hint RVA      name";
        bool completed = false;
        while(!completed)
        {
            bool started = false; 
            while( std::getline (exports, anExport, '\n'))
            {
                // Skip everything before exports
                if (!started)
                {
                    if (anExport.find(startString) != std::string::npos)
                    {
                        started = true;
                    }
                    continue;
                }
                else if (completed) continue; // if we are done skip everything
                else if (!completed)
                {
                    // Check if this is the line after exports (tells us if we are done)
                    if (anExport.find("Summary") != std::string::npos)
                    {
                        completed = true;
                        break;
                    }
                }

                // We found an export
                // if our export empty string or just spaces, skip it.
                size_t workingCharacter = anExport.find_first_not_of(" ");
                if (workingCharacter == std::string::npos)
                {
                    continue;
                }

                // first non space char is our export number so we don't need it
                workingCharacter = anExport.find_first_not_of(" ", workingCharacter+1);
                // second non space char is our "hint" (zero indexed export number?) so we don't need it
                workingCharacter = anExport.find_first_not_of(" ", workingCharacter+1);
                // third non space char is "RVA" (looks like a hash of the symbol or a pointer) so we don't need it
                workingCharacter = anExport.find(" ", workingCharacter + 2);

                // Strip the first bit of symbol that we don't care about then push it back to return it
                dllInfo.m_dllExports.push_back(anExport.substr(workingCharacter));
            }

            // check we hit something interesting
            if (started)
            {
                dlls.push_back(dllInfo);
                // We are done 
                return;
            } 

            // If we have failed return.
            if (6 > startString.length()) return;
            // if we haven't
            startString = startString.substr(0, startString.length() - 4);
        }
    };

    std::for_each(std::execution::par_unseq, a_allDlls.begin(), a_allDlls.end(), RunDumpBin);

    return dlls;
}

[[nodiscard]] LINKERRORHELPERTOOL_API
std::vector<std::string> GetSuggestions(const std::vector<DllInfo>& a_allExports)
{
    throw NotImplemented();
    return {};
}

} // end Namespace