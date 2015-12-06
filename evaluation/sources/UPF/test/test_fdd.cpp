#include "base_test.hpp"
#include "fdd.h"

namespace test_fdd{


class TestFDD : public base_test::BaseTest {
public:

    TestFDD() : base_test::BaseTest("Test FDD") {
        REGISTER_TEST(test_upward_fdd);
        }

      static const base_test::TestResult* test_upward_fdd(){

        cbParser cbparser;

        RuleGenerator generator;
        UPF_Ruleset ruleset_covers;
        UPF_Ruleset ruleset_fdd;
        std::string str;
        std::ifstream myfile;
        myfile.open("E:\\f.w1_100");


        while (std::getline(myfile, str)){

            std::istringstream iss(str);
            std::vector<Token> result_tokens;
            cbparser.parse(str, result_tokens);
            //ruleset_covers.add_rule(generator.generate(result_tokens));
            ruleset_fdd.add_rule(generator.generate(result_tokens));
        }

//        std::cout << ruleset_fdd.size()
//        << " Rules before FDD removal" << std::endl;
//        auto start = std::chrono::system_clock::now();
//        FDD fdd;
//        fdd.remove_with_fdd(ruleset_fdd);



//        auto end = std::chrono::system_clock::now();
//        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
//         std::cout << ruleset_fdd.size() <<
//         " Rules after FDD removal" << std::endl;
//
////        std::cout << "Processing Time: " << elapsed.count() << std::endl;
////        ruleset_fdd.remove_with_fdd();
//        std::cout << std::endl;

        return TEST_OK;

    }
};
}
