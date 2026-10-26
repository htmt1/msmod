#include "../src/wallet_ui_model.hpp"
#include <cassert>
#include <iostream>
int main(){
    using namespace walletui;
    assert(spendable(L"12.5099999999999999999")==1250);
    assert(spendable(L"0.009")==0);assert(spendable(L"1")==100);
    assert(spendable(L"-1")==0);assert(spendable(L"999999999999999")==MaxCents);
    assert(amount(1250)==L"12.50"&&amount(10000)==L"100");
    Model m;m.available=10000;m.roster({{1,L"Alice"},{2,L"Bob"}});
    assert(m.validation(true)==Validation::NoRecipient);m.selected=2;
    assert(m.validation(true)==Validation::EmptyAmount);m.input=L"12.50";
    assert(m.validation(false)==Validation::Syncing);assert(m.confirm(true));
    assert(m.review.target==2&&m.review.cents==1250&&m.review.name==L"Bob");
    // Freeze the reviewed recipient/amount even if the form or roster order changes.
    m.input=L"99";m.selected=1;m.roster({{2,L"Bob renamed"},{1,L"Alice"}});
    assert(m.review.target==2&&m.review.cents==1250&&m.review.name==L"Bob");
    assert(m.beginSend(true));assert(!m.beginSend(true));m.finish();assert(m.step==Step::Result);
    m.again();m.selected=2;m.input=L"100.01";assert(!m.confirm(true));
    assert(m.validation(true)==Validation::Insufficient);m.input=L"0.01";assert(m.confirm(true));
    // A recipient leaving invalidates the confirmation, rather than choosing another row.
    m.roster({{1,L"Alice"}});assert(m.step==Step::Draft&&!m.selected);
    m.selected=1;m.input=L"12";assert(m.confirm(true));m.available=100;assert(!m.beginSend(true));m.back();assert(m.input==L"12");
    for(auto text:{L"0",L"-1",L"1.001",L"1e3",L"NaN"}){m.input=text;assert(!m.confirm(true));}
    m.page=9;m.roster({{1,L"one"}});assert(m.page==0);
    std::cout<<"PASS: UI flow, frozen confirmation, duplicate-send guard, leave/reorder, balance changes, cents precision and pagination\n";
}
