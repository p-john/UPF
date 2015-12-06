#include "base_test.hpp"
#include "dec_tree.h"



namespace test_dec_tree{


class TestDecTree : public base_test::BaseTest {
public:

    TestDecTree() : base_test::BaseTest("Test Decision Tree") {
//        REGISTER_TEST(test_cut);
        REGISTER_TEST(test_find_best_cut);
//        REGISTER_TEST(test_collect_rulesets);
    }

//    static const base_test::TestResult* test_cut() {
//
//        cbParser cbparser;
//
//        RuleGenerator generator;
//        UPF_Ruleset ruleset_hicuts;
//
//        std::string str;
//        std::ifstream myfile;
//        myfile.open("E:\\acl1_1K");
//
//
//        while (std::getline(myfile, str)){
//
//            std::istringstream iss(str);
//            std::vector<Token> result_tokens;
//            cbparser.parse(str, result_tokens);
//            ruleset_hicuts.add_rule(generator.generate(result_tokens));
//        }
////        ruleset_hicuts.split_and_remove_redundancy_mt();
//        dec_tree::Node tree(ruleset_hicuts);
//        root.cut(weighted_seg,4);
//
//
//        return TEST_OK;
//
//   }

    static const base_test::TestResult* test_find_best_cut() {

        using namespace dec_tree;

        typedef std::unique_ptr<UPF_Rule> Rule;

        Rule rule1(new UPF_Rule());
        Rule rule2(new UPF_Rule());
        Rule rule3(new UPF_Rule());
        Rule rule4(new UPF_Rule());
        Rule rule5(new UPF_Rule());

        Field* ipv4_1 = new IPv4Field(Range(1000,2000));
        Field* ipv4_2 = new IPv4Field(Range(2500,5000));
        Field* ipv4_3 = new IPv4Field(Range(3000,4000));
        Field* ipv4_4 = new IPv4Field(Range(1500,3000));
        Field* ipv4_5 = new IPv4Field(Range(7000,8000));


        rule1->set_field(l3_src,IPV4,ipv4_1);
        rule2->set_field(l3_src,IPV4,ipv4_2);
        rule3->set_field(l3_src,IPV4,ipv4_2);
        rule4->set_field(l3_src,IPV4,ipv4_4);
        rule5->set_field(l3_src,IPV4,ipv4_5);

        UPF_Ruleset ruleset;

        ruleset.add_rule(std::move(rule1));
        ruleset.add_rule(std::move(rule2));
        ruleset.add_rule(std::move(rule3));
        ruleset.add_rule(std::move(rule4));
        ruleset.add_rule(std::move(rule5));


        Bounding_Box box;
//        box.set_range(ipv4src,Range(0,10000));
//
//        uint128_t cut_position = find_best_cut(ruleset,box,ipv4src,equi_rule);
//
//        CHECK(cut_position == 3000);

        delete(ipv4_1);
        delete(ipv4_2);
        delete(ipv4_3);
        delete(ipv4_4);
        delete(ipv4_5);

        return TEST_OK;

   }

//    static const base_test::TestResult* test_collect_rulesets(){
//        using namespace dec_tree;
//
//        typedef std::unique_ptr<UPF_Rule> Rule;
//
//        Rule rule1(new UPF_Rule());
//        Rule rule2(new UPF_Rule());
//        Rule rule3(new UPF_Rule());
//        Rule rule4(new UPF_Rule());
//        Rule rule5(new UPF_Rule());
//        Rule rule6(new UPF_Rule());
//
//        Field* ipv4_1 = new IPv4Field(Range(1000,2000));
//        Field* ipv4_2 = new IPv4Field(Range(2500,5000));
//        Field* ipv4_3 = new IPv4Field(Range(3000,4000));
//        Field* ipv4_4 = new IPv4Field(Range(1500,3000));
//        Field* ipv4_5 = new IPv4Field(Range(7000,8000));
//        Field* ipv4_6 = new IPv4Field(Range(4001,4001));
//        Field* tcp = new PortField(Range(400,400),TCP);
//
//        rule1->set_field(l3_src,IPV4,ipv4_1);
//        rule2->set_field(l3_src,IPV4,ipv4_2);
//        rule3->set_field(l3_src,IPV4,ipv4_3);
//        rule3->set_field(l3_dst,IPV4,ipv4_3);
//        rule4->set_field(l3_src,IPV4,ipv4_4);
//        rule5->set_field(l3_src,IPV4,ipv4_5);
//        rule6->set_field(l3_src,IPV4,ipv4_6);
//        rule3->set_field(l4_src,TCP, tcp);
//
//        rule1->set_type(freerule);
//        rule2->set_type(freerule);
//        rule3->set_type(freerule);
//        rule4->set_type(freerule);
//        rule5->set_type(freerule);
//        rule6->set_type(freerule);
//        UPF_Ruleset ruleset;
//
//        ruleset.add_rule(std::move(rule1));
//        ruleset.add_rule(std::move(rule2));
//        ruleset.add_rule(std::move(rule3));
//        ruleset.add_rule(std::move(rule4));
//        ruleset.add_rule(std::move(rule5));
//        ruleset.add_rule(std::move(rule6));
//
//        Bounding_Box box;
////        box.set_range(ipv4src,Range(0,10000));
//        std::vector<double> stat_vec;
//        dec_tree::DecisionTree tree;
//        tree.create_tree(equi_rule,1);
//
//
//        std::vector<std::unique_ptr<UPF_Ruleset>> collection;
//        root->collect_rulesets(collection);
////        for (unsigned int i = 0; i < collection.size(); ++i){
////
////            std::cout << collection[i]->get_name() << std::endl;
////            std::cout << "Rules in Ruleset: " << std::endl;
//////            collection[i]->print();
////        }
//
//        delete(ipv4_1);
//        delete(ipv4_2);
//        delete(ipv4_3);
//        delete(ipv4_4);
//        delete(ipv4_5);
//        delete(ipv4_6);
//        delete(tcp);
//        delete(root);
//
//        return TEST_OK;
//
//    }
};
}
