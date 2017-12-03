#include <future>
#include "graph.h"


static void subtract(std::vector<Range>& ranges_union,
                std::vector<Range>& intervals){

    Range def = Range(uint128_t::min(), uint128_t::max());
    std::vector<Range> working_set;
    working_set.push_back(def);

    for(auto iter = ranges_union.begin(); iter != ranges_union.end(); ++iter){
        Range& curr = (*iter);

        std::vector<Range> sec_set;
        for (auto iterw = working_set.begin(); iterw != working_set.end();++iterw){

            std::vector<Range> result;
            result = iterw->diff(curr);
            if(!result.empty())
                sec_set.insert(sec_set.end(), result.begin(), result.end());

        }
        working_set = sec_set;
    }
    intervals = working_set;
}

static std::string label_to_string(std::vector<Range>& label){

  std::stringstream ss;

  for(auto& range : label)
    ss << range;

  return ss.str();

}

Node::Node(const std::string& name)
  : id_(0),
    name_(name),
    incoming_edge_(nullptr)
    {}


Node::Node(const std::string& name, uint64_t node_id)
  : id_(node_id),
    name_(name),
    incoming_edge_(nullptr){}

void Node::add_edge(Edge* edge_ptr){
  edges_.push_back(edge_ptr);
}

Node::Node(const Node& node)
  : id_(node.id_),
    name_(node.name_),
    incoming_edge_(nullptr){}

Node::~Node(){}
Node* Node::clone() const {return new Node(*this);}
Node* FieldNode::clone() const {return new FieldNode(*this);}
Node* TerminalNode::clone() const {return new TerminalNode(*this);}

uint64_t Node::hash(){

  if (edges_.empty())
    return std::hash<std::string>()(name_);
  else
    return  std::hash<std::string>()(name_) ^
            std::hash<uint64_t>()(edges_.back()->hash()) ^
            std::hash<uint64_t>()(edges_.back()->target_node_->hash());

}

FieldNode::FieldNode(const FieldNode& node)
  : Node(node),
    field_key_(node.field_key_)
    {}

FieldNode::FieldNode(const FieldType field_key)
  : Node(fieldtype_to_string(field_key)),
    field_key_(field_key){}

TerminalNode::TerminalNode(const ActionTypes action)
  : Node(action_to_string(action)),
    action_(action){}

TerminalNode::TerminalNode(const TerminalNode& node)
  : Node(node),
    action_(node.action_)
    {}

uint64_t Node::size() const{

  unsigned int count = 1;
  for (auto iter = edges_.begin(); iter != edges_.end(); ++iter)
    count += (*iter)->target_node_->size();

  return count;
}

Node* Node::replicate() const{

  Node* copy = this->clone();
  for (auto iter = edges_.begin(); iter != edges_.end(); ++iter){
    Edge* curr_edge = *iter;
    Edge* new_edge =
      new Edge(curr_edge->label_, copy, curr_edge->target_node_->replicate());
    copy->add_edge(new_edge);
  }
  return copy;
}

bool Node::is_isomorphic(Node* rhs){

  if(this->id_ == rhs->id_)
    return false;
  else if (this->is_terminal() && rhs->is_terminal()){
    if(static_cast<TerminalNode*>(this)->action_ ==
       static_cast<TerminalNode*>(rhs)->action_)
       {
//          std::cout << "ISOMORPHIC: " << *this << "       |       " << *rhs << std::endl;
              return true;
       }
  }
  else if (this->is_terminal() || rhs->is_terminal())
    return false;
  else if (this->edges_.size() == rhs->edges_.size()){
    bool edges_equal = true;
    for (auto& a_edge : this->edges_){
      bool edge_in_b = false;
      for (auto& b_edge : rhs->edges_){
        if(*a_edge == *b_edge){
          edge_in_b = true;
        }
      }
      if(!edge_in_b){
        edges_equal = false;
        break;
      }
    }
//    if(edges_equal)
//      std::cout << "ISOMORPHIC: " << *this << "       |       " << *rhs << std::endl;
    return edges_equal;
  }
  return false;
}



void Node::delete_edge(const uint64_t edge_id){

  for(auto iter = edges_.begin(); iter != edges_.end();){
    if((*iter)->id_ == edge_id){
        iter = edges_.erase(iter);
//        break;
    }
    else
      ++iter;
  }
}


Edge* Edge::clone() const {return new Edge(*this);}

Edge::Edge(const Range& range)
  : id_(0),
    target_node_(nullptr){
      label_.push_back(range);
    }

Edge::Edge(const Range& range, const uint64_t edge_id)
  : id_(edge_id),
    origin_node_(nullptr),
    target_node_(nullptr){
      label_.push_back(range);
    }

Edge::Edge(const Range& range, Node* origin_node, Node* target_node)
  : id_(0),
    origin_node_(origin_node),
    target_node_(target_node){

      label_.push_back(range);
      target_node->incoming_edge_ = this;
    }

Edge::Edge(const std::vector<Range>& label)
  : id_(0),
    label_(label),
    target_node_(nullptr){}

Edge::Edge(const std::vector<Range>& label, const uint64_t edge_id)
  : id_(edge_id),
    label_(label),
    target_node_(nullptr){}

Edge::Edge(const std::vector<Range>& label, Node* origin_node, Node* target_node)
  : id_(0),
    label_(label),
    origin_node_(origin_node),
    target_node_(target_node){

      target_node->incoming_edge_ = this;
    }


Edge::Edge(const Edge& edge)
  : id_(edge.id_),
    label_(edge.label_),
    origin_node_(nullptr),
    target_node_(nullptr){}

uint64_t Edge::hash(){

  uint64_t hash_num = 0;

  for(auto range : label_)
    hash_num ^= std::hash<uint64_t>()(range.hash());

  return hash_num;

}

void Edge::set_target(Node* node){
  target_node_ = node;
  node->incoming_edge_ = this;
}

bool Edge::covers(const Range& field_range){
  bool covers = false;
  for(auto& range : label_){
    if(range.covers(field_range)){
      covers = true;
      break;
    }
  }
  return covers;
}

bool Edge::covered_by(const Range& field_range){
  bool covers = false;
  for(auto& range : label_){
    if(field_range.covers(range)){
      covers = true;
      break;
    }
  }
  return covers;
}

bool Edge::operator==(const Edge& rhs){

  // Sort both labels for easier comparison
  if(label_.size() == rhs.label_.size())
    if(std::is_permutation(this->label_.begin(), this->label_.end(),
                           rhs.label_.begin()))
    return (*target_node_ == *rhs.target_node_);

  return false;
}


bool Edge::equals(const Range& range){

  return (label_.size() == 1 && label_.back() == range);
}

bool Edge::intersects(const Range& field_range){
  bool intersects = false;

  for (auto& range : label_){
    if(range.intersect(field_range)){
      intersects = true;
      break;
    }
  }
  return intersects;

}

void Edge::set_label(const std::vector<Range>& new_label){
  label_ = new_label;
}

std::vector<Range> Edge::field_diff_edge(const Range& field_range){
  std::vector<Range> diffs;
  for (auto& range : label_){
    const std::vector<Range>& r_diff = field_range.diff(range);
    diffs.insert(diffs.end(), r_diff.begin(),r_diff.end());
  }
  return diffs;
}

std::vector<Range> Edge::edge_diff_field(const Range& field_range){
  std::vector<Range> diffs;
  for (auto& range : label_){
    const std::vector<Range>& f_diff = range.diff(field_range);
    diffs.insert(diffs.end(), f_diff.begin(),f_diff.end());
  }
  return diffs;
}

std::vector<Range> Edge::intersection(const Range& field_range){

  std::vector<Range> intersection;

  for (auto& range : label_){
    if(range.intersect(field_range)){
        intersection.push_back(range.intersection(field_range));
    }
  }
  return intersection;
}


Path::Path()
  : root_(nullptr),
    last_(root_){}

Path::Path(const Node& node)
  : root_(node.clone()),
    last_(root_){}

Path::Path(const Path& copy){
  root_ = copy.get_root()->clone();
  last_ = root_;
  Node* current_node = copy.get_root();
  while(!current_node->edges_.empty()){
    Edge* current_edge = current_node->edges_.back();
    Node* next_node = current_edge->target_node_->clone();
    append(current_edge->clone(),next_node);
    last_ = next_node;
    current_node = current_edge->target_node_;
  }
}


//bool Path::operator==(const Path& rhs){
//
//    Node* lhs_node = root_;
//    Node* rhs_node = rhs.get_root();
//
//    if(root_ != rhs.get_root())
//      return false;
//    else{
//      while(!lhs_node->edges_.empty() && !rhs_node->edges_.empty()){
//        Edge* lhs_edge = lhs_node->edges_.back();
//        Edge* rhs_edge = rhs_node->edges_.back();
//        if(lhs_edge != rhs_edge)
//          return false;
//        else{
//          Node* lhs_node = lhs_edge->target_node_;
//          Node* rhs_node = rhs_edge->target_node_;
//        }
//      }
//    }
//
//    return true;
//}

Path::Path(const UPF_Rule& rule) : Path(rule, l2_src){}

Path::Path(const UPF_Rule& rule, const FieldType start_field)
  : root_(new FieldNode(start_field)),
    last_(root_){

  /* Get Range for first Edge */
  Range first_range;
  if(rule.get_field(start_field) != nullptr)
    first_range = rule.get_field(start_field)->get_range();

  /* Get all Ranges of Rule Fields */
  std::vector<Range> all_ranges = rule.get_ranges_next_from(start_field);
  all_ranges.insert(all_ranges.begin(), first_range);

  FieldType field_key = start_field;
  FieldNode* current_node = static_cast<FieldNode*>(root_);

  /* Build Path */
  for(auto r_iter = all_ranges.begin(); r_iter != all_ranges.end() -1;
    ++r_iter){

    /* Build next Node with next Field as name */
    field_key = next_field_key(field_key);
    FieldNode* next_node = new FieldNode(field_key);

    /* Create Edge with current Range and connect to next Node */
    Edge* new_edge_ptr = new Edge(*r_iter, current_node, next_node);
    current_node->add_edge(new_edge_ptr);
    /* Set Current Node to new created Node */
    current_node = next_node;
    last_ = current_node;
    }

  /* Append Terminal Node */
  ActionTypes type = rule.get_action().get_type();
  Node* terminal_node = new TerminalNode(type);
  Edge* new_edge_ptr = new Edge(all_ranges.back(), current_node, terminal_node);
  current_node->add_edge(new_edge_ptr);
  last_ = terminal_node;
  }

void Path::append(Edge* edge, Node* node){
  edge->origin_node_ = get_last();
  edge->target_node_ = node;
  get_last()->add_edge(edge);
  set_last(node);
}

Node* Path::get_root() const{return root_;}

void Path::set_root(Node* new_root){ root_ = new_root;}

Node* Path::get_last() const { return last_;}

void Path::set_last(Node* new_last){ last_ = new_last;}

uint64_t Path::length() const{

  uint64_t length = 1;

  Node* current_node = get_root();
  while(!current_node->edges_.empty()){
    ++length;
    Edge* current_edge = current_node->edges_.back();
    current_node = current_edge->target_node_;
  }
  return length;
}

void Path::merge_path (Path& rootpath, Edge* merge_edge){
    Path cloned_path(rootpath);
    Edge* new_edge = new Edge(merge_edge->label_,cloned_path.get_last(),get_root());
    cloned_path.get_last()->add_edge(new_edge);
    set_root(cloned_path.release_root());
}


std::vector<UPF_Rule*> Path::build_rules() const{

  std::vector<UPF_Rule*> ruleset;
  std::vector<UPF_Rule*> rulevec;
  Node* current_node = root_;
  Edge* current_edge = current_node->edges_.back();
  FieldType field_key = static_cast<FieldNode*>(current_node)->field_key_;

//  std::cout << *this->get_root() << std::endl;

  if(current_edge != nullptr){
    for(Range& range : current_edge->label_){
      UPF_Rule* new_rule = new UPF_Rule();
      new_rule->set_field(field_key, range);
      ruleset.push_back(new_rule);
    }
  }

  current_node = current_edge->target_node_;
  current_edge = current_node->edges_.back();



  while(current_edge != nullptr){
    rulevec.clear();
    if (current_node->is_field()){
      field_key = static_cast<FieldNode*>(current_node)->field_key_;
      for(unsigned int j = 0; j < ruleset.size(); ++j){
          const UPF_Rule& rule = *ruleset[j];
        for(unsigned int i = 1; i < current_edge->label_.size(); ++i){
            UPF_Rule* new_rule = rule.clone();
            new_rule->set_field(field_key,current_edge->label_[i]);
            rulevec.push_back(new_rule);
          }
        ruleset[j]->set_field(field_key, current_edge->label_[0]);
      }
    ruleset.insert(ruleset.end(), rulevec.begin(), rulevec.end());
    current_node = current_edge->target_node_;
    if(!current_node->edges_.empty())
      current_edge = current_node->edges_.back();
    else
      current_edge = nullptr;
    }
  }
  if (current_node->is_terminal())
      for(unsigned int j = 0; j < ruleset.size(); ++j)
        ruleset[j]->set_action(static_cast<TerminalNode*>(current_node)->action_);

  return ruleset;
}

Node* Path::release_root(){
  Node* released = root_;
  root_ = nullptr;
  return released;
}

Path::~Path(){

  if(root_ != nullptr){
    Node* current_node = root_;
    if(!current_node->edges_.empty()){
      Edge* current_edge = current_node->edges_.back();
      while(current_edge != nullptr){
        if(!current_node->edges_.empty()){
          current_edge = current_node->edges_.back();
          delete(current_node);
          current_node = current_edge->target_node_;
          delete(current_edge);
        }
        else{
          delete(current_node);
          current_edge = nullptr;
        }
      }
    }
  }
}

Tree::Tree(Node* root_node)
  : current_threads(1),
    max_threads(std::thread::hardware_concurrency()),
    edge_counter_(0),
    node_counter_(1){
  nodes_.emplace(1,std::move(std::unique_ptr<Node>(root_node)));
}

Tree::Tree(const UPF_Rule& rule)
  : current_threads(1),
    max_threads(std::thread::hardware_concurrency()),
    edge_counter_(0),

    node_counter_(0){

    Path p(rule, l2_src);
    register_tree(p.release_root());
  }


uint64_t Tree::add_edge_at(Edge* edge, const uint64_t& node_id){
  edge->id_ = ++edge_counter_;
  nodes_.at(node_id)->add_edge(edge);
  edges_.emplace(edge_counter_,std::move(std::unique_ptr<Edge>(edge)));

  return edge->id_;
}

uint64_t Tree::add_node_at(Node* node, const uint64_t& edge_id){

  node->id_ = ++node_counter_;
  edges_.at(edge_id)->set_target(node);
  nodes_.emplace(node_counter_,std::move(std::unique_ptr<Node>(node)));

  return node->id_;
}

void Tree::remove_edge(const uint64_t edge_id){
  Edge* edge = edges_.at(edge_id).get();
  edge->origin_node_->delete_edge(edge_id);
//  if(edge->target_node_ != nullptr)
//    remove_node(edges_.at(edge_id)->target_node_->id_);
  edges_.erase(edge_id);
}

void Tree::remove_node(const uint64_t node_id){
  Node* node = nodes_.at(node_id).get();

  for(auto edge : node->edges_)
    remove_edge(edge->id_);
  nodes_.erase(node_id);

}

void Tree::register_node(Node* node){

  node->id_ = ++node_counter_;
  nodes_.emplace(node_counter_,std::move(std::unique_ptr<Node>(node)));
}

void Tree::register_edge(Edge* edge){

  edge->id_ = ++edge_counter_;
  edges_.emplace(node_counter_,std::move(std::unique_ptr<Edge>(edge)));
}

void Tree::merge_nodes(uint64_t id_a, uint64_t id_b){

  Node* a = get_node_by_id(id_a);
  Node* b = get_node_by_id(id_b);

  Node* orig = b->incoming_edge_->origin_node_;
  b->incoming_edge_->set_target(a);

  std::vector<uint64_t> edges_to_erase;
  for(auto& edge : b->edges_){
    edges_to_erase.push_back(edge->id_);
  }

    for(auto iter = orig->edges_.begin(); iter != orig->edges_.end(); ++iter){
      for(auto titer = iter + 1; titer != orig->edges_.end(); ++ titer){
        if( (*iter)->target_node_ == (*titer)->target_node_){
          merge_edges((*iter)->id_,(*titer)->id_);
          edges_to_erase.push_back((*titer)->id_);
        }
      }
    }

  for (auto& id : edges_to_erase)
    remove_edge(id);

}

uint64_t Tree::count_edges(){

  Node* current_node = get_node_by_id(1);
  std::queue<Node*> nodes_to_process;
  nodes_to_process.push(get_node_by_id(1));
  std::set<uint64_t> id_set;
  while(!nodes_to_process.empty()){

    current_node = nodes_to_process.front();
    for(auto& edge : current_node->edges_){
      id_set.insert(edge->id_);
      nodes_to_process.push(edge->target_node_);
    }

    nodes_to_process.pop();
  }

  return id_set.size();
}

void Tree::merge_edges(uint64_t id_a, uint64_t id_b){

  Edge* a = get_edge_by_id(id_a);
  Edge* b = get_edge_by_id(id_b);

  std::vector<Range> new_label;

  for(auto& b_range : b->label_){
    if(std::find(a->label_.begin(), a->label_.end(), b_range) == a->label_.end())
      a->label_.push_back(b_range);
  }

  std::sort(a->label_.begin(), a->label_.end(),
            [](const Range& lhs, const Range& rhs) -> bool{
              return lhs.lower_ < rhs.lower_;});

    bool has_changed = true;
    while(has_changed){
      has_changed = false;
      for (auto iter = a->label_.begin(); iter != a->label_.end();++iter){
          for(auto titer = iter + 1; titer != a->label_.end();){
            if(iter->border(*titer)){
              iter->merge(*titer);
              titer = a->label_.erase(titer);
              has_changed = true;
              }
              else
                ++titer;
              }

      }
    }
//  remove_edge(id_b);
}

void Tree::queue_node(Node* node){

  node_register_queue.push_back(node);
}

void Tree::register_all(){
  for(auto& path : path_register_queue){
//
//    std::cout << "Register Path: " << cnt++ << std::endl;

    path.second->origin_node_ = path.first;
    path.first->add_edge(path.second);
     Node* temp = path.second->target_node_;
    register_edge(path.second);
    register_tree(temp);
}

  path_register_queue.clear();
}

Edge* Tree::get_edge_by_id(const uint64_t& edge_id) const{
  return edges_.at(edge_id).get();
}

Node* Tree::get_node_by_id(const uint64_t& node_id) const{
  return nodes_.at(node_id).get();
}

uint64_t Tree::search_node_by_name(const std::string& name) const{

  /* Linear Search :( */
  uint64_t key = 0;
  for (auto iter = nodes_.begin(); iter != nodes_.end(); ++iter){
    if(iter->second->name_ == name)
     key = iter->first;
  }
  return key;
}

void Tree::queue_path(std::pair<Node*,Edge*> path){

  path_register_queue.push_back(path);

}

Node* Tree::replicate_at(uint64_t node_id) const{
  return get_node_by_id(node_id)->replicate();
}

void Tree::register_tree(Node* node){

  register_node(node);

  for (auto iter = node->edges_.begin(); iter != node->edges_.end(); ++iter){
    Edge* curr_edge = *iter;
    register_edge(curr_edge);
    register_tree(curr_edge->target_node_);
  }
}

void Tree::traverse_down(Node* node, const UPF_Rule& rule, RuleVector& rulevec,
              const Range& field_range, Edge* current_edge){

      if(current_edge->covered_by(field_range))
        append_rule(rulevec, current_edge->target_node_,rule);
      else if (current_edge->intersects(field_range)){
        std::vector<Range> range_diffs(current_edge->edge_diff_field(field_range));
        std::vector<Range> range_int(current_edge->intersection(field_range));
        Node* new_target_node = current_edge->target_node_->replicate();
        Edge* new_edge = new Edge(range_int, node, new_target_node);
        queue_path(std::make_pair(node,new_edge));
        current_edge->label_ = range_diffs;
        append_rule(rulevec, new_target_node,rule);
//        node->add_edge(new_edge);
      }
      current_threads--;

}

UPF_Ruleset* Tree::get_effset_for_rule(const UPF_Rule& rule){

  RuleVector global_vec;
  append_rule(global_vec, root(), rule);
  register_all();
  UPF_Ruleset* rset = new UPF_Ruleset();
  rset->append(global_vec.rules);
  reset_threads();
  return rset;
}

void Tree::add_rule(const UPF_Rule& rule){
  RuleVector global_vec;
  append_rule(global_vec, root(), rule);
  register_all();
}


void Tree::append_rule(RuleVector& global_vec, Node* node,
                       const UPF_Rule& rule){


  std::vector<std::unique_ptr<std::thread>> threads;
  RuleVector local_vec;
  FieldType field_key = static_cast<FieldNode*>(node)->field_key_;
  Field* field = rule.get_field(field_key);
  Range field_range;
  if(field != nullptr)
     field_range = rule.get_field(field_key)->get_range();

  std::vector<Range> edges_union;
  for(auto iter = node->edges_.begin();iter != node->edges_.end(); ++iter){
    std::vector<Range>& curr_label = (*iter)->label_;
    edges_union.insert(edges_union.end(),curr_label.begin(), curr_label.end());
  }
  std::vector<Edge*> temp_edges = node->edges_;
  std::vector<Range> field_diffs_edges;

  if (field != nullptr)
    field->subtract(edges_union,field_diffs_edges);
  else
    subtract(edges_union,field_diffs_edges);



  // Add new Paths and build effective rules

  if(!field_diffs_edges.empty()){
    //std::cout << fieldtype_to_string(field_key) << std::endl;
    //std::cout << "Field diff Edges: " <<  std::endl;
    //for (auto& range : field_diffs_edges)
    //  std::cout << range << std::endl;
    std::unique_ptr<Path> path(new Path(rule,next_field_key(field_key)));
    Edge* new_edge = new Edge(field_diffs_edges, node, path->release_root());
//    node->add_edge(new_edge);
    queue_path(std::make_pair(node,new_edge));
    Path rulepath(rule, next_field_key(field_key));
    Node* clone_node = node->clone();
    Edge* clone_edge = new Edge(new_edge->label_, clone_node, rulepath.get_root());
    rulepath.get_root()->incoming_edge_ = clone_edge;
    clone_node->add_edge(clone_edge);
    rulepath.set_root(clone_node);
    append_path_to_root(rulepath, node);
    local_vec.rules = rulepath.build_rules();
  }

  // Traverse Down
  if(field_key != tcpflagfield){
    for(auto iter = temp_edges.begin();iter != temp_edges.end(); ++iter){
      Edge* current_edge = *iter;
      if(current_edge->intersects(field_range)){
//        if(temp_edges.size() > 1 && current_threads < max_threads){
//          add_thread();
//          threads.push_back(std::unique_ptr<std::thread>(
//            new std::thread(&Tree::traverse_down, this, node, rule,
//                            std::ref(local_vec), field_range, current_edge)));
//        }
//        else
          //std::cout << "Intersection : " << current_edge->label_[0] << "- " <<  field_range << std::endl;
          traverse_down(node,rule,local_vec,field_range,current_edge);
      }
    }
  }
  // Wait for Threads to deliver rules
  for (auto& thread : threads)
    thread->join();
  global_vec.append_rules(local_vec.rules);

//  std::cout << "RULE ADD DONE" << std::endl;
}

void Tree::append_path_to_root(Path& path, Node* start_node){

  Node* current_node = start_node;

 // path.set_root(current_node->clone());

  if(current_node->incoming_edge_ != nullptr){

    Edge* upper_edge = current_node->incoming_edge_->clone();
    Node* upper_node = current_node->incoming_edge_->origin_node_->clone();
    upper_edge->origin_node_ = upper_node;
    upper_edge->target_node_ = path.get_root();
    upper_node->add_edge(upper_edge);
    path.get_root()->incoming_edge_ = upper_edge;
    path.set_root(upper_node);
    append_path_to_root(path,current_node->incoming_edge_->origin_node_);
  }
}

void Tree::add_thread(){
  ++current_threads;
}

void Tree::reset_threads(){
  current_threads = 1;
}

uint16_t Tree::get_current_threads(){
  return current_threads;
}


void Tree::append_path(Path& path, const uint64_t edge_id){
  append_node_at(path.release_root(), edge_id);
}

void Tree::set_root(Node* node){
  nodes_.emplace(node_counter_,std::unique_ptr<Node>(node));
}

void Tree::append_node_at(Node* node, const uint64_t edge_id){

  get_edge_by_id(edge_id)->set_target(node);
  register_tree(node);

}

// Tree with Path Table Implementation

void Tree_PT::add_path_to_table(const Path& path){

  auto iter = path_map_.find(path);
  if (iter == path_map_.end()){
      Path key_path(path);
      Path value_path(path);
      Node* value_node = value_path.get_root();
//    std::make_pair(new_path, std::make_pair(std::unique_ptr<Node>(new_path.release_root(),1)));
    path_map_.emplace(key_path,std::make_pair(value_path.release_root(),1));
    register_tree(value_node);
  }
  else{
     iter->second.second++;
//     std::cout << "Path already in Table - Count: " << iter->second.second << std::endl;
  }
}

uint64_t Tree_PT::get_counter(const Path& path){

  auto iter = path_map_.find(path);

  if (iter == path_map_.end())
    return 0;
  else
    return iter->second.second;

}

uint64_t Tree_PT::num_paths(){
  return path_map_.size();
}

void Tree_PT::add_all_subpaths(Path& path){

  while(!path.get_root()->edges_.empty()){
    Edge* current_edge = path.get_root()->edges_.back();
    path.set_root(current_edge->target_node_);
//    std::cout << "Subpath added" << num_paths() - 1 << std::endl;
//    std::cout << path << std::endl;
    add_path_to_table(path);
  }

}

void Tree_PT::print_paths(){

  for(auto path : path_map_){

    std::cout << *path.first.get_root() << std::endl;
    std::cout << "Count: " << path.second.second << std::endl;


  }

}

void Tree_PT::register_paths(Node* orig_node, Path& path){

//  Node* current_node = orig_node->clone();

  if(orig_node->is_terminal()){
//    std::cout << "Path added" << num_paths() - 1 << std::endl;
////    std::cout << path << std::endl;
////    Path& derp = path;
//    Node& root = *path.get_root();
    add_path_to_table(path);
    add_all_subpaths(path);
  }
//  else{
//    for (auto& edge : orig_node->edges_){
////      Path edge_path;
////      edge_path.set_root(current_node ->clone());
////      edge_path.set_last(edge_path.get_root());
////      Edge* new_edge = edge->clone();
//      Node* next_node = edge->target_node_->clone();
//      path.append(edge->clone(),edge->target_node_->clone());
//////      edge_path.append(new_edge,next_node);
////      register_paths(edge->target_node_, edge_path);
//      register_paths(edge->target_node_, path);
//    }
    if(!orig_node->edges_.empty()){
//      if(orig_node->edges_ > 1){
        for (auto& edge : orig_node->edges_){
          Path edge_path(path);
//          Edge* edge = orig_node->edges_.back();
          edge_path.append(edge->clone(),edge->target_node_->clone());
          register_paths(edge->target_node_, edge_path);
        }
    }

//              std::cout << "Path added" << std::endl;
//          std::cout << edge_path << std::endl;
//          add_path_to_table(edge_path);

//    add_path_to_table(path);
}


void Tree_PT::register_paths(Node* orig_node){

    Path root_path;
//    uint64_t edg_cnt = orig_node->edges_.size();

    root_path.set_root(orig_node->clone());

    for (auto& edge : orig_node->edges_){
      Path root_path;
      Node* current_node = orig_node->clone();
      root_path.set_root(current_node);
      root_path.set_last(current_node);
      Edge* new_edge = edge->clone();
      Node* next_node = edge->target_node_->clone();
      root_path.append(new_edge,next_node);
      register_paths(edge->target_node_, root_path);
      }
//
//    std::cout << "Path added" << std::endl;
//    std::cout << root_path << std::endl;
//    add_path_to_table(root_path);
    std::cout << num_paths() << std::endl;


}

void Tree_PT::export_stats(){

    std::ofstream write;
     write.open("path_stats");
    uint64_t cnt = 1;
    write << "#Path     Length    Count" << std::endl;
    for(auto path: path_map_){
      write << "P_" << cnt << "     " << path.first.length() << "     "
      << path.second.second << std::endl;
      ++cnt;
    }

//    write << stream.str();
    write.close();

}


bool operator==(const Path& lhs, const Path& rhs){
    Node* lhs_node = lhs.get_root();
    Node* rhs_node = rhs.get_root();

    if(*lhs_node != *rhs_node)
      return false;
    else{
      while(!lhs_node->edges_.empty() && !rhs_node->edges_.empty()){
        Edge* lhs_edge = lhs_node->edges_.back();
        Edge* rhs_edge = rhs_node->edges_.back();
        if(*lhs_edge != *rhs_edge)
          return false;
        else{
         lhs_node = lhs_edge->target_node_;
         rhs_node = rhs_edge->target_node_;
         if (*lhs_node != *rhs_node)
          return false;
        }
      }
    }

    return true;
}




/* << Operator Overloading */

std::ostream& operator<<(std::ostream& out, Node& node){

  out << node.name_ << "(" << node.id_ << ")";

  if(node.is_terminal())
    out << " TERMINAL" << std::endl;

  for(auto iter = node.edges_.begin(); iter != node.edges_.end(); ++iter){
    out << **iter;

  }
  return out;

}
std::ostream& operator<<(std::ostream& out, Path& path){
  out << "Path output:" << std::endl << *path.root_ << std::endl;

  return out;
}

std::ostream& operator<<(std::ostream& out, Edge& edge){

  out << "- " << label_to_string(edge.label_) << " (" << edge.id_ << ")"  << "->";
//  out << "- " << "(" << edge.id_ << ")" << "->";
  if(edge.target_node_ != nullptr)
    out << *edge.target_node_;
  return out;
}

std::ostream& operator<<(std::ostream& out, Tree& tree){
  out << "Tree output:" << std::endl << "Number of Nodes: " << tree.num_nodes() <<
    std::endl << "Number of Edges: " << tree.num_edges() << std::endl;
    std::cout << *tree.get_node_by_id(1);

  out << std::endl;
  return out;
}
