#include "DumpBinWrapper.hpp"
#include "pch.hpp"

#include <array>
#include <functional>
#include <vector>
#include <string>

#if defined(_WIN32) || defined(_WIN64) || defined(WINDOWS_BUILD)

[[nodiscard]] std::stringstream ExecuteProcess(const std::string &a_cmd) {
  std::array<char, 128> buffer;
  std::stringstream result;
  std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(a_cmd.c_str(), "r"),
                                                 _pclose);
  // if we got a nullptr, return (or throw) instead of crashing
  if (!pipe) {
    result << "Fail";
    return result;
  }

  while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) !=
         nullptr) {
    result << buffer.data();
  }
  return result;
}

[[nodiscard]] bool IsDir(const std::filesystem::path &a_path) {
  if (!std::filesystem::exists(a_path)) {
    return false;
  }
  return std::filesystem::is_directory(a_path);
}

namespace link_error_helper
{

/////////////////////////////////
//        VISUAL STUDIO        //
/////////////////////////////////
class VisualStudio : DumpBinWrapper {
  // TODO Static the string so we don't have to get it every time (this can be quite slow)
  // TODO Use windows hackery to find the program files path (may not be on C drive)
public:
    VisualStudio()
    {
        SetVisualStudioPath();
    }

    bool IsValid() override
    {
        if (State::Invalid == state)
        {
            return false;
        }
        if (State::Valid == state)
        {
            return true;
        }
    }
    
    //std::filesystem::path GetDumpBinPath() override 
    //{
    //    return visualStudioPath;
    //}

    DllInfo GetExports(const std::filesystem::path& a_dll) override
    {
        std::string command = dumpBinCommand + "/EXPORTS" + a_dll.string();
    }

    DllInfo GetSymbols(const std::filesystem::path& a_objectFile) override 
    {
        std::string command = dumpBinCommand + "/EXPORTS" + a_objectFile.string();
    }

    std::string GetName() const override
    {
        return "Visual Studio";
    }

private:
    void SetVisualStudioPath()
    {
        std::string visualStudioPathTemp = ExecuteProcess(
          "\"C:\\Program Files (x86)\\Microsoft Visual "
          "Studio\\Installer\\vswhere.exe\" -property installationPath")
          .str();
        if ("Fail" == visualStudioPathTemp)
        {
            state = State::Invalid; 
            return;
        }
        visualStudioPathTemp = visualStudioPathTemp.substr(0, visualStudioPathTemp.length() - 1);
        visualStudioPath = std::move(std::filesystem::path(visualStudioPathTemp));
        state = State::Valid;
        dumpBinCommand =
          "\"\"" + visualStudioPath.string() +
          "\\VC\\Tools\\MSVC\\14.29.30133\\bin\\Hostx64\\x64\\dumpbin.exe\" ";
    }

    std::vector<std::string>&& ParseDumpBinOutput(std::stringstream& a_dumpBinOutput) const
    {
        std::string anExport;
        std::vector<std::string> allExports;
        allExports.reserve(10); // Number of exported items in a dll, ten seems a good lower bound
        
        // Easiest thing to search for before dlls, if RVA gets long dumpbin may add more spaces, needs testing
        // We currently shorten the string until we get somewhere to avoid this
        std::string startString = "ordinal hint RVA      name";
        bool completed = false;
        while(!completed)
        {
            bool started = false; 
            while( std::getline(a_dumpBinOutput, anExport, '\n'))
            {
                // Skip everything before exports
                if (!started)
                {
                    if (anExport.find(startString) != std::string::npos)
                    {
                        started = true;
                    }
                    continue;
                }
                else if (completed) continue; // if we are done skip everything
                else if (!completed)
                {
                    // Check if this is the line after exports (tells us if we are done)
                    if (anExport.find("Summary") != std::string::npos)
                    {
                        completed = true;
                        break;
                    }
                }

                // We found an export
                // if our export empty string or just spaces, skip it.
                size_t workingCharacter = anExport.find_first_not_of(" ");
                if (workingCharacter == std::string::npos)
                {
                    continue;
                }

                // first non space char is our export number so we don't need it
                workingCharacter = anExport.find_first_not_of(" ", workingCharacter+1);
                // second non space char is our "hint" (zero indexed export number?) so we don't need it
                workingCharacter = anExport.find_first_not_of(" ", workingCharacter+1);
                // third non space char is "RVA" (looks like a hash of the symbol or a pointer) so we don't need it
                workingCharacter = anExport.find(" ", workingCharacter + 2);

                // Strip the first bit of symbol that we don't care about then push it back to return it
                allExports.push_back(anExport.substr(workingCharacter));
            }

            // check we hit something interesting
            if (started)
            {
                // We are done 
                return std::move(allExports);
            } 

            // If we have failed return.
            if (6 > startString.length())
            {
                throw "Failed to parse DumpBin Output";
            }
            // if we haven't
            startString = startString.substr(0, startString.length() - 4);
        }
    }

    std::string dumpBinCommand;
    std::filesystem::path visualStudioPath;

};
}
#endif // Windows