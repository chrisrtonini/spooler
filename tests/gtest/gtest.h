#ifndef GTEST_LITE_H
#define GTEST_LITE_H
#include <iostream>
#include <vector>
#include <functional>

namespace testing {
struct TestCase { std::string name; std::function<void()> func; };
inline std::vector<TestCase>& get_tests() { static std::vector<TestCase> v; return v; }
inline int tests_failed = 0;
}

#define TEST(tc, tn) \
void tc##_##tn##_func(); \
struct tc##_##tn##_registrar { \
    tc##_##tn##_registrar() { ::testing::get_tests().push_back({#tc "." #tn, tc##_##tn##_func}); } \
} tc##_##tn##_instance; \
void tc##_##tn##_func()

#define EXPECT_TRUE(cond) \
do { if(!(cond)) { std::cerr << __FILE__ << ":" << __LINE__ << ": Failure EXPECT_TRUE(" #cond ")\n"; ::testing::tests_failed++; } } while(0)

#define EXPECT_EQ(exp, act) \
do { if(!((exp) == (act))) { std::cerr << __FILE__ << ":" << __LINE__ << ": Failure EXPECT_EQ\nExpected: " << (exp) << "\nActual: " << (act) << "\n"; ::testing::tests_failed++; } } while(0)

inline int RUN_ALL_TESTS() {
    for(auto& t : ::testing::get_tests()) {
        std::cout << "[ RUN      ] " << t.name << std::endl;
        t.func();
    }
    if(::testing::tests_failed==0) {
        std::cout << "[  PASSED  ] " << ::testing::get_tests().size() << " tests." << std::endl;
    } else {
        std::cout << "[  FAILED  ] " << ::testing::tests_failed << " tests." << std::endl;
    }
    return ::testing::tests_failed==0 ? 0 : 1;
}

#endif // GTEST_LITE_H
