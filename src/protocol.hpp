#pragma once
#include <cstdint>
#include <cstring>
#include <vector>
namespace wallet {
constexpr uint32_t Magic=0x3157534d; // MSW1
constexpr int Channel=28741;
enum class Kind:uint8_t { Hello=1, Snapshot=2, Transfer=3, Reply=4 };
struct Writer {
    std::vector<uint8_t> bytes;
    void u8(uint8_t v){bytes.push_back(v);}
    void u32(uint32_t v){for(int i=0;i<4;++i)u8(uint8_t(v>>(8*i)));}
    void u64(uint64_t v){for(int i=0;i<8;++i)u8(uint8_t(v>>(8*i)));}
    void raw(const void* p,size_t n){auto s=static_cast<const uint8_t*>(p);bytes.insert(bytes.end(),s,s+n);}
    Writer(Kind kind,uint64_t epoch){u32(Magic);u8(1);u8(uint8_t(kind));u64(epoch);}
};
struct Reader {
    const uint8_t* p;size_t left;bool valid=true;
    Reader(const void* p,size_t n):p(static_cast<const uint8_t*>(p)),left(n){}
    uint8_t u8(){if(!left){valid=false;return 0;}--left;return *p++;}
    uint32_t u32(){uint32_t v=0;for(int i=0;i<4;++i)v|=uint32_t(u8())<<(i*8);return v;}
    uint64_t u64(){uint64_t v=0;for(int i=0;i<8;++i)v|=uint64_t(u8())<<(i*8);return v;}
    bool raw(void* out,size_t n){if(left<n){valid=false;return false;}std::memcpy(out,p,n);p+=n;left-=n;return true;}
    bool done()const{return valid && left==0;}
};
// Transfer UI accepts up to two fractional digits, no signs/exponents/locale ambiguity.
inline bool parseCents(const wchar_t* text,int64_t& cents) {
    uint64_t whole=0,fraction=0;int digits=0,decimals=0;bool dot=false;
    for(auto p=text;*p;++p) {
        if(*p==L'.' && !dot){dot=true;continue;}
        if(*p<L'0'||*p>L'9')return false;
        if(!dot){if(++digits>12)return false;whole=whole*10+(*p-L'0');}
        else {if(++decimals>2)return false;fraction=fraction*10+(*p-L'0');}
    }
    if(!digits || (dot&&!decimals))return false;
    if(decimals==1)fraction*=10;
    cents=int64_t(whole*100+fraction);return cents>0;
}
}
