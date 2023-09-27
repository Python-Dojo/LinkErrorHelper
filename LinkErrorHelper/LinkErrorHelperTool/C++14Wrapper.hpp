#pragma once

#include "LinkErrorHelperAPI.hpp"
#include <string>
#include <vector>

/// Gives back suggestions the tool came up with
LINKERRORHELPERTOOL_API
::std::vector<std::string> GetSuggestions(const std::vector<DllInfo>& a_allExports, const std::string& a_errorMessage);

// Not yet implemented
LINKERRORHELPERTOOL_API
std::vector<std::string> GetAllDlls(const std::string& a_binPath);

LINKERRORHELPERTOOL_API
std::string GetBinPath(const std::string& a_rootDir);

LINKERRORHELPERTOOL_API
std::vector<DllInfo> GetAllExports(const std::vector<std::string>& a_allDlls);