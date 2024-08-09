#include <intrin.h>
#include <tlhelp32.h>
#include "d3dguix.h"
#include "MemProtector.h"
#include "AimBot.h"
#include "Driver.h"
#include "Entity.h"

void TestLoop() {
    while (1) {
        int b[20][4] = {0};
        for (int i = 0; i < 20; i++) {
            const int x[4] = {rand() % 1000, rand() % 400, 50, 100};
            Sleep(1);
            memcpy(b[i], x, sizeof(x));
        }
        Box_T box;
        memcpy(box.data, b, sizeof(b));
        pushBox(box);
        Sleep(10);
    }
}

uintptr_t milliseconds_now() {
    static LARGE_INTEGER s_frequency;
    static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
    if (s_use_qpc) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / s_frequency.QuadPart;
    } else {
        return GetTickCount();
    }
}

DWORD GetProcessIdByName(wchar_t *name) {
    Protect(_ReturnAddress());

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE) {
        while (Process32Next(snapshot, &entry) == TRUE) {
            if (_wcsicmp(entry.szExeFile, name) == 0) {
                Unprotect(_ReturnAddress());
                return entry.th32ProcessID;
            }
        }
    }

    CloseHandle(snapshot);
    Unprotect(_ReturnAddress());
    return 0;
}

void LoadProtectedFunctions() {
    uintptr_t t = milliseconds_now();
    BYTE xorkey = 0x0;
    for (DWORD i = 0; i < 8; i++) {
        xorkey = ((BYTE *) &t)[i];
        if (xorkey > 0x3 && xorkey < 0xf0) {
            break;
        }
    }
    if (xorkey <= 0x3 || xorkey >= 0xf0) {
        xorkey = 0x56;
    }

    addFunc({pushBox, (uintptr_t) LoadProtectedFunctions - (uintptr_t) pushBox - 0x3, xorkey, false});
    addFunc({LoadProtectedFunctions, (uintptr_t) InitMouse - (uintptr_t) LoadProtectedFunctions - 0x3, xorkey, false});
    addFunc({InitMouse, (uintptr_t) GetProcessIdByName - (uintptr_t) InitMouse - 0x3, xorkey, false});
    addFunc({GetProcessIdByName, (uintptr_t) milliseconds_now - (uintptr_t) GetProcessIdByName - 0x3, xorkey, false});
    addFunc({milliseconds_now, (uintptr_t) ProcessPlayer - (uintptr_t) milliseconds_now - 0x3, xorkey, false});
    addFunc({ProcessPlayer, (uintptr_t) UpdatePlayersInfo - (uintptr_t) ProcessPlayer - 0x3, xorkey, false});
    addFunc({UpdatePlayersInfo, (uintptr_t) PredictPosition - (uintptr_t) UpdatePlayersInfo - 0x3, xorkey, false});
    addFunc({PredictPosition, (uintptr_t) AutoBoneSwitch - (uintptr_t) PredictPosition - 0x3, xorkey, false});
    addFunc({AutoBoneSwitch, (uintptr_t) SmoothType_Asist - (uintptr_t) AutoBoneSwitch - 0x3, xorkey, false});
    addFunc({SmoothType_Asist, (uintptr_t) SmoothType_TargetLock - (uintptr_t) SmoothType_Asist - 0x3, xorkey, false});
    addFunc({SmoothType_TargetLock, (uintptr_t) AimAngles - (uintptr_t) SmoothType_TargetLock - 0x3, xorkey, false});
    addFunc({AimAngles, (uintptr_t) CheatLoop - (uintptr_t) AimAngles - 0x3, xorkey, false});
    addFunc({CheatLoop, (uintptr_t) Driver::SendCommand - (uintptr_t) CheatLoop - 0x3, xorkey, false});


    addFunc({
        Driver::SendCommand, (uintptr_t) Driver::GetBaseAddress - (uintptr_t) Driver::SendCommand - 0x3, xorkey, false
    });
    addFunc({
        Driver::GetBaseAddress, (uintptr_t) Driver::copy_memory - (uintptr_t) Driver::GetBaseAddress - 0x3, xorkey,
        false
    });
    addFunc({
        Driver::copy_memory, (uintptr_t) GetKernelModuleExport - (uintptr_t) Driver::copy_memory - 0x3, xorkey, false
    });
    addFunc({
        GetKernelModuleExport, (uintptr_t) GetKernelModuleAddress - (uintptr_t) GetKernelModuleExport - 0x3, xorkey,
        false
    });
    addFunc({
        GetKernelModuleAddress, (uintptr_t) Driver::initialize - (uintptr_t) GetKernelModuleAddress - 0x3, xorkey, false
    });
    addFunc({
        Driver::initialize, (uintptr_t) Driver::read_memory - (uintptr_t) Driver::initialize - 0x3, xorkey, false
    });
    addFunc({
        Driver::read_memory, (uintptr_t) Driver::write_memory - (uintptr_t) Driver::read_memory - 0x3, xorkey, false
    });
    addFunc({Driver::write_memory, (uintptr_t) getEntity - (uintptr_t) Driver::write_memory - 0x3, xorkey, false});

    for (size_t i = 0; i < funcCount; i++) {
        if (functions[i].address != LoadProtectedFunctions)
            Protect(functions[i].address);
    }
    Unprotect(_ReturnAddress());
}

int main() {
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) Start, NULL, 0, NULL);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) TestLoop, NULL, 0, NULL);
    while (1) {
        cout << "Input 1 close:" << endl;
        int a = 0;
        cin >> a;
        if (a == 1) {
            return 0;
        }
    }

    return 0;
}
