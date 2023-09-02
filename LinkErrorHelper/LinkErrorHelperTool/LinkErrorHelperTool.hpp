// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the LINKERRORHELPERTOOL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// LINKERRORHELPERTOOL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include "LinkErrorHelperAPI.hpp"

// Some people don't like using STL through DLLs, let's be nice and take a "you do you attitude" 
#if !LINK_ERROR_HELPER_USE_C_STYLE_API
#include <vector>
#include <string>
#include <stdexcept> // For logic_error

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

}

#if __cplusplus > 201402L  // CPP 17 or greater
#include <filesystem>

namespace link_error_helper
{

/// If no Bin path was given, try and find one
[[nodiscard]] LINKERRORHELPERTOOL_API  
::std::filesystem::path GetBinPath(const std::filesystem::path& a_rootDir);

/// Gets all the Dlls from a specifed path
[[nodiscard]] LINKERRORHELPERTOOL_API
::std::vector<std::filesystem::path> GetAllDlls(const std::filesystem::path& a_binPath);

/// Gets everything exported by all Dlls
[[nodiscard]] LINKERRORHELPERTOOL_API
::std::vector<DllInfo> GetAllExports(const std::vector<std::filesystem::path>& a_allDlls);
    
    // define nodiscard if we have it, has to be in an if block to avoid errors on C++ 14 
    #define LEH_NO_DISCARD [[nodiscard]]
#else 
    #define LEH_NO_DISCARD
#endif // end if C++ 17 


/// Gives back suggestions the tool came up with
LEH_NO_DISCARD LINKERRORHELPERTOOL_API
::std::vector<std::string> GetSuggestions(const std::vector<DllInfo>& a_allExports, const std::string& a_errorMessage);

/// Makes the error message smaller, easier to read and easier to understand
LEH_NO_DISCARD LINKERRORHELPERTOOL_API
::std::string ParseErrorMessage(const ::std::string& a_errorMessage);

// C++ 14 API (otherwise Handy overloads if we have 17)

// Not yet implemented
LEH_NO_DISCARD LINKERRORHELPERTOOL_API
std::vector<std::string> GetAllDlls(const std::string& a_binPath);

LEH_NO_DISCARD LINKERRORHELPERTOOL_API
std::string GetBinPath(const std::string& a_rootDir);

LEH_NO_DISCARD LINKERRORHELPERTOOL_API
std::vector<DllInfo> GetAllExports(const std::vector<std::string>& a_allDlls);


#else // C style May work with C++ 98 if ABI issues are solved by user, but is not a maintained feature
    // Not yet implemented -- and not recommended

_EXTERN_C

    enum ErrorCode
    {
        // Success with no warnings
        Success = 0, 
        // Errors are negative
        UnknownFail = -1, 
        NotImpleneted = -2,
        // Warnings are positive (higher numbers are more sever)
        Warning = 1 
    };

    LINKERRORHELPERTOOL_API struct LEH_DllInfo
    {
        const char* m_dllName;
        const int m_lengthOfExports;
        const char ** m_dllExports;
    };

    LINKERRORHELPERTOOL_API
    ErrorCode LEH_GetAllDlls(const char* a_binPath, char** a_dllNames, int* a_lengthOfDllNames);

    LINKERRORHELPERTOOL_API
    ErrorCode LEH_GetBinPath(const char* a_rootDir, char*);

    LINKERRORHELPERTOOL_API
    ErrorCode LEH_GetAllExports(const char** a_allDllPaths, const int a_lengthOfDllPaths, LEH_DllInfo* a_dllInfo, int* a_lengthOfDllInfo);

    LINKERRORHELPERTOOL_API
    ErrorCode LEH_GetSuggestions(const DllInfo* a_allExports, const int a_lengthOfExports , char** a_suggestions, int* a_lengthOfSuggestions);

_END_EXTERN_C

#endif // C

} // end namespace