// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the LINKERRORHELPERTOOL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// LINKERRORHELPERTOOL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#pragma once

#include "LinkErrorHelperAPI.hpp"

#if defined(__cplusplus)
#include <vector>
#include <string>
#include <stdexcept> // For logic_error

namespace link_error_helper
{
/// Contains a dll's name and all the exports

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
LINKERRORHELPERTOOL_API  
::std::filesystem::path GetBinPath(const std::filesystem::path& a_rootDir);

/// Gets all the Dlls from a specifed path
LINKERRORHELPERTOOL_API
::std::vector<std::filesystem::path> GetAllDlls(const std::filesystem::path& a_binPath);

/// Gets everything exported by all Dlls
LINKERRORHELPERTOOL_API
::std::vector<DllInfo> GetAllExports(const std::vector<std::filesystem::path>& a_allDlls);

#endif // end if C++ 17 

#endif // C

/// Makes the error message smaller, easier to read and easier to understand
LINKERRORHELPERTOOL_API
::std::string ParseErrorMessage(const ::std::string& a_errorMessage);

} // end namespace