#include "base_test.hpp"
#include "UPF_manager.h"
#include "UPF_exporter.h"


namespace test_upf_manager{


class TestCBSets_multithreaded : public base_test::BaseTest{
public:
    TestCBSets_multithreaded()
        : base_test::BaseTest("Test Synthetic Rule Sets - Multithreaded"){
        REGISTER_TEST(test_cbsets);
        REGISTER_TEST(test_iptables_exporter);
        REGISTER_TEST(test_ipfw_exporter);
        REGISTER_TEST(test_pf_exporter);
    }

    static const base_test::TestResult* test_cbsets(){

        cbParser cbparser;

        RuleGenerator generator;
        UPF_Ruleset ruleset_covers;
        UPF_Ruleset ruleset_fdd;
        std::string str;
        std::ifstream myfile;
        myfile.open("E:\\acl1_5K");


        while (std::getline(myfile, str)){

            std::istringstream iss(str);
            std::vector<Token> result_tokens;
            cbparser.parse(str, result_tokens);
            ruleset_covers.add_rule(generator.generate(result_tokens));
            ruleset_fdd.add_rule(generator.generate(result_tokens));
        }

        std::cout << ruleset_covers.size()
        << " Rules before simple removal" << std::endl;
        auto start = std::chrono::system_clock::now();
        Statistic stats;
        ruleset_covers.split_and_remove_redundancy_mt(stats);
        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
        std::cout << ruleset_covers.size()
        << " Rules after simple removal" << std::endl;
        std::cout << "Processing Time: " << elapsed.count() << std::endl;

        std::cout << std::endl;

        return TEST_OK;

    }

    static const base_test::TestResult* test_iptables_exporter(){

        cbParser cbparser;
        RuleGenerator generator;
        UPF_Ruleset ruleset_covers;
        IPTables_Exporter exporter;
        std::string str;
        std::ifstream myfile;
        myfile.open("E:\\acl1_100");


        while (std::getline(myfile, str)){

            std::istringstream iss(str);
            std::vector<Token> result_tokens;
            cbparser.parse(str, result_tokens);
            ruleset_covers.add_rule(generator.generate(result_tokens));
        }

        std::cout << "Exporting IPTables " << std::endl;
//        exporter.upf_export(ruleset_covers,"./iptables_100",false);



        return TEST_OK;

    }

    static const base_test::TestResult* test_ipfw_exporter(){

        cbParser cbparser;
        RuleGenerator generator;
        std::unique_ptr<UPF_Ruleset> ruleset_covers(new UPF_Ruleset());
        std::vector<std::unique_ptr<UPF_Ruleset>> rulesets;
        ipfw_Exporter exporter;
        std::string str;
        std::ifstream myfile;
        myfile.open("E:\\acl1_10");


        while (std::getline(myfile, str)){

            std::istringstream iss(str);
            std::vector<Token> result_tokens;
            cbparser.parse(str, result_tokens);
            ruleset_covers->add_rule(generator.generate(result_tokens));
        }

        rulesets.push_back(std::move(ruleset_covers));
        std::cout << "Exporting ipfw " << std::endl;
        exporter.upf_export(rulesets,"E:\\ipfw_set",false, false);



        return TEST_OK;

    }


    static const base_test::TestResult* test_pf_exporter(){

        cbParser cbparser;
        RuleGenerator generator;
        UPF_Ruleset ruleset_covers;
        pf_Exporter exporter;
        std::string str;
        std::ifstream myfile;
        myfile.open("./acl1_1K");


        while (std::getline(myfile, str)){

            std::istringstream iss(str);
            std::vector<Token> result_tokens;
            cbparser.parse(str, result_tokens);
            ruleset_covers.add_rule(generator.generate(result_tokens));
        }

        std::cout << "Exporting pf " << std::endl;
//        exporter.upf_export(ruleset_covers,"./pf_set",false);



        return TEST_OK;

    }

};


class TestCBSets_single : public base_test::BaseTest{
public:
    TestCBSets_single()
        : base_test::BaseTest("Test Synthetic Rule Sets - Singlethreaded"){
        REGISTER_TEST(test_cbsets);
    }

    static const base_test::TestResult* test_cbsets(){

        cbParser cbparser;

        RuleGenerator generator;
        UPF_Ruleset ruleset_covers;
        UPF_Ruleset ruleset_fdd;
        std::string str;
        std::ifstream myfile;
        myfile.open("E:\\acl1_1K");


        while (std::getline(myfile, str)){

            std::istringstream iss(str);
            std::vector<Token> result_tokens;
            cbparser.parse(str, result_tokens);
            ruleset_covers.add_rule(generator.generate(result_tokens));
            ruleset_fdd.add_rule(generator.generate(result_tokens));
        }

        std::cout << ruleset_covers.size()
        << " Rules before simple removal" << std::endl;
        auto start = std::chrono::system_clock::now();
        ruleset_covers.split_and_remove_redundancy();
        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
        std::cout << ruleset_covers.size()
         << " Rules after simple removal" << std::endl;
        std::cout << std::endl << "Processing Time: " << elapsed.count() << std::endl;

        return TEST_OK;

    }




};
}
