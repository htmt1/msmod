#pragma once
#include "protocol.hpp"
#include <algorithm>
#include <string>
#include <vector>
#include <cstdint>
namespace walletui {
constexpr uint64_t MaxCents=99999999999999ull;
enum class Step { Draft, Confirm, Sending, Result };
enum class Validation { Ready, NoRecipient, RecipientLeft, EmptyAmount, BadAmount, Insufficient, Syncing };
struct Recipient {uint64_t id=0;std::wstring name;};
struct Transfer {uint64_t target=0,cents=0;std::wstring name;};
inline uint64_t spendable(const std::wstring& decimal){
    if(decimal.empty()||decimal[0]==L'-')return 0;
    uint64_t whole=0,fraction=0;size_t i=0;
    for(;i<decimal.size() && decimal[i]!=L'.';++i){if(decimal[i]<L'0'||decimal[i]>L'9')return 0;
        if(whole>(MaxCents/100)/10)return MaxCents;
        whole=whole*10+uint64_t(decimal[i]-L'0');if(whole>MaxCents/100)return MaxCents;
    }
    if(i<decimal.size()){++i;for(int n=0;n<2;++n){fraction*=10;if(i<decimal.size()){if(decimal[i]<L'0'||decimal[i]>L'9')return 0;fraction+=decimal[i++]-L'0';}}}
    return std::min(MaxCents,whole*100+fraction);
}
inline std::wstring amount(uint64_t cents){
    auto out=std::to_wstring(cents/100);auto part=cents%100;
    if(part){out+=L'.';out+=wchar_t(L'0'+part/10);out+=wchar_t(L'0'+part%10);}return out;
}
struct Model {
    Step step=Step::Draft;uint64_t selected=0,available=0;size_t page=0;
    std::wstring input;std::vector<Recipient> recipients;Transfer review;
    static constexpr size_t Rows=5;
    void roster(std::vector<Recipient> next){
        recipients=std::move(next);
        if(selected&&!contains(selected))selected=0;
        if(step==Step::Confirm&&!contains(review.target)){step=Step::Draft;review={};}
        size_t pages=std::max(size_t(1),(recipients.size()+Rows-1)/Rows);page=std::min(page,pages-1);
    }
    bool contains(uint64_t id)const{return std::any_of(recipients.begin(),recipients.end(),[id](const auto& r){return r.id==id;});}
    const Recipient* recipient()const {auto p=std::find_if(recipients.begin(),recipients.end(),[&](auto& r){return r.id==selected;});return p==recipients.end()?nullptr:&*p;}
    uint64_t cents()const{int64_t n=0;return wallet::parseCents(input.c_str(),n)?uint64_t(n):0;}
    Validation validation(bool synced)const{
        if(!selected)return Validation::NoRecipient;
        if(!contains(selected))return Validation::RecipientLeft;
        if(input.empty())return Validation::EmptyAmount;
        auto n=cents();if(!n)return Validation::BadAmount;
        if(n>available)return Validation::Insufficient;
        if(!synced)return Validation::Syncing;
        return Validation::Ready;
    }
    bool confirm(bool synced){if(step!=Step::Draft||validation(synced)!=Validation::Ready)return false;
        auto r=recipient();review={r->id,cents(),r->name};step=Step::Confirm;return true;
    }
    bool beginSend(bool synced){
        if(step!=Step::Confirm || !synced || !contains(review.target)||!review.cents||review.cents>available)return false;
        step=Step::Sending;return true;
    }
    void back(){if(step==Step::Confirm){step=Step::Draft;review={};}}
    void finish(){if(step==Step::Sending)step=Step::Result;}
    void again(){step=Step::Draft;input.clear();review={};}
};
}
