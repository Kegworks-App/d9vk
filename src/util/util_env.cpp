#include "util_env.h"
#include <vector>

#include "./com/com_include.h"

namespace dxvk::env {

  std::string getEnvVar(const wchar_t* name) {
    DWORD len = ::GetEnvironmentVariableW(name, nullptr, 0);
    
    std::vector<WCHAR> result;
    
    while (len > result.size()) {
      result.resize(len);
      len = ::GetEnvironmentVariableW(
        name, result.data(), result.size());
    }
    
    result.resize(len);
    return str::fromws(result.data());
  }
  
  
  std::string getExeName() {
    std::vector<WCHAR> exePath;
    exePath.resize(MAX_PATH + 1);
    
    DWORD len = ::GetModuleFileNameW(NULL, exePath.data(), MAX_PATH);
    exePath.resize(len);
    
    std::string fullPath = str::fromws(exePath.data());
    auto n = fullPath.find_last_of('\\');
    
    return (n != std::string::npos)
      ? fullPath.substr(n + 1)
      : fullPath;
  }

  std::string getTempDirectory() {
    WCHAR windowsTempDir[MAX_PATH] = {0};
    UINT ret = ::GetTempPathW(MAX_PATH, windowsTempDir);
    if (ret > MAX_PATH || ret == 0)
      return std::string();

    auto dxvkTempDir = std::wstring(windowsTempDir) + L"dxvk\\";
    if (::CreateDirectoryW(dxvkTempDir.c_str(), 0) == 0) {
      if (::GetLastError() != ERROR_ALREADY_EXISTS)
        return std::string();
    }

    return str::fromws(dxvkTempDir.c_str());
  }

  void setThreadName(const wchar_t* name) {
    using SetThreadDescriptionProc = void (WINAPI *) (HANDLE, PCWSTR);

    HMODULE module = ::GetModuleHandleW(L"kernel32.dll");

    if (module == nullptr)
      return;

    auto proc = reinterpret_cast<SetThreadDescriptionProc>(
      ::GetProcAddress(module, "SetThreadDescription"));

    if (proc != nullptr)
      (*proc)(::GetCurrentThread(), name);
  }
  
}
