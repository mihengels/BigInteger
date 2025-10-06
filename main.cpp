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
    mpz_class res1 = a * a * a * a * a * a * a * a * a * a * a * a * a * a * a * a * a * a;

    __mpz_struct* ptr = res1.get_mpz_t();
    mp_size_t n = std::abs(ptr->_mp_size); // число используемых лимбов

    std::cout << "Number of limbs: " << n << std::endl;

    for (mp_size_t i = 0; i < n; i++) {
        std::cout << "limb[" << i << "] = "
                  << std::hex << std::showbase
                  << ptr->_mp_d[i]
                  << std::dec << std::endl;
    }

    BigInteger b(std::string("12345678901234567890123456789028785762752687368256386576785"));
    auto res2 = b * b * b * b * b * b * b * b * b * b * b * b * b * b * b * b * b * b;
    auto limbs = res2.get_limbs();
    std::cout << limbs.size() << std::endl;

    for (mp_size_t i = 0; i < limbs.size(); i++) {
        std::cout << "limb[" << i << "] = "
                  << std::hex << std::showbase
                  << limbs[i]
                  << std::dec << std::endl;
    }
}