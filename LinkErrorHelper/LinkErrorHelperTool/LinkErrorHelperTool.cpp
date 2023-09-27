// LinkErrorHelperTool.cpp : Defines the exported functions for the DLL.
//

#include "pch.hpp"
#include "LinkErrorHelperTool.hpp"
#include "DumpBin/DumpBinWrapper.hpp"
#include "DumpBin/DumpBinContext.hpp"

LINKERRORHELPERTOOL_API std::logic_error; // avoids a warning but is otherwise useless
#define MY_LOG_LEVEL 100
#define MY_LOG(a_level) if(MY_LOG_LEVEL > (int)a_level) std::cout <<"\n"

namespace link_error_helper 
{

LINKERRORHELPERTOOL_API [[nodiscard]] 
std::vector<std::filesystem::path> GetAllDlls(const std::filesystem::path& a_binPath)
{
    using std::filesystem::path;
    using std::filesystem::recursive_directory_iterator;
    std::vector<path> result;
    // TODO - Find a more sensible way of estimating this value.
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
        result += static_cast<char>(a_folderName.find("src") != std::string::npos);
        result += static_cast<char>(a_folderName.find("Src") != std::string::npos);
        result += static_cast<char>(a_folderName.find("source") != std::string::npos);
        result += static_cast<char>(a_folderName.find("Source") != std::string::npos);
        // implicate cast from char to bool, 0 is false, everything else is true
        if(result)
        {
            // if in a source folder, exit early
            return -1;
        }

        // we are in a second party or thrid party folder
        result += static_cast<char>(a_folderName.find("party") != std::string::npos);
        result += static_cast<char>(a_folderName.find("Party") != std::string::npos);
        result += static_cast<char>(a_folderName.find("Debug") != std::string::npos);
        result += static_cast<char>(a_folderName.find("Release") != std::string::npos);
        result += static_cast<char>(a_folderName.find("x64") != std::string::npos);
        result += static_cast<char>(a_folderName.find("bin") != std::string::npos) * 3;
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
LINKERRORHELPERTOOL_API
std::vector<DllInfo> GetAllExports(const std::vector<std::filesystem::path>& a_allDlls)
{
#if !defined(_WIN32) && !defined(_WIN64)
    throw NotImplemented("Function is not yet implemented on non windows Systems, consider adding it at https://github.com/Python-Dojo/LinkErrorHelper/");
#endif

    std::vector<DllInfo> results;
    results.reserve(a_allDlls.size());

    DumpBinWrapper& dumpBin = DumpBinContext::GetWrapper();

    const auto runDumpBin = [&dumpBin](const std::filesystem::path& a_anyDll, DllInfo& result_info)
    {
        result_info = dumpBin.GetExports(a_anyDll);
    };

    std::transform(std::execution::par_unseq, a_allDlls.begin(), a_allDlls.end(), results.begin(), runDumpBin);

    return results;
}

[[nodiscard]] LINKERRORHELPERTOOL_API
std::vector<std::string> GetSuggestions(const std::vector<DllInfo>& a_allExports)
{
    throw NotImplemented();
    return {};
}

} // end Namespace