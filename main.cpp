#include <iostream>
#include <gmpxx.h>
#include <iomanip>  // для hex-формата
#include "BigInteger_DLL/include/BigInteger.h"


template <typename T>
class test {
    test(T t) = delete;
};

int main() {
    mpz_class a("12345678901234567890123456789028785762752687368256386576785");
    mpz_class a1(2);

    mpz_class res1;
    mpz_pow_ui(res1.get_mpz_t(), a1.get_mpz_t(), 136279841);

    res1 -= 1;

    __mpz_struct* ptr = res1.get_mpz_t();
    mp_size_t n = std::abs(ptr->_mp_size); // число используемых лимбов

    std::cout << "Number of limbs: " << n << std::endl;

    // for (mp_size_t i = 0; i < n; i++) {
    //     std::cout << "limb[" << i << "] = "
    //               << std::hex << std::showbase
    //               << ptr->_mp_d[i]
    //               << std::dec << std::endl;
    // }

    BigInteger b("12345678901234567890123456789028785762752687368256386576785");
    BigInteger b1("1234567890");
    auto res2 = (2_bi).pow(136279841) - 1;
    auto limbs = res2.get_limbs();
    std::cout << limbs.size() << std::endl;

    // for (mp_size_t i = 0; i < limbs.size(); i++) {
    //     std::cout << "limb[" << i << "] = "
    //               << std::hex << std::showbase
    //               << limbs[i]
    //               << std::dec << std::endl;
    // }
    


    
}