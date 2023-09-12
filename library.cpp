#include "library.h"

class InterfaceReg
{
public:
    void*                   (*m_CreateFn)();
    const char				*m_pName;

    InterfaceReg			*m_pNext; // For the global list.
};

void server();
void engine2();

// client handle
void client() {
    AllocConsole();
    FILE* file = nullptr;

    // redirect console stream
    freopen_s(&file, "CONOUT$", "w", stdout);

    InterfaceReg* pCur;

    printf("CLIENT.DLL INTERFACES\n");

    // get client dll module
    HMODULE clientHandle = GetModuleHandleA("client.dll");

    if(!clientHandle)
        return;

    // point to memory address of createinterface
    uint64_t addr = (uint64_t)clientHandle + 0x180c098;

    // pointer to createinterface
    auto test = *(InterfaceReg**)addr;

    // print out all interface names
    while(test != nullptr) {
        printf("%s\n", test->m_pName);

        test = test->m_pNext;
    }

    server();
}

// server handle
void server() {
    FILE* file = nullptr;

    // redirect console stream
    freopen_s(&file, "CONOUT$", "w", stdout);

    printf("\nSERVER.DLL INTERFACES\n");

    InterfaceReg* pCur;

    // get client dll module
    HMODULE serverHandle = GetModuleHandleA("server.dll");

    if(!serverHandle)
        return;

    // point to memory address of createinterface
    uint64_t addr = (uint64_t)serverHandle + 0x15c7b50;

    // pointer to createinterface
    auto test = *(InterfaceReg**)addr;

    // print out all interface names
    while(test != nullptr) {
        printf("%s\n", test->m_pName);

        test = test->m_pNext;
    }

    engine2();
}

// engine handle

void engine2() {
    FILE* file = nullptr;

    // redirect console stream
    freopen_s(&file, "CONOUT$", "w", stdout);

    printf("\nENGINE2.DLL INTERFACES\n");

    InterfaceReg* pCur;

    // get client dll module
    HMODULE engineHandle = GetModuleHandleA("engine2.dll");

    if(!engineHandle)
        return;

    // point to memory address of createinterface
    uint64_t addr = (uint64_t)engineHandle + 0x4e6a80;

    // pointer to createinterface
    auto test = *(InterfaceReg**)addr;

    // print out all interface names
    while(test != nullptr) {
        printf("%s\n", test->m_pName);

        test = test->m_pNext;
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason != DLL_PROCESS_ATTACH)
        return FALSE;

    DisableThreadLibraryCalls(hinstDLL);

    if (const HANDLE thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(client), hinstDLL, 0, nullptr))
        CloseHandle(thread);

    return TRUE;
}
