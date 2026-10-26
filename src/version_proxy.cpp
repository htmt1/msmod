#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <cstdint>

namespace {
HMODULE own=nullptr,systemVersion=nullptr;
INIT_ONCE versionOnce=INIT_ONCE_STATIC_INIT;
constexpr const char* names[]={
    "GetFileVersionInfoA","GetFileVersionInfoByHandle","GetFileVersionInfoExA","GetFileVersionInfoExW",
    "GetFileVersionInfoSizeA","GetFileVersionInfoSizeExA","GetFileVersionInfoSizeExW","GetFileVersionInfoSizeW",
    "GetFileVersionInfoW","VerFindFileA","VerFindFileW","VerInstallFileA","VerInstallFileW",
    "VerLanguageNameA","VerLanguageNameW","VerQueryValueA","VerQueryValueW"
};
BOOL CALLBACK loadSystemVersion(PINIT_ONCE,PVOID,PVOID*){
    wchar_t path[32768];UINT n=GetSystemDirectoryW(path,32768);
    if(!n || n+13>=32768)return FALSE;
    lstrcatW(path,L"\\version.dll");
    // An absolute system path prevents recursively loading the local proxy.
    systemVersion=LoadLibraryExW(path,nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);return systemVersion!=nullptr;
}
void bootLog(const wchar_t* root,const char* text){
    wchar_t path[32768];if(lstrlenW(root)+25>=32768)return;
    lstrcpyW(path,root);lstrcatW(path,L"\\MoonWallet\\loader.log");
    HANDLE h=CreateFileW(path,FILE_APPEND_DATA,FILE_SHARE_READ,nullptr,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(h!=INVALID_HANDLE_VALUE){DWORD n;WriteFile(h,text,DWORD(lstrlenA(text)),&n,nullptr);CloseHandle(h);}
}
DWORD WINAPI loadPlugin(void*){
    wchar_t root[32768];DWORD n=GetModuleFileNameW(own,root,32768);if(!n||n>=32768)return 0;
    wchar_t* slash=nullptr;for(wchar_t* p=root;*p;++p)if(*p==L'\\'||*p==L'/')slash=p;
    if(!slash)return 0;*slash=0;
    // Do not load game code when another executable (e.g. the crash handler) imports version.dll.
    wchar_t exe[32768];n=GetModuleFileNameW(nullptr,exe,32768);if(!n||n>=32768)return 0;
    wchar_t* filename=exe;for(wchar_t* p=exe;*p;++p)if(*p==L'\\'||*p==L'/')filename=p+1;
    if(lstrcmpiW(filename,L"MoonSweepers.exe")!=0)return 0;
    if(lstrlenW(root)+55>=32768)return 0;
    wchar_t path[32768];lstrcpyW(path,root);lstrcatW(path,L"\\MoonWallet\\disabled.txt");
    if(GetFileAttributesW(path)!=INVALID_FILE_ATTRIBUTES){bootLog(root,"Moon Wallet autoload disabled by disabled.txt.\r\n");return 0;}
    lstrcpyW(path,root);lstrcatW(path,L"\\MoonWallet\\plugins\\MoonWallet.dll");
    HMODULE plugin=LoadLibraryExW(path,nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!plugin){bootLog(root,"Moon Wallet native plugin could not be loaded. Check MoonWallet/plugins/MoonWallet.dll.\r\n");return 0;}
    bootLog(root,"Moon Wallet native plugin loaded. Waiting for IL2CPP startup; see MoonWallet/plugins/MoonWallet.log.\r\n");
    // Keep the DLL loaded for the process lifetime; detours must never be unloaded while active.
    return 0;
}
}
extern "C" FARPROC wallet_resolve_version(unsigned index){
    DWORD previousError=GetLastError();
    if(index<sizeof(names)/sizeof(names[0]) && InitOnceExecuteOnce(&versionOnce,loadSystemVersion,nullptr,nullptr)){
        FARPROC address=GetProcAddress(systemVersion,names[index]);if(address){SetLastError(previousError);return address;}
    }
    // A missing system API cannot safely be substituted with a different signature.
    RaiseFailFastException(nullptr,nullptr,0);return nullptr;
}
BOOL WINAPI DllMain(HINSTANCE instance,DWORD reason,LPVOID){
    if(reason==DLL_PROCESS_ATTACH){own=instance;DisableThreadLibraryCalls(instance);
        HANDLE thread=CreateThread(nullptr,0,loadPlugin,nullptr,0,nullptr);if(thread)CloseHandle(thread);
    }return TRUE;
}
