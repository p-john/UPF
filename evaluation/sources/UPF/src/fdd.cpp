#include "fdd.h"

bool FDD::have_same_decision(const Node* node, const UPF_Rule& rule){

  FieldType field_key = static_cast<const FieldNode*>(node)->field_key_;
  Field* field = rule.get_field(field_key);
  Range range;
  if (field != nullptr)
    range = field->get_range();
  for (auto edge : node->edges_){
    if(edge->covers(range)){
      if(field_key != tcpflagfield){
        if(have_same_decision(edge->target_node_,rule))
          return true;
      }
      else if (edge->target_node_->is_terminal()){
        ActionTypes act = static_cast<TerminalNode*>(edge->target_node_)->action_;
        if(act == rule.get_action().get_type())
          return true;
      }
    }
  }
  return false;
}

bool FDD::is_downward_redundant(Node* node, const UPF_Ruleset& eff_set){

  for(unsigned int i = 0; i < eff_set.size(); ++i){
    const UPF_Rule& rule = eff_set.get_rule(i);
    if(have_same_decision(node,rule))
      return true;
  }
  return false;
}

void FDD::create_diff_paths(Tree& tree, const UPF_Rule& rule, Node* node,
                            Edge* current_edge, const Range& field_range){


  FieldType field_key = static_cast<const FieldNode*>(node)->field_key_;

  // Field Range - Edge Range
  std::vector<Range> edge_diffs(current_edge->field_diff_edge(field_range));
  Path p(rule,next_field_key(field_key));
  Edge* path_edge = new Edge(edge_diffs, node, p.get_root());
  tree.add_edge_at(path_edge,node->id_);
  tree.register_tree(p.release_root());

  // Edge Range - Field Range

  std::vector<Range> range_diffs(current_edge->edge_diff_field(field_range));
  current_edge->set_label(range_diffs);

}


void FDD::fdd_append(Tree& tree, Node* node, const UPF_Rule& rule){

  FieldType field_key = static_cast<const FieldNode*>(node)->field_key_;
  Field* field = rule.get_field(field_key);
  Range field_range;
  if (field != nullptr)
    field_range = field->get_range();
   std::vector<Edge*> temp_edges(node->edges_);
  if(field_key != tcpflagfield){
    bool is_disjunct = true;
    for(auto iter = temp_edges.begin();iter != temp_edges.end(); ++iter){
//      std::cout << "Number of Edges in Field: " << fieldtype_to_string(field_key) << "   " << temp_edges.size() << std::endl;
      Edge* current_edge = *iter;
      if(current_edge->equals(field_range)){
//        std::cout << "Edge == field" << std::endl;
        is_disjunct = false;
        fdd_append(tree, current_edge->target_node_, rule);
        break;
      }
      else if(current_edge->covers(field_range)){
        is_disjunct = false;
//        std::cout << "Edge covers field" << std::endl;
        std::vector<Range> edge_diff(current_edge->edge_diff_field(field_range));
        std::vector<Range> range_int(current_edge->intersection(field_range));
        Node* new_replicate_node = current_edge->target_node_->replicate();
        Edge* new_edge = new Edge(range_int, node, new_replicate_node);
        tree.add_edge_at(new_edge, node->id_);
        tree.register_tree(new_replicate_node);
        current_edge->set_label(edge_diff);
        fdd_append(tree, new_replicate_node,rule);
        break;
        }
      else if(current_edge->covered_by(field_range)){
        is_disjunct = false;
//        std::cout << "Edge covered by field" << std::endl;
        std::vector<Range> edge_diff(current_edge->edge_diff_field(field_range));
        std::vector<Range> range_diff(current_edge->field_diff_edge(field_range));
        Node* new_replicate_node1 = current_edge->target_node_->replicate();
        Edge* new_edge1 = new Edge(edge_diff, node, new_replicate_node1);
        tree.add_edge_at(new_edge1, node->id_);
        tree.register_tree(new_replicate_node1);
        Node* new_replicate_node2 = current_edge->target_node_->replicate();
        Edge* new_edge2 = new Edge(range_diff, node, new_replicate_node2);
        tree.add_edge_at(new_edge2, node->id_);
        tree.register_tree(new_replicate_node2);
        fdd_append(tree, current_edge->target_node_, rule);
        break;
      }
      else if(current_edge->intersects(field_range)){
        is_disjunct = false;
//        std::cout << "Edge intersect field" << std::endl;
        std::vector<Range> range_int(current_edge->intersection(field_range));
        Node* new_replicate_node = current_edge->target_node_->replicate();
        Edge* new_edge = new Edge(range_int, node, new_replicate_node);
        tree.add_edge_at(new_edge, node->id_);
        tree.register_tree(new_replicate_node);

        create_diff_paths(tree, rule, node, current_edge, field_range);
        fdd_append(tree, new_replicate_node, rule);
        break;
      }
    }
    if(is_disjunct){
//        std::cout << "Edge disjunct field" << std::endl;
        Path p(rule,next_field_key(field_key));
        Edge* path_edge = new Edge(field_range, node, p.get_root());
        tree.add_edge_at(path_edge,node->id_);
        tree.register_tree(p.release_root());
      }
  }
  else{
//    for(auto iter = temp_edges.begin();iter != temp_edges.end(); ++iter){
//      Edge* current_edge = *iter;
//      if(current_edge->covered_by(field_range) && terminal_node.){
//        std::vector<Range> range_diffs(current_edge->edge_diff_field(field_range));
//        if(range_diffs.empty())
//          tree.remove_edge(current_edge->id_);
//        else
//          current_edge->set_label(range_diffs);
//      }
//    }
    TerminalNode* new_terminal_node = new TerminalNode(rule.get_action().get_type());
    Edge* new_edge = new Edge(field_range, node, new_terminal_node);
    tree.add_edge_at(new_edge,node->id_);
    tree.register_node(new_terminal_node);
  }
}

void FDD::remove_with_fdd(UPF_Ruleset& ruleset){



  // Upward Redundancy Removal
  if(!ruleset.empty()){
    std::unique_ptr<Tree> t_ptr(new Tree(ruleset.get_rule(0)));
    std::vector<std::unique_ptr<UPF_Ruleset>> eff_sets;
    std::unique_ptr<UPF_Ruleset> first_set(new UPF_Ruleset());
    first_set->add_rule(std::unique_ptr<UPF_Rule>(ruleset.get_rule(0).clone()));
    eff_sets.push_back(std::move(first_set));
    std::vector<std::unique_ptr<UPF_Rule>> temp_rules;
    temp_rules.push_back(std::move(std::unique_ptr<UPF_Rule>(ruleset.get_rule(0).clone())));
    for (unsigned int i = 1 ;i < ruleset.size(); ++i){
      UPF_Rule& rule = ruleset.get_rule(i);
//      std::cout << "Appending Rule " << i << std::endl;
      std::unique_ptr<UPF_Ruleset> rset(t_ptr->get_effset_for_rule(rule));
      if(rset->size() > 0){
        eff_sets.push_back(std::move(rset));
        temp_rules.push_back(std::move(std::unique_ptr<UPF_Rule>(rule.clone())));
      }

    }
    ruleset.replace_ruleset(temp_rules);
//     t_ptr.release();
//    std::cout << "Counting Paths" << std::endl;
//    Tree_PT tpt;
//    tpt.register_paths(t_ptr->root());
//    tpt.export_stats();

    // Downward Redundancy Removal
    temp_rules.clear();
    Tree t_d(ruleset.get_rule(ruleset.size()-1));
    temp_rules.push_back(std::move(std::unique_ptr<UPF_Rule>(ruleset.get_rule(ruleset.size()-1).clone())));

//    std::cout << "--------------------------------" << std::endl;
//    std::cout << *t_d.root() << std::endl;

    uint64_t append_counter = 0;

    for(int i = ruleset.size() - 2; i >= 0; --i){

      const UPF_Rule& rule = ruleset.get_rule(i);
      if (!is_downward_redundant(t_d.root(),*eff_sets[i])){
        temp_rules.push_back(std::move(std::unique_ptr<UPF_Rule>(rule.clone())));
        fdd_append(t_d, t_d.root(),rule);
//        std::cout << "append Rule " << i+1 << std::endl;
//        std::cout << "--------------------------------" << std::endl;
        ++append_counter;
//        std::cout << *t_d.root() << std::endl;
      }

//      eff_sets[i].release();
    }

    //        std::cout << "append Rule " << i+1 << std::endl;
//        std::cout << "--------------------------------" << std::endl;
//        std::cout << *t_d.root() << std::endl;

    std::reverse(temp_rules.begin(), temp_rules.end());
    ruleset.replace_ruleset(temp_rules);

  }
}

void FDD::create_eff_sets(const UPF_Ruleset& ruleset, UPF_Ruleset& result){

  std::unique_ptr<Tree> t_ptr(new Tree(ruleset.get_rule(0)));
  result.add_rule(std::unique_ptr<UPF_Rule>(ruleset.get_rule(0).clone()));
  for (unsigned int i = 1 ;i < ruleset.size(); ++i){
    UPF_Rule& rule = ruleset.get_rule(i);
//      std::cout << "Appending Rule " << i << std::endl;
    std::unique_ptr<UPF_Ruleset> rset(t_ptr->get_effset_for_rule(rule));
    result.append(*rset);
  }

//
//  std::cout << result.size() << " new disjunct rules" << std::endl;

}

std::unique_ptr<Tree> FDD::create_fdd(const UPF_Ruleset& ruleset){

  std::unique_ptr<Tree> t_ptr(new Tree(ruleset.get_rule(0)));
  for (unsigned int i = 1 ;i < ruleset.size(); ++i){
    UPF_Rule& rule = ruleset.get_rule(i);
    UPF_Ruleset* rset = t_ptr->get_effset_for_rule(rule);
    delete(rset);
  }

  return std::move(t_ptr);
}
