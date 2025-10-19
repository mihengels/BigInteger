#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <climits>
#include <compare>
#include <iomanip>

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
    const auto& get_limbs() const {
        return limbs_;
    }

    // Далее определяем сравнение больших чисел через спейсшип
    std::strong_ordering operator<=>(const BigInteger& other) const {
        // Сначала учитываем знак
        if (negative_ != other.negative_) {
            return negative_ ? std::strong_ordering::less
                             : std::strong_ordering::greater;
        }

        // Если разное количество лимбов
        if (limbs_.size() != other.limbs_.size()) {
            if (limbs_.size() < other.limbs_.size())
                return negative_ ? std::strong_ordering::greater
                                 : std::strong_ordering::less;
            else
                return negative_ ? std::strong_ordering::less
                                 : std::strong_ordering::greater;
        }

        // Сравниваем по лимбам начиная с самых старших
        for (size_t i = limbs_.size(); i-- > 0;) {
            if (limbs_[i] < other.limbs_[i])
                return negative_ ? std::strong_ordering::greater
                                 : std::strong_ordering::less;
            else if (limbs_[i] > other.limbs_[i])
                return negative_ ? std::strong_ordering::less
                                 : std::strong_ordering::greater;
        }

        // Все лимбы равны
        return std::strong_ordering::equal;
    }

    bool operator==(const BigInteger& other) const {
        if (negative_ != other.negative_) return false;
        if (limbs_.size() != other.limbs_.size()) return false;
        for (size_t i = 0; i < limbs_.size(); ++i) {
            if (limbs_[i] != other.limbs_[i]) return false;
        }
        return true;
    }

    // Расширенный алгоритм Евклида для вычисления НОДа и коэффициентов безу
    static std::tuple<BigInteger, BigInteger, BigInteger> extendedGCD(BigInteger a, BigInteger b) {
        BigInteger x0(1), y0(0);
        BigInteger x1(0), y1(1);

        while (!b.isZero()) {
            BigInteger q = a / b;
            BigInteger r = a % b;

            BigInteger x2 = x0 - q * x1;
            BigInteger y2 = y0 - q * y1;

            a = std::move(b);
            b = std::move(r);
            x0 = std::move(x1);
            y0 = std::move(y1);
            x1 = std::move(x2);
            y1 = std::move(y2);
        }

        return {a, x0, y0}; // a — gcd, x0 и y0 — коэффициенты Безу
    }

    // НОД двух чисел
    static BigInteger gcd(const BigInteger& a, const BigInteger& b) {
        return std::get<0>(extendedGCD(a, b));
    }

    // НОК двух чисел
    static BigInteger lcm(const BigInteger& a, const BigInteger& b) {
        if (a.isZero() || b.isZero()) return BigInteger(0);

        BigInteger g = gcd(a, b);
        BigInteger res = (a / g) * b;
        res.negative_ = false; // НОК всегда положительный
        return res;
    }

    friend std::ostream& operator<<(std::ostream& stream, const BigInteger& bigint);
};

// Литерал для строковых констант (для очень больших чисел)
inline BigInteger operator"" _bi(const char* str, std::size_t) {
    return BigInteger(std::string(str));
}

// Литерал для чисел типа unsigned long long (для обычных целых)
inline BigInteger operator"" _bi(unsigned long long value) {
    return BigInteger(static_cast<long long>(value));
}