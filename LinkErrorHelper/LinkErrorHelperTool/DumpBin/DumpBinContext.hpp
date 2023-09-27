#pragma once


#include <vector>
#include <string>
#include <sstream>

namespace link_error_helper
{

class DumpBinWrapper;

class DumpBinContext
{
public:
    static DumpBinWrapper& GetWrapper();
private:
    static DumpBinWrapper* validWrapper;
    static std::vector<DumpBinWrapper> stragies;
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
    return *validWrapper;
}

auto DumpBinContext::validWrapper = nullptr;
auto DumpBinContext::stragies = {};

} // end namespace
