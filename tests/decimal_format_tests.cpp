#include "../src/decimal.hpp"
#include <cassert>
#include <iostream>
int main(){
    assert(Dec{}.text()==L"0");
    assert(Dec::cents(1).text()==L"0.01");
    assert(Dec::cents(12345).text()==L"123.45");
    assert(Dec::cents(150).text()==L"1.5");
    Dec negative=Dec::cents(234);negative.flags|=0x80000000u;assert(negative.text()==L"-2.34");
    Dec smallest{28u<<16,0,1,0};assert(smallest.text()==L"0.0000000000000000000000000001");
    Dec maximum{0,0xffffffff,0xffffffff,0xffffffff};assert(maximum.text()==L"79228162514264337593543950335");
    assert(maximum.structural());assert(!Dec({29u<<16,0,0,0}).structural());
    assert(!Dec({1,0,0,0}).structural());
    std::cout<<"PASS: exact Decimal layout, formatting, scale and reserved flags\n";
}
