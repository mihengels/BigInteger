#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>

/** class BigInteger
 *  class for operations on big integers
 */
class BigInteger {
public:
    using bi_limb_t = u_int64_t;    // Один лимб - 64 битное беззнаковое целое
private:
    std::vector<bi_limb_t> limbs_;  // массив лимбов
    bool negative_;                 // знак числа (false = positive)

    void normalize();
    static int cmpAbs(const BigInteger& a, const BigInteger& b);

    inline bool isZero() const { return limbs_.size() == 1 && limbs_[0] == 0; }

    // Вспомогательные функции для арифметики
    static BigInteger addAbs(const BigInteger& a, const BigInteger& b);
    static BigInteger subAbs(const BigInteger& a, const BigInteger& b);

    using MulFunc = BigInteger(*)(const BigInteger&, const BigInteger&);

    static BigInteger schoolMul(const BigInteger& a, const BigInteger& b);
    static BigInteger karatsubaMul(const BigInteger& a, const BigInteger& b);
    static std::pair<BigInteger, BigInteger> divMod(const BigInteger& a, const BigInteger& b);

public:
    // Конструкторы
    BigInteger() : limbs_(0), negative_(false) {}

    BigInteger(long long value)
        : negative_(value < 0)
        , limbs_{static_cast<bi_limb_t>(value < 0 ? -value : value)}{}

    BigInteger(const std::string& str);
    BigInteger(const BigInteger&) = default;
    BigInteger(BigInteger&&) noexcept = default;
    BigInteger& operator=(const BigInteger&) = default;
    BigInteger& operator=(BigInteger&&) noexcept = default;

    // Операции с присваиванием, через них потом френдов реализуем
    // типо чтобы было меньше копирований, в реализациях этих операций 
    // всё делаем по честному через лимбы, а потом юзаем их во френдах
    BigInteger& operator+=(const BigInteger& other);
    BigInteger& operator-=(const BigInteger& other);
    BigInteger& operator*=(const BigInteger& other);
    BigInteger& operator/=(const BigInteger& other);
    BigInteger& operator%=(const BigInteger& other);

    // друзья арифметические операции над большими числами
    friend BigInteger operator+(const BigInteger& a, const BigInteger& b);
    friend BigInteger operator-(const BigInteger& a, const BigInteger& b);
    friend BigInteger operator/(const BigInteger& a, const BigInteger& b);
    friend BigInteger operator%(const BigInteger& a, const BigInteger& b);
    friend BigInteger operator*(const BigInteger& a, const BigInteger& b);

    // Функция для возведения в степень (для небольших степеней)
    BigInteger pow(unsigned long long exp) const;

    // Функции для доступа к приватным членам
    auto& get_limbs() {
        return limbs_;
    }
};

// Литерал для строковых констант (для очень больших чисел)
inline BigInteger operator"" _bi(const char* str, std::size_t) {
    return BigInteger(std::string(str));
}

// Литерал для чисел типа unsigned long long (для обычных целых)
inline BigInteger operator"" _bi(unsigned long long value) {
    return BigInteger(static_cast<long long>(value));
}