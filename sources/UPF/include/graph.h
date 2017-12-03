#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include "UPF_manager.h"

class Edge;
class Path;
class UPF_Ruleset;

class Node{
public:
  bool operator==(const Node& rhs){return id_ == rhs.id_;}
  bool operator!=(const Node& rhs){return !(*this == rhs);}
  bool is_isomorphic(Node* rhs);
  uint64_t hash();
  void add_edge(Edge* edge_ptr);
  void new_edge(const Range& range, const Node& node);
  virtual inline bool is_terminal() const {return false;}
  virtual inline bool is_field() const {return false;}
  virtual Node* clone() const;
  void delete_edge(const uint64_t edge_id);
  Node* replicate() const;
  uint64_t size() const;
  Node(const std::string& name);
  Node(const std::string& name, uint64_t id);
  Node(const FieldType field_key);
  Node(const Node& node);
  virtual ~Node();
  friend std::ostream& operator<<(std::ostream& out, Node& node);

  uint64_t id_;
  std::string name_;
  Edge* incoming_edge_;
  std::vector<Edge*> edges_;

};

class FieldNode : public Node{
public:
  bool operator==(const FieldNode& rhs){return field_key_ == rhs.field_key_;}
  bool operator!=(const FieldNode& rhs){return !(field_key_ == rhs.field_key_);}
  virtual inline bool is_field() const{return true;}
  virtual inline bool is_terminal() const{return false;}
  virtual Node* clone() const;
  FieldNode(const FieldType field_key);
  FieldNode(const FieldNode& node);

  FieldType field_key_;
};

class TerminalNode : public Node{
public:
  bool operator==(const TerminalNode& rhs){return action_ == rhs.action_;}
  bool operator!=(const TerminalNode& rhs){return !(action_ == rhs.action_);}
  ActionTypes action_;
  virtual inline bool is_field() const {return false;}
  virtual inline bool is_terminal() const {return true;}
  virtual Node* clone() const;
  TerminalNode(const ActionTypes action);
  TerminalNode(const TerminalNode& node);

};

class Edge{
public:
  bool operator==(const Edge& rhs);
  bool operator!=(const Edge& rhs){return !(label_ == rhs.label_);}
  void set_target(Node* node);
  Edge* clone() const;
  uint64_t hash();
  bool intersects(const Range& range);
  bool covers(const Range& range);
  bool covered_by(const Range& range);
  bool equals(const Range& range);
  std::vector<Range> intersection(const Range& range);
  std::vector<Range> edge_diff_field(const Range& range);
  std::vector<Range> field_diff_edge(const Range& range);
  void set_label(const std::vector<Range>& new_label);
  Edge(const Range& label);
  Edge(const Range& label, const uint64_t id);
  Edge(const std::vector<Range>& label);
  Edge(const std::vector<Range>& label, const uint64_t id);
  Edge(const Range& label, Node* origin_node, Node* target_node);
  Edge(const std::vector<Range>& label, Node* origin_node, Node* target_node);
  Edge(const Edge& edge);

  friend std::ostream& operator<<(std::ostream& out, Edge& edge);

  uint64_t id_;
  std::vector<Range> label_;
  Node* origin_node_;
  Node* target_node_;


};


class Path{
public:

//  bool operator==(const Path& rhs);

  std::mutex m;

  std::vector<UPF_Rule*> build_rules() const;
  Node* get_root() const;
  Node* release_root();
  Node* get_last() const;
  void merge_path(Path& rootpath, Edge* merge_edge);
  void set_last(Node* new_last);
  void set_root(Node* new_root);
  uint64_t length() const;
  void append(Edge* edge, Node* node);
  void build_rule_thread(std::vector<UPF_Rule*>& rulevec,
                             const UPF_Rule& rule, FieldType field_key,
                             std::vector<Range>& range);
  Path();
  Path(const Node& root);
  Path(const Path& path);
  Path(const UPF_Rule& rule);
  Path(const UPF_Rule& rule, const FieldType field_key);
  ~Path();
  friend std::ostream& operator<< (std::ostream& out, Path &path);

private:
  Node* root_;
  Node* last_;
};


class RuleVector{
public:
  void append_rules(std::vector<UPF_Rule*>& rulevec) {
    m.lock();
    rules.insert(rules.end(), rulevec.begin(), rulevec.end());
    m.unlock();}
  std::recursive_mutex m;
  std::vector<UPF_Rule*> rules;


};



class Tree{

public:
  std::vector<std::unique_ptr<std::thread>> threads;
//  std::recursive_mutex m;
  void add_thread();
  void reset_threads();
  uint16_t get_current_threads();
  uint64_t get_root_id() const;
  Node* get_node_by_id(const uint64_t& node_id) const;
  uint64_t search_node_by_name(const std::string& node_name) const;
  Edge* get_edge_by_id(const uint64_t& edge_id) const;
  uint64_t add_node_at(Node* node, const uint64_t& edge_id);
  uint64_t add_edge_at(Edge* edge, const uint64_t& node_id);
  void remove_node(const uint64_t node_id);
  void remove_edge(const uint64_t edge_id);
  void register_node(Node* node);
  void register_edge(Edge* edge);
  void register_tree(Node* node);
  void register_all();
  void queue_node(Node* node);
  void queue_path(std::pair<Node*,Edge*>);
  inline uint64_t get_node_counter() const {return node_counter_;}
  inline uint64_t get_edge_counter() const {return edge_counter_;}
  inline uint64_t num_nodes() const {return nodes_.size();}
  inline uint64_t num_edges() const {return edges_.size();}
  UPF_Ruleset* build_rules(const UPF_Rule& rule, Node* node,
                                std::vector<Range>& diffs);
  inline Node* root() const {return nodes_.at(1).get();}
  void set_root(Node* node);
  Node* replicate_at(uint64_t node_id) const;
  void append_node_at(Node* node, const uint64_t node_id);
  UPF_Ruleset* get_effset_for_rule(const UPF_Rule& rule);
  void add_rule(const UPF_Rule& rule);
 void build_rule_threaded(const UPF_Rule& rule,
                                        Node* node, const Range& range,
                                        UPF_Ruleset* eff_set);
  void traverse_down(Node* node, const UPF_Rule& rule,RuleVector& rulevec,
              const Range& field_range, Edge* current_edge) ;
  void init_with_path(Path& path);
  void append_path(Path& path, const uint64_t edge_id);
  void append_path_to_root(Path& path, Node* node);
  void append_rule(RuleVector& rulevec, Node* node, const UPF_Rule& rule);
  void merge_nodes(uint64_t id_a, uint64_t id_b);
  void merge_edges(uint64_t edge_a, uint64_t edge_b);
  uint64_t count_edges();
  Tree(){}
  Tree(const UPF_Rule& rule);
  Tree(Node* root_node);
 // ~Tree();
  friend std::ostream& operator<< (std::ostream& out, Tree& tree);

//private:
  uint16_t current_threads;
  uint16_t max_threads;
  uint64_t edge_counter_;
  uint64_t node_counter_;
  std::unordered_map<uint64_t,std::unique_ptr<Edge>> edges_;
  std::unordered_map<uint64_t,std::unique_ptr<Node>> nodes_;
  std::vector<Node*> node_register_queue;
  std::vector<std::pair<Node*,Edge*>> path_register_queue;

};

struct path_hash{
public:

  uint64_t operator()(const Path& path) const{

  return std::hash<uint64_t>()(path.get_root()->hash());
  }
};

class Tree_PT : public Tree{
public:
  void add_path_to_table(const Path& path);
  void add_all_subpaths(Path& path);
  uint64_t get_counter(const Path& path);
  uint64_t num_paths();
  void print_paths();
  void export_stats();
  void register_paths(Node* node);
  void register_paths(Node* node, Path& path);
  Tree_PT(){};

private:
  std::unordered_map<
    Path, std::pair<Node*,uint64_t>, path_hash> path_map_;
//  std::unordered_map<Path*, uint64_t>

};



bool operator==(const Path& lhs, const Path& rhs);

#endif // GRAPH_H
