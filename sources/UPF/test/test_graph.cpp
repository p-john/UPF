#include "base_test.hpp"
#include "graph.h"
#include "UPF_manager.h"
#include "stdlib.h"
#include "time.h"
#include "fdd.h"

namespace test_graph{

class TestGraph : public base_test::BaseTest {
public:

    TestGraph() : base_test::BaseTest("Test Graph") {
      REGISTER_TEST(test_buildPath);
      REGISTER_TEST(test_buildTree);
      REGISTER_TEST(test_build_rule);
      REGISTER_TEST(test_queue_path);
      REGISTER_TEST(test_path_to_root);
      REGISTER_TEST(test_append_path);
      REGISTER_TEST(test_append_rule);
      REGISTER_TEST(test_replicate);
      REGISTER_TEST(test_merge_edges);
    }

    static const base_test::TestResult* test_buildPath(){
      typedef std::unique_ptr<UPF_Rule> Rule;
      typedef std::unique_ptr<Field> field;

      Rule rule1(new UPF_Rule());
      field ipv4_field(new IPv4Field(Range(1000,2000)));
      field ifield(new InterfaceField(Utility::interface_to_range("eth0")));
      field tcpportfield(new PortField(Range(500,600), TCP));
      field macfield(new MacField(Range(100,200)));
      field ipv4_field_2(new IPv4Field(Range(1000,2000)));
      rule1->set_field(l2_src,ETHERNET, macfield.get());
      rule1->set_field(l3_src, IPV4, ipv4_field_2.get());
      rule1->set_field(l3_dst, IPV4, ipv4_field.get());
      rule1->set_field(interface_in_field, ETHERNET, ifield.get());
      rule1->set_field(l4_src, TCP, tcpportfield.get());

      Path path(*rule1);
//      std::cout << path;

      return TEST_OK;
    }

    static const base_test::TestResult* test_buildTree(){

      Node* node_ptr = new Node("test_node1");
      Tree t(node_ptr);
      uint64_t new_edge = t.add_edge_at(new Edge(Range(100,200)),1);
      t.add_node_at(new Node("KeksNode"),new_edge);
      new_edge = t.add_edge_at(new Edge(Range(200,300)),1);
      t.add_node_at(new Node("BlaNode"),new_edge);
//      std::cout << t;
      Node* root = t.get_node_by_id(1);
      Node* app_tree(root->replicate());
      t.add_edge_at(new Edge(Range(500,700),root, app_tree),1);
//      std::cout << t;
      t.register_tree(app_tree);
//      std::cout << t;
      return TEST_OK;
    }

    static const base_test::TestResult* test_build_rule(){

      typedef std::unique_ptr<UPF_Rule> Rule;
      typedef std::unique_ptr<Field> field;

      Rule rule1(new UPF_Rule());
      field ipv4_field(new IPv4Field(Range(1000,2000)));
      field ifield(new InterfaceField(Utility::interface_to_range("eth0")));
      field tcpportfield(new PortField(Range(500,600), TCP));
      field macfield(new MacField(Range(100,200)));
      field ipv4_field_2(new IPv4Field(Range(1000,2000)));
      rule1->set_field(l2_src,ETHERNET, macfield.get());
      rule1->set_field(l3_src, IPV4, ipv4_field_2.get());
      rule1->set_field(l3_dst, IPV4, ipv4_field.get());
      rule1->set_field(interface_in_field, ETHERNET, ifield.get());
      rule1->set_field(l4_src, TCP, tcpportfield.get());

//      Path path(*rule1);
//
//      std::unique_ptr<UPF_Ruleset> rset(path.build_rules());
//
//      CHECK(rset->get_rule(0) == *rule1);

      return TEST_OK;
    }

    static const base_test::TestResult* test_append_rule(){

      typedef std::unique_ptr<Field> field;
//      typedef std::unique_ptr<UPF_Ruleset> Ruleset;

      std::unique_ptr<UPF_Rule> rule1(new UPF_Rule());
      std::unique_ptr<UPF_Rule> rule2(new UPF_Rule());
      std::unique_ptr<UPF_Rule> rule3(new UPF_Rule());
      std::unique_ptr<UPF_Rule> rule4(new UPF_Rule());
        field ipv4_1(new IPv4Field(Range(20,50)));
        field ipv4_2(new IPv4Field(Range(10,60)));
        field ipv4_3(new IPv4Field(Range(35,65)));
        field ipv4_4(new IPv4Field(Range(15,45)));
        field ipv4_5(new IPv4Field(Range(30,40)));
        field ipv4_6(new IPv4Field(Range(25,55)));
        field ipv4_7(new IPv4Field(Range(1,100)));

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


      UPF_Ruleset rset;
      rset.add_rule(std::move(rule1));
      rset.add_rule(std::move(rule2));
      rset.add_rule(std::move(rule3));
      rset.add_rule(std::move(rule4));

      FDD fdd;
//      fdd.remove_with_fdd(rset);
//      Tree t(*rule1);
      UPF_Ruleset result;
      fdd.create_eff_sets(rset,result);
//      std::cout  << result;
//      std::cout << *fdd.create_fdd(rset);
//      t.add_rule(*rule2);
//      t.add_rule(*rule3);
//      t.add_rule(*rule4);

//      Tree_PT tpt;
////      std::cout << t << std::endl;
////      std::cout << *t.root() << std::endl;
//      tpt.register_paths(t.root());
//      tpt.print_paths();
//      UPF_Ruleset* r1 = new UPF_Ruleset();
//      UPF_Ruleset* r2 = new UPF_Ruleset();
//      UPF_Ruleset* r3 = new UPF_Ruleset();


//      std::cout << t;
//      std::cout << *t.root();
////      RuleVector rulevec;
////      t.append_rule(rulevec, t.root(), *rule2);
////      t.register_all();
//
//        std::cout << *t.add_rule(*rule2);
//              std::cout << t;
//      std::cout << *t.root();
//        std::cout << *t.add_rule(*rule3);
//              std::cout << t;
//      std::cout << *t.root();
//        std::cout << *t.add_rule(*rule4);
//              std::cout << t;
//      std::cout << *t.root();
////      r1.append(rulevec.rules);
////      rulevec.rules.clear();
//      std::cout << t;
//      std::cout << *t.root();
//      t.append_rule(rulevec, t.root(), *rule3);
//        t.register_all();
//
//       r2.append(rulevec.rules);
//            rulevec.rules.clear();
//      std::cout << t;
//      std::cout << *t.root();
//      t.append_rule(rulevec, t.root(), *rule4);
//      t.register_all();
//       r1.append(rulevec.rules);
//      rulevec.rules.clear();

//      std::cout << *r1;
//      std::cout << "------------------------";
//      std::cout << *r2;
//      std::cout << "------------------------";
//      std::cout << *r3;
//
//      std::cout << t;
      return TEST_OK;
    }

     static const base_test::TestResult* test_queue_path(){

      Node* n1 = new Node("n1");
      Node* n2 = new Node("n2");
      Node* n3 = new Node("n3");
      Node* n4 = new Node("n4");
      Edge* e2 = new Edge(Range(5,6));
      Edge* e3 = new Edge(Range(8,10));
      Edge* e4 = new Edge(Range(5,9));
      e2->origin_node_ = n2;
      e2->target_node_ = n3;
      e3->origin_node_ = n3;
      e3->target_node_ = n4;
      n2->add_edge(e2);
      n3->add_edge(e3);
      e4->set_target(n2);

      Tree t(n1);
      std::pair<Node*, Edge*> pa(std::make_pair(t.root(), e4));
      t.queue_path(pa);
      t.register_all();

      return TEST_OK;

     }
     static const base_test::TestResult* test_append_path(){

      Node* n1 = new Node("n1");
      Node* n2 = new Node("n2");
      Node* n3 = new Node("n3");
      Node* n4 = new Node("n4");
      Edge* e1 = new Edge(Range(2,3));
      Edge* e2 = new Edge(Range(5,6));
      Edge* e3 = new Edge(Range(8,10));
      Edge* e4 = new Edge(Range(5,9));
      e2->origin_node_ = n2;
      e2->target_node_ = n3;
      e3->origin_node_ = n3;
      e3->target_node_ = n4;
      n2->add_edge(e2);
      n3->add_edge(e3);
      Tree t(n1);
      uint64_t edge1 = t.add_edge_at(e1,1);
      t.append_node_at(n2,edge1);
//      std::cout << t;
      uint64_t edge2 = t.add_edge_at(e4,2);
      Node* rep = t.replicate_at(3);
//      std::cout << *rep;
      t.append_node_at(rep, edge2);
//      std::cout << t;
      return TEST_OK;
    }


     static const base_test::TestResult* test_path_to_root(){

      typedef std::unique_ptr<Field> field;


      std::unique_ptr<UPF_Rule> rule1(new UPF_Rule());
      std::unique_ptr<UPF_Rule> rule2(new UPF_Rule());
      std::unique_ptr<UPF_Rule> rule3(new UPF_Rule());
      std::unique_ptr<UPF_Rule> rule4(new UPF_Rule());
        field ipv4_1(new IPv4Field(Range(20,50)));
        field ipv4_2(new IPv4Field(Range(10,60)));
        field ipv4_3(new IPv4Field(Range(35,65)));
        field ipv4_4(new IPv4Field(Range(15,45)));
        field ipv4_5(new IPv4Field(Range(30,40)));
        field ipv4_6(new IPv4Field(Range(25,55)));
        field ipv4_7(new IPv4Field(Range(1,100)));

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


        Tree t(*rule1);
        Path p(*rule1,l3_src);

//        std::cout << p;
//
//        t.append_path_to_root(p,t.);
//
//        std::cout << p;

      return TEST_OK;
    }

    static const base_test::TestResult* test_replicate(){

      Node* n1 = new Node("n1");
      Node* n2 = new Node("n2");
      Node* n3 = new Node("n3");
      Node* n4 = new Node("n4");
      Edge* e1 = new Edge(Range(2,3));
      Edge* e2 = new Edge(Range(5,6));
      Edge* e3 = new Edge(Range(8,10));
      Edge* e4 = new Edge(Range(5,9));
      e2->origin_node_ = n2;
      e2->target_node_ = n3;
      e3->origin_node_ = n3;
      e3->target_node_ = n4;
      n2->add_edge(e2);
      n3->add_edge(e3);
      Tree t(n1);
      uint64_t edge1 = t.add_edge_at(e1,1);
      t.append_node_at(n2,edge1);
      uint64_t edge2 = t.add_edge_at(e4,2);
      Node* rep = t.replicate_at(3);
      t.append_node_at(rep, edge2);

      return TEST_OK;
    }

    static const base_test::TestResult* test_merge_edges(){

      std::vector<Range> label1;
      label1.push_back(Range(10,20));
      label1.push_back(Range(40,50));
      label1.push_back(Range(31,39));
      label1.push_back(Range(51,80));
      Node* root =  new Node("root");
      Edge* e1 = new Edge(label1);
      Edge* e2 = new Edge(Range(21,30));

      Tree t(root);
      uint64_t id1 = t.add_edge_at(e1,1);
      uint64_t id2 = t.add_edge_at(e2,1);
      t.merge_edges(id1,id2);

      CHECK(e1->label_[0] == Range(10,80));


      return TEST_OK;
    }
   };
}
