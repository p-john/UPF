#include "base_test.hpp"
#include "Parser.h"
#include "RuleGenerator.h"
#include <chrono>
#include "UPF_manager.h"
#include "UPF_exporter.h"

namespace test_parser{


class TestParser : public base_test::BaseTest{
    public:

        TestParser() : base_test::BaseTest("Test Parser"){
            REGISTER_TEST(test_IPTablesParser);
//            REGISTER_TEST(test_IPtables_is_valid);
            REGISTER_TEST(test_cbParser);
            REGISTER_TEST(test_pf_normalize);
            REGISTER_TEST(test_pf_parser);
        }



    static const base_test::TestResult* test_IPTablesParser(){

        IPTablesParser ipparser;
        RuleGenerator generator;
        std::unique_ptr<UPF_Ruleset> ruleset_covers(new UPF_Ruleset());
        IPTables_Exporter exporter;
        std::string str;
        std::ifstream myfile;
        myfile.open("E:\\iptables_fw_100");
        std::vector<std::unique_ptr<UPF_Ruleset>> rulesets;

        while (std::getline(myfile, str)){
            std::istringstream iss(str);
            std::vector<Token> result_tokens;
            ipparser.parse(str, result_tokens);
            ruleset_covers->add_rule(generator.generate(result_tokens));
        }

        rulesets.push_back(std::move(ruleset_covers));

        IPTablesParser myparser;
        std::string string_to_tokenize =
        "-A Node_26680 -p 47 -s 161.67.98.48 -j ACCEPT";
        //  Tokenize string
        std::vector<Token> result_tokens;
        myparser.parse(string_to_tokenize, result_tokens);
        std::string res = "-p 47 -s 161.67.98.48 -j ACCEPT";
        // Create list of expected Tokens

//
//        for (auto iter = result_tokens.begin(); iter != result_tokens.end(); ++iter)
//            iter->print();

//        generator.generate(result_tokens)->print();


        std::vector<Token> expectedTokens;
        expectedTokens.push_back(Token("[L2_SRC]","WILDCARD"));
        expectedTokens.push_back(Token("[L2_DST]","WILDCARD"));
        expectedTokens.push_back(Token("[INTERFACE_IN]","WILDCARD"));
        expectedTokens.push_back(Token("[INTERFACE_OUT]","WILDCARD"));
        expectedTokens.push_back(Token("[L3_PROTOCOL]","IPv4"));
        expectedTokens.push_back(Token("[L3_SRC]","161.67.98.48"));
        expectedTokens.push_back(Token("[L3_DST]","WILDCARD"));
        expectedTokens.push_back(Token("[L4_PROTOCOL]","47"));
        expectedTokens.push_back(Token("[L4_SRC]","WILDCARD"));
        expectedTokens.push_back(Token("[L4_DST]","WILDCARD"));
        expectedTokens.push_back(Token("[TCPFLAGS]","WILDCARD"));
        expectedTokens.push_back(Token("[TCPSTATES]","WILDCARD"));
        expectedTokens.push_back(Token("[ACTION]","ACCEPT"));
        expectedTokens.push_back(Token("[ORIGINAL]",res));
        expectedTokens.push_back(Token("[RULESET]","Node_26680"));
        expectedTokens.push_back(Token("[RULE]","FREE"));
        expectedTokens.push_back(Token("[UNKNOWN_MATCHES]","FALSE"));


        CHECK(expectedTokens == result_tokens);

        string_to_tokenize =
        "-A FORWARD -o eth0 -s 10.10.10.0/24 -p tcp --dport 443 -m state --state NEW -j ACCEPT";
        //  Tokenize string
        res = "-o eth0 -s 10.10.10.0/24 -p tcp --dport 443 -m state --state NEW -j ACCEPT";
        result_tokens.clear();
        myparser.parse(string_to_tokenize, result_tokens);

        // Create list of expected Tokens

        //for (auto iter = result_tokens.begin(); iter != result_tokens.end(); ++iter)
        //    iter->print();

        expectedTokens.clear();
        expectedTokens.push_back(Token("[L2_SRC]","WILDCARD"));
        expectedTokens.push_back(Token("[L2_DST]","WILDCARD"));
        expectedTokens.push_back(Token("[INTERFACE_IN]","WILDCARD"));
        expectedTokens.push_back(Token("[INTERFACE_OUT]","eth0"));
        expectedTokens.push_back(Token("[L3_PROTOCOL]","IPv4"));
        expectedTokens.push_back(Token("[L3_SRC]","10.10.10.0/24"));
        expectedTokens.push_back(Token("[L3_DST]","WILDCARD"));
        expectedTokens.push_back(Token("[L4_PROTOCOL]","tcp"));
        expectedTokens.push_back(Token("[L4_SRC]","WILDCARD"));
        expectedTokens.push_back(Token("[L4_DST]","443"));
        expectedTokens.push_back(Token("[TCPFLAGS]","WILDCARD"));
        expectedTokens.push_back(Token("[TCPSTATES]","NEW"));
        expectedTokens.push_back(Token("[ACTION]","ACCEPT"));
        expectedTokens.push_back(Token("[ORIGINAL]",res));
        expectedTokens.push_back(Token("[RULESET]","FORWARD"));
        expectedTokens.push_back(Token("[RULE]","FREE"));
        expectedTokens.push_back(Token("[UNKNOWN_MATCHES]","FALSE"));


        CHECK(expectedTokens == result_tokens);
        return TEST_OK;
    }


//    static const base_test::TestResult* test_IPtables_is_valid(){
//
//        IPTablesParser myParser;
//        Token tok1("[PARAMETER]","-p");
//        Token tok2("[PROTOCOL]","tcp");
//        Token tok3("[PARAMETER]","-m");
//        Token tok4("[STATE]","state");
//        Token tok5("[OPTION]","--state");
//
//        CHECK(myParser.is_valid_successor(tok1,tok2));
//        CHECK(myParser.is_valid_successor(tok3,tok4));
//        CHECK(myParser.is_valid_successor(tok4,tok5));
//        CHECK(!myParser.is_valid_successor(tok1,tok3));
//
//
//        std::string str =
//        "-A INPUT -i eth1 -s 10.10.10.0/24 -d 10.10.10.1/32 -p tcp
//        -m tcp --dport 22 -m state --state NEW -j ACCEPT";
//
//        std::vector<Token> result_tokens;
//        myParser.parse(str,result_tokens);
//
//
//        return TEST_OK;
//
//    }


    static const base_test::TestResult* test_ipfwParser(){


        return TEST_OK;

    }

    static const base_test::TestResult* test_cbParser(){


        cbParser cbparser;

        std::string string_to_tokenize =
        "@64.91.107.21/32 128.222.130.81/32 \
         0 : 65535 1221 : 1221 0x06/0xFF";
        //  Tokenize string
        std::vector<Token> result_tokens;
        cbparser.parse(string_to_tokenize, result_tokens);

        // Create list of expected Tokens
////
//        for (auto iter = result_tokens.begin(); iter != result_tokens.end(); ++iter)
//            iter->print();

        std::vector<Token> expectedTokens;
        expectedTokens.push_back(Token("[L2_SRC]","WILDCARD"));
        expectedTokens.push_back(Token("[L2_DST]","WILDCARD"));
        expectedTokens.push_back(Token("[INTERFACE_IN]","WILDCARD"));
        expectedTokens.push_back(Token("[INTERFACE_OUT]","WILDCARD"));
        expectedTokens.push_back(Token("[L3_PROTOCOL]","IPv4"));
        expectedTokens.push_back(Token("[L3_SRC]","64.91.107.21/32"));
        expectedTokens.push_back(Token("[L3_DST]","128.222.130.81/32"));
        expectedTokens.push_back(Token("[L4_PROTOCOL]","tcp"));
        expectedTokens.push_back(Token("[L4_SRC]","WILDCARD"));
        expectedTokens.push_back(Token("[L4_DST]","1221:1221"));
        expectedTokens.push_back(Token("[TCPFLAGS]","WILDCARD"));
        expectedTokens.push_back(Token("[TCPSTATES]","WILDCARD"));
        expectedTokens.push_back(Token("[ACTION]","ACCEPT"));
        expectedTokens.push_back(Token("[ORIGINAL]","UPF_RULE"));
        expectedTokens.push_back(Token("[RULESET]","DEFAULT"));
        expectedTokens.push_back(Token("[RULE]","FREE"));
        expectedTokens.push_back(Token("[UNKNOWN_MATCHES]","FALSE"));

        CHECK(expectedTokens == result_tokens);

        return TEST_OK;
    }

    static const base_test::TestResult* test_pf_normalize(){

      pfParser parser;
      std::unique_ptr<UPF_Ruleset> ruleset(new UPF_Ruleset());
      std::string str;
      std::ifstream myfile;
      myfile.open("E:\\acl1_100_pf");
      std::vector<std::unique_ptr<UPF_Ruleset>> rulesets;
      std::stringstream iss;
      parser.normalize(myfile,iss);

      return TEST_OK;
    }

    static const base_test::TestResult* test_pf_parser(){

      pfParser parser;
      std::unique_ptr<UPF_Ruleset> ruleset(new UPF_Ruleset());
      std::vector<Token> result_tokens;
      std::string string_to_tokenize =
        "pass in quick proto tcp from 64.91.107.35 to 251.226.233.233";
      parser.parse(string_to_tokenize, result_tokens);

//      for (auto iter = result_tokens.begin(); iter != result_tokens.end(); ++iter)
//        iter->print();

      std::vector<Token> expectedTokens;
      expectedTokens.push_back(Token("[L2_SRC]","WILDCARD"));
      expectedTokens.push_back(Token("[L2_DST]","WILDCARD"));
      expectedTokens.push_back(Token("[INTERFACE_IN]","WILDCARD"));
      expectedTokens.push_back(Token("[INTERFACE_OUT]","WILDCARD"));
      expectedTokens.push_back(Token("[L3_PROTOCOL]","IPv4"));
      expectedTokens.push_back(Token("[L3_SRC]","64.91.107.35"));
      expectedTokens.push_back(Token("[L3_DST]","251.226.233.233"));
      expectedTokens.push_back(Token("[L4_PROTOCOL]","tcp"));
      expectedTokens.push_back(Token("[L4_SRC]","WILDCARD"));
      expectedTokens.push_back(Token("[L4_DST]","WILDCARD"));
      expectedTokens.push_back(Token("[TCPFLAGS]","WILDCARD"));
      expectedTokens.push_back(Token("[TCPSTATES]","WILDCARD"));
      expectedTokens.push_back(Token("[ACTION]","ACCEPT"));
      expectedTokens.push_back(Token("[ORIGINAL]",string_to_tokenize));
      expectedTokens.push_back(Token("[RULESET]","DEFAULT"));
      expectedTokens.push_back(Token("[RULE]","FREE"));
      expectedTokens.push_back(Token("[UNKNOWN_MATCHES]","FALSE"));

      CHECK(result_tokens == expectedTokens);
      return TEST_OK;
    }
};
}
