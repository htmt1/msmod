#include "../src/ledger.hpp"
#include "../src/protocol.hpp"
#include "../src/storage.hpp"
#include <cassert>
#include <iostream>
#include <limits>
struct Money {
    int64_t n=0;uint64_t padding=0;
    bool bounded()const{return padding==0;}
    bool operator<(Money b)const{return n<b.n;}
    bool operator==(Money b)const{return n==b.n;}
    static bool add(Money a,Money b,Money& c){return !__builtin_add_overflow(a.n,b.n,&c.n);}
    static bool sub(Money a,Money b,Money& c){return !__builtin_sub_overflow(a.n,b.n,&c.n);}
};
int main(){
    using namespace wallet; Ledger<Money> l; l.accounts[1].balance={10000};l.accounts[2].balance={500};
    assert(l.transfer(1,2,{2500},1)==Result::Ok);
    assert(l.balance(1).n==7500 && l.balance(2).n==3000);
    assert(l.transfer(1,2,{2500},1)==Result::Ok); // retransmission is exactly once
    assert(l.balance(1).n==7500 && l.balance(2).n==3000);
    assert(l.transfer(1,2,{2600},1)==Result::StaleRequest);
    assert(l.transfer(1,2,{8000},2)==Result::Insufficient);
    assert(l.transfer(1,1,{1},3)==Result::SelfTransfer);
    assert(l.transfer(1,2,{0},4)==Result::InvalidAmount);
    assert(l.transfer(1,2,{-100},5)==Result::InvalidAmount);
    assert(l.transfer(1,3,{1},6)==Result::UnknownRecipient);
    assert(l.transfer(1,2,{1},2)==Result::StaleRequest);
    auto saved=l.accounts;
    l.persist=[](const auto&){return false;};
    assert(l.transfer(1,2,{100},7)==Result::StorageError);
    assert(l.balance(1).n==7500 && l.balance(2).n==3000 && l.accounts[1].last.request==6);
    assert(!l.delta(1,{10}));assert(l.balance(1).n==7500);
    l.persist=[](const auto&){return true;};
    assert(l.transfer(1,2,{100},7)==Result::Ok);
    Ledger<Money> restored;restored.accounts=l.accounts;
    assert(restored.transfer(1,2,{100},7)==Result::Ok); // receipt survives reload
    assert(restored.balance(1).n==7400);
    restored.accounts[2].balance={std::numeric_limits<int64_t>::max()};
    assert(restored.transfer(1,2,{1},8)==Result::Overflow);
    assert(restored.balance(1).n==7400);
    assert(!restored.delta(1,{-999999}));
    // Individual incomes and purchases cannot spend another account's balance.
    assert(l.delta(1,{500}));assert(l.balance(2).n==3100);
    assert(l.delta(2,{-100}));assert(l.balance(1).n==7900);
    int64_t cents;
    assert(parseCents(L"123.45",cents)&&cents==12345);
    assert(parseCents(L"0.01",cents)&&cents==1);
    assert(parseCents(L"1.5",cents)&&cents==150);
    for(auto s:{L"0",L"-1",L"NaN",L"1e5",L"1.001",L"1,000",L"9999999999999",L".5",L"1.",L"",L" 1"})assert(!parseCents(s,cents));
    Writer w(Kind::Transfer,42);w.u64(7);w.u64(2);w.u64(12345);
    Reader r(w.bytes.data(),w.bytes.size());assert(r.u32()==Magic && r.u8()==1 && r.u8()==3 && r.u64()==42);assert(r.u64()==7 && r.u64()==2 && r.u64()==12345 && r.done());
    for(size_t n=0;n<w.bytes.size();++n){Reader truncated(w.bytes.data(),n);truncated.u32();truncated.u8();truncated.u8();for(int i=0;i<4;++i)truncated.u64();assert(!truncated.done());}
    auto encoded=encodeAccounts<Money>(l.accounts);
    Ledger<Money> loaded;
    assert(decodeAccounts<Money>(encoded,loaded.accounts));
    assert(loaded.balance(1).n==l.balance(1).n && loaded.balance(2).n==l.balance(2).n);
    assert(loaded.transfer(1,2,{100},7)==Result::Ok && loaded.balance(1).n==l.balance(1).n);
    for(size_t i=0;i<encoded.size();++i){auto corrupt=encoded;corrupt[i]^=0x40;assert(!decodeAccounts<Money>(corrupt,loaded.accounts));}
    for(size_t i=0;i<encoded.size();++i){auto truncated=std::vector<uint8_t>(encoded.begin(),encoded.begin()+i);assert(!decodeAccounts<Money>(truncated,loaded.accounts));}
    assert(loaded.balance(1).n==l.balance(1).n); // a failed load never overwrites live balances
    std::cout<<"PASS: isolation, transfer, bounds, replay, persistence rollback/reload, overflow, money input, protocol truncation, save roundtrip/corruption/truncation\n";
}
