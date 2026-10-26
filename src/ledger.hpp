#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <functional>

namespace wallet {
enum class Result : uint8_t { Ok, InvalidAmount, SelfTransfer, UnknownRecipient, Insufficient, Overflow, StorageError, StaleRequest, NotReady };
// Money supplies exact checked add/subtract/compare; never use floating point for transfers.
template<class Money> class Ledger {
public:
    struct Receipt { uint64_t request=0, target=0; Money amount{}; Result result=Result::NotReady; };
    struct Account { Money balance{}; Receipt last{}; };
    using Accounts=std::map<uint64_t,Account>;
    Accounts accounts;
    std::function<bool(const Accounts&)> persist=[](const Accounts&){return true;};
    Money balance(uint64_t id) const { auto i=accounts.find(id); return i==accounts.end()?Money{}:i->second.balance; }
    bool delta(uint64_t id, Money change) {
        if(!id)return false;
        auto next=accounts;
        Money updated;
        if(!Money::add(balance(id),change,updated) || updated<Money{})return false;
        next[id].balance=updated;
        if(!persist(next))return false;
        accounts.swap(next);return true;
    }
    Result transfer(uint64_t from,uint64_t to,Money amount,uint64_t request) {
        auto it=accounts.find(from);
        if(!from || !request || it==accounts.end())return Result::NotReady;
        auto& last=it->second.last;
        if(request==last.request) {
            if(to!=last.target || !(amount==last.amount))return Result::StaleRequest;
            return last.result;
        }
        if(request<last.request)return Result::StaleRequest;
        Result result=Result::Ok;
        if(!(Money{}<amount))result=Result::InvalidAmount;
        else if(from==to)result=Result::SelfTransfer;
        else if(!accounts.count(to))result=Result::UnknownRecipient;
        else if(balance(from)<amount)result=Result::Insufficient;
        auto next=accounts;
        if(result==Result::Ok) {
            Money a,b;
            if(!Money::sub(balance(from),amount,a) || !Money::add(balance(to),amount,b))result=Result::Overflow;
            else {next[from].balance=a; next[to].balance=b;}
        }
        next[from].last={request,to,amount,result};
        // Receipt and both balances commit atomically. Failed writes never acknowledge success.
        if(!persist(next))return Result::StorageError;
        accounts.swap(next);return result;
    }
};
inline const wchar_t* message(Result r) {
    switch(r) {
    case Result::Ok:return L"送金が完了しました。";
    case Result::InvalidAmount:return L"金額は0より大きい値を指定してください。";
    case Result::SelfTransfer:return L"自分自身には送金できません。";
    case Result::UnknownRecipient:return L"送金先のウォレットがまだ準備できていません。";
    case Result::Insufficient:return L"残高が不足しています。";
    case Result::Overflow:return L"金額が大きすぎます。";
    case Result::StorageError:return L"保存に失敗しました。送金は確定していません。";
    case Result::StaleRequest:return L"古い送金要求です。残高を確認してください。";
    default:return L"ホストとの同期を待っています。";
    }
}
}
