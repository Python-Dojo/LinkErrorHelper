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

#include <filesystem>
#include <vector>

LINKERRORHELPERTOOL_API [[nodiscard]] 
std::vector<std::filesystem::path> GetAllDlls(const std::filesystem::path& a_binPath);

// if no Bin path was given, try and find one
LINKERRORHELPERTOOL_API [[nodiscard]] 
std::filesystem::path GetBinPath(const std::filesystem::path& a_rootDir);

LINKERRORHELPERTOOL_API [[nodiscard]]
std::vector<std::string> GetAllExports(const std::vector<std::filesystem::path>& a_allBinDirs);

LINKERRORHELPERTOOL_API [[nodiscard]]
std::vector<std::string> GetSuggestions(const std::vector<std::string>& a_allExports);
