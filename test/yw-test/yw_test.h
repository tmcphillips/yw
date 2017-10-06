#pragma once

#include "yw_test_msvc.h"
#include "yw_test_cpputest.h"
#include "yw_types.h"

#define EOL "\n"

namespace yw {
    namespace test {

        class Assert {

        public:

            static void AreEqual(const char* expected, std::string actual, bool ignoreCase = false, const wchar_t* w_message = nullptr) {

                #ifdef MSTEST
                    MSTEST::AreEqual(expected, actual.c_str(), ignoreCase, w_message);
                #endif

                #ifdef CPPUTEST
                    STRCMP_EQUAL(expected, actual.c_str());
                #endif
            }

            static void EmptyString(std::string actual) {

                #ifdef MSTEST
                    MSTEST::IsTrue(actual.empty());
                #endif

                #ifdef CPPUTEST
                    CHECK(actual.empty());
                #endif
            }

            static void NonEmptyString(std::string actual) {

                #ifdef MSTEST
                    MSTEST::IsTrue(!actual.empty());
                #endif

                #ifdef CPPUTEST
                    CHECK(!actual.empty());
                #endif
            }

            template<typename T>
            static void IsNull(const T* actual, const wchar_t* message = nullptr) {

                #ifdef MSTEST
                    MSTEST::IsNull(actual, message);
                #endif

                #ifdef CPPUTEST
                    POINTERS_EQUAL(nullptr, actual);
                #endif
            }

            template<typename T>
            static void IsNotNull(const T* actual, const wchar_t* message = nullptr) {

                #ifdef MSTEST
                    MSTEST::IsNotNull(actual, message);
                #endif

                #ifdef CPPUTEST
                    CHECK(actual != nullptr);
                #endif
            }

            template<typename T>
            static void IsNull(nullable<T> actual, const wchar_t* message = nullptr) {

                #ifdef MSTEST
                    MSTEST::IsTrue(!actual.hasValue(), message);
                #endif

                #ifdef CPPUTEST
                    CHECK(!actual.hasValue());
                #endif
            }

            template<typename T>
            static void AreEqual(const T& expected, const T& actual, const wchar_t* message = nullptr) {

                #ifdef MSTEST
                    MSTEST::AreEqual(expected, actual, message);
                #endif

                #ifdef CPPUTEST
                    CHECK_EQUAL(expected, actual);
                #endif
            }

            template<typename T>
            static void AreEqual(const T& expected, const nullable<T>& actual, const wchar_t* message = nullptr) {

                #ifdef MSTEST
                    MSTEST::AreEqual(expected, actual.getValue(), message);
                #endif

                #ifdef CPPUTEST
                    CHECK_EQUAL(expected, actual.getValue());
                #endif
            }

            template<typename T>
            static void AreEqual(const nullable<T>& expected, const T& actual, const wchar_t* message = nullptr) {

                #ifdef MSTEST
                    MSTEST::AreEqual(expected.getValue(), actual, message);
                #endif

                #ifdef CPPUTEST
                    CHECK_EQUAL(expected.getValue(), actual);
                #endif
            }

            template<typename T>
            static void AreEqual(const nullable<T>& expected, const nullable<T>& actual, const wchar_t* message = nullptr) {

                #ifdef MSTEST
                    MSTEST::AreEqual(expected.getValue(), actual.getValue(), message);
                #endif

                #ifdef CPPUTEST
                    CHECK_EQUAL(expected.getValue(), actual.getValue());
                #endif
            }

            static void AreEqual(const int& expected, const long& actual, const wchar_t* message = nullptr) {

                #ifdef MSTEST
                    MSTEST::AreEqual((long)expected, actual, message);
                #endif

                #ifdef CPPUTEST
                    LONGS_EQUAL(expected, actual);
                #endif
            }

            static void AreEqual(const int& expected, const size_t& actual, const wchar_t* message = nullptr) {

                #ifdef MSTEST
                    MSTEST::AreEqual((size_t)expected, actual, message);
                #endif

                #ifdef CPPUTEST
                    UNSIGNED_LONGS_EQUAL(expected, actual);
                #endif
            }

            static void Fail(const wchar_t* w_message = nullptr) {

                #ifdef MSTEST
                    MSTEST::Fail(w_message);
                #endif      

                #ifdef CPPUTEST
                    FAIL((wchar2string(w_message)).c_str());
                #endif
            }

            static void IsTrue(const bool& condition, const wchar_t* message = nullptr) {

                #ifdef MSTEST
                    MSTEST::IsTrue(condition);
                #endif

                #ifdef CPPUTEST
                    CHECK(condition);
                #endif
            }

            static void IsFalse(const bool& condition, const wchar_t* message = nullptr) {

                #ifdef MSTEST
                    MSTEST::IsFalse(condition);
                #endif

                #ifdef CPPUTEST
                    CHECK(!condition);
                #endif
            }
        };

        class Expect : public Assert {
        };
    }
}
