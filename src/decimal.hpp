#pragma once
#include "game_build.hpp"
#include <cstdint>
#include <string>
// IL2CPP System.Decimal layout verified against supplied metadata (flags, hi, lo, mid).
// All game calls use the Windows x64 ABI: 16-byte Decimal parameters are by reference,
// and Decimal returns use an explicit hidden first result-buffer argument.
struct Dec {
    uint32_t flags=0,hi=0,lo=0,mid=0;
    static inline uintptr_t base=0;
    static Dec cents(uint64_t v){return {2u<<16,0,uint32_t(v),uint32_t(v>>32)};}
    static Dec limit(){return {0,0,uint32_t(1000000000000000ull),uint32_t(1000000000000000ull>>32)};}
    bool structural()const{return !(flags&0x7f00ffffu) && ((flags>>16)&255)<=28;}
    bool operator<(Dec b)const {return reinterpret_cast<bool(*)(const Dec*,const Dec*,void*)>(base+game::DecimalLess)(this,&b,nullptr);}
    bool operator==(Dec b)const {return reinterpret_cast<bool(*)(const Dec*,const Dec*,void*)>(base+game::DecimalEqual)(this,&b,nullptr);}
    bool bounded()const {if(!structural())return false;auto abs=*this;abs.flags&=0x7fffffff;return !(limit()<abs);}
    static bool add(Dec a,Dec b,Dec& out){
        if(!a.bounded() || !b.bounded())return false;
        reinterpret_cast<Dec*(*)(Dec*,const Dec*,const Dec*,void*)>(base+game::DecimalAdd)(&out,&a,&b,nullptr);
        return out.bounded();
    }
    static bool sub(Dec a,Dec b,Dec& out){b.flags^=0x80000000u;return add(a,b,out);}
    std::wstring text()const {
        // Exact decimal formatting; no float conversion. Divide the 96-bit significand by ten.
        uint32_t words[3]={lo,mid,hi};std::wstring digits;
        do {uint64_t rem=0;for(int i=2;i>=0;--i){uint64_t x=(rem<<32)|words[i];words[i]=uint32_t(x/10);rem=x%10;}digits.insert(digits.begin(),wchar_t(L'0'+rem));}while(words[0]||words[1]||words[2]);
        auto scale=(flags>>16)&255;while(digits.size()<=scale)digits.insert(digits.begin(),L'0');
        if(scale)digits.insert(digits.end()-scale,L'.');
        if(scale){while(digits.back()==L'0')digits.pop_back();if(digits.back()==L'.')digits.pop_back();}
        if(flags&0x80000000u && digits!=L"0")digits.insert(digits.begin(),L'-');
        return digits;
    }
};
static_assert(sizeof(Dec)==16);
