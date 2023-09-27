
#pragma once

#include "LinkErrorHelperAPI.hpp"
#include <filesystem>
#include <string>
#include <sstream>

namespace link_error_helper
{

class DumpBinWrapper 
{
public:
    virtual DllInfo GetExports(const std::filesystem::path& a_dll) = 0;
    virtual DllInfo GetSymbols(const std::filesystem::path& a_headerfile) = 0;
    virtual bool IsValid(){ return (Valid == state); }
    virtual std::string GetName() const = 0;

protected:
    enum State : char {
        Unknown = -1,
        Invalid = 0,
        Valid = 1
    };
    State state = State::Unknown;
};


} //end namespace