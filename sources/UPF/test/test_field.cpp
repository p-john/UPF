#include "base_test.hpp"
#include "Field.h"

namespace test_field{


class TestField : public base_test::BaseTest {
public:

    TestField() : base_test::BaseTest("Test Field") {
        REGISTER_TEST(test_uint128_t_operators);
        REGISTER_TEST(test_field_equal_operator);
        REGISTER_TEST(test_range_equal_operator);
        REGISTER_TEST(test_range_covers);
        REGISTER_TEST(test_clone);
        REGISTER_TEST(test_intersect);
        REGISTER_TEST(test_fields_equal_operator);
        REGISTER_TEST(test_diff);
        REGISTER_TEST(test_subtract);
        REGISTER_TEST(test_range_template);
    }

    static const base_test::TestResult* test_uint128_t_operators(){

        uint128_t var1(18446744073709551615U); // max_uint64_t
        uint128_t var2(18446744073709551615U);
        uint128_t var3 = (var1 + var2);

        CHECK(var1 + var2 == var3);
        CHECK(var3 - var1 - var2 == 0);
        CHECK(var1 == var2);
        CHECK(var1 != var3);
        CHECK(var1 < var3);
        CHECK(var3 > var1);
        CHECK(var1 <= var2);
        CHECK(var1 <= var3);
        CHECK(var3 >= var3);

        return TEST_OK;
    }

    static const base_test::TestResult* test_field_equal_operator(){

        IPv4Field field1(Range(1000,2000));
        IPv4Field field2(Range(1000,2000));
        CHECK(field1 == field2);

        return TEST_OK;
    }
    static const base_test::TestResult* test_range_equal_operator(){

        Range rng1(1821,1182);
        Range rng2(1921,4919);
        Range rng3(1821,2341);
        Range rng4(1821,1182);

        CHECK(rng1 != rng2);
        CHECK(rng1 != rng3);
        CHECK(rng1 == rng4);
        CHECK(rng2 != rng3);
        return TEST_OK;
    }

    static const base_test::TestResult* test_range_covers(){

        Range rng1(182112,321201);
        Range rng2(182156,246721);
        Range rng3(128418,219212);
        Range rng4(241921,594321);

        CHECK(rng1.covers(rng2));
        CHECK(!rng1.covers(rng3));
        CHECK(!rng1.covers(rng4));
        CHECK(!rng2.covers(rng3));
        CHECK(rng1.covers(rng1));
        return TEST_OK;
    }


    static const base_test::TestResult* test_clone(){




        IPv4Field field1(Range(1000,2000));
        IPv4Field field2(Range(1000,2000));
        IPv4Field* ptr1 = field1.clone();
        IPv4Field* ptr2 = field2.clone();

        CHECK(field1 == *ptr1);
        CHECK(field2 == *ptr2);

        delete(ptr1);
        delete(ptr2);
        return TEST_OK;
    }


    static const base_test::TestResult* test_intersect(){


    Range rng1(1000,2000);
    Range rng2(1500,2000);
    Range rng3(1900,2100);
    Range rng4(800,1200);

    CHECK(rng1.intersect(rng2));
    CHECK(rng2.intersect(rng1));
    CHECK(rng2.intersect(rng3));
    CHECK(!rng3.intersect(rng4));
    CHECK(rng4.intersect(rng1));
    CHECK(!rng2.intersect(rng4));

    return TEST_OK;
    }

    static const base_test::TestResult* test_diff(){

    // Case right edge
    Range rng1(8,12);
    Range rng2(10,14);
    std::vector<Range> diff_rng1_rng2;
    diff_rng1_rng2.push_back(Range(8,9));

    // case right outside
    Range rng3(8,10);
    Range rng4(6,7);
    std::vector<Range> diff_rng3_rng4;
    diff_rng3_rng4.push_back(Range(8,10));

    // case interval is split
    Range rng5(10,20);
    Range rng6(14,16);
    std::vector<Range> diff_rng5_rng6;
    diff_rng5_rng6.push_back(Range(10,13));
    diff_rng5_rng6.push_back(Range(17,20));

    // case left edge
    Range rng7(10,14);
    Range rng8(8,11);
    std::vector<Range> diff_rng7_rng8;
    diff_rng7_rng8.push_back(Range(12,14));

    // case left outside
    Range rng9(15,18);
    Range rng10(20,25);
    std::vector<Range> diff_rng9_rng10;
    diff_rng9_rng10.push_back(Range(15,18));

    // case interval falls entirely in subtracted interval

    Range rng11(14,16);
    Range rng12(12,18);
    std::vector<Range> diff_rng11_rng12;

    CHECK(rng1.diff(rng2) == diff_rng1_rng2);
    CHECK(rng3.diff(rng4) == diff_rng3_rng4);
    CHECK(rng5.diff(rng6) == diff_rng5_rng6);
    CHECK(rng7.diff(rng8) == diff_rng7_rng8);
    CHECK(rng9.diff(rng10) == diff_rng9_rng10);
    CHECK(rng11.diff(rng12) == diff_rng11_rng12);

    return TEST_OK;
    }

    static const base_test::TestResult* test_subtract(){

    Range rng1(1000,2000);
    Range rng2(1500,2000);
    Range rng3(1900,2100);
    Range rng4(800,1200);
    Range rng5(20,50);
    Range rng6(10,60);
    IPv4Field field1(rng1);
    IPv4Field field2(rng6);


    std::vector<Range> ranges_union1;
    ranges_union1.push_back(rng2);
    ranges_union1.push_back(rng4);

    std::vector<Range> ranges_union2;
    ranges_union2.push_back(rng5);
    ranges_union2.push_back(Range(10,19));
    ranges_union2.push_back(Range(51,60));


    std::vector<Range> result1;
    std::vector<Range> result2;
    field1.subtract(ranges_union1, result1);
    field2.subtract(ranges_union2, result2);
    std::vector<Range> subtract_result1;

    subtract_result1.push_back(Range(1201,1499));

    for (auto iter = result2.begin(); iter != result2.end(); ++iter)
        iter->print();

    CHECK(result1 == subtract_result1);
    return TEST_OK;
    }

    static const base_test::TestResult* test_fields_equal_operator(){

        IPv4Field ipfield1(Range(1000,6000));
        IPv4Field ipfield2(Range(1000,6000));
        IPv4Field ipfield3(Range(1010,6000));;

        CHECK(ipfield1 == ipfield2);
        CHECK(ipfield2 != ipfield3);
        return TEST_OK;
    }
    static const base_test::TestResult* test_range_template(){

//        Range_T<uint8_t> r1(1,2);
//        Range_T<uint32_t> r2(1,2);
//
//        CHECK(r1 == r2);
        return TEST_OK;




    }
};
}
