
#pragma once

#include <filesystem>

std::string GetDumpBinCommand(const std::filesystem::path& a_dumpBinPath = GetdDumpBinPath());

std::filesystem::path GetdDumpBinPath();

