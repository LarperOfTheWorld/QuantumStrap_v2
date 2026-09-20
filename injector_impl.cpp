#include "injector.h"
#include <cstdio>

namespace qs {

DWORD FindProcessByName(const wchar_t* processName) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;
    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(pe);
    DWORD pid = 0;
    if (Process32FirstW(snap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, processName) == 0) {
                pid = pe.th32ProcessID;
                break;
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return pid;
}

bool InjectDLL(DWORD pid, const char* dllPath) {
    HANDLE hProc = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
        PROCESS_VM_OPERATION  | PROCESS_VM_WRITE    | PROCESS_VM_READ,
        FALSE, pid);
    if (!hProc) { printf("[-] OpenProcess failed (%lu)\n", GetLastError()); return false; }

    SIZE_T len = strlen(dllPath) + 1;
    void* remote = VirtualAllocEx(hProc, NULL, len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remote) { printf("[-] VirtualAllocEx failed\n"); CloseHandle(hProc); return false; }

    bool ok = false;
    if (WriteProcessMemory(hProc, remote, dllPath, len, NULL)) {
        LPTHREAD_START_ROUTINE loadLib = (LPTHREAD_START_ROUTINE)
            GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
        HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, loadLib, remote, 0, NULL);
        if (hThread) {
            WaitForSingleObject(hThread, 10000);
            DWORD exitCode = 0;
            GetExitCodeThread(hThread, &exitCode);  /* DLL base, non-zero = OK */
            ok = (exitCode != 0);
            CloseHandle(hThread);
        } else {
            printf("[-] CreateRemoteThread failed\n");
        }
    } else {
        printf("[-] WriteProcessMemory failed\n");
    }

    VirtualFreeEx(hProc, remote, 0, MEM_RELEASE);
    CloseHandle(hProc);
    return ok;
}

bool EnableDebugPrivilege() {
    HANDLE hToken;
    if (!OpenProcessToken(GetCurrentProcess(),
                         TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false;
    LUID luid;
    if (!LookupPrivilegeValueW(NULL, SE_DEBUG_NAME, &luid)) {
        CloseHandle(hToken);
        return false;
    }
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    BOOL ok = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
    CloseHandle(hToken);
    return (ok && GetLastError() != ERROR_NOT_ALL_ASSIGNED);
}

bool IsRunAsAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup = nullptr;
    SID_IDENTIFIER_AUTHORITY ntAuth = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&ntAuth, 2,
            SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
            0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(nullptr, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    return isAdmin != FALSE;
}

DWORD WaitForProcessAndInject(const wchar_t* processName,
                              const char* dllPath, unsigned int timeoutMs) {
    unsigned int waited = 0;
    DWORD pid = 0;
    while (!(pid = FindProcessByName(processName))) {
        if (timeoutMs && waited >= timeoutMs) return 0;
        Sleep(500);
        waited += 500;
    }
    return InjectDLL(pid, dllPath) ? pid : 0;
}

} // namespace qs
