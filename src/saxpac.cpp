#include "saxpac.h"
#include <random>

bool field_independent1(const UPF_Rule& lhs, const UPF_Rule& rhs,
                           DimensionType field_key){

    if(lhs.get_field(field_key) == nullptr ||
       rhs.get_field(field_key) == nullptr)
      return false;
    else
      return !lhs.get_field(field_key)->intersect(rhs.get_field(field_key));

}

uint64_t max_overlapping(const UPF_Ruleset& ruleset){

  uint64_t max_block_size = 0;
//  std::cout << "guggu";
  std::vector<DimensionType> fieldkeys = UPF_Rule::get_dimkeys();
  bool overlapping = true;
  uint64_t current_block_high = 0;

  for(auto iter = fieldkeys.begin(); iter != fieldkeys.end(); ++iter){
          const DimensionType field_key = *iter;
    for(uint64_t i = 0; i < ruleset.size(); ++i){
        if (ruleset.size() - i < max_block_size)
          break;
      current_block_high = i+1;
      for(uint64_t j = i+1; j < ruleset.size(); ++j){
        const UPF_Rule& rule1 = ruleset.get_rule(j);
        for(uint64_t k = i; k < j; ++k){
          const UPF_Rule& rule2 = ruleset.get_rule(k);
          if(!field_independent1(rule1,rule2, field_key)){
            if(current_block_high - i > max_block_size){
                max_block_size = current_block_high - i;
            }
            else
              // Skip to end of current block
              i = j;
            overlapping = false;
            break;
          }
        }
      if(overlapping){
//        std::cout << j << std::endl;
        ++current_block_high;
      }
      else{
        overlapping = true;
        break;
      }
      }
    }
  }
  return max_block_size;
}



uint128_t saxpac::find_best_cut(const UPF_Ruleset& ruleset,
                     const Bounding_Box& box,
                     const DimensionType dim,
                     const CuttingType cutting_type){

    switch(cutting_type){
      // Standard Equi-distant HiCuts
      case equi_dist:{
        std::vector<uint128_t> endpoints;
        collect_endpoints_in_box(ruleset,dim, box, endpoints);
        return equal_distant(endpoints, dim, box);
      }
      // Equal-Sized Segments
      case equi_seg:{
          std::vector<uint128_t> endpoints;
          collect_endpoints_in_box(ruleset, dim, box, endpoints);
          return equal_segments(endpoints, dim, box);
      }
      // Equal Rules in Segments
      case equi_rule:{
          std::vector<uint128_t> endpoints;
          collect_endpoints_in_box(ruleset, dim, box, endpoints);
          return equal_rules(ruleset,dim, box, endpoints);
      }
      // Weighted Segments
      case weighted_seg:{
          std::vector<uint128_t> endpoints;
          collect_endpoints_in_box(ruleset,dim, box, endpoints);
          return weighted_segments(ruleset, dim,box,  endpoints);
      }
      default: break;
    }
    return 0;
}

void saxpac::collect_ranges(const UPF_Ruleset& ruleset,
                              const DimensionType dim,
                              std::vector<Range>& ranges){

  for(unsigned int i = 0; i < ruleset.size(); ++i){
    if(ruleset.get_rule(i).get_field(dim) != nullptr){
      const Range& current_range = ruleset.get_rule(i).get_field(dim)->get_range();
      ranges.push_back(current_range);
    }
    else
      ranges.push_back(Range(uint128_t::min(), uint128_t::max()));
  }
  std::sort(ranges.begin(), ranges.end(),
            [](const Range& a, const Range& b) -> bool{
              return (a.lower_ < b.lower_);
              });
}

void saxpac::collect_endpoints(const UPF_Ruleset& ruleset,
                                 const DimensionType dim,
                                 std::vector<uint128_t>& endpoints){

  for(unsigned int i = 0; i < ruleset.size(); ++i){
    if(ruleset.get_rule(i).get_field(dim) != nullptr){
      const Range& current_range = ruleset.get_rule(i).get_field(dim)->get_range();
      endpoints.push_back(current_range.lower_);
      endpoints.push_back(current_range.upper_);
    }
    else{
      endpoints.push_back(0);
      endpoints.push_back(uint128_t::max());
    }
  }
    std::sort(endpoints.begin(), endpoints.end(),
            [](const uint128_t& a, const uint128_t& b) -> bool{
              return (a < b);
              });

    endpoints.erase(unique(endpoints.begin(), endpoints.end(),
      [](const uint128_t& a, const uint128_t& b){ return a == b;}),
      endpoints.end());

}

void saxpac::collect_endpoints_in_box(const UPF_Ruleset& ruleset,
                                        const DimensionType dim,
                                        const Bounding_Box& box,
                                        std::vector<uint128_t>& endpoints){
  for(unsigned int i = 0; i < ruleset.size(); ++i){
    if(ruleset.get_rule(i).get_field(dim) != nullptr){
      const Range& current_range = ruleset.get_rule(i).get_field(dim)->get_range();
    if(current_range.lower_ > box.get_lower_bound(dim) &&
       current_range.lower_ <= box.get_upper_bound(dim)){
      endpoints.push_back(current_range.lower_);
    }
    else if(current_range.lower_ >= box.get_lower_bound(dim) &&
       current_range.lower_ < box.get_upper_bound(dim)){
      endpoints.push_back(current_range.lower_);
    }
    if(current_range.upper_ > box.get_lower_bound(dim) &&
       current_range.upper_ <= box.get_upper_bound(dim)){
      endpoints.push_back(current_range.upper_);
    }
    else if(current_range.upper_ >= box.get_lower_bound(dim) &&
       current_range.upper_ < box.get_upper_bound(dim)){
      endpoints.push_back(current_range.upper_);
    }
  }
  }
  std::sort(endpoints.begin(), endpoints.end(),
            [](const uint128_t& a, const uint128_t& b){return (a < b);});

  endpoints.erase(unique(endpoints.begin(), endpoints.end(),
    [](const uint128_t& a, const uint128_t& b){ return a == b;}),
    endpoints.end());
}

// Equal Segments heuristic

uint128_t saxpac::equal_distant(const std::vector<uint128_t>&,
                                  const DimensionType dim,
                                  const Bounding_Box& box){

    uint128_t cut_position = box.get_upper_bound(dim);
    uint128_t diff = box.get_upper_bound(dim) - box.get_lower_bound(dim);
    diff.downshift(1);
    if(diff == uint128_t::min())
      diff = 1;
    std::cout << cut_position << std::endl;
    cut_position = box.get_upper_bound(dim)  - diff;
    std::cout << "new: " << cut_position << std::endl;
    return cut_position;

//  uint128_t cut_position(endpoints.back());
//
//  cut_position.downshift(1);
//
//  return cut_position;

}
uint128_t saxpac::equal_segments(const std::vector<uint128_t>& endpoints,
                                   const DimensionType dim ,
                                   const Bounding_Box& box){

  if(endpoints.size() == 0){
    uint128_t diff = box.get_upper_bound(dim) - box.get_lower_bound(dim);
    diff.downshift(1);
    if(diff == uint128_t::min())
      diff = 1;
    return box.get_upper_bound(dim) - diff;
  }
  if (endpoints.size() == 1)
    return endpoints[0];

  unsigned int index = floor((endpoints.size() / 2));
  return endpoints[index];
}

uint128_t saxpac::equal_rules(const UPF_Ruleset& ruleset,
                                const DimensionType dim,
                                const Bounding_Box& box,
                                const std::vector<uint128_t>& endpoints){

  uint128_t cut_position = 0;
  if(endpoints.size() == 0){
    uint128_t diff = box.get_upper_bound(dim) - box.get_lower_bound(dim);
    diff.downshift(1);
    if(diff == uint128_t::min())
      diff = 1;
    return box.get_upper_bound(dim) - diff;
  }
  if (endpoints.size() == 1)
    return endpoints[0];
  else if(endpoints.size() >= 2){
  Range current_range = Range(endpoints[endpoints.size()-1],endpoints[endpoints.size()-1]);
  unsigned int target_overlap = floor(ruleset.size() / 2) + 1;
  unsigned int current_overlap = 1;
  unsigned int i = endpoints.size()-1;
    while(current_overlap < target_overlap){
      --i;
      current_range.lower_ = endpoints[i];
      for (unsigned int j = 0; j < ruleset.size(); ++j){
        if(ruleset.get_rule(j).get_field(dim) != nullptr){
          if(ruleset.get_rule(j).get_field(dim)->get_range().intersect(current_range))
            ++current_overlap;
        }
        else
          ++current_overlap;
      }
      cut_position = endpoints[i];
    }
  }
  return cut_position;
}

//Weighted Segments heuristic
uint128_t saxpac::weighted_segments(const UPF_Ruleset& ruleset,
                                      const DimensionType dim,
                                      const Bounding_Box& box,
                                      const std::vector<uint128_t>& endpoints){

//  std::cout << "ENDPOINTS: " << std::endl;
//  for(auto& endpoint : endpoints)
//    std::cout << endpoint << std::endl;

//  std::cout << std::endl;
  // If no endpoints in box, use equal distant cut
  if(endpoints.size() == 0){
    uint128_t diff = box.get_upper_bound(dim) - box.get_lower_bound(dim);
    diff.downshift(1);
    if(diff == 0)
      diff = 1;
    return box.get_upper_bound(dim) - diff;
  }
  if(endpoints.size() == 1)
    return endpoints[0];
  std::vector<Segment> segs;
  for (unsigned int i = 0; i < endpoints.size() - 1; ++i){
    Segment current_segment(Range(endpoints[i], endpoints[i+1]));
    segs.push_back(current_segment);
  }
  // count overlapping rule
  unsigned int total_overlaps = 0;
  for (unsigned int i = 0; i < segs.size(); ++i){
    const Range& current_range = segs[i].range_;
    for (unsigned int j = 0; j < ruleset.size(); ++j){
      if(ruleset.get_rule(j).get_field(dim) != nullptr){
        if(ruleset.get_rule(j).get_field(dim)->get_range().intersect(current_range)){
          ++segs[i].overlaps_;
          ++total_overlaps;
        }
      }
      else{
        // Wildcard
        ++segs[i].overlaps_;
        ++total_overlaps;
      }
    }
  }
  unsigned int count_overlaps = 0;
  unsigned int target_overlaps = total_overlaps / 2;
  uint128_t cut_position;
  for (unsigned int i = 0; i < segs.size(); ++i){
    count_overlaps += segs[i].overlaps_;
    if(count_overlaps > target_overlaps){
      cut_position = segs[i].range_.lower_;
      break;
      }
  }
//  std::cout << "BEST CUT: " << cut_position << std::endl;

  return cut_position;
}

void saxpac::limit_rule_on_box(const UPF_Rule& rule,
                                 Bounding_Box& box){

  if(rule.has_unknown_match())
    box.set_unknown(true);

  for(unsigned int i = 0; i < box.num_dimensions(); ++i){
    const DimensionType curr_dim = static_cast<DimensionType>(i);
    if(rule.get_field(curr_dim) != nullptr){
      Range limit_range = rule.get_field(curr_dim)->get_range();
      if(limit_range.lower_ < box.get_lower_bound(curr_dim))
        limit_range.lower_ = box.get_lower_bound(curr_dim);
      if(limit_range.upper_ > box.get_upper_bound(curr_dim))
        limit_range.upper_ = box.get_upper_bound(curr_dim);
      box.set_range(curr_dim, limit_range);
    }
  }
}


static void build_dimvector(std::vector<DimensionType>& dims,
                            const UPF_Ruleset&){

    dims.push_back(ipv4src);
    dims.push_back(ipv4dst);
    dims.push_back(udpsrcport);
    dims.push_back(udpdstport);
    dims.push_back(tcpsrcport);
    dims.push_back(tcpdstport);
//    dims.push_back(l4protocol);
    dims.push_back(macsrc);
    dims.push_back(macdst);
}


void saxpac::get_most_distinct_rules(Node* current_node,
                                       std::vector<Distinct>& distincts){

  const UPF_Ruleset& current_ruleset = current_node->get_ruleset();
  std::vector<DimensionType> dims;
  build_dimvector(dims, current_node->get_ruleset());
  for (unsigned int i = 0; i < dims.size(); ++i){
    DimensionType current_dim = dims[i];

    // Count number of distinct rules
    unsigned int distinct_rules = 0;
    std::vector<Range> ranges;
    collect_ranges(current_ruleset,current_dim, ranges);
    Range& current_range = ranges[0];
    uint128_t right_most_endpoint = current_range.upper_;
      for (unsigned int j = 1; j < ranges.size(); ++j){
      const Range& next_range = ranges[j];
      if(right_most_endpoint < next_range.lower_){
        ++distinct_rules;
        right_most_endpoint = next_range.upper_;
      }
      current_range = next_range;
    }
    distincts.push_back(Distinct(current_dim,distinct_rules));
  }
}

void saxpac::get_most_distinct_endpoints(Node* current_node,
                                        std::vector<Distinct>& distincts){

//  std::cout << "most distinct endpoints" << std::endl;
  const UPF_Ruleset& current_ruleset = current_node->get_ruleset();
  std::vector<DimensionType> dims;
  build_dimvector(dims, current_node->get_ruleset());
  for (unsigned int i = 0; i < dims.size(); ++i){
    DimensionType current_dim = dims[i];

    // Count number of distinct endpoints
    std::vector<uint128_t> endpoints;
    collect_endpoints_in_box(current_ruleset,current_dim,
                             current_node->get_box(), endpoints);
    distincts.push_back(Distinct(current_dim, endpoints.size()));
  }
}

bool has_endpoints(saxpac::Node* current_node, const DimensionType dim){

  std::vector<uint128_t> endpoints;

  collect_endpoints_in_box(current_node->get_ruleset(), dim,
                           current_node->get_box(), endpoints);

  return endpoints.size() > 0;
}

saxpac::DimCut saxpac::determine_best_dimcut(Node* current_node,
                                                 const CuttingType cutting_type){

  using namespace saxpac;
  std::vector<Distinct> distincts;
  const UPF_Ruleset& current_ruleset = current_node->get_ruleset();
  const Bounding_Box& current_box = current_node->get_box();
  get_most_distinct_rules(current_node,distincts);

  std::sort(distincts.begin(), distincts.end(),
            [](const Distinct& a, const Distinct& b) -> bool{
              return (a.distinct_number_ > b.distinct_number_);
              });

  // if all zero, then take most distinct endpoints

  if(distincts[0].distinct_number_ == 0){
    distincts.clear();
    get_most_distinct_endpoints(current_node,distincts);
    std::sort(distincts.begin(), distincts.end(),
        [](const Distinct& a, const Distinct& b) -> bool{
          return (a.distinct_number_ > b.distinct_number_);
          });
  }
  std::vector<Distinct> best_distincts;
  for(unsigned int i = 0; i < distincts.size(); ++i){
    if(distincts[0].distinct_number_ == distincts[i].distinct_number_)
          best_distincts.push_back(distincts[i]);
  }
  unsigned int random_index = rand() % best_distincts.size();
  DimensionType best_dim = best_distincts[random_index].dim_;
  uint128_t best_cut = find_best_cut(current_ruleset, current_box,
                                     best_dim, cutting_type);

  return DimCut(best_dim, best_cut);
}

void saxpac::duplicate_intersected_rules(
    const UPF_Ruleset& orig_ruleset, const Range& range,
    const DimensionType best_dim, UPF_Ruleset& target_ruleset){

  for (unsigned int i = 0; i < orig_ruleset.size(); ++i){

    const UPF_Rule& curr_rule = orig_ruleset.get_rule(i);
    bool same_prot = true;
    ProtocolType curr_prot = curr_rule.get_l4_protocol();
    if(best_dim == udpdstport || best_dim == udpsrcport)
      if(curr_prot != UDP && curr_prot != TRANSPORT)
        same_prot = false;

    if(best_dim == tcpdstport || best_dim == tcpsrcport)
      if(curr_prot != TCP && curr_prot != TRANSPORT )
        same_prot = false;

    if(same_prot){
      if(curr_rule.get_field(best_dim) != nullptr && !curr_rule.has_unknown_match()){
        const Range& curr_range = curr_rule.get_field(best_dim)->get_range();
        if(curr_range.intersect(range))
          target_ruleset.add_rule(std::unique_ptr<UPF_Rule>(curr_rule.clone()));
      }
      else
        target_ruleset.add_rule(std::unique_ptr<UPF_Rule>(curr_rule.clone()));
    }
  }
}

void saxpac::build_child_node(Node* current_node,
                      UPF_Ruleset& child_ruleset,
                      const Bounding_Box& child_box,
                      const DimensionType best_dim,
                      std::queue<Node*>& nodes_to_process,
                      unsigned int& number){

  if(child_ruleset.size() > 0){
    saxpac::Node* child = current_node->create_child(child_ruleset,
                                                       child_box,
                                                       number);

    std::string name = child->get_name();
    UPF_Rule* jump(new UPF_Rule());
    jump->set_type(freerule);
    jump->set_field(best_dim,child_box.get_range(best_dim));
    jump->set_action(Jump(name));
    current_node->get_ruleset().add_rule(std::unique_ptr<UPF_Rule>(jump));
//    std::cout << "Jump Rule added" << std::endl;
    nodes_to_process.push(std::move(child));
  }
//  else if (tree.name_ != tree.next_tree_){
}

void two_node_hicut(saxpac::Node* current_node,
                    std::queue<saxpac::Node*>& nodes_to_process,
                    unsigned int& number_nodes){

  using namespace saxpac;
  // Determine best cut
  DimCut best_dimcut = determine_best_dimcut(current_node,equi_dist );
  DimensionType best_dim = best_dimcut.dim_;
  uint128_t best_cut = best_dimcut.cut_position_;

  std::cout << "Best Dimcut: " << dimensiontype_to_string(best_dimcut.dim_) << " " << best_dimcut.cut_position_ << std::endl;

  // Duplicate every intersected rule in respective child nodes
  UPF_Ruleset left_child_ruleset, right_child_ruleset;
  uint128_t low_bound = current_node->get_box().get_lower_bound(best_dim);
  uint128_t high_bound = current_node->get_box().get_upper_bound(best_dim);

  const UPF_Ruleset& current_ruleset = current_node->get_ruleset();
  const Range left_range = Range(low_bound,best_cut-1);
  const Range right_range = Range(best_cut,high_bound);

  if(best_cut > low_bound){
    duplicate_intersected_rules(current_ruleset, left_range,
                              best_dim, left_child_ruleset);
  }
  if(best_cut < high_bound){
    duplicate_intersected_rules(current_ruleset, right_range,
                                best_dim, right_child_ruleset);
  }

  current_node->get_ruleset().clear();
  if(best_cut > low_bound){
    Bounding_Box left_box(current_node->get_box());
    left_box.set_range(best_dim,left_range);
    build_child_node(current_node, left_child_ruleset, left_box,
            best_dim, nodes_to_process, number_nodes);
  }
  if(best_cut < high_bound){
    Bounding_Box right_box(current_node->get_box());
    right_box.set_range(best_dim,right_range);
    build_child_node(current_node, right_child_ruleset, right_box,
                    best_dim, nodes_to_process, number_nodes);
  }
}

void three_node_hypersplit(saxpac::Node* current_node,
                           std::queue<saxpac::Node*>& nodes_to_process,
                           unsigned int& number_nodes,
                           const CuttingType cutting_type){




  using namespace saxpac;
 // Determine best cut
  DimCut best_dimcut = determine_best_dimcut(current_node,cutting_type );
  DimensionType best_dim = best_dimcut.dim_;
  uint128_t best_cut = best_dimcut.cut_position_;

  // Duplicate every intersected rule in respective child nodes
  UPF_Ruleset left_child_ruleset, middle_child_ruleset, right_child_ruleset;
  uint128_t low_bound = current_node->get_box().get_lower_bound(best_dim);
  uint128_t high_bound = current_node->get_box().get_upper_bound(best_dim);

//  if(low_bound == best_cut-1)
//    low_bound = best_cut;
//  if(high_bound == best_cut+1)
//    high_bound = best_cut;
  const UPF_Ruleset& current_ruleset = current_node->get_ruleset();
  const Range left_range = Range(low_bound,best_cut-1);
  const Range middle_range = Range(best_cut, best_cut);
  const Range right_range = Range(best_cut+1,high_bound);



  if(best_cut > low_bound){
    duplicate_intersected_rules(current_ruleset, left_range,
                              best_dim, left_child_ruleset);
  }
  if(best_cut < high_bound){
    duplicate_intersected_rules(current_ruleset, right_range,
                                best_dim, right_child_ruleset);
  }
  duplicate_intersected_rules(current_ruleset, middle_range,
                              best_dim, middle_child_ruleset);
  Bounding_Box middle_box(current_node->get_box());
  middle_box.set_range(best_dim, middle_range);
  current_node->get_ruleset().clear();
  build_child_node(current_node, middle_child_ruleset, middle_box,
                   best_dim, nodes_to_process, number_nodes);
  if(best_cut > low_bound){
    Bounding_Box left_box(current_node->get_box());
    left_box.set_range(best_dim,left_range);
    build_child_node(current_node, left_child_ruleset, left_box,
            best_dim, nodes_to_process, number_nodes);
  }
  if(best_cut < high_bound){
    Bounding_Box right_box(current_node->get_box());
    right_box.set_range(best_dim,right_range);
    build_child_node(current_node, right_child_ruleset, right_box,
                    best_dim, nodes_to_process, number_nodes);
  }


}

void saxpac::process_node(Node* current_node,
                            const CuttingType cutting_type,
                            std::queue<Node*>& nodes_to_process,
                            unsigned int& number_nodes){
//
//  if(cutting_type == equi_dist)
//    two_node_hicut(current_node, nodes_to_process, number_nodes);
//  else
    three_node_hypersplit(current_node, nodes_to_process, number_nodes,
                          cutting_type);

}

void saxpac::Node::split_protocols(std::queue<Node*>& nodes_to_process,
                                     unsigned int& number_nodes){
    std::vector<Range> protocol_ranges;
    std::vector<UPF_Ruleset> new_sets;
    std::vector<Bounding_Box> new_boxes;
    const UPF_Ruleset& current_ruleset = get_ruleset();
    for(unsigned int i = 0; i < current_ruleset.size(); ++i){
      bool l4_wildcard = false;
      if(current_ruleset.get_rule(i).get_field(l4protocol) != nullptr){
       const Range& curr_range =  current_ruleset.get_rule(i).get_field(l4protocol)->get_range();
       protocol_ranges.push_back(curr_range);
      }
      else
        l4_wildcard = true;
      if(l4_wildcard)
        protocol_ranges.push_back(Range(uint128_t::min(),uint128_t::max()));
      }
    std::sort(protocol_ranges.begin(), protocol_ranges.end(),
              [](const Range& a, const Range& b)-> bool
                {return a.upper_ < b.upper_;});

    protocol_ranges.erase(unique(protocol_ranges.begin(), protocol_ranges.end(),
      [](const Range& a, const Range& b){ return a == b;}),
      protocol_ranges.end());

    for(unsigned int i = 0; i < protocol_ranges.size(); ++i){
      const Range curr_range = protocol_ranges[i];
      UPF_Ruleset new_set;
      Bounding_Box box = get_box();
      box.set_range(l4protocol, curr_range);
      duplicate_intersected_rules(current_ruleset, curr_range, l4protocol,
                                  new_set);
      new_sets.push_back(std::move(new_set));
      new_boxes.push_back(box);
    }
    get_ruleset().clear();
    for(unsigned int i = 0; i < new_sets.size(); ++i)
      build_child_node(this, new_sets[i], new_boxes[i], l4protocol,
                       nodes_to_process,number_nodes);
}

void saxpac::Node::collect_rulesets(
  std::vector<std::unique_ptr<UPF_Ruleset>>& collection){

    std::queue<Node*> nodes_to_collect;
    nodes_to_collect.push(this);
    unsigned int max_rules = 0;
    unsigned int max_non_jump_rules = 0;
    unsigned int count_non_jump_rules = 0;
    unsigned int count_all_rules = 0;
    unsigned int total_nodes = 0;
    while(!nodes_to_collect.empty()){
      Node* current_node = nodes_to_collect.front();
      if(current_node->number_of_rules() > max_non_jump_rules)
        max_non_jump_rules = current_node->number_of_rules();
      if(current_node->get_ruleset().size() > max_rules)
        max_rules = current_node->get_ruleset().size();
      count_non_jump_rules += current_node->number_of_rules();
      count_all_rules += current_node->get_ruleset().size();
      ++total_nodes;
      std::unique_ptr<UPF_Ruleset> new_set(
        new UPF_Ruleset(std::move(current_node->get_ruleset())));
      collection.push_back(std::move(new_set));
      for(unsigned int i = 0; i < current_node->child_nodes_.size(); ++i)
          nodes_to_collect.push(std::move(current_node->child_nodes_[i]).get());
      nodes_to_collect.pop();
    }
}

void saxpac::Node::remove_redundancy_in_box(){



  UPF_Ruleset new_rules(ruleset_.get_name(),freeset);
  std::vector<Bounding_Box> boxes;
  for (unsigned int i = 0; i < ruleset_.size(); ++i){
    Bounding_Box current_rule_box = this->box_;
    limit_rule_on_box(ruleset_.get_rule(i), current_rule_box);
    boxes.push_back(current_rule_box);
    bool covered = false;
    for(unsigned int j = 0; j < i; ++j){
      if(boxes[j].covers(current_rule_box)){
        covered = true;
        break;
      }
    }
    if(!covered)
      new_rules.add_rule(std::unique_ptr<UPF_Rule>(ruleset_.get_rule(i).clone()));
  }
  ruleset_ = new_rules;
}

saxpac::Node::Node(UPF_Ruleset& ruleset)
    :   name_("root"),
        num_childs_(0),
        ruleset_(std::move(ruleset)),
        node_number_(0){
            ruleset_.set_name("root");
        }

saxpac::Node::Node(UPF_Ruleset& ruleset, const std::string& tree_name)
    :   name_("root"),
        num_childs_(0),
        ruleset_(std::move(ruleset)),
        node_number_(0),
        tree_name_(tree_name){
            ruleset_.set_name(tree_name);
        }

saxpac::Node::Node(UPF_Ruleset& ruleset, const std::string& name,
                     const std::string& tree_name)
    :   name_(name),
        num_childs_(0),
        ruleset_(std::move(ruleset)),
        node_number_(0),
        tree_name_(tree_name){
            ruleset_.set_name(tree_name + "_" + name);
        }

saxpac::Node::Node(UPF_Ruleset& ruleset, const std::string& name,
                     const std::string& tree_name,
                     const unsigned int number, const Bounding_Box& box)
    :   name_(name),
        num_childs_(0),
        ruleset_(std::move(ruleset)),
        box_(box),
        node_number_(number),
        tree_name_(tree_name)
        {}

saxpac::Node* saxpac::Node::create_child(UPF_Ruleset& ruleset,
                                             const Bounding_Box& box,
                                             unsigned int& number){

    ++num_childs_;
    std::string new_name = tree_name_ + "_Node_" + std::to_string(number + 1);
    ruleset.set_name(new_name);
    std::unique_ptr<Node> new_child( new Node(ruleset,new_name, tree_name_,
                             (++number), box));
    Node* ptr = new_child.get();
    child_nodes_.push_back(std::move(new_child));
    return ptr;
}

UPF_Ruleset& saxpac::Node::get_ruleset(){
  return ruleset_;
}

uint64_t saxpac::Node::depth() const{

  uint64_t max_depth;

  if(child_nodes_.empty())
    return 0;
  else
    max_depth = 0;
  for(auto& child : child_nodes_){
    max_depth = std::max(max_depth,child->depth());
  }

  return max_depth + 1;
}

saxpac::Bounding_Box& saxpac::Node::get_box() {return box_;}

const std::string& saxpac::Node::get_name() const{return name_;}

unsigned int saxpac::Node::number_of_rules() const{
  uint64_t rules = 0;
  for(unsigned int i = 0; i < ruleset_.size(); ++i)
    if(ruleset_.get_rule(i).get_action().get_type() != JUMP)
      ++rules;
  return rules;
}

void saxpac::Node::cut(DecisionTree& tree,
                         const CuttingType cutting_type,
                         const unsigned int threshold){

    unsigned int number_nodes = 1;
    std::queue<Node*> nodes_to_process;
    //     Split Rules by Layer 4 Protocols

    if(tree.name_ != tree.next_tree_ && tree.next_tree_ != "FINAL_SET"){
        std::string next_set_name = tree.next_tree_;
          UPF_Rule* jump(new UPF_Rule());
          jump->set_type(freerule);
          jump->set_action(Jump(next_set_name));
          this->get_ruleset().add_rule(std::unique_ptr<UPF_Rule>(jump));
      }

    this->split_protocols(nodes_to_process, number_nodes);
    nodes_to_process.push(this);
    while(!nodes_to_process.empty()){
      Node* current_node = nodes_to_process.front();
      if(current_node->is_cuttable()){
//        std::cout << "NODES TO PROCESS: " << nodes_to_process.size() << std::endl;
        current_node->remove_redundancy_in_box();
        // Check if Node needs to be splitted
        if(current_node->number_of_rules() > threshold){
//          if(current_node->number_of_rules() < 4){
//            std::cout << current_node->get_ruleset();
//            current_node->get_box().print();
//          }
          process_node(current_node,cutting_type, nodes_to_process, number_nodes);
        }
      }
      if(tree.name_ != tree.next_tree_ && tree.next_tree_ != "FINAL_SET"){
        std::string next_set_name = tree.next_tree_;
          UPF_Rule* jump(new UPF_Rule());
          jump->set_type(freerule);
          jump->set_action(Jump(next_set_name));
          current_node->get_ruleset().add_rule(std::unique_ptr<UPF_Rule>(jump));
      }
      nodes_to_process.pop();
      }
}


bool saxpac::Node::is_cuttable(){

  bool is_cuttable = false;
  std::vector<DimensionType> dims;
  build_dimvector(dims,this->get_ruleset());

  for(auto& dim : dims){
    if(has_endpoints(this, dim)){
      is_cuttable = true;
      break;
    }
  }
//  if(is_cuttable){
//    std::cout << "CUTTABLE RULESET " << std::endl;
//    std::cout << this->get_ruleset();
//  }
  return is_cuttable;

}



saxpac::DecisionTree::DecisionTree(UPF_Ruleset& ruleset,
                                     const std::string& tree_name,
                                     const std::string& next_tree)
  : root_node_(Node(ruleset, tree_name)),
    name_(tree_name),
    next_tree_(next_tree){}

void saxpac::DecisionTree::create_tree(const CuttingType cut_type,
                                    const unsigned int binth){

  root_node_.cut(*this, cut_type, binth);
}

void saxpac::DecisionTree::collect_nodes(
  std::vector<std::unique_ptr<UPF_Ruleset>>& collection){
  root_node_.collect_rulesets(collection);
}

unsigned int saxpac::Bounding_Box::num_dimensions() const{
  return dimensions_;
}

saxpac::Bounding_Box::Bounding_Box()
    :   macsrc_(Range(uint128_t::min(), 281474976710655)),
        macdst_(Range(uint128_t::min(), 281474976710655)),
        ipv4src_(Range(uint128_t::min(),4294967295)),
        ipv4dst_(Range(uint128_t::min(),4294967295)),
        ipv6src_(Range(uint128_t::min(), uint128_t::max())),
        ipv6dst_(Range(uint128_t::min(), uint128_t::max())),
        l4protocol_(Range(uint128_t::min(),255)),
        tcpsrcport_(Range(uint128_t::min(),65535)),
        tcpdstport_(Range(uint128_t::min(),65535)),
        udpsrcport_(Range(uint128_t::min(),65535)),
        udpdstport_(Range(uint128_t::min(),65535)),
        dimensions_(11),
        unknown_match_(false)
        {}

saxpac::Bounding_Box::Bounding_Box(const Bounding_Box& rhs)
    :   macsrc_(rhs.get_range(macsrc)),
        macdst_(rhs.get_range(macdst)),
        ipv4src_(rhs.get_range(ipv4src)),
        ipv4dst_(rhs.get_range(ipv4dst)),
        ipv6src_(rhs.get_range(ipv6src)),
        ipv6dst_(rhs.get_range(ipv6dst)),
        l4protocol_(rhs.get_range(l4protocol)),
        tcpsrcport_(rhs.get_range(tcpsrcport)),
        tcpdstport_(rhs.get_range(tcpdstport)),
        udpsrcport_(rhs.get_range(udpsrcport)),
        udpdstport_(rhs.get_range(udpdstport)),
        dimensions_(rhs.dimensions_),
        unknown_match_(rhs.unknown_match_)
        {}

void saxpac::Bounding_Box::print() const{
  for (unsigned int i = 0; i < dimensions_; ++i){
//    std::cout << "Dimension " << i << " : ";
    this->get_range((DimensionType)i).print();
 }
}

bool saxpac::Bounding_Box::covers(const Bounding_Box& rhs) const{

  if(rhs.unknown_match_)
    return false;

  bool covered = true;
  for (unsigned int i = 0; i < dimensions_; ++i){
    DimensionType curr_dim = static_cast<DimensionType>(i);
    if(get_lower_bound(curr_dim) > rhs.get_lower_bound(curr_dim)){
      covered = false;
      break;
    }
    if(get_upper_bound(curr_dim) < rhs.get_upper_bound(curr_dim)){
      covered = false;
      break;
    }
  }
  return covered;
}

void saxpac::Bounding_Box::set_unknown(const bool unknown){
  unknown_match_ = unknown;
}

void saxpac::Bounding_Box::set_range(const DimensionType dim,
                                       const Range& range){

    switch(dim){
        case macsrc:        {macsrc_ = range; break;}
        case macdst:        {macdst_ = range; break;}
        case ipv4src:       {ipv4src_  = range; break;}
        case ipv4dst:       {ipv4dst_  = range; break;}
        case ipv6src:       {ipv6src_  = range; break;}
        case ipv6dst:       {ipv6dst_  = range; break;}
        case l4protocol:    {l4protocol_ = range; break;}
        case tcpsrcport:    {tcpsrcport_  = range; break;}
        case tcpdstport:    {tcpdstport_  = range; break;}
        case udpsrcport:    {udpsrcport_  = range; break;}
        case udpdstport:    {udpdstport_  = range; break;}
    }
}

inline const Range& saxpac::Bounding_Box::get_range(const DimensionType dim) const{

      switch(dim){
        case macsrc:        {return macsrc_;}
        case macdst:        {return macdst_;}
        case ipv4src:       {return ipv4src_;}
        case ipv4dst:       {return ipv4dst_;}
        case ipv6src:       {return ipv6src_;}
        case ipv6dst:       {return ipv6dst_;}
        case l4protocol:    {return l4protocol_;}
        case tcpsrcport:    {return tcpsrcport_;}
        case tcpdstport:    {return tcpdstport_;}
        case udpsrcport:    {return udpsrcport_;}
        case udpdstport:    {return udpdstport_;}
    }
    return macsrc_;
}


inline const uint128_t& saxpac::Bounding_Box::get_lower_bound(const DimensionType dim) const{

    switch(dim){
        case macsrc:        {return macsrc_.lower_;}
        case macdst:        {return macdst_.lower_;}
        case ipv4src:       {return ipv4src_.lower_;}
        case ipv4dst:       {return ipv4dst_.lower_;}
        case ipv6src:       {return ipv6src_.lower_;}
        case ipv6dst:       {return ipv6dst_.lower_;}
        case l4protocol:    {return l4protocol_.lower_;}
        case tcpsrcport:    {return tcpsrcport_.lower_;}
        case tcpdstport:    {return tcpdstport_.lower_;}
        case udpsrcport:    {return udpsrcport_.lower_;}
        case udpdstport:    {return udpdstport_.lower_;}
    }
    return macsrc_.lower_;
}


inline const uint128_t& saxpac::Bounding_Box::get_upper_bound(const DimensionType dim) const{

    switch(dim){
        case macsrc:        {return macsrc_.upper_;}
        case macdst:        {return macdst_.upper_;}
        case ipv4src:       {return ipv4src_.upper_;}
        case ipv4dst:       {return ipv4dst_.upper_;}
        case ipv6src:       {return ipv6src_.upper_;}
        case ipv6dst:       {return ipv6dst_.upper_;}
        case l4protocol:    {return l4protocol_.upper_;}
        case tcpsrcport:    {return tcpsrcport_.upper_;}
        case tcpdstport:    {return tcpdstport_.upper_;}
        case udpsrcport:    {return udpsrcport_.upper_;}
        case udpdstport:    {return udpdstport_.upper_;}
    }
   return macsrc_.lower_;
}
