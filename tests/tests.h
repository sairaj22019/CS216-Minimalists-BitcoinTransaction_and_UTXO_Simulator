#pragma once
#include <string>

class TestRunner {
public:
    static void runTest(int testNumber);

private:
    static void test1_BasicValidTransaction();
    static void test2_MultipleInputs();
    static void test3_DoubleSpendSameTX();
    static void test4_MempoolDoubleSpend();
    static void test5_InsufficientFunds();
    static void test6_NegativeAmount();
    static void test7_ZeroFee();
    static void test8_RaceAttack();
    static void test9_MiningFlow();
    static void test10_UnconfirmedChain();
};