#include "pch.hpp"
#include "DumpBinWrapper.hpp"

#if defined(_WIN32) || defined(_WIN64) || defined(WINDOWS_BUILD)

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



// TODO Store vs and dumpbin path in an exported struct so we don't have to get it every time
// TODO Use windows hackery to find the program files path (may not be on C drive)
std::string visualStudioPath = ExecuteProcess("\"C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe\" -property installationPath").str();
// remove the \n
visualStudioPath = visualStudioPath.substr(0, visualStudioPath.length()-1);
MY_LOG(90) << visualStudioPath;
std::string dumpBinCommand = "\"\"" + visualStudioPath + "\\VC\\Tools\\MSVC\\14.29.30133\\bin\\Hostx64\\x64\\dumpbin.exe\" /exports ";
MY_LOG(60) << dumpBinCommand; 


#endif // Windows