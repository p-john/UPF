#include "base_test.hpp"
#include "fw_compressor.h"

namespace test_fw_compressor{


class TestFWCompressor : public base_test::BaseTest {
public:

    TestFWCompressor() : base_test::BaseTest("Test FirewallCompressor") {
        REGISTER_TEST(test_reduce_fdd);
        REGISTER_TEST(test_segment_ordering);
        REGISTER_TEST(test_compress_ruleset);
        }

      static const base_test::TestResult* test_reduce_fdd(){

        FW_Compressor fw;
        typedef std::unique_ptr<Field> field;


        std::unique_ptr<UPF_Rule> rule1(new UPF_Rule());
        std::unique_ptr<UPF_Rule> rule2(new UPF_Rule());
        std::unique_ptr<UPF_Rule> rule3(new UPF_Rule());
        std::unique_ptr<UPF_Rule> rule4(new UPF_Rule());
        std::unique_ptr<UPF_Rule> rule5(new UPF_Rule());
        field ipv4_1(new IPv4Field(Range(41,60)));
        field ipv4_2(new IPv4Field(Range(21,55)));
        field ipv4_3(new IPv4Field(Range(45,80)));
        field ipv4_4(new IPv4Field(Range(1,65)));
        field ipv4_5(new IPv4Field(Range(75,100)));
        field ipv4_6(new IPv4Field(Range(30,50)));
        field ipv4_7(new IPv4Field(Range(45,80)));

        rule1->set_field(l3_src,IPV4, ipv4_1.get());
        rule2->set_field(l3_src,IPV4,ipv4_2.get());
        rule3->set_field(l3_src,IPV4,ipv4_3.get());
        rule4->set_field(l3_src,IPV4,ipv4_4.get());
        rule5->set_field(l3_src,IPV4,ipv4_5.get());
        rule1->set_action(ActionTypes::ACCEPT);
        rule2->set_action(ActionTypes::BLOCK);
        rule3->set_action(ActionTypes::BLOCK);
//        rule3->set_field(l3_dst,IPV4,ipv4_5.get());
        rule3->set_field(l3_dst,IPV4,ipv4_6.get());
        rule4->set_action(ActionTypes::JUMP);
        rule5->set_action(ActionTypes::JUMP);
        rule4->set_field(l3_dst,IPV4,ipv4_7.get());
////        rule4->set_field(l3_dst,IPV4,ipv4_7.get());
//        rule4->set_action(ActionTypes::BLOCK);
//        rule5->

        std::unique_ptr<UPF_Ruleset> rset(new UPF_Ruleset());

        rset->add_rule(std::move(std::unique_ptr<UPF_Rule>(rule1->clone())));
        rset->add_rule(std::move(std::unique_ptr<UPF_Rule>(rule2->clone())));
        rset->add_rule(std::move(std::unique_ptr<UPF_Rule>(rule3->clone())));
        rset->add_rule(std::move(std::unique_ptr<UPF_Rule>(rule4->clone())));
        rset->add_rule(std::move(std::unique_ptr<UPF_Rule>(rule5->clone())));



        fw.compress_ruleset(*rset);
//        UPF_Ruleset result;
//        FDD fdd;
//        fdd.create_eff_sets(*rset, result);
//        std::cout << *fdd.create_fdd(*rset);
//        std::cout << result;

        return TEST_OK;
    }
    static const base_test::TestResult* test_segment_ordering(){

      typedef std::unique_ptr<Node> Ptr_Node;

//      Ptr_Node n1(new Node("root"));
//      Ptr_Node n2(new Node("keks"));
//      Ptr_Node n3(new Node("ugha"));
//
//      Segment seg1(Range(1,1), n1.get());
//      Segment seg2(Range(6,6), n1.get());
////      Segment seg3(Range(31,50), n1.get());
////      Segment seg4(Range(51,60), n2.get());
////      Segment seg5(Range(61,85), n3.get());
////      Segment seg6(Range(86,100), n1.get());
//
//      SegmentSorter seg_sorter(l4_proto);
////      seg_sorter.add_segment(seg3);
////      seg_sorter.add_segment(seg5);
//      seg_sorter.add_segment(seg2);
//      seg_sorter.add_segment(seg1);
////      seg_sorter.add_segment(seg6);
////      seg_sorter.add_segment(seg4);
//
//      seg_sorter.find_minimal_order();
//

      Ptr_Node n1(new Node("root"));
      Ptr_Node n2(new Node("keks"));
      Ptr_Node n3(new Node("ugha"));

      Segment seg1(Range(1,10), n1.get());
      Segment seg2(Range(11,30), n3.get());
      Segment seg3(Range(31,50), n1.get());
      Segment seg4(Range(51,60), n2.get());
      Segment seg5(Range(61,85), n3.get());
      Segment seg6(Range(86,100), n1.get());

      SegmentSorter seg_sorter(tcpflagfield);
      seg_sorter.add_segment(seg3);
      seg_sorter.add_segment(seg5);
      seg_sorter.add_segment(seg2);
      seg_sorter.add_segment(seg1);
      seg_sorter.add_segment(seg6);
      seg_sorter.add_segment(seg4);

      seg_sorter.find_minimal_order();
//      std::cout << seg_sorter;
      std::vector<Range> results;
      results.push_back(Range(51,60));
      results.push_back(Range(11,85));
      results.push_back(Range(1,100));

      std::vector<Segment> segs(seg_sorter.get_segments());

      CHECK(results[0] == segs[0].get_range());
      CHECK(results[1] == segs[1].get_range());
      CHECK(results[2] == segs[2].get_range());

      return TEST_OK;
    }
     static const base_test::TestResult* test_compress_ruleset(){

        cbParser cbparser;

        RuleGenerator generator;
        UPF_Ruleset ruleset_covers;
        UPF_Ruleset ruleset_fdd;
        std::string str;
        std::ifstream myfile;
        myfile.open("./fw1_100");


        while (std::getline(myfile, str)){

            std::istringstream iss(str);
            std::vector<Token> result_tokens;
            cbparser.parse(str, result_tokens);
            //ruleset_covers.add_rule(generator.generate(result_tokens));
            ruleset_fdd.add_rule(generator.generate(result_tokens));
        }
        FW_Compressor fw;
        fw.compress_ruleset(ruleset_fdd);



        return TEST_OK;

     }
};
}
