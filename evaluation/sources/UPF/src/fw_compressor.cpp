#include "fw_compressor.h"
#include <limits>

Segment::Segment(const Range& range, Node* target)
  : range_(range),
    target_(target),
    level_(1)
    {}

Segment::Segment(const Range& range, Node* target, const uint64_t level)
  : range_(range),
    target_(target),
    level_(level)
    {}


void Segment::set_level(const uint64_t level){
  level_ = level;
}

uint64_t Segment::get_level() const{
  return level_;
}

Node* Segment::get_target() const{
  return target_;
}

const Range& Segment::get_range() const{
  return range_;
}

void Segment::merge(const Segment& rhs){
  if(range_.upper_ < rhs.get_range().upper_)
    range_.upper_ = rhs.get_range().upper_;
}

SegmentSorter::SegmentSorter(const std::vector<Edge*>& edges,
                             const FieldType fieldkey)
  : field_key_(fieldkey){

  for (auto& edge : edges){
    for (auto& range : edge->label_){
      add_segment(Segment(range,edge->target_node_));
    }
  }
}

SegmentSorter::SegmentSorter(const FieldType fieldkey)
  : field_key_(fieldkey) {}

void SegmentSorter::add_segment(const Segment& segment){
  segments_.push_back(segment);
}

const std::vector<Segment>& SegmentSorter::get_segments() const{
  return segments_;
}

FieldType SegmentSorter::get_fieldkey() const{
  return field_key_;
}

Segment& SegmentSorter::find_lowest_seg_level(Node* target, uint64_t minimal_level){

  uint64_t lowest_level = std::numeric_limits<uint64_t>::max();

  Segment* low_seg = &segments_[0];

  for(auto& segment : segments_){
    if(segment.get_target() == target){
      if(segment.get_level() < lowest_level &&
         segment.get_level() >= minimal_level){
          lowest_level = segment.get_level();
          low_seg = &segment;
         }
    }
  }
  return *low_seg;;

}

void SegmentSorter::find_minimal_order(){


  std::sort(segments_.begin(),segments_.end(),
            [](const Segment& lhs, const Segment& rhs) -> bool
            {return lhs.get_range().lower_ < rhs.get_range().lower_;});

  uint64_t current_level = 1;
  Node* current_target = segments_[0].get_target();
  Segment* last_segment = &segments_[0];




    for(auto iter = segments_.begin() + 1; iter != segments_.end();){
      Segment& current_segment = *iter;
      if(current_segment.get_target() == current_target && field_key_ != l4_proto){
        last_segment->merge(current_segment);
        iter = segments_.erase(iter);
      }
      else{
        current_segment.set_level(++current_level);
        current_target = current_segment.get_target();
        last_segment = &current_segment;
        ++iter;
      }
    }
    current_level = 1;
    std::vector<std::vector<Segment>::reverse_iterator> riters;
    for(auto riter = segments_.rbegin(); riter != segments_.rend();++riter){
      Segment& current_segment = *riter;
      Segment& lowest_seg = find_lowest_seg_level(current_segment.get_target(),
                                                        current_level);
      if(current_segment.get_level() > lowest_seg.get_level()){
        current_segment.set_level(lowest_seg.get_level());
        if(field_key_ != l4_proto){
          lowest_seg.merge(current_segment);
          riters.push_back(riter);
        }
      }
    }
    for(auto& iter : riters)
      segments_.erase(std::next(iter).base());
  std::sort(segments_.begin(),segments_.end(),
            [](const Segment& lhs, const Segment& rhs) -> bool
            {return lhs.get_level() > rhs.get_level();});

}


void FW_Compressor::reduce_fdd(Tree& fdd_tree){

  bool has_changed = true;
  while(has_changed){
    has_changed = false;
    for(auto iter = fdd_tree.nodes_.begin(); iter != fdd_tree.nodes_.end(); ++iter){
      Node* i_node = (*iter).second.get();
      for(auto niter = fdd_tree.nodes_.begin(); niter != fdd_tree.nodes_.end(); ){
        Node* j_node = (*niter).second.get();
        if(i_node->is_isomorphic(j_node)){
          fdd_tree.merge_nodes(i_node->id_,j_node->id_);
          niter = fdd_tree.nodes_.erase(niter);
          has_changed = true;
        }
        else
          ++niter;
      }
    }
  }
}

void build_rules(Node* current_node, std::vector<UPF_Rule*>& rules,
                std::vector<std::vector<UPF_Rule*>*>& rulevectors){


  if(current_node->is_field()){
    FieldNode* fnode = static_cast<FieldNode*>(current_node);
    SegmentSorter new_sorter(fnode->edges_,fnode->field_key_);
    new_sorter.find_minimal_order();
    std::set<Node*> target_set;
    std::vector<Node*> targets;

    std::vector<UPF_Rule*> temp_rules(rules);

    for(unsigned int i = 0; i < new_sorter.get_segments().size(); ++i){
      const Segment& curr_segment = new_sorter.get_segments()[i];
      targets.push_back(curr_segment.get_target());
    }
    auto it = std::unique(targets.begin(), targets.end());
    targets.resize( std::distance(targets.begin(),it) );
    for(unsigned int j = 0; j < targets.size(); ++j){
      if(j == 0){
        for(auto iter = temp_rules.begin(); iter != temp_rules.end(); ++iter){
          UPF_Rule& current_rule = **iter;
          for(unsigned int i = 0; i < new_sorter.get_segments().size(); ++i){
            const Segment& curr_segment = new_sorter.get_segments()[i];
            if(targets[j] == curr_segment.get_target()){
              if(i == 0)
                current_rule.set_field(fnode->field_key_,curr_segment.get_range());
              else{
                UPF_Rule* new_rule = current_rule.clone();
                new_rule->set_field(new_sorter.get_fieldkey(),curr_segment.get_range());
                rules.push_back(new_rule);
              }
            }
          }
        }
        build_rules(targets[j],rules, rulevectors);
      }
      else{
        std::vector<UPF_Rule*>* new_vec = new std::vector<UPF_Rule*>();
        rulevectors.push_back(new_vec);
        for(unsigned int i = 0; i < new_sorter.get_segments().size(); ++i){
          const Segment& curr_segment = new_sorter.get_segments()[i];
          if(targets[j] == curr_segment.get_target()){
            for(auto iter = temp_rules.begin(); iter != temp_rules.end(); ++iter){
              UPF_Rule& current_rule = **iter;
              if(i == 0){
                (*iter)->set_field(fnode->field_key_,curr_segment.get_range());
              }
              else{
                UPF_Rule* new_rule = current_rule.clone();
                new_rule->set_field(new_sorter.get_fieldkey(),curr_segment.get_range());
                new_vec->push_back(new_rule);
              }
            }
          }
        }
        build_rules(targets[j],*new_vec, rulevectors);
      }
    }
  }
  else
    for(auto& rule : rules)
      rule->set_action(static_cast<TerminalNode*>(current_node)->action_);
}

void FW_Compressor::compress_ruleset(UPF_Ruleset& ruleset){

  if(ruleset.empty())
    return;

  FDD fdd;
  std::unique_ptr<Tree> fdd_tree(fdd.create_fdd(ruleset));
  reduce_fdd(*fdd_tree);
  std::vector<std::vector<UPF_Rule*>*> rulevectors;

  FieldNode* fnode = static_cast<FieldNode*>(fdd_tree->get_node_by_id(1));
  SegmentSorter new_sorter(fnode->edges_,fnode->field_key_);
  new_sorter.find_minimal_order();
  for(auto& segment : new_sorter.get_segments()){
    UPF_Rule* new_rule = new UPF_Rule();
    new_rule->set_field(new_sorter.get_fieldkey(),segment.get_range());
    std::vector<UPF_Rule*>* rulevec = new std::vector<UPF_Rule*>();
    rulevec->push_back(new_rule);
    rulevectors.push_back(rulevec);
    build_rules(segment.get_target(),*rulevec,rulevectors);
  }
  ruleset.clear();

   for(auto& rulevec : rulevectors){
    for(auto& rule : *rulevec)
      ruleset.add_rule(std::unique_ptr<UPF_Rule>(rule));
    delete(rulevec);
  }
  fdd.remove_with_fdd(ruleset);
}
std::ostream& operator<<(std::ostream& out, const Segment& rhs){

  out << "Range: " << rhs.get_range() << " Level: " << rhs.get_level()
  << " Target : " << rhs.get_target() << std::endl;
  return out;
}

std::ostream& operator<<(std::ostream& out, const SegmentSorter& rhs){

  out << "Field: " << fieldtype_to_string(rhs.get_fieldkey()) << std::endl;
  for (auto& segment : rhs.segments_)
    out << segment;
  return out;
}
