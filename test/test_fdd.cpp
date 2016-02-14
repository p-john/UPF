#include "base_test.hpp"
#include "fdd.h"

namespace test_fdd{


class TestFDD : public base_test::BaseTest {
public:

    TestFDD() : base_test::BaseTest("Test FDD") {
        REGISTER_TEST(test_upward_fdd);
        REGISTER_TEST(test_create_fdd);
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

    static const base_test::TestResult* test_create_fdd(){

        typedef std::unique_ptr<Field> field;

        std::unique_ptr<UPF_Rule> rule1(new UPF_Rule());
        std::unique_ptr<UPF_Rule> rule2(new UPF_Rule());
        std::unique_ptr<UPF_Rule> rule3(new UPF_Rule());
        std::unique_ptr<UPF_Rule> rule4(new UPF_Rule());
        std::unique_ptr<UPF_Rule> rule5(new UPF_Rule());
        std::unique_ptr<UPF_Rule> rule6(new UPF_Rule());
        field ipv4_1(new PortField(Range(70,95),UDP));
        field ipv4_2(new PortField(Range(10,60),UDP));
        field ipv4_3(new PortField(Range(10,40),UDP));
        field ipv4_4(new PortField(Range(25,50),UDP));
        field ipv4_5(new PortField(Range(45,69),UDP));
        field ipv4_6(new PortField(Range(15,45),UDP));
        field ipv4_7(new PortField(Range(55,80),UDP));
        field ipv4_8(new PortField(Range(20,40),UDP));
        field ipv4_9(new PortField(Range(25,45),UDP));
        field ipv4_10(new PortField(Range(75,90),UDP));
        field ipv4_11(new PortField(Range(15,75),UDP));
        field ipv4_12(new PortField(Range(60,95),UDP));



        rule1->set_field(l4_src,UDP, ipv4_1.get());
        rule1->set_field(l4_dst,UDP, ipv4_2.get());
        rule2->set_field(l4_src,UDP, ipv4_3.get());
        rule2->set_field(l4_dst,UDP, ipv4_4.get());
        rule3->set_field(l4_src,UDP, ipv4_5.get());
        rule3->set_field(l4_dst,UDP, ipv4_6.get());
        rule4->set_field(l4_src,UDP, ipv4_7.get());
        rule4->set_field(l4_dst,UDP, ipv4_8.get());
        rule5->set_field(l4_src,UDP, ipv4_9.get());
        rule5->set_field(l4_dst,UDP, ipv4_10.get());
        rule6->set_field(l4_src,UDP, ipv4_11.get());
        rule6->set_field(l4_dst,UDP, ipv4_12.get());
        rule1->set_action(ActionTypes::ACCEPT);
        rule2->set_action(ActionTypes::BLOCK);
        rule3->set_action(ActionTypes::BLOCK);
        rule4->set_action(ActionTypes::ACCEPT);
        rule5->set_action(ActionTypes::BLOCK);
        rule6->set_action(ActionTypes::BLOCK);

        std::unique_ptr<UPF_Ruleset> rset(new UPF_Ruleset());
        rset->add_rule(std::move(std::unique_ptr<UPF_Rule>(rule1->clone())));
        rset->add_rule(std::move(std::unique_ptr<UPF_Rule>(rule2->clone())));
        rset->add_rule(std::move(std::unique_ptr<UPF_Rule>(rule3->clone())));
        rset->add_rule(std::move(std::unique_ptr<UPF_Rule>(rule4->clone())));
        rset->add_rule(std::move(std::unique_ptr<UPF_Rule>(rule5->clone())));
        rset->add_rule(std::move(std::unique_ptr<UPF_Rule>(rule6->clone())));

        //FDD fdd;
        //std::cout << *fdd.create_fdd(*rset);



        return TEST_OK;

    }
};
}
