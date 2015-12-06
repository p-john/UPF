#include "base_test.hpp"
#include "Tokenizer.h"


namespace test_tokenizer{

class TestTokenizer : public base_test::BaseTest {
public:

    TestTokenizer() : base_test::BaseTest("Test Tokenizer") {
        REGISTER_TEST(test_equalOperatorToken);
        REGISTER_TEST(test_unequalOperatorToken);
        REGISTER_TEST(test_equalOperatorTokenlist);
        REGISTER_TEST(test_tokenize);
    }

    static const base_test::TestResult* test_equalOperatorToken(){

        Token tok1("test","123");
        Token tok2("test","123");
        Token tok3("123","test");

        CHECK((tok1 == tok2) == true);
        CHECK((tok1 == tok3) == false);
        return TEST_OK;



    }


    static const base_test::TestResult* test_unequalOperatorToken(){

        Token tok1("test","123");
        Token tok2("test","123");
        Token tok3("123","test");

        CHECK((tok1 != tok2) == false);
        CHECK((tok1 != tok3) == true);
        return TEST_OK;
    }


    static const base_test::TestResult* test_equalOperatorTokenlist() {

        using namespace std;

        list<Token> tok1;
        list<Token> tok2;
        tok1.push_back(Token("[OPTION]","-A"));
        tok1.push_back(Token("BLA","BLA"));
        tok2.push_back(Token("[OPTION]","-A"));
        tok2.push_back(Token("KEKS","BLA"));

        CHECK((tok1 == tok2) == false);
        return TEST_OK;
    }


    static const base_test::TestResult* test_tokenize(){

        using namespace std;

        unordered_map<string,string> validTokens;
        string string_to_tokenize = "-a 192.168.2.1";

        validTokens.emplace("-j","[OPTION]");
        validTokens.emplace("-a","[OPTION]");

        vector<Token> expectedTokens;
        expectedTokens.push_back(Token("[OPTION]","-a"));
        expectedTokens.push_back(Token("[IP]","192.168.2.1"));


        Tokenizer tokenizer(validTokens);
        vector<Token> readTokens = tokenizer.tokenize(string_to_tokenize);

        CHECK(readTokens == expectedTokens);
        return TEST_OK;
    }
};
}
