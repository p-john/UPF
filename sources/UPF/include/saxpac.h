#ifndef SAXPAC_HPP
#define SAXPAC_HPP

#include "UPF_manager.h"


namespace saxpac{


class Segment{
  public:
    Range range_;
    unsigned int overlaps_;
    Segment(Range rng): range_(rng), overlaps_(0){}
};

class Bounding_Box{
    public:
        void set_range(const DimensionType dim, const Range& range);
        void set_unknown(const bool unknown);
        inline const Range& get_range(const DimensionType dim) const;
        inline const uint128_t& get_lower_bound(const DimensionType dim) const;
        inline const uint128_t& get_upper_bound(const DimensionType dim) const;
        unsigned int num_dimensions() const;
        void print() const;
        inline bool covers(const Bounding_Box& box) const;
        Bounding_Box();
        Bounding_Box(const Bounding_Box& rhs);
    private:
    Range macsrc_;
    Range macdst_;
    Range ipv4src_;
    Range ipv4dst_;
    Range ipv6src_;
    Range ipv6dst_;
    Range l4protocol_;
    Range tcpsrcport_;
    Range tcpdstport_;
    Range udpsrcport_;
    Range udpdstport_;
    unsigned int dimensions_;
    bool unknown_match_;
};

class DimCut{
public:
  DimensionType dim_;
  uint128_t cut_position_;
  DimCut(const DimensionType dim, const uint128_t& cut_pos)
    :   dim_(dim),
        cut_position_(cut_pos)
        {}
};

class Distinct{
public:
  DimensionType dim_;
  unsigned int distinct_number_;
  Distinct(const DimensionType dim, const unsigned int distinct_number)
    :   dim_(dim),
        distinct_number_(distinct_number)
        {}
};

class DecisionTree;

class Node{
public:
    uint64_t depth() const;
    bool is_cuttable();
    Node* create_child(UPF_Ruleset& ruleset, const Bounding_Box& box,
                       unsigned int& num);
    void cut(DecisionTree& tree, const CuttingType type, const unsigned int threshold);
    unsigned int number_of_rules() const;
    const std::string& get_name() const;
    UPF_Ruleset& get_ruleset();
    Bounding_Box& get_box();
//    void replace_ruleset(const UPF_Ruleset& rhs);
    void split_protocols(std::queue<Node*>&, unsigned int& number_nodes);
    void remove_redundancy_in_box();
    void collect_rulesets(std::vector<std::unique_ptr<UPF_Ruleset>>&);
    void collect_rulesets(std::vector<UPF_Ruleset*>&);
    Node(UPF_Ruleset& ruleset);
    Node(UPF_Ruleset& ruleset, const std::string& tree_name);
    Node(UPF_Ruleset& ruleset, const std::string& name,
         const std::string& tree_name);
    Node(UPF_Ruleset& ruleset, const std::string& name,
         const std::string& tree_name_,
         const unsigned int number, const Bounding_Box& box);
private:
    std::string name_;
    unsigned int num_childs_;
    unsigned int threshold_;
    UPF_Ruleset ruleset_;
    std::vector<std::unique_ptr<Node>> child_nodes_;
    Bounding_Box box_;
    unsigned int node_number_;
    std::string tree_name_;
};

class DecisionTree{
public:
  void create_tree(const CuttingType cutting_type, const unsigned int binth);
  void collect_nodes(std::vector<std::unique_ptr<UPF_Ruleset>>& rulesets);
  void collect_nodes(std::vector<UPF_Ruleset*>& rulesets);
  DecisionTree(UPF_Ruleset& ruleset, const std::string& name,
               const std::string& jump_ruleset);
  DecisionTree(std::unique_ptr<UPF_Ruleset> ruleset, const std::string& name,
               const std::string& jump_ruleset);
//private:
  Node root_node_;
  std::string name_;
  std::string next_tree_;
};

void collect_ranges(const UPF_Ruleset& ruleset,
                    const DimensionType dim,
                    std::vector<Range>& ranges);

void collect_endpoints(const UPF_Ruleset& ruleset,
                              const DimensionType dim ,
                              std::vector<uint128_t>& endpoints);

void collect_endpoints_in_box(const UPF_Ruleset& ruleset,
                              const DimensionType dim ,
                              const Bounding_Box& box,
                              std::vector<uint128_t>& endpoints);

void get_most_distinct_rules(Node* current_node,
                             std::vector<Distinct>& distincts);

void get_most_distinct_endpoints(Node* current_node,
                             std::vector<Distinct>& endpoints);

uint128_t equal_distant(const std::vector<uint128_t>& endpoints,
                        const DimensionType dim,
                        const Bounding_Box& box);

uint128_t equal_segments(const std::vector<uint128_t>& endpoints,
                         const DimensionType dim,
                         const Bounding_Box& box);

uint128_t equal_rules(const UPF_Ruleset& ruleset,
                      const DimensionType dim,
                      const Bounding_Box& box,
                      const std::vector<uint128_t>& endpoints);

uint128_t weighted_segments(const UPF_Ruleset& ruleset,
                            const DimensionType dim,
                            const Bounding_Box& box,
                            const std::vector<uint128_t>& endpoints);

uint128_t find_best_cut(const UPF_Ruleset& ruleset, const Bounding_Box& box,
                        const DimensionType dim, const CuttingType type);

void process_node(Node* current_node, const CuttingType cutting_type,
                  std::queue<Node*>& nodes_to_process, unsigned int& num);

void build_child_node(Node* current_node,
                      UPF_Ruleset& child_ruleset,
                      const Bounding_Box& child_box,
                      const DimensionType best_dim,
                      std::queue<Node*>& nodes_to_process,
                      unsigned int& number);

void duplicate_intersected_rules(const UPF_Ruleset& orig_ruleset,
                                 const Range& range,
                                 const DimensionType dim,
                                 UPF_Ruleset& target_ruleset);
void get_cut_positions(Node* current_node, const CuttingType cutting_type,
                       std::vector<DimCut>& cuts);

DimCut determine_best_dimcut(std::vector<DimCut>& cuts);

DimCut determine_best_dimcut(Node* current_node, const CuttingType type);

void remove_redundancy_in_box(UPF_Ruleset& ruleset,const Bounding_Box& box);

void limit_rule_on_box(const UPF_Rule& rule, Bounding_Box& box);


}
#endif // SAXPAC_HPP
