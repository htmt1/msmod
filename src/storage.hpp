#pragma once
#include "ledger.hpp"
#include "protocol.hpp"
namespace wallet {
inline uint32_t checksum(const void* data,size_t len){uint32_t crc=~0u;auto p=static_cast<const uint8_t*>(data);for(size_t i=0;i<len;++i){crc^=p[i];for(int b=0;b<8;++b)crc=(crc>>1)^(0xedb88320u&uint32_t(-int(crc&1)));}return ~crc;}
template<class Money> std::vector<uint8_t> encodeAccounts(const typename Ledger<Money>::Accounts& accounts){
    static_assert(sizeof(Money)==16);
    Writer w(Kind::Snapshot,0);w.u32(uint32_t(accounts.size()));
    for(auto& [id,a]:accounts){w.u64(id);w.raw(&a.balance,16);w.u64(a.last.request);w.u64(a.last.target);w.raw(&a.last.amount,16);w.u8(uint8_t(a.last.result));}
    w.u32(checksum(w.bytes.data(),w.bytes.size()));return w.bytes;
}
template<class Money> bool decodeAccounts(const std::vector<uint8_t>& bytes,typename Ledger<Money>::Accounts& accounts){
    static_assert(sizeof(Money)==16);
    if(bytes.size()<22)return false;
    Reader crc(bytes.data()+bytes.size()-4,4);if(crc.u32()!=checksum(bytes.data(),bytes.size()-4))return false;
    Reader r(bytes.data(),bytes.size()-4);if(r.u32()!=Magic||r.u8()!=1||r.u8()!=uint8_t(Kind::Snapshot)||r.u64()!=0)return false;
    uint32_t count=r.u32();if(count>256||r.left!=size_t(count)*57)return false;
    typename Ledger<Money>::Accounts next;
    for(uint32_t i=0;i<count;++i){uint64_t id=r.u64();typename Ledger<Money>::Account a;
        r.raw(&a.balance,16);a.last.request=r.u64();a.last.target=r.u64();r.raw(&a.last.amount,16);a.last.result=Result(r.u8());
        if(!id||next.count(id)||!a.balance.bounded()||a.balance<Money{}||!a.last.amount.bounded()||uint8_t(a.last.result)>uint8_t(Result::NotReady))return false;
        next[id]=a;
    }if(!r.done())return false;accounts.swap(next);return true;
}
}
