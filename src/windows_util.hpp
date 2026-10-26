#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <bcrypt.h>
#include <string>
#include <vector>
#include <cstdio>
namespace win {
inline std::wstring utf16(const std::string& s){
    int n=MultiByteToWideChar(CP_UTF8,0,s.data(),int(s.size()),nullptr,0);std::wstring w(n,L'\0');
    MultiByteToWideChar(CP_UTF8,0,s.data(),int(s.size()),w.data(),n);return w;
}
inline std::string utf8(const std::wstring& w){
    int n=WideCharToMultiByte(CP_UTF8,0,w.data(),int(w.size()),nullptr,0,nullptr,nullptr);std::string s(n,'\0');
    WideCharToMultiByte(CP_UTF8,0,w.data(),int(w.size()),s.data(),n,nullptr,nullptr);return s;
}
inline std::wstring modulePath(HMODULE m=nullptr){std::wstring p(32768,L'\0');DWORD n=GetModuleFileNameW(m,p.data(),DWORD(p.size()));p.resize(n);return p;}
inline std::wstring directory(const std::wstring& p){return p.substr(0,p.find_last_of(L"\\/"));}
inline bool read(const std::wstring& path,std::vector<uint8_t>& data,size_t limit=64*1024*1024){
    HANDLE h=CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(h==INVALID_HANDLE_VALUE)return false;LARGE_INTEGER size{};DWORD got=0;
    bool ok=GetFileSizeEx(h,&size)&&size.QuadPart>=0&&uint64_t(size.QuadPart)<=limit;
    if(ok){data.resize(size_t(size.QuadPart));ok=ReadFile(h,data.data(),DWORD(data.size()),&got,nullptr)&&got==data.size();}
    CloseHandle(h);return ok;
}
inline bool atomicWrite(const std::wstring& path,const std::vector<uint8_t>& data){
    auto tmp=path+L".tmp";HANDLE h=CreateFileW(tmp.c_str(),GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(h==INVALID_HANDLE_VALUE)return false;DWORD written=0;
    bool ok=WriteFile(h,data.data(),DWORD(data.size()),&written,nullptr)&&written==data.size()&&FlushFileBuffers(h);CloseHandle(h);
    if(ok && GetFileAttributesW(path.c_str())!=INVALID_FILE_ATTRIBUTES)ok=CopyFileW(path.c_str(),(path+L".bak").c_str(),FALSE)!=0;
    if(ok)ok=MoveFileExW(tmp.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
    return ok;
}
inline std::string sha256(const std::wstring& path){
    std::vector<uint8_t> bytes;if(!read(path,bytes))return {};
    BCRYPT_ALG_HANDLE alg=nullptr;BCRYPT_HASH_HANDLE hash=nullptr;DWORD n=0,got=0;std::string result;
    if(BCryptOpenAlgorithmProvider(&alg,BCRYPT_SHA256_ALGORITHM,nullptr,0)<0)return {};
    if(BCryptGetProperty(alg,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&n),4,&got,0)>=0){
        std::vector<uint8_t> object(n);uint8_t digest[32];
        if(BCryptCreateHash(alg,&hash,object.data(),n,nullptr,0,0)>=0){
            if(BCryptHashData(hash,bytes.data(),ULONG(bytes.size()),0)>=0 && BCryptFinishHash(hash,digest,32,0)>=0){
                constexpr char hex[]="0123456789abcdef";for(auto b:digest){result+=hex[b>>4];result+=hex[b&15];}
            }BCryptDestroyHash(hash);
        }
    }BCryptCloseAlgorithmProvider(alg,0);return result;
}
inline uint32_t crc32(const void* data,size_t len){uint32_t crc=~0u;auto p=static_cast<const uint8_t*>(data);for(size_t i=0;i<len;++i){crc^=p[i];for(int b=0;b<8;++b)crc=(crc>>1)^(0xedb88320u&uint32_t(-int(crc&1)));}return ~crc;}
inline uint64_t timeId(){FILETIME ft;GetSystemTimeAsFileTime(&ft);return (uint64_t(ft.dwHighDateTime)<<32)|ft.dwLowDateTime;}
}
