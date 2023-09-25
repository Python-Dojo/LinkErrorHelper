
#include "C_Wrapper.hpp"
#include "LinkErrorHelperTool.hpp"

#if !defined(__cplusplus)
// Since this is compiled this should always be defined
#   error "Please compile with C++ not C!"
#endif


namespace link_error_helper{
// compile all the following symbols as if they were C (so that they can be used in C)
extern "C" {

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
    LEH_ErrorCode LEH_GetAllDlls(const char* a_binPath_in, char** a_dllNames_out, int* a_lengthOfDllNames_out)
    {
        return LEH_ErrorCode::NotImpleneted;
    }

    LINKERRORHELPERTOOL_API
    LEH_ErrorCode LEH_GetBinPath(const char* a_rootDir_in, const char** a_binPath_out)
    {
        return LEH_ErrorCode::NotImpleneted;
    }

    LINKERRORHELPERTOOL_API
    LEH_ErrorCode LEH_GetAllExports(const char** a_allDllPaths_in, const int a_lengthOfDllPaths, LEH_DllInfo* a_dllInfo_out, int* a_lengthOfDllInfo_out)
    {
        return LEH_ErrorCode::NotImpleneted;
    }

    LINKERRORHELPERTOOL_API
    LEH_ErrorCode LEH_GetSuggestions(const LEH_DllInfo* a_allExports_in, const int a_lengthOfExports , char** a_suggestions_out, int* a_lengthOfSuggestions_out)
    {
        return LEH_ErrorCode::NotImpleneted;
    }

} // end extern C 
} // end namespace
