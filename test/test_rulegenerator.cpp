#include "base_test.hpp"
#include "RuleGenerator.h"
#include "Parser.h"

namespace test_rulegenerator {

class TestRulegenerator : public base_test::BaseTest {
public:

    TestRulegenerator() : base_test::BaseTest("Test Rulegenerator") {

        REGISTER_TEST(test_generate);
    }

    static const base_test::TestResult* test_generate(){
    IPTablesParser parser;
    RuleGenerator gen;
    std::string rule =  "-A CB_T_1_Node_2 -m iprange --src-range \
                          0.0.0.0-131.177.163.12 -j CB_T_1_Node_6";
    std::vector<Token> result;
    parser.parse(rule,result);

//    for(auto& token : result)
//      std::cout << token.type_ << " : " << token.value_ << std::endl;

    std::unique_ptr<UPF_Rule> r1(gen.generate(result));
//
//    std::cout << *r1 << std::endl;


    return TEST_OK;

    }




 };
}
