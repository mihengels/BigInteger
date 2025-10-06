#include "../include/BigInteger.h"


BigInteger::BigInteger(const std::string& str) {
    negative_ = false;
    limbs_.clear();
    if (str.empty()) {
        limbs_.push_back(0);
        return;
    }
    size_t pos = 0;
    if (str[0] == '-') {
        negative_ = true;
        pos = 1;
    }
    *this = BigInteger(0);
    for (; pos < str.size(); ++pos) {
        if (!isdigit(str[pos])) 
            throw std::runtime_error("invalid digit");
        *this = *this * 10 + (str[pos] - '0');
    }
}

void BigInteger::normalize() {
    while (limbs_.size() > 1 && limbs_.back() == 0)
        limbs_.pop_back();
    if (limbs_.size() == 1 && limbs_[0] == 0)
        negative_ = false;
}

int BigInteger::cmpAbs(const BigInteger& a, const BigInteger& b) {
    if (a.limbs_.size() != b.limbs_.size())
        return a.limbs_.size() < b.limbs_.size() ? -1 : 1;

    for (int i = (int)a.limbs_.size() - 1; i >= 0; i--) {
        if (a.limbs_[i] != b.limbs_[i])
            return a.limbs_[i] < b.limbs_[i] ? -1 : 1;
    }
    return 0;
}

BigInteger BigInteger::addAbs(const BigInteger& a, const BigInteger& b) {
    BigInteger res;
    size_t n = std::max(a.limbs_.size(), b.limbs_.size());
    res.limbs_.resize(n + 1, 0);

    unsigned __int128 carry = 0;
    for (size_t i = 0; i < n; i++) {
        unsigned __int128 sum = carry;
        if (i < a.limbs_.size()) sum += a.limbs_[i];
        if (i < b.limbs_.size()) sum += b.limbs_[i];
        res.limbs_[i] = static_cast<bi_limb_t>(sum);
        carry = sum >> 64;
    }
    if (carry) res.limbs_[n] = static_cast<bi_limb_t>(carry);

    res.normalize();
    return res;
}

BigInteger BigInteger::subAbs(const BigInteger& a, const BigInteger& b) {
    BigInteger res;
    res.limbs_.resize(a.limbs_.size());

    bi_limb_t borrow = 0;
    for (size_t i = 0; i < a.limbs_.size(); ++i) {
        bi_limb_t limb_b = i < b.limbs_.size() ? b.limbs_[i] : 0;
        u_int64_t ai = a.limbs_[i];
        
        // Вычитаем с учётом заёма
        u_int64_t tmp = ai - limb_b - borrow;
        res.limbs_[i] = tmp;

        // Обновляем borrow
        borrow = (ai < limb_b + borrow) ? 1 : 0;
    }

    res.normalize();
    return res;
}


BigInteger& BigInteger::operator+=(const BigInteger& other) {
    if (negative_ == other.negative_) {
        *this = addAbs(*this, other);
    } else {
        if (cmpAbs(*this, other) >= 0) {
            *this = subAbs(*this, other);
        } else {
            *this = subAbs(other, *this);
            negative_ = other.negative_;
        }
    }
    normalize();
    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& other) {
    if (negative_ != other.negative_) {
        *this = addAbs(*this, other);
    } else {
        if (cmpAbs(*this, other) >= 0) {
            *this = subAbs(*this, other);
        } else {
            *this = subAbs(other, *this);
            negative_ = !other.negative_;
        }
    }
    normalize();
    return *this;
}

BigInteger BigInteger::schoolMul(const BigInteger& a, const BigInteger& b) {
    BigInteger res;
    res.limbs_.assign(a.limbs_.size() + b.limbs_.size(), 0);

    for (size_t i = 0; i < a.limbs_.size(); i++) {
        unsigned __int128 carry = 0;
        for (size_t j = 0; j < b.limbs_.size(); j++) {
            unsigned __int128 temp = (unsigned __int128)a.limbs_[i] * b.limbs_[j] + res.limbs_[i+j] + carry;
            res.limbs_[i+j] = static_cast<bi_limb_t>(temp);
            carry = temp >> 64;
        }
        res.limbs_[i + b.limbs_.size()] = static_cast<bi_limb_t>(carry);
    }

    res.negative_ = a.negative_ != b.negative_;
    res.normalize();
    return res;
}

BigInteger BigInteger::karatsubaMul(const BigInteger& a, const BigInteger& b) {
    size_t n = std::max(a.limbs_.size(), b.limbs_.size());
    if (n <= 32) return schoolMul(a, b); // базовый случай

    size_t k = n / 2;

    // Делим числа на старшую и младшую половины
    BigInteger a1, a0, b1, b0;
    a0.limbs_ = std::vector<bi_limb_t>(a.limbs_.begin(), a.limbs_.begin() + std::min(k, a.limbs_.size()));
    a1.limbs_ = std::vector<bi_limb_t>(a.limbs_.begin() + std::min(k, a.limbs_.size()), a.limbs_.end());
    b0.limbs_ = std::vector<bi_limb_t>(b.limbs_.begin(), b.limbs_.begin() + std::min(k, b.limbs_.size()));
    b1.limbs_ = std::vector<bi_limb_t>(b.limbs_.begin() + std::min(k, b.limbs_.size()), b.limbs_.end());

    BigInteger z0 = karatsubaMul(a0, b0);
    BigInteger z2 = karatsubaMul(a1, b1);
    BigInteger z1 = karatsubaMul(a0 + a1, b0 + b1) - z0 - z2;

    // C = z2*B^(2k) + z1*B^k + z0
    BigInteger res;
    res.limbs_.assign(z2.limbs_.size() + 2*k, 0);
    for (size_t i = 0; i < z0.limbs_.size(); i++) res.limbs_[i] += z0.limbs_[i];
    for (size_t i = 0; i < z1.limbs_.size(); i++) res.limbs_[i + k] += z1.limbs_[i];
    for (size_t i = 0; i < z2.limbs_.size(); i++) res.limbs_[i + 2*k] += z2.limbs_[i];

    res.negative_ = a.negative_ != b.negative_;
    res.normalize();
    return res;
}

BigInteger& BigInteger::operator*=(const BigInteger& other) {
    static MulFunc mulAlgo = [](const BigInteger& a, const BigInteger& b) {
        size_t n = a.limbs_.size();
        size_t m = b.limbs_.size();

        if (n <= 32 || m <= 32)
            return schoolMul(a, b);         // маленькие числа
        else if (n <= 1024 && m <= 1024)
            return karatsubaMul(a, b);      // средние числа
        else
            return karatsubaMul(a, b);      // для больших пока оставим schoolMul или FFT
    };

    *this = mulAlgo(*this, other);
    return *this;
}

std::pair<BigInteger, BigInteger> BigInteger::divMod(const BigInteger& a, const BigInteger& b) {
    if (b.isZero())
        throw std::runtime_error("Division by zero");

    BigInteger dividend = a;
    BigInteger divisor = b;
    dividend.negative_ = false;
    divisor.negative_ = false;

    if (cmpAbs(dividend, divisor) < 0)
        return {BigInteger(0), dividend};

    BigInteger quotient;
    quotient.limbs_.resize(dividend.limbs_.size(), 0);
    BigInteger remainder;

    // long division
    for (int i = dividend.limbs_.size() - 1; i >= 0; --i) {
        // сдвигаем remainder на лимб
        remainder.limbs_.insert(remainder.limbs_.begin(), dividend.limbs_[i]);
        remainder.normalize();

        bi_limb_t q = 0;
        bi_limb_t left = 0, right = ULONG_LONG_MAX;

        // бинарный поиск для q
        while (left <= right) {
            bi_limb_t mid = left + (right - left) / 2;
            BigInteger t = divisor * BigInteger(mid);
            if (cmpAbs(t, remainder) <= 0) {
                q = mid;
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        remainder -= divisor * BigInteger(q);
        quotient.limbs_[i] = q;
    }

    quotient.negative_ = a.negative_ != b.negative_;
    remainder.negative_ = a.negative_;

    quotient.normalize();
    remainder.normalize();

    return {quotient, remainder};
}


BigInteger& BigInteger::operator/=(const BigInteger& other) {
    *this = divMod(*this, other).first;
    return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& other) {
    *this = divMod(*this, other).second;
    return *this;
}

BigInteger operator+(const BigInteger& a, const BigInteger& b) {
    BigInteger result = a;
    result += b;
    return result;
}

BigInteger operator-(const BigInteger& a, const BigInteger& b) {
    BigInteger result = a;
    result -= b;
    return result;
}

BigInteger operator/(const BigInteger& a, const BigInteger& b) {
    BigInteger result = a;
    result /= b;
    return result;
}

BigInteger operator%(const BigInteger& a, const BigInteger& b) {
    BigInteger result = a;
    result %= b;
    return result;
}

BigInteger operator*(const BigInteger& a, const BigInteger& b) {
    BigInteger result = a;
    result *= b;
    return result;
}

// Функция для возведения в степень (для небольших степеней)
BigInteger BigInteger::pow(unsigned long long exp) const {
    BigInteger base = *this;
    BigInteger result(1);

    while (exp > 0) {
        if (exp & 1) {       // если текущий бит показателя равен 1
            result *= base;
        }
        base *= base;         // возводим основание в квадрат
        exp >>= 1;            // сдвигаем показатель на 1 бит вправо
    }

    return result;
}

// Функция для возведения в большую степень
BigInteger BigInteger::pow(const BigInteger& exponent) const {
    if (exponent.negative_) 
        throw std::runtime_error("Negative exponent not supported");

    BigInteger exp = exponent;  // копия показателя
    BigInteger base = *this;
    BigInteger result(1);

    while (!exp.isZero()) {
        if ((exp.limbs_[0] & 1) == 1) { // проверка на нечетность
            result *= base;
        }
        base *= base;
        exp /= 2; // деление показателя на 2
    }

    return result;
}