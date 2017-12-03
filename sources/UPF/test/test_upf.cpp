#include "base_test.hpp"
#include "UPF.h"
#include "RuleGenerator.h"
#include "Parser.h"
#include "UPF_manager.h"
#include <memory>
#include <thread>

namespace test_upf{
class TestUPF : public base_test::BaseTest {
public:

    TestUPF() : base_test::BaseTest("Test UPF") {
        REGISTER_TEST(test_set_and_get_field);
        REGISTER_TEST(test_get_ranges_next_from);
        REGISTER_TEST(test_copy_constructors);
        REGISTER_TEST(test_clone);
        REGISTER_TEST(test_set_range_on_field);
        REGISTER_TEST(test_covers);
        REGISTER_TEST(test_redundancy_removal);
        REGISTER_TEST(test_redundancy_removal_mt);
        REGISTER_TEST(test_new_set_field);

    }

    static const base_test::TestResult* test_set_and_get_field(){

        typedef std::unique_ptr<UPF_Rule> Rule;
        typedef std::unique_ptr<Field> Field_ptr;

        Rule rule1(new UPF_Rule());
        Field_ptr ipv4_field(new IPv4Field(Range(1000,2000)));
        Field_ptr ifield(new InterfaceField(Utility::interface_to_range("eth0")));
        Field_ptr tcpportfield(new PortField(Range(500,600), TCP));
        Field_ptr macfield(new MacField(Range(100,200)));
        Field_ptr ipv4_field_2(new IPv4Field(Range(1000,2000)));
        rule1->set_field(l2_src,ETHERNET, macfield.get());
        rule1->set_field(l3_src, IPV4, ipv4_field_2.get());
        rule1->set_field(l3_dst, IPV4, ipv4_field.get());
        rule1->set_field(interface_in_field, ETHERNET, ifield.get());
        rule1->set_field(l4_src, TCP, tcpportfield.get());
        Field* testipv4field = rule1->get_field(l3_src);
        Field* testifield = rule1->get_field(interface_in_field);
        Field* testtcpfield = rule1->get_field(l4_src);
        CHECK(testipv4field->get_range() == ipv4_field->get_range());
        CHECK(testifield->get_range() == Range(10111610448,10111610448)); // "eth0" as ASCII
        CHECK(testtcpfield->get_range() == Range(500,600));

        Field_ptr udpportfield(new PortField(Range(700,800), UDP));
        rule1->set_field(l4_src, UDP, udpportfield.get());
        Field* testudpportfield = rule1->get_field(l4_src);

//        rule2->set_field(ipv4src,Range(5000,5000));
//        rule2->set_field(ipv4dst,Range(6000,7000));


        CHECK(testudpportfield->get_range() == Range(700,800));
        return TEST_OK;
    }

    static const base_test::TestResult* test_get_ranges_next_from(){

        typedef std::unique_ptr<UPF_Rule> Rule;
        typedef std::unique_ptr<Field> Field;
        Rule rule1(new UPF_Rule());
        Field macfield(new MacField(Range(500,799)));
        Field ipv4_field(new IPv4Field(Range(1000,2000)));
        Field port_field(new PortField(Range(300,200), TCP));
        rule1->set_field(l2_src, ETHERNET, macfield.get());
        rule1->set_field(l3_src,IPV4, ipv4_field.get());
        rule1->set_field(l4_src,TCP, port_field.get());
        std::vector<Range> ranges = rule1->get_ranges_next_from(l2_src);
        CHECK(ranges[4] == Range(1000,2000));
        CHECK(ranges[7] == Range(200,300));

        return TEST_OK;

    }
    static const base_test::TestResult* test_copy_constructors(){

        L2Rule l2_rule;
        L3Rule l3_rule;
        L4Rule l4_rule;
        MacRule mac_rule;
        IPv4Rule ipv4_rule;
        IPv6Rule ipv6_rule;
        TCPRule tcp_rule;
        UDPRule udp_rule;
        ICMPRule icmp_rule;

        L2Rule new_l2(l2_rule);
        L3Rule new_l3(l3_rule);
        L4Rule new_l4(l4_rule);
        MacRule new_mac(mac_rule);
        IPv4Rule new_ipv4(ipv4_rule);
        IPv6Rule new_ipv6(ipv6_rule);
        TCPRule new_tcp(tcp_rule);
        UDPRule new_udp(udp_rule);
        ICMPRule new_icmp(icmp_rule);

        CHECK(l2_rule == new_l2);
        CHECK(l3_rule == new_l3);
        CHECK(l4_rule == new_l4);
        CHECK(mac_rule == new_mac);
        CHECK(ipv4_rule == new_ipv4);
        CHECK(ipv6_rule == new_ipv6);
        CHECK(tcp_rule == new_tcp);
        CHECK(udp_rule == new_udp);
        CHECK(icmp_rule == new_icmp);

        return TEST_OK;

    }

    static const base_test::TestResult* test_clone(){

        typedef std::unique_ptr<UPF_Rule> Rule;
        typedef std::unique_ptr<Field> Field;

        Rule rule1(new UPF_Rule());


        Field mac(new MacField(Range(100,200)));
        Field ipv4(new IPv4Field(Range(3000,4000)));
        Field tcp(new PortField(Range(200,300),TCP));

        rule1->set_field(l2_src,ETHERNET,mac.get());
        rule1->set_field(l3_src,IPV4,ipv4.get());
        rule1->set_field(l4_src,TCP, tcp.get());
        Rule clone_rule1(rule1->clone());

        Range rng1 = rule1->get_field(l2_src)->get_range();
        Range rng2 = clone_rule1->get_field(l2_src)->get_range();

        CHECK(*rule1 == *clone_rule1);
        return TEST_OK;

    }


    static const base_test::TestResult* test_set_range_on_field(){

        typedef std::unique_ptr<Field> Field;

        UPF_Rule rule1;

        Field ipv4(new IPv4Field(Range(1000,2000)));
        rule1.set_field(l3_src,IPV4,ipv4.get());
        rule1.set_range_on_field(l3_src, Range(2000,3000));

        CHECK(rule1.get_field(l3_src)->get_range() == Range(2000,3000));

        return TEST_OK;
    }

    static const base_test::TestResult* test_redundancy_removal(){

        typedef std::unique_ptr<UPF_Rule> Rule;
        typedef std::unique_ptr<Field> Field;


        Rule rule1(new UPF_Rule());
        Rule rule2(new UPF_Rule());
        Rule rule3(new UPF_Rule());
        Rule rule4(new UPF_Rule());
        Field ipv4_1(new IPv4Field(Range(20,50)));
        Field ipv4_2(new IPv4Field(Range(10,60)));
        Field ipv4_3(new IPv4Field(Range(35,65)));
        Field ipv4_4(new IPv4Field(Range(15,45)));
        Field ipv4_5(new IPv4Field(Range(30,40)));
        Field ipv4_6(new IPv4Field(Range(25,55)));
        Field ipv4_7(new IPv4Field(Range(1,100)));

        rule1->set_field(l3_src,IPV4,ipv4_1.get());
        rule2->set_field(l3_src,IPV4,ipv4_2.get());
        rule1->set_field(l3_dst,IPV4,ipv4_3.get());
        rule2->set_field(l3_dst,IPV4,ipv4_4.get());
        rule1->set_action(ActionTypes::ACCEPT);
        rule2->set_action(ActionTypes::BLOCK);
        rule3->set_field(l3_src,IPV4,ipv4_5.get());
        rule3->set_field(l3_dst,IPV4,ipv4_6.get());
        rule3->set_action(ActionTypes::ACCEPT);
        rule4->set_field(l3_src,IPV4,ipv4_7.get());
        rule4->set_field(l3_dst,IPV4,ipv4_7.get());
        rule4->set_action(ActionTypes::BLOCK);

        UPF_Ruleset ruleset;

        ruleset.add_rule(std::move(std::unique_ptr<UPF_Rule>(rule1->clone())));
        ruleset.add_rule(std::move(std::unique_ptr<UPF_Rule>(rule2->clone())));
        ruleset.add_rule(std::move(std::unique_ptr<UPF_Rule>(rule3->clone())));
        ruleset.add_rule(std::move(std::unique_ptr<UPF_Rule>(rule4->clone())));

        ruleset.remove_redundancy();

        CHECK(ruleset.size() == 4);
        CHECK( (ruleset.get_rule(0)) == (*rule1));
        CHECK( (ruleset.get_rule(1)) == (*rule2));
        CHECK( (ruleset.get_rule(2)) == (*rule3));
        CHECK( (ruleset.get_rule(3)) == (*rule4));

        return TEST_OK;
    }


    static const base_test::TestResult* test_redundancy_removal_mt(){

        typedef std::unique_ptr<UPF_Rule> Rule;
        typedef std::unique_ptr<Field> Field;

        Rule rule1(new UPF_Rule());
        Rule rule2(new UPF_Rule());
        Rule rule3(new UPF_Rule());
        Rule rule4(new UPF_Rule());
        Field ipv4_1(new IPv4Field(Range(20,50)));
        Field ipv4_2(new IPv4Field(Range(10,60)));
        Field ipv4_3(new IPv4Field(Range(35,65)));
        Field ipv4_4(new IPv4Field(Range(15,45)));
        Field ipv4_5(new IPv4Field(Range(30,40)));
        Field ipv4_6(new IPv4Field(Range(25,55)));
        Field ipv4_7(new IPv4Field(Range(1,100)));

        rule1->set_field(l3_src,IPV4,ipv4_1.get());
        rule2->set_field(l3_src,IPV4,ipv4_2.get());
        rule1->set_field(l3_dst,IPV4,ipv4_3.get());
        rule2->set_field(l3_dst,IPV4,ipv4_4.get());
        rule1->set_action(ActionTypes::ACCEPT);
        rule2->set_action(ActionTypes::BLOCK);
        rule3->set_field(l3_src,IPV4,ipv4_5.get());
        rule3->set_field(l3_dst,IPV4,ipv4_6.get());
        rule3->set_action(ActionTypes::ACCEPT);
        rule4->set_field(l3_src,IPV4,ipv4_7.get());
        rule4->set_field(l3_dst,IPV4,ipv4_7.get());
        rule4->set_action(ActionTypes::BLOCK);

        UPF_Ruleset ruleset;

        ruleset.add_rule(std::move(std::unique_ptr<UPF_Rule>(rule1->clone())));
        ruleset.add_rule(std::move(std::unique_ptr<UPF_Rule>(rule2->clone())));
        ruleset.add_rule(std::move(std::unique_ptr<UPF_Rule>(rule3->clone())));
        ruleset.add_rule(std::move(std::unique_ptr<UPF_Rule>(rule4->clone())));

        ruleset.remove_redundancy_mt(1);

        CHECK(ruleset.size() == 4);
        CHECK((ruleset.get_rule(0)) == (*rule1));
        CHECK((ruleset.get_rule(1)) == (*rule2));
        CHECK((ruleset.get_rule(2)) == (*rule3));
        CHECK((ruleset.get_rule(3)) == (*rule4));

        return TEST_OK;
    }

       static const base_test::TestResult* test_covers() {

        typedef std::unique_ptr<UPF_Rule> Rule;
        typedef std::unique_ptr<Field> Field;

        Rule rule1(new UPF_Rule());
        Rule rule2(new UPF_Rule());
        Rule rule3(new UPF_Rule());
        Rule rule4(new UPF_Rule());
        Rule rule5(new UPF_Rule());

        Field ipv4_1(new IPv4Field(Range(1000,2000)));
        Field ipv4_2(new IPv4Field(Range(2500,5000)));
        Field ipv4_3(new IPv4Field(Range(3000,4000)));
        Field ipv4_4(new IPv4Field(Range(1500,3000)));
        Field ipv4_5(new IPv4Field(Range(7000,8000)));

        rule1->set_field(l3_src,IPV4,ipv4_1.get());
        rule2->set_field(l3_src,IPV4,ipv4_2.get());
        rule3->set_field(l3_src,IPV4,ipv4_3.get());
        rule4->set_field(l3_src,IPV4,ipv4_4.get());
        rule5->set_field(l3_src,IPV4,ipv4_5.get());
        rule1->set_type(freerule);
        rule2->set_type(freerule);
        rule3->set_type(freerule);
        rule4->set_type(freerule);
        rule5->set_type(freerule);

        CHECK(!rule2->covers(*rule1));
        CHECK(rule2->covers(*rule3));
        CHECK(!rule5->covers(*rule1));

        return TEST_OK;
       }

    static const base_test::TestResult* test_new_set_field(){

      std::unique_ptr<UPF_Rule> rule(new UPF_Rule());
      Range range(200,300);
      rule->set_field(l3_proto, Range(4,4));
      rule->set_field(l3_src,range);
      rule->set_field(l4_proto,Range(6,6));
      rule->set_field(l4_src, Range(500,700));
      rule->set_field(l3_proto,Range(41,41));
      CHECK(rule->get_field(l4_src)->get_range() == Range(500,700));

      return TEST_OK;

    }
};
}
