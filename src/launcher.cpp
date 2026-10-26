#include "windows_util.hpp"
#include "game_build.hpp"
#include <tlhelp32.h>

int WINAPI wWinMain(HINSTANCE,HINSTANCE,PWSTR,int){
    auto root=win::directory(win::modulePath());
    auto error=[](const wchar_t* text){MessageBoxW(nullptr,text,L"Moon Wallet",MB_OK|MB_ICONERROR);return 1;};
    if(win::sha256(root+L"\\GameAssembly.dll")!=game::AssemblySha256 || win::sha256(root+L"\\MoonSweepers_Data\\il2cpp_data\\Metadata\\global-metadata.dat")!=game::MetadataSha256)
        return error(L"ゲームの対応ビルドと一致しません。\nMoonWallet.exeとMoonWallet.dllをMoonSweepers.exeと同じフォルダーへ置いてください。\nゲームの更新後は対応版Modが必要です。");
    auto dll=root+L"\\MoonWallet.dll";
    if(GetFileAttributesW(dll.c_str())==INVALID_FILE_ATTRIBUTES)return error(L"MoonWallet.dllが見つかりません。");
    DWORD pid=0;HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);PROCESSENTRY32W entry{};entry.dwSize=sizeof(entry);
    if(snapshot==INVALID_HANDLE_VALUE)return error(L"ゲームのプロセスを確認できません。");
    if(Process32FirstW(snapshot,&entry)){do{
        if(_wcsicmp(entry.szExeFile,L"MoonSweepers.exe")!=0)continue;
        HANDLE process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,entry.th32ProcessID);
        if(process){std::wstring path(32768,L'\0');DWORD n=DWORD(path.size());
            if(QueryFullProcessImageNameW(process,0,path.data(),&n)){path.resize(n);if(_wcsicmp(path.c_str(),(root+L"\\MoonSweepers.exe").c_str())==0)pid=entry.th32ProcessID;}
            CloseHandle(process);
        }
    }while(Process32NextW(snapshot,&entry));}
    CloseHandle(snapshot);
    if(!pid)return error(L"SteamからMoon Sweepersを起動し、タイトル画面でこのツールを起動してください。\nセーブの読込前に起動する必要があります。");
    // Explicit, one-shot native mod loader. Only the matching MoonSweepers.exe in this
    // directory is eligible. No process persistence, executable patch, or global loader.
    HANDLE modules=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,pid);
    if(modules!=INVALID_HANDLE_VALUE){MODULEENTRY32W mod{};mod.dwSize=sizeof(mod);
        if(Module32FirstW(modules,&mod))do{if(_wcsicmp(mod.szModule,L"MoonWallet.dll")==0){CloseHandle(modules);MessageBoxW(nullptr,L"Modはすでに読み込まれています。F8でウォレットを表示できます。",L"Moon Wallet",MB_OK);return 0;}}while(Module32NextW(modules,&mod));CloseHandle(modules);
    }
    HANDLE process=OpenProcess(PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ,FALSE,pid);
    if(!process)return error(L"ゲームへModを読み込めません。ゲームとツールを同じ権限で起動してください。");
    SIZE_T bytes=(dll.size()+1)*sizeof(wchar_t),written=0;
    void* remote=VirtualAllocEx(process,nullptr,bytes,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
    if(!remote || !WriteProcessMemory(process,remote,dll.c_str(),bytes,&written)||written!=bytes){if(remote)VirtualFreeEx(process,remote,0,MEM_RELEASE);CloseHandle(process);return error(L"Mod読込用のメモリーを準備できません。");}
    // Compute LoadLibraryW relative to its actual owning module (forwarded exports may
    // resolve inside KernelBase rather than Kernel32), then use the target module base.
    auto local=GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"LoadLibraryW");HMODULE owner=nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(local),&owner);
    auto ownerName=win::modulePath(owner);ownerName=ownerName.substr(ownerName.find_last_of(L"\\/")+1);uintptr_t remoteOwner=0;
    modules=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,pid);
    if(modules!=INVALID_HANDLE_VALUE){MODULEENTRY32W mod{};mod.dwSize=sizeof(mod);if(Module32FirstW(modules,&mod))do{if(_wcsicmp(mod.szModule,ownerName.c_str())==0)remoteOwner=reinterpret_cast<uintptr_t>(mod.modBaseAddr);}while(Module32NextW(modules,&mod));CloseHandle(modules);}
    if(!remoteOwner){VirtualFreeEx(process,remote,0,MEM_RELEASE);CloseHandle(process);return error(L"ゲーム内のDLLローダーを確認できません。");}
    auto loader=reinterpret_cast<LPTHREAD_START_ROUTINE>(remoteOwner+(reinterpret_cast<uintptr_t>(local)-reinterpret_cast<uintptr_t>(owner)));
    HANDLE thread=CreateRemoteThread(process,nullptr,0,loader,remote,0,nullptr);
    if(!thread){VirtualFreeEx(process,remote,0,MEM_RELEASE);CloseHandle(process);return error(L"Modの読込に失敗しました。");}
    auto wait=WaitForSingleObject(thread,15000);
    // Do not free a buffer that a timed-out remote loader might still be reading.
    if(wait==WAIT_OBJECT_0)VirtualFreeEx(process,remote,0,MEM_RELEASE);
    CloseHandle(thread);CloseHandle(process);
    if(wait!=WAIT_OBJECT_0)return error(L"Modの読込結果を確認できません。再実行せずMoonWallet.logを確認してください。");
    MessageBoxW(nullptr,L"読込要求を送りました。ウォレット画面の表示とMoonWallet.logを確認してください。\nタイトル画面からセーブを読み込んでください。\nマルチプレイではホスト・参加者全員に同じModが必要です。",L"Moon Wallet",MB_OK);return 0;
}
