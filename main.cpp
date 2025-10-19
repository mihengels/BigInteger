#include <iostream>
#include <gmpxx.h>
#include <iomanip>  // для hex-формата
#include <chrono>
#include "BigInteger_DLL/include/BigInteger.h"

template <typename T>
class test {
    test(T t) = delete;
};


bool equal(mpz_class mpz_a, BigInteger bi_b) {
    // Получаем "сырые" данные из mpz
    __mpz_struct* ptr_mpz_a = mpz_a.get_mpz_t();
    auto ptr_bi_b = bi_b.get_limbs();

    mp_size_t mpz_a_sz = std::abs(ptr_mpz_a->_mp_size); // число используемых лимбов
    size_t bi_b_sz = ptr_bi_b.size();

    // Проверка на знак
    bool mpz_neg = (ptr_mpz_a->_mp_size < 0);
    bool bi_neg = (bi_b < BigInteger(0));
    if (mpz_neg != bi_neg) {
        return false;
    }

    // Проверка на размер
    if (mpz_a_sz != static_cast<mp_size_t>(bi_b_sz)) {
        return false;
    }

    // Сравнение лимбов
    for (mp_size_t i = 0; i < mpz_a_sz; ++i) {
        if (static_cast<uint64_t>(ptr_mpz_a->_mp_d[i]) != ptr_bi_b[i]) {
            return false;
        }
    }

    return true;
}


int main() {

    using clock = std::chrono::steady_clock;

    // test1 Сравним объект типа mpz_class и объект нашего типа, если строки в конструкторе равны
    {
        mpz_class a("2314254674839203746587393920022746464");
        BigInteger b("2314254674839203746587393920022746464");
        std::cout << (equal(a, b) ? "Test 1 passed\n" : "Test 1 failed\n");
    }

    // test2 Сравним объект типа mpz_class и объект нашего типа, если строки в конструкторе разные
    {
        mpz_class a("14254674839203746587393920022746464");
        BigInteger b("2314254674839203746587393920022746464");
        std::cout << (!equal(a, b) ? "Test 2 passed\n" : "Test 2 failed\n");
    }

    // test3 Сравним объект типа mpz_class и объект нашего типа, если числа в конструкторе одинаковые
    {
        mpz_class a(45637);
        BigInteger b(45637);
        std::cout << (equal(a, b) ? "Test 3 passed\n" : "Test 3 failed\n");
    }

    // test4 Сравним объект типа mpz_class и объект нашего типа, если числа в конструкторе разные
    {
        mpz_class a(45637);
        BigInteger b(41637);
        std::cout << (!equal(a, b) ? "Test 4 passed\n" : "Test 4 failed\n");
    }

    // test 5 сложение
    {
        mpz_class a1("1234567890987654321234567667");
        mpz_class a2("98765432234567890876543456654345");
        mpz_class mpz_result = a1 + a2;

        BigInteger b1("1234567890987654321234567667");
        BigInteger b2("98765432234567890876543456654345");
        BigInteger bi_result = b1 + b2;

        std::cout << (equal(mpz_result, bi_result) ? "Test 5 passed\n" : "Test 5 failed\n");
    }

    // test 6 вычитание
    {
        mpz_class a1("1234567890987654321234567667");
        mpz_class a2("98765432234567890876543456654345");
        mpz_class mpz_result = a1 - a2;

        BigInteger b1("1234567890987654321234567667");
        BigInteger b2("98765432234567890876543456654345");
        BigInteger bi_result = b1 - b2;

        std::cout << (equal(mpz_result, bi_result) ? "Test 6 passed\n" : "Test 6 failed\n");
    }

    // test 7 умножение
    {
        mpz_class a1("1234567890987654321234567667");
        mpz_class a2("98765432234567890876543456654345");
        mpz_class mpz_result = a1 * a2;

        BigInteger b1("1234567890987654321234567667");
        BigInteger b2("98765432234567890876543456654345");
        BigInteger bi_result = b1 * b2;

        std::cout << (equal(mpz_result, bi_result) ? "Test 7 passed\n" : "Test 7 failed\n");
    }

    // test 8 деление 
    {
        mpz_class a1("85674567");
        mpz_class a2("27787878887767676767678787989389839345679876545678");
        mpz_class mpz_result = a2 / a1;

        BigInteger b1("85674567");
        BigInteger b2("27787878887767676767678787989389839345679876545678");
        BigInteger bi_result = b2 / b1;

        std::cout << (equal(mpz_result, bi_result) ? "Test 8 passed\n" : "Test 8 failed\n");
    }

    // test 8 остаток от деления
    {
        mpz_class a1("85674567");
        mpz_class a2("27787878887767676767678787989389839345679876545678");
        mpz_class mpz_result = a2 % a1;

        BigInteger b1("85674567");
        BigInteger b2("27787878887767676767678787989389839345679876545678");
        BigInteger bi_result = b2 % b1;

        std::cout << (equal(mpz_result, bi_result) ? "Test 8 passed\n" : "Test 8 failed\n");
    }

    // test 9 >
    {
        
        BigInteger b1("1234567890987654321234567667");
        BigInteger b2("9876543223456789087654345665");

        std::cout << (b1 > b2 ? "Test 9 failed\n" : "Test 9 passed\n");
    }
    
    // test 10 >=
    {
        
        BigInteger b1("1234567890987654321234567667");
        BigInteger b2("9876543223456789087654345665");

        std::cout << (b1 >= b2 ? "Test 10 failed\n" : "Test 10 passed\n");
    }

    // test 11 <
    {
        
        BigInteger b1("1234567890987654321234567667");
        BigInteger b2("9876543223456789087654345665");

        std::cout << (b1 < b2 ? "Test 10 passed\n" : "Test 10 failed\n");
    }

    // test 12 <=
    {
        
        BigInteger b1("1234567890987654321234567667");
        BigInteger b2("9876543223456789087654345665");

        std::cout << (b1 <= b2 ? "Test 11 passed\n" : "Test 11 failed\n");
    }

    // test 12 ==
    {
        
        BigInteger b1("1234567890987654321234567667");
        BigInteger b2("9876543223456789087654345665");

        std::cout << (b1 == b2 ? "Test 12 failed\n" : "Test 12 passed\n");
    }

    // test 12 ==
    {
        
        BigInteger b1("1234567890987654321234567667");
        BigInteger b2("1234567890987654321234567667");

        std::cout << (b1 == b2 ? "Test 12 passed\n" : "Test 12 failed\n");
    }

    // test 12 !=
    {
        
        BigInteger b1("1234567890987654321234567667");
        BigInteger b2("1234567890987654321234567667");

        std::cout << (b1 != b2 ? "Test 12 failed\n" : "Test 12 passed\n");
    }

    // test 13 !=
    {
        
        BigInteger b1("1234567890987654321234567667");
        BigInteger b2("8937367890987654321234567667");

        std::cout << (b1 != b2 ? "Test 13 passed\n" : "Test 13 failed\n");
    }

    // test 14 2^136279841 -1
    {
        
        mpz_class a = 2;
        mpz_class mpz_rez;
        mpz_pow_ui(mpz_rez.get_mpz_t(), a.get_mpz_t(), 136279841);
        mpz_rez -= 1;

        auto start = clock::now();

        BigInteger bi_rez = (2_bi).pow(136279841) - 1_bi;

        auto end = clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

        std::cout << (equal(mpz_rez, bi_rez) ? "Test 14 (Mersen number) passed\t" : "Test 14 failed\t") << "| Duration: " << duration.count() << " seconds\n";
        
    }


}