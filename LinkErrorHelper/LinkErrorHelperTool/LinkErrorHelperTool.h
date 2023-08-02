// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the LINKERRORHELPERTOOL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// LINKERRORHELPERTOOL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifndef LINK_TO_LINK_ERROR_HELPER_STATICALLY // if linking statically ( built as lib) avoid exports
    // Define normal dll export macros
    #ifdef LINKERRORHELPERTOOL_EXPORTS
        #define LINKERRORHELPERTOOL_API __declspec(dllexport)
    #else
        #define LINKERRORHELPERTOOL_API __declspec(dllimport)
    #endif
#else 
    #define LINKERRORHELPERTOOL_API // No exports
#endif

// Some people don't like using STL through DLLs, let's be nice and take a "you do you attitude" 
#ifndef LINK_ERROR_HELPER_USE_C_STYLE_API
#include <vector>
#include <string>
#include <stdexcept> // For logic_error

#if __cplusplus > 201402L  // 17 or greater
    #include <filesystem>
#endif

namespace link_error_helper
{
/// Contains a dll's name and all the exports
LINKERRORHELPERTOOL_API struct DllInfo
{
    ::std::string m_dllName;
    ::std::vector<::std::string> m_dllExports;
};

// Exported so people can catch it
/// Function is not yet supported (like on Linux systems)
LINKERRORHELPERTOOL_API class NotImplemented : public ::std::logic_error
{
public:
    NotImplemented(const ::std::string& a_message = "") : ::std::logic_error("Function not yet implemented" + a_message) {};
};
#if __cplusplus > 201402L  // CPP 17 or greater
    
    [[nodiscard]] LINKERRORHELPERTOOL_API
    std::vector<std::filesystem::path> GetAllDlls(const std::filesystem::path& a_binPath);

    // if no Bin path was given, try and find one
    [[nodiscard]] LINKERRORHELPERTOOL_API  
    std::filesystem::path GetBinPath(const std::filesystem::path& a_rootDir);

    // Nice to have overload 
    [[nodiscard]]
    std::filesystem::path GetBinPath(const std::string& a_pathToRootDir){return GetBinPath(std::filesystem::path(a_pathToRootDir));}

    [[nodiscard]] LINKERRORHELPERTOOL_API
    std::vector<DllInfo> GetAllExports(const std::vector<std::filesystem::path>& a_allDlls);

    [[nodiscard]] LINKERRORHELPERTOOL_API
    std::vector<std::string> GetSuggestions(const std::vector<DllInfo>& a_allExports);


#else // CPP 14 API (We still compile with 17)

    // Not yet implemented
    LINKERRORHELPERTOOL_API
    std::vector<std::string> GetAllDlls(const std::string& a_binPath);

    // if no Bin path was given, try and find one
    LINKERRORHELPERTOOL_API
    std::string GetBinPath(const std::string& a_rootDir);

    LINKERRORHELPERTOOL_API
    std::vector<DllInfo> GetAllExports(const std::vector<std::string>& a_allDlls);

    LINKERRORHELPERTOOL_API
    std::vector<std::string> GetSuggestions(const std::vector<DllInfo>& a_allExports);

#endif





#else // C style May work with C++ 98 if ABI issues are solved by user, but is not a maintained feature
    // Not yet implemented -- Consider implementing as extern "C" and mark it as such here

    enum class ErrorCode
    {
        // No warnings success is 0
        Success = 0, 
        // Errors are negative
        UnknownFail = -1, 
        NotImpleneted = -2,
        // Warnings are positive (higher number is more sever)
        Warning = 1 
    };

    LINKERRORHELPERTOOL_API struct C_DllInfo
    {
        const char* m_dllName;
        const char ** m_dllExports;
    };

    LINKERRORHELPERTOOL_API
    ErrorCode GetAllDlls(const char* a_binPath, char** a_dllNames, int* a_lengthOfDllNames);

    LINKERRORHELPERTOOL_API
    ErrorCode GetBinPath(const char* a_rootDir, char*);

    LINKERRORHELPERTOOL_API
    ErrorCode GetAllExports(const char** a_allDlls, C_DllInfo* a_dllInfo, int* a_lengthOfDllInfo);

    LINKERRORHELPERTOOL_API
    ErrorCode GetSuggestions(const DllInfo* a_allExports, char** a_suggestions, int* a_lengthOfSuggestions);

#endif // C

} // end namespace