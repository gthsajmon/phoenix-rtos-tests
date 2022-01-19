// 
// pow_rtos function test 
//

#ifdef TEST

#include "unity.h"

#include "power.h"
#include "mock_exp.h"
#include "mock_common.h"

double tst_exponent;
double tst_base;

void setUp(void)
{
    tst_exponent = 2.0;
    tst_base = 2.0;
}

void tearDown(void)
{
}

// Default double flow
void test_pow_rtosNormalVector(void)
{

    isInteger_ExpectAndReturn(tst_exponent, 0); // not integer exponent case
    log_ExpectAndReturn(tst_base, 3.0);         // log return magic number 3.0
    exp_ExpectAndReturn(6, 5.0);                // exp return magic number 5.0

    TEST_ASSERT_EQUAL_DOUBLE(5, pow_rtos(tst_base, tst_exponent));
}

// Integer optimalisation flow
void test_pow_rtosIntegerExponentVector(void)
{
    log_IgnoreAndReturn(0);
    exp_IgnoreAndReturn(123.456);

    tst_exponent = INT_MAX;

    isInteger_ExpectAndReturn(tst_exponent, 1); // integer exponent case
    quickPow_ExpectAndReturn(tst_base, tst_exponent, 123.456);

    TEST_ASSERT_EQUAL_DOUBLE(123.456, pow_rtos(tst_base, tst_exponent));

    tst_exponent = INT_MIN;

    isInteger_ExpectAndReturn(tst_exponent, 1); // integer exponent case
    quickPow_ExpectAndReturn(tst_base, tst_exponent, 123.456);

    TEST_ASSERT_EQUAL_DOUBLE(123.456, pow_rtos(tst_base, tst_exponent));

    // Fallback to double if out of INT range
    isInteger_IgnoreAndReturn(1); // integer exponent case

    tst_exponent = (double)INT_MAX + 1; // INT overflow

    TEST_ASSERT_EQUAL_DOUBLE(123.456, pow_rtos(tst_base, tst_exponent));

    tst_exponent = (double)INT_MIN - 1; // INT underflow

    TEST_ASSERT_EQUAL_DOUBLE(123.456, pow_rtos(tst_base, tst_exponent));
}

// Base = 0, Exponent = 0 case
void test_pow_rtosZeroBaseExponentVector(void)
{
    TEST_ASSERT_EQUAL_DOUBLE(1.0, pow_rtos(0.0, 0.0));
}

// Base > 0, Exponent = 0 case
void test_pow_rtosZeroExponentVector(void)
{
    TEST_ASSERT_EQUAL_DOUBLE(1.0, pow_rtos(123.456, 0.0));
}

// Base = 0, Exponent > 0 case
void test_pow_rtosZeroBaseVector(void)
{
    TEST_ASSERT_EQUAL_DOUBLE(0.0, pow_rtos(0.0, 123.456));
}

// Base = 0, Exponent < 0 case
void test_pow_rtosZeroBaseNegExponent(void)
{
    TEST_ASSERT_EQUAL_DOUBLE(INFINITY, pow_rtos(0.0, -123.456));
    TEST_ASSERT_EQUAL_DOUBLE(-INFINITY, pow_rtos(-0.0, -123.456));
}

// Base < 0, Exponent > 0 case
void test_pow_rtosNegBaseVector(void)
{
    fmod_ExpectAndReturn(1.0, 2, 1);
    isInteger_ExpectAndReturn(1.0, 1);
    quickPow_ExpectAndReturn(3.0, 1, 3.0);
    TEST_ASSERT_EQUAL_DOUBLE(3.0, pow_rtos(-3.0, 1.0));
}

#endif // TEST
