/* ==========================================================================
   INJECTOR.H - Process injection API
   ========================================================================== */
#pragma once
#include <windows.h>
#include <tlhelp32.h>

namespace qs {

// Returns PID of the first process matching name (case-insensitive), or 0.
// Never blocks. Safe to call in a polling loop.
DWORD FindProcessByName(const wchar_t* processName);

// Classic CreateRemoteThread + LoadLibraryA injection.
// dllPath must be an ABSOLUTE path (relative paths resolve against the
// target process's working directory, not yours).
bool InjectDLL(DWORD pid, const char* dllPath);

// Grants the current process SeDebugPrivilege so we can open
// processes owned by other users/sessions. Call once at startup.
bool EnableDebugPrivilege();

// True if the current process token is in the Administrators group.
bool IsRunAsAdmin();

// Polls for processName forever (timeoutMs = 0) or until timeout,
// then injects. Returns PID on success, 0 on timeout/failure.
DWORD WaitForProcessAndInject(const wchar_t* processName,
                              const char*    dllPath,
                              unsigned int   timeoutMs = 0);

} // namespace qs
