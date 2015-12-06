#ifndef FDD_H
#define FDD_H

#include "graph.h"

class FDD{

public:

  bool have_same_decision(const Node* node, const UPF_Rule& rule);
  bool is_downward_redundant(Node* node, const UPF_Ruleset& eff_set);
  void fdd_append(Tree& tree, Node* node, const UPF_Rule& rule);
  void remove_with_fdd(UPF_Ruleset& ruleset);
  void create_diff_paths(Tree& tree, const UPF_Rule& rule, Node* node,
                            Edge* current_edge, const Range& field_range);
  void create_eff_sets(const UPF_Ruleset& orig_set, UPF_Ruleset& result);

  std::unique_ptr<Tree> create_fdd(const UPF_Ruleset& ruleset);

};
#endif // FDD_H
