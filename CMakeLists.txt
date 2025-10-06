cmake_minimum_required(VERSION 3.16)
project(BigIntegerProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include для BigInteger
include_directories(BigInteger_DLL/include)

# Создаем библиотеку BigInteger
add_library(BigInteger STATIC BigInteger_DLL/src/BigInteger.cpp)

# Исполняемый файл
add_executable(main main.cpp)

# Линкуем BigInteger и GMP/GMPXX вручную
target_link_libraries(main PRIVATE BigInteger gmp gmpxx)
