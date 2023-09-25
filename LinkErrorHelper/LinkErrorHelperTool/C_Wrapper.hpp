#pragma once

#include "LinkErrorHelperAPI.hpp"

#if defined(__cplusplus)
#if __cplusplus >= 202302L
#   warning "The C style API is worse, slower and less well supported. Consider using the C++ style API instead."
#endif
// even though its C symbols we can still use namespaces.
namespace link_error_helper{
// compile all the following symbols as if they were C
extern "C" {
#endif

    LINKERRORHELPERTOOL_API struct LEH_DllInfo
    {
        const char* m_dllName;
        const char** m_dllExports;
        const int m_lengthOfExports;
    };

    enum LEH_ErrorCode
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
    LEH_ErrorCode LEH_GetAllDlls(const char* a_binPath_in, char** a_dllNames_out, int* a_lengthOfDllNames_out);

    LINKERRORHELPERTOOL_API
    LEH_ErrorCode LEH_GetBinPath(const char* a_rootDir_in, const char** a_binPath_out);

    LINKERRORHELPERTOOL_API
    LEH_ErrorCode LEH_GetAllExports(const char** a_allDllPaths_in, const int a_lengthOfDllPaths, LEH_DllInfo* a_dllInfo_out, int* a_lengthOfDllInfo_out);

    LINKERRORHELPERTOOL_API
    LEH_ErrorCode LEH_GetSuggestions(const LEH_DllInfo* a_allExports_in, const int a_lengthOfExports , char** a_suggestions_out, int* a_lengthOfSuggestions_out);

#if defined(__cplusplus)
} // end extern C 
} // end namespace
#endif