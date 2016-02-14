#include "base_test.hpp"
#include "utility.h"

namespace test_utility {

class TestUtility : public base_test::BaseTest {
public:

    TestUtility() : base_test::BaseTest("Test utility functions") {
        REGISTER_TEST(test_validateIPv4);
        REGISTER_TEST(test_validateIPv6);
        REGISTER_TEST(test_getIPType);
        REGISTER_TEST(test_mac_to_range);
        REGISTER_TEST(test_interface_to_range);
        REGISTER_TEST(test_cidr_to_range);
        REGISTER_TEST(test_ipv4_cidr_to_range);
        REGISTER_TEST(test_ipv4_single_to_range);
        REGISTER_TEST(test_ipv4_range_to_range);
//        REGISTER_TEST(test_ipv6_to_range);
        REGISTER_TEST(test_port_to_range);
        REGISTER_TEST(test_icmp_to_range);
        REGISTER_TEST(test_int_to_ipv4);
        REGISTER_TEST(test_range_to_ipv4_range_score_pf);
        REGISTER_TEST(test_range_to_ipv4_range_score_iptables);
        REGISTER_TEST(test_range_to_ipv4_range_colon);
        REGISTER_TEST(test_range_to_ipv4_range_cidr);
        REGISTER_TEST(test_range_to_port_score);
        REGISTER_TEST(test_range_to_port_colon);
        REGISTER_TEST(test_range_to_l4_protocol);
//        REGISTER_TEST(test_range_to_interface);
        REGISTER_TEST(test_range_to_icmptype);
        REGISTER_TEST(test_ipv4_wildcard);
        REGISTER_TEST(test_port_wildcard);
        REGISTER_TEST(test_l4_prot_wildcard);
        REGISTER_TEST(test_range_to_cidr_list);
//        REGISTER_TEST(test_generate);

    }

    static const base_test::TestResult* test_validateIPv4(){

        using namespace std;

        string ip1 = "192.168.2.1";
        string ip2 = "192.168.2.265";
        string ip3 = "bla.bla.bla.bla";
        string ip4 = "abcdkokasdoköpgks102i112";
        string ip5 = "120";
        string ip6 = "192.168.1";

        using namespace Utility;

        CHECK(validateIPv4(ip1));
        CHECK(!validateIPv4(ip3));
        CHECK(!validateIPv4(ip4));


        return TEST_OK;

    }

    static const base_test::TestResult* test_validateIPv6(){

        using namespace std;

        string ip1 = "192.168.2.1";
        string ip2 = "12dc:f31a:c314:18ab::192a";
        string ip3 = "12sa:asc3:kfg9";

        using namespace Utility;

        CHECK(!validateIPv6(ip1));
        CHECK(validateIPv6(ip2));
        CHECK(!validateIPv6(ip3));


        return TEST_OK;

    }

    static const base_test::TestResult* test_getIPType(){

        using namespace std;

        string ip_single = "192.168.2.1";
        string ip_cidr = "192.168.2.1/24";
        string ip_range_colon = "192.168.2.1:192.168.2.255";
        string ip_range_colon_invalid = "192.168.2.1:192.168.2.256";
        string ip_range_score = "192.168.2.1-192.168.2.255";

        using namespace Utility;

        CHECK(getIPType(ip_single) == ipv4_single);
        CHECK(getIPType(ip_cidr) == ipv4_cidr);
        CHECK(getIPType(ip_range_colon) == ipv4_range_colon);
        CHECK(getIPType(ip_range_score) == ipv4_range_score);


        return TEST_OK;


    }

    static const base_test::TestResult* test_mac_to_range(){

        using namespace Utility;

        std::string mac1 = "01-A0-FF-D3-F1-38"; // 1790998475064
        std::string mac2 = "0a:0b:39:b3:a1:c4"; // 11043328991684

        CHECK(mac_to_range(mac1) == Range(1790998475064,1790998475064));
        CHECK(mac_to_range(mac2) == Range(11043328991684,11043328991684));


        return TEST_OK;
    }

    static const base_test::TestResult* test_interface_to_range(){

        using namespace Utility;

        std::string iface1 = "eth0";    // 101 116 104 48
        std::string iface2 = "xo0";     // 120 111 48
        std::string iface3 = "blub3";   // 98 108 117 98 51

        CHECK(interface_to_range(iface1) == Range(10111610448,10111610448));
        CHECK(interface_to_range(iface2) == Range(12011148,12011148));
        CHECK(interface_to_range(iface3) == Range(981081179851,981081179851));


        return TEST_OK;
    }

    static const base_test::TestResult* test_cidr_to_range(){

        using namespace Utility;

        uint64_t ip_1 = 123489113;    // 	7.92.75.89
        short mask_1 = 24;                //  7.92.75.89/24
        uint64_t ip_2 = 2941081755;

        Range exprng(123489024,123489279);
        Range exprng_2(ip_2,ip_2);
        CHECK(exprng == cidr_to_range(ip_1,mask_1));
        CHECK(exprng_2 == cidr_to_range(ip_2,32));

        return TEST_OK;

    }

    static const base_test::TestResult* test_ipv4_cidr_to_range(){

        using namespace Utility;

        std::string ip1 = "175.77.88.155/32";
        std::string ip2 = "175.77.88.0/24";

        unsigned int ip_int1 = 2941081755;
        unsigned int ip_int2_low = 2941081600;
        unsigned int ip_int2_high = 2941081855;

        CHECK(Range(ip_int1,ip_int1) == ipv4_cidr_to_range(ip1));
        CHECK(Range(ip_int2_low,ip_int2_high) == ipv4_cidr_to_range(ip2));
        return TEST_OK;

    }

    static const base_test::TestResult* test_ipv4_single_to_range(){

        using namespace Utility;

        std::string ip = "175.77.88.155";

        unsigned int ip_int = 2941081755;
        CHECK(Range(ip_int,ip_int) == ipv4_single_to_range(ip));
        return TEST_OK;

    }

    static const base_test::TestResult* test_ipv4_range_to_range(){

        using namespace Utility;

        std::string ip1 = "175.77.88.0:175.77.88.126";  // 2941081600 - 2941081726
        std::string ip2 = "125.76.228.16:128.77.88.126"; // 2102191120 - 2152552574

        CHECK(Range(2941081600,2941081726) == ipv4_range_to_range(ip1));
        CHECK(Range(2102191120,2152552574) == ipv4_range_to_range(ip2));
        return TEST_OK;

    }

    static const base_test::TestResult* test_port_to_range(){

        using namespace Utility;

        std::string port1 = "31243";
        std::string port2 = "4000:30038";
        std::string port3 = "8112-18212";
        CHECK(Range(31243,31243) == port_to_range(port1));
        CHECK(Range(4000,30038) == port_to_range(port2));
        CHECK(Range(8112,18212) == port_to_range(port3));
        return TEST_OK;

    }

    static const base_test::TestResult* test_icmp_to_range(){

        using namespace Utility;

        std::string icmptype1 = "12";
        std::string icmptype2 = "32";

        CHECK(Range(12,12) == icmp_to_range(icmptype1));
        CHECK(Range(32,32) == icmp_to_range(icmptype2));
        return TEST_OK;

    }


    static const base_test::TestResult* test_int_to_ipv4(){

        using namespace Utility;

        uint64_t ip = 8183110903;
        std::string ip_string = int_to_ipv4(ip);

        CHECK(ip_string == "231.192.92.247");


        return TEST_OK;


    }

    static const base_test::TestResult* test_range_to_ipv4_range_score_pf(){

        using namespace Utility;

        Range rng(102812812,132812812);

        std::string ip = "6.32.204.140 - 7.234.144.12";

        CHECK(range_to_ipv4_range_score_pf(rng) == ip);

        return TEST_OK;

    }

    static const base_test::TestResult* test_range_to_ipv4_range_score_iptables(){

        using namespace Utility;

        Range rng(102812812,132812812);

        std::string ip = "6.32.204.140-7.234.144.12";

        CHECK(range_to_ipv4_range_score_iptables(rng) == ip);

        return TEST_OK;

    }
   static const base_test::TestResult* test_range_to_ipv4_range_colon(){

        using namespace Utility;

        Range rng(102812812,132812812);

        std::string ip = "6.32.204.140:7.234.144.12";

        CHECK(range_to_ipv4_range_colon(rng) == ip);

        return TEST_OK;

   }

   static const base_test::TestResult* test_range_to_ipv4_range_cidr(){

        using namespace Utility;

        Range rng(102812812,102812815);
        std::string ip = "6.32.204.140/30";

        CHECK(range_to_ipv4_range_cidr(rng) == ip);

        return TEST_OK;


    }
    static const base_test::TestResult* test_range_to_port_score(){

        using namespace Utility;

        Range rng(24561,32561);

        std::string ip = "24561-32561";

        CHECK(range_to_port_score(rng) == ip);

        return TEST_OK;


    }

    static const base_test::TestResult* test_range_to_port_colon(){

        using namespace Utility;

        Range rng(24561,32561);

        std::string ip = "24561:32561";

        CHECK(range_to_port_colon(rng) == ip);

        return TEST_OK;


    }

    static const base_test::TestResult* test_range_to_l4_protocol(){

        using namespace Utility;

        Range rng(17,17);
        Range rng2(6,6);
        Range rng3(47,47);
        Range rng4(1,1);

        std::string pr = "udp";
        std::string pr2 = "tcp";
        std::string pr3 = "47";
        std::string pr4 = "icmp";

        CHECK(range_to_l4_protocol(rng) == pr);
        CHECK(range_to_l4_protocol(rng2) == pr2);
        CHECK(range_to_l4_protocol(rng3) == pr3);
        CHECK(range_to_l4_protocol(rng4) == pr4);

        return TEST_OK;
    }

    static const base_test::TestResult* test_range_to_interface(){

        using namespace Utility;

        Range rng(10111610448,10111610448);
        Range rng2(12011148,12011148);
        Range rng3(981081179851,981081179851);

        std::string iface1 = "eth0";    // 101 116 104 48
        std::string iface2 = "xo0";     // 120 111 48
        std::string iface3 = "blub3";   // 98 108 117 98 51

        CHECK(range_to_interface(rng) == iface1);
        CHECK(range_to_interface(rng2) == iface2);
        CHECK(range_to_interface(rng3) == iface3);


        return TEST_OK;

    }


    static const base_test::TestResult* test_range_to_icmptype(){

        using namespace Utility;

        Range rng(16,16);

        std::string ip = "16";

        CHECK(range_to_icmptype(rng) == ip);

        return TEST_OK;


    }

    static const base_test::TestResult* test_ipv4_wildcard(){

        using namespace Utility;

        Range rng = ipv4_wildcard();
        Range rng_wildcard = Range(0,4294967295);

        CHECK(rng == rng_wildcard);

        return TEST_OK;
    }

static const base_test::TestResult* test_port_wildcard(){

        using namespace Utility;

        Range rng = port_wildcard();
        Range rng_wildcard = Range(0,65535);

        CHECK(rng == rng_wildcard);

        return TEST_OK;

    }

static const base_test::TestResult* test_l4_prot_wildcard(){

        using namespace Utility;

        Range rng = l4_prot_wildcard();
        Range rng_wildcard = Range(0,255);

        CHECK(rng == rng_wildcard);

        return TEST_OK;

    }

static const base_test::TestResult* test_range_to_cidr_list(){

        using namespace Utility;
        std::string ip1 = "192.1.5.10";
        std::string ip2 = "192.13.7.231";
//        std::string ip1 = "1.2.3.4";
//        std::string ip2 = "1.2.4.1";
        Range rng(ipv4_range_to_range(ip1 + "-" + ip2));

        std::vector<CIDR> results;

        results.push_back(CIDR(ipv4_to_int("192.1.5.10"),31));
        results.push_back(CIDR(ipv4_to_int("192.1.5.12"),30));
        results.push_back(CIDR(ipv4_to_int("192.1.5.16"),28));
        results.push_back(CIDR(ipv4_to_int("192.1.5.32"),27));
        results.push_back(CIDR(ipv4_to_int("192.1.5.64"),26));
        results.push_back(CIDR(ipv4_to_int("192.1.5.128"),25));
        results.push_back(CIDR(ipv4_to_int("192.1.6.0"),23));
        results.push_back(CIDR(ipv4_to_int("192.1.8.0"),21));
        results.push_back(CIDR(ipv4_to_int("192.1.16.0"),20));
        results.push_back(CIDR(ipv4_to_int("192.1.32.0"),19));
        results.push_back(CIDR(ipv4_to_int("192.1.64.0"),18));
        results.push_back(CIDR(ipv4_to_int("192.1.128.0"),17));
        results.push_back(CIDR(ipv4_to_int("192.2.0.0"),15));
        results.push_back(CIDR(ipv4_to_int("192.4.0.0"),14));
        results.push_back(CIDR(ipv4_to_int("192.8.0.0"),14));
        results.push_back(CIDR(ipv4_to_int("192.12.0.0"),16));
        results.push_back(CIDR(ipv4_to_int("192.13.0.0"),22));
        results.push_back(CIDR(ipv4_to_int("192.13.4.0"),23));
        results.push_back(CIDR(ipv4_to_int("192.13.6.0"),24));
        results.push_back(CIDR(ipv4_to_int("192.13.7.0"),25));
        results.push_back(CIDR(ipv4_to_int("192.13.7.128"),26));
        results.push_back(CIDR(ipv4_to_int("192.13.7.192"),27));
        results.push_back(CIDR(ipv4_to_int("192.13.7.224"),29));

        Range rng5(4294967296,uint128_t::max());
        std::vector<CIDR> cidr_list = ipv4_range_to_cidr_list(rng);
//        for (unsigned int i = 0; i < cidr_list.size(); ++i)
//          std::cout << int_to_ipv4(cidr_list[i].addr_) << "/" <<  cidr_list[i].mask_ << std::endl;

        std::string ip3 = int_to_ipv4(1079733014);
        std::string ip4 = int_to_ipv4(4294967295);
        Range rng2(ipv4_range_to_range(ip3 + "-" + ip4));

        std::vector<CIDR> cidr_list2 = ipv4_range_to_cidr_list(rng2);

        CHECK(cidr_list == results);

        return TEST_OK;

    }

//    static const base_test::TestResult* test_brute_force(){
//    using namespace Utility;
//    for (uint32_t i = 0; i < std::numeric_limits<uint32_t>::max() ;++i){
//      for (uint32_t j = 0; j < std::numeric_limits<uint32_t>::max() ;++j){
//        Range rng(i,j);
//        std::cout << i << " - " << j  << std::endl;
//        ipv4_range_to_cidr_list(rng);
//      }

};
}
