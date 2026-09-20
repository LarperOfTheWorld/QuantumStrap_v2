#include "blacklist.h"
#include <string.h>

// Prevents crashing when you try to inject into a game that isn't supported
char targetExe[256];

void CheckBlacklist() {
    GetModuleFileNameA(NULL, targetExe, sizeof(targetExe));
    
    // Only allow Roblox for now in this build
    if (strstr(targetExe, "RobloxPlayerBeta.exe")) return;
    
    // If it's not Roblox, the script executor won't fire automatically
    // to prevent crashing other games.
}
