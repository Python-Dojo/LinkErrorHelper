
#pragma once

#include "LinkErrorHelperAPI.hpp"
#include <filesystem>
#include <string>
#include <sstream>

namespace link_error_helper
{
LINKERRORHELPERTOOL_API struct DllInfo
{
    ::std::string m_dllName;
    ::std::vector<::std::string> m_dllExports;
};

class DumpBinWrapper;

class DumpBinContext
{
public:
    static DumpBinWrapper& GetWrapper();
private:
    static DumpBinWrapper* validWrapper;
    static std::vector<DumpBinWrapper> stragies;
};
auto DumpBinContext::validWrapper = nullptr;
auto DumpBinContext::stragies = {};

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

DumpBinWrapper& DumpBinContext::GetWrapper()
{
    if ( nullptr == validWrapper)
    {
        std::stringstream attempts;
        attempts << ", tried ";
        for(auto& stratagy : stragies)
        {
            if (stratagy.IsValid())
            {
                return stratagy;
            }
            attempts << stratagy.GetName();
        }
        throw "Could not find a valid way of getting symbols from binaries" + attempts.str(); 
    }
    return validWrapper;
}
}