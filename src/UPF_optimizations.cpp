#include "UPF_manager.h"
#include "dec_tree.h"
#include "graph.h"
#include "fdd.h"
#include "saxpac.h"
#include "fw_compressor.h"
#include "UPF_exporter.h"

// --- Optimizations ---------------------------------------------------------//


// Ruleset Splitter ------------------------------------------------------------

UPF_Ruleset* get_subset_copy(const UPF_Ruleset& ruleset, std::string name,
                             const uint64_t start, const uint64_t end){

    UPF_Ruleset* copy_set = new UPF_Ruleset(name, freeset);
     for(uint64_t i = start; i < end; ++i){
      const UPF_Rule& current_rule = ruleset.get_rule(i);
      std::unique_ptr<UPF_Rule> new_rule(current_rule.clone());
      copy_set->add_rule(std::move(new_rule));
    }

  return copy_set;

}

UPF_Ruleset* get_subset_by_protocol(const UPF_Ruleset& ruleset,
                                    const ProtocolType type){

    const std::string new_name = ruleset.get_name() + "_" + protocoltype_to_string(type);
    UPF_Ruleset* copy_set = new UPF_Ruleset(new_name, freeset);

    for(uint64_t i = 0; i < ruleset.size(); ++i){
      const UPF_Rule& current_rule = ruleset.get_rule(i);
      if(current_rule.get_field(l4_proto)->get_protocol() == type){
        std::unique_ptr<UPF_Rule> new_rule(current_rule.clone());
        copy_set->add_rule(std::move(new_rule));
      }
    }

  return copy_set;
}


bool field_independent(const UPF_Rule& lhs, const UPF_Rule& rhs,
                           DimensionType field_key){

    if(lhs.get_field(field_key) == nullptr ||
       rhs.get_field(field_key) == nullptr)
      return false;
    else
      return !lhs.get_field(field_key)->intersect(rhs.get_field(field_key));

}

//SAXPAC_Block find_independent_block(const UPF_Ruleset& ruleset){
//
//  uint64_t index_low = 1;
//  uint64_t index_high = 1;
//  uint64_t max_block_size = 0;
//  DimensionType best_field = macsrc;
//  std::vector<DimensionType> fieldkeys = UPF_Rule::get_dimkeys();
//
//  for(uint64_t i = 0; i < ruleset.size(); ++i){
//    for(auto iter = fieldkeys.begin(); iter != fieldkeys.end(); ++iter){
//      const DimensionType field_key = *iter;
//      for(uint64_t j = i; j < ruleset.size() - 1; ++j){
//        const UPF_Rule& rule1 = ruleset.get_rule(j);
//        const UPF_Rule& rule2 = ruleset.get_rule(j+1);
//        if(!field_independent(rule1,rule2, field_key)){
//          if(j - i > max_block_size){
//            index_low = i + 1 ;
//            index_high = j + 1;
//            max_block_size = j - i;
//            best_field = field_key;
//          }
//          break;
//        }
//      }
//    }
//  }
//  return SAXPAC_Block(index_low, index_high, best_field);
//}

SAXPAC_Block find_independent_block(const UPF_Ruleset& ruleset,
                                    const uint64_t start,
                                    const uint64_t end){

  uint64_t index_low = start;
  uint64_t index_high = start;
  uint64_t max_block_size = 0;
  DimensionType best_field = macsrc;
  std::vector<DimensionType> fieldkeys = UPF_Rule::get_dimkeys();
  bool independent = true;
  uint64_t current_block_high = start;

  for(auto iter = fieldkeys.begin(); iter != fieldkeys.end(); ++iter){
    const DimensionType field_key = *iter;
    for(uint64_t i = start; i < end; ++i){
        if (end - i < max_block_size)
          break;
      current_block_high = i+1;
      for(uint64_t j = i+1; j < end; ++j){
        const UPF_Rule& rule1 = ruleset.get_rule(j);
        for(uint64_t k = i; k < j; ++k){
          const UPF_Rule& rule2 = ruleset.get_rule(k);
          if(!field_independent(rule1,rule2, field_key)){
            if(current_block_high - i > max_block_size){
                index_low = i;
                index_high = current_block_high;
                max_block_size = current_block_high - i;
                best_field = field_key;
            }
            else
              // Skip to end of current block
              i = j;
            independent = false;
            break;
          }
        }
      if(independent){
        ++current_block_high;
      }
      else{
        independent = true;
        break;
      }
      }
    }
  }
  return SAXPAC_Block(index_low, index_high, best_field);
}


std::vector<SAXPAC_Block> get_saxpack_blocks(const UPF_Ruleset& ruleset,
                                             const uint64_t threshold){

  uint64_t start_index = 0;
  uint64_t end_index = ruleset.size();
  std::vector<SAXPAC_Block> blocks;

  std::queue<std::pair<uint64_t,uint64_t>> blocks_to_process;
  blocks_to_process.push(std::make_pair(start_index,end_index));
  std::pair<uint64_t,uint64_t> prev_pair, next_pair;

  while(!blocks_to_process.empty()){
    std::pair<uint64_t, uint64_t> current_block = blocks_to_process.front();
    uint64_t curr_start =  current_block.first;
    uint64_t curr_end = current_block.second;

    SAXPAC_Block new_block = find_independent_block(ruleset, curr_start, curr_end);
    if(new_block.end_index - new_block.start_index >= threshold){
      if(new_block.start_index > curr_start){
          prev_pair = std::make_pair(curr_start, new_block.start_index - 1);
          blocks_to_process.push(prev_pair);
      }
      if(new_block.end_index <= curr_end){
          next_pair = std::make_pair(new_block.end_index + 1, curr_end);
          blocks_to_process.push(next_pair);
      }
//      std::cout << "Max Order Independent Block Size : " << new_block.end_index - new_block.start_index <<  std::endl <<
//      "From rule " << new_block.start_index << " to rule " <<  new_block.end_index << std::endl <<
//      "Order Independent Field: " << dimensiontype_to_string(new_block.field_key) << std::endl;
      blocks.push_back(new_block);
    }
    blocks_to_process.pop();
  }
  std::cout << "Found " << blocks.size() << " SAXPAC Blocks" << std::endl;
  return blocks;
}

std::vector<std::shared_ptr<UPF_Ruleset>> RulesetSplitter::equal_split(
  UPF_Ruleset& ruleset, const uint64_t block_size){

  const std::string& orig_name = ruleset.get_name();
  uint64_t rset_number = (ruleset.size() / block_size) + 1;
  std::vector<std::shared_ptr<UPF_Ruleset>> rsets;
  uint64_t index = 0;
  for(unsigned int k = 0; k < rset_number; ++k){
    if(k > 0){
      rsets.push_back(std::shared_ptr<UPF_Ruleset>(
        new UPF_Ruleset(orig_name + "_B" + std::to_string(k), freeset)));
    }
    else{
      rsets.push_back(std::shared_ptr<UPF_Ruleset>(
        new UPF_Ruleset(orig_name, freeset)));

    }
    for(unsigned int i = 0; i < block_size && index < ruleset.size(); ++i){
      rsets[k]->add_rule(std::move(ruleset.draw_rule(index)));
      ++index;
      }
    }
  for(auto iter = rsets.begin(); iter != rsets.end(); ++iter){
    if((*iter)->size() == 0){
      rsets.erase(iter,rsets.end());
      break;
    }
  }
  return std::move(rsets);
}

void reduce_rule_to_field(UPF_Rule& rule, const DimensionType field_key){

  std::vector<DimensionType> field_keys =  UPF_Rule::get_dimkeys();

  for(auto key : field_keys){
    if(field_key == udpsrcport || field_key == udpdstport ||
       field_key == tcpsrcport || field_key == tcpdstport){
      if(key != field_key && key != l4protocol){
        if(rule.get_field(key) != nullptr)
            rule.set_field(key, Range());
      }
    }
    else{
      if(key != field_key ){
        if(rule.get_field(key) != nullptr)
            rule.set_field(key, Range());
      }
    }
  }
}

UPF_Ruleset* create_field_reduced_ruleset(const std::string& name,
            std::vector<std::shared_ptr<UPF_Ruleset>>& fp_sets,
            const DimensionType field_key){

  UPF_Ruleset* field_reduced_set = new UPF_Ruleset(name + "_FR", freeset);

  for(uint64_t k = 0; k < fp_sets.size(); ++k){
    const UPF_Rule& current_rule = fp_sets[k]->get_rule(0);
    std::unique_ptr<UPF_Rule> new_rule(current_rule.clone());
    new_rule->set_action(Jump(fp_sets[k]->get_name()));
    reduce_rule_to_field(*new_rule, field_key);
//    std::cout << "RULEBLABLA" << *new_rule << std::endl;
    field_reduced_set->add_rule(std::move(new_rule));
  }
//    std::unique_ptr<UPF_Rule> new_rule(new UPF_Rule());
//    new_rule->set_action(Jump(name + "_NOD"));
//    std::cout << "RULEBLABLA" << *new_rule << std::endl;
//    field_reduced_set->add_rule(std::move(new_rule));


  return field_reduced_set;


}

std::vector<std::shared_ptr<UPF_Ruleset>> create_false_positive_block(
                                          const UPF_Ruleset& ruleset,
                                          const std::string& next_set){

  const std::string& set_name = ruleset.get_name();
  std::vector<std::shared_ptr<UPF_Ruleset>> fp_sets;
  UPF_Rule jump_rule;
  jump_rule.set_action(Jump(next_set));

  for(uint64_t i = 0; i < ruleset.size(); ++i){
    std::shared_ptr<UPF_Ruleset> fp_set(new UPF_Ruleset(set_name + "_FP_Rule_" + std::to_string(i), freeset));
    const UPF_Rule& current_rule = ruleset.get_rule(i);
    std::unique_ptr<UPF_Rule> new_rule(current_rule.clone());
//    std::unique_ptr<UPF_Rule> new_jump_rule(jump_rule.clone());
    fp_set->add_rule(std::move(new_rule));
//    fp_set->add_rule(std::move(new_jump_rule));
    fp_sets.push_back(std::move(fp_set));
  }
  return std::move(fp_sets);
}


std::vector<std::shared_ptr<UPF_Ruleset>> RulesetSplitter::saxpac_split(
  const UPF_Ruleset& ruleset, const uint64_t threshold = 1){

  std::vector<std::shared_ptr<UPF_Ruleset>> saxpac_sets;
  // get indexes of largest order independent block
  std::vector<SAXPAC_Block> blocks(get_saxpack_blocks(ruleset, threshold));

  if(blocks.empty()){
    std::shared_ptr<UPF_Ruleset> orig_set(get_subset_copy(ruleset,
                                                          ruleset.get_name(),
                                                          0, ruleset.size()));
    orig_set->set_transformable(true);
    saxpac_sets.push_back(std::move(orig_set));
    std::cout << "No SAXPAC Blocks found, Threshold too high" << std::endl;
    return saxpac_sets;
  }

//  std::cout << "Order Independent Blocks found: "
//  + std::to_string(blocks.size()) << std::endl;

  std::sort(blocks.begin(), blocks.end());
  std::string set_name = ruleset.get_name();

  uint64_t curr_start = 0;

  for(uint64_t i = 0; i < blocks.size(); ++i){
    const SAXPAC_Block& block = blocks[i];
    uint64_t curr_end = block.start_index - 1;

    if(i > 0){
      set_name = ruleset.get_name() + "_SP" + std::to_string(i);
      curr_start = block.end_index;
    }

    // Build POD Block

//    if(curr_start < block.start_index){
      std::shared_ptr<UPF_Ruleset> prev_set(
      get_subset_copy(ruleset, set_name, curr_start, block.start_index));
      prev_set->set_transformable(true);
      saxpac_sets.push_back(std::move(prev_set));
//    }

    //Build Field Reduced Block and False Positive Sets

    std::shared_ptr<UPF_Ruleset> oi_ruleset(get_subset_copy(ruleset, set_name,
                                            block.start_index, block.end_index));
    std::vector<std::shared_ptr<UPF_Ruleset>> fp_sets(
              create_false_positive_block(*oi_ruleset, set_name + "_NOD"));

    std::shared_ptr<UPF_Ruleset> field_reduced_set(create_field_reduced_ruleset(
                                 set_name, fp_sets, block.field_key));
    field_reduced_set->set_transformable(true);

    saxpac_sets.push_back(std::move(field_reduced_set));
    saxpac_sets.insert(saxpac_sets.end(), std::make_move_iterator(fp_sets.begin()),
                       std::make_move_iterator(fp_sets.end()));
    // Build NOD Block if gap to next block

    if(i < blocks.size() - 1){
      curr_end = blocks[i+1].start_index;
      std::shared_ptr<UPF_Ruleset> next_set(
        get_subset_copy(ruleset, set_name + "_NOD", block.end_index, curr_end));
      next_set->set_transformable(true);
      saxpac_sets.push_back(std::move(next_set));
    }
    // If at end
    else if(curr_start < ruleset.size()){
      std::shared_ptr<UPF_Ruleset> next_set(
        get_subset_copy(ruleset, set_name + "_NOD", block.end_index,ruleset.size()));
      next_set->set_transformable(true);
      saxpac_sets.push_back(std::move(next_set));
    }
  }
    return std::move(saxpac_sets);
}



void remove_with_fdd_single(std::shared_ptr<UPF_Ruleset> ruleset,
                              UPF_Ruleset& merge_set){
    FDD fdd;
    fdd.remove_with_fdd(*ruleset);
    merge_set.append(ruleset);

}

void remove_with_fdd_threaded(UPF_Manager& manager, std::shared_ptr<UPF_Ruleset> ruleset,
                              UPF_Ruleset& merge_set){

    FDD fdd;
    fdd.remove_with_fdd(*ruleset);
    merge_set.append(ruleset);
    manager.close_thread();

}

void UPF_Manager::optimize_fdd(const FDD_Redundancy& opt, Statistic& stats){

      std::vector<std::unique_ptr<UPF_Ruleset>> temp_sets;
      std::vector<UPF_Ruleset*> merge_sets;
      stats.add_stat(Statistic::Entry("Total Rules Before",
                                      std::to_string(total_rules())));
      auto start = std::chrono::system_clock::now();
      for(unsigned int i = 0; i < rulesets_.size(); ++i){
        UPF_Ruleset* merge_set = new UPF_Ruleset(rulesets_[i]->get_name(),freeset);
        merge_sets.push_back(merge_set);
        std::vector<std::shared_ptr<UPF_Ruleset>> split_sets(
          RulesetSplitter::equal_split(*rulesets_[i], opt.block_size_));
        for(unsigned int j = 0; j < split_sets.size(); ++j){
          std::cout << "Split set : " << j << " - " << split_sets[j]->size() << std::endl;
          remove_with_fdd_single(std::move(split_sets[j]), *merge_set);
        }
      }
      for (auto merge_set : merge_sets)
        temp_sets.push_back(std::unique_ptr<UPF_Ruleset>(merge_set));
      replace_rulesets(temp_sets);
      stats.add_stat(Statistic::Entry("Total Rules After",
                                      std::to_string(total_rules())));
      auto end = std::chrono::system_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
      std::cout << "Processing Time (in ms) " << elapsed.count() << std::endl;
      stats.add_stat(Statistic::Entry("Processing Time (in ms)", std::to_string(elapsed.count())));
}
void UPF_Manager::optimize_fdd_mt(const FDD_Redundancy& opt, Statistic& stats){

      typedef std::vector<std::unique_ptr<UPF_Ruleset>> RulesetVec;
      typedef std::vector<std::shared_ptr<UPF_Ruleset>> SplitSetVec;

      std::string set_name = rulesets_[0]->get_name();
      std::vector<std::unique_ptr<std::thread>> threads;
      RulesetVec temp_sets;
      SplitSetVec split_sets;
      std::vector<UPF_Ruleset*> merge_sets;
      stats.add_stat(Statistic::Entry("Total Rules Before",
                                      std::to_string(total_rules())));
      auto start = std::chrono::system_clock::now();
      for(unsigned int i = 0; i < rulesets_.size();++i){
        if(rulesets_[i]->transformable() && rulesets_[i]->size() > 1){
          SplitSetVec curr_sets(RulesetSplitter::equal_split(*rulesets_[i],
                                                              opt.block_size_));
          split_sets.insert(split_sets.end(),
                            std::make_move_iterator(curr_sets.begin()),
                            std::make_move_iterator(curr_sets.end()));
        }
        else{
          std::shared_ptr<UPF_Ruleset> new_ptr(new UPF_Ruleset(std::move(*rulesets_[i])));
          split_sets.push_back(std::move(new_ptr));
        }
      }
      for(unsigned int j = 0; j < split_sets.size(); ++j){
        UPF_Ruleset* merge_set = new UPF_Ruleset();
        merge_sets.push_back(merge_set);
        if(thread_available()){
          open_thread();
          std::unique_ptr<std::thread> new_thread(new std::thread(
            &remove_with_fdd_threaded,std::ref(*this),
            std::move(split_sets[j]), std::ref(*merge_set)));
          threads.push_back(std::move(new_thread));
        }
        else
          remove_with_fdd_single(std::move(split_sets[j]), *merge_set);
      }
      for (auto& thread : threads)
        thread->join();
      std::unique_ptr<UPF_Ruleset> merged_set(new UPF_Ruleset(set_name, freeset));
      for (auto merge_set : merge_sets)
        merged_set->append(*merge_set);
      temp_sets.push_back(std::move(merged_set));
      replace_rulesets(temp_sets);
      stats.add_stat(Statistic::Entry("Total Rules After",
                                      std::to_string(total_rules())));
      auto end = std::chrono::system_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
      std::cout << "Processing Time (in ms) " << elapsed.count() << std::endl;
      stats.add_stat(Statistic::Entry("Processing Time (in ms)", std::to_string(elapsed.count())));
}

void hypersplit_single(std::shared_ptr<UPF_Ruleset> ruleset,
                         const Hypersplit& opti,
                         const std::string& nset,
                         std::vector<std::unique_ptr<UPF_Ruleset>>& collect_set){


    dec_tree::DecisionTree tree(*ruleset,ruleset->get_name(), nset);
    tree.create_tree(opti.cutting_type_, opti.binth_);
    std::vector<std::unique_ptr<UPF_Ruleset>> temp_set;
    tree.collect_nodes(collect_set);

}


void hypersplit_threaded(UPF_Manager& manager, std::shared_ptr<UPF_Ruleset> ruleset,
                         const Hypersplit& opti,
                         const std::string& nset,
                         std::vector<std::unique_ptr<UPF_Ruleset>>& collect_set){


    dec_tree::DecisionTree tree(*ruleset,ruleset->get_name(), nset);
    tree.create_tree(opti.cutting_type_, opti.binth_);
    std::vector<std::unique_ptr<UPF_Ruleset>> temp_set;
    tree.collect_nodes(collect_set);
    manager.close_thread();

}

void saxpac_single(std::shared_ptr<UPF_Ruleset> ruleset,
                         const SAXPAC&,
                         const std::string& nset,
                         std::vector<std::unique_ptr<UPF_Ruleset>>& collect_set){
  saxpac::DecisionTree tree(*ruleset, ruleset->get_name() , nset);
  tree.create_tree(weighted_seg, 4);
  std::vector<std::unique_ptr<UPF_Ruleset>> temp_set;
  tree.collect_nodes(collect_set);

}

void saxpac_threaded(UPF_Manager& manager, std::shared_ptr<UPF_Ruleset> ruleset,
                         const SAXPAC&,
                         const std::string& nset,
                         std::vector<std::unique_ptr<UPF_Ruleset>>& collect_set){

  saxpac::DecisionTree tree(*ruleset, ruleset->get_name() , nset);
  tree.create_tree(weighted_seg, 4);
  std::vector<std::unique_ptr<UPF_Ruleset>> temp_set;
  tree.collect_nodes(collect_set);
  manager.close_thread();
}

void UPF_Manager::hypersplit(const Hypersplit& opt, Statistic& stats){

      auto start = std::chrono::system_clock::now();
      using namespace dec_tree;
      std::vector<Statistic*> stat_vec;
      std::vector<std::unique_ptr<UPF_Ruleset>> temp_sets;
      std::vector<std::vector<std::unique_ptr<UPF_Ruleset>>*> collect_sets;
      stats.add_stat(Statistic::Entry("Total Rules Before",
                                      std::to_string(total_rules())));
      for(unsigned int i = 0; i < rulesets_.size();++i){
        std::string next_nt_set = rulesets_.back()->get_name();
        std::string next_set = rulesets_.back()->get_name();
        if(i < rulesets_.size() - 1)
          next_nt_set = rulesets_[i+1]->get_name();
        else
          next_nt_set = "FINAL_SET";
        if(rulesets_[i]->transformable()){
          std::vector<std::shared_ptr<UPF_Ruleset>> split_sets(
            RulesetSplitter::equal_split(*rulesets_[i], opt.block_size_));
          for(uint64_t j = 0; j < split_sets.size(); ++j){
            if (j < split_sets.size() - 1)
              next_set = split_sets[j+1]->get_name();
            else
              next_set = next_nt_set;
            std::vector<std::unique_ptr<UPF_Ruleset>>* collect_set_ptr =
              new std::vector<std::unique_ptr<UPF_Ruleset>>();
            collect_sets.push_back(collect_set_ptr);
            hypersplit_single(std::move(split_sets[j]),
                                opt, next_set,*collect_set_ptr);
          }
        }
        else{
          if(i < rulesets_.size() - 1){
            std::unique_ptr<UPF_Rule> new_jump_rule(new UPF_Rule());
            new_jump_rule->set_action(Jump(next_nt_set));
            rulesets_[i]->add_rule(std::move(new_jump_rule));
          }
          std::vector<std::unique_ptr<UPF_Ruleset>>* collect_set_ptr =
              new std::vector<std::unique_ptr<UPF_Ruleset>>();
          collect_set_ptr->push_back(std::move(rulesets_[i]));
          collect_sets.push_back(collect_set_ptr);

        }
      }
      for(auto& set_vec : collect_sets){
        for(auto iter = set_vec->begin(); iter != set_vec->end(); ++iter)
          temp_sets.push_back(std::move(*iter));
        delete(set_vec);
      }
      replace_rulesets(temp_sets);
    stats.add_stat(Statistic::Entry("Total Rules After",
                                    std::to_string(total_rules())));;
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
    std::cout << "Processing Time (in ms) " << elapsed.count() << std::endl;
    stats.add_stat(Statistic::Entry("Processing Time (in ms)", std::to_string(elapsed.count())));
}

void UPF_Manager::hypersplit_mt(const Hypersplit& opt, Statistic& stats){



      auto start = std::chrono::system_clock::now();
      using namespace dec_tree;
      std::vector<std::unique_ptr<std::thread>> threads;
      std::vector<std::unique_ptr<UPF_Ruleset>> temp_sets;
      std::vector<std::vector<std::unique_ptr<UPF_Ruleset>>*> collect_sets;
      stats.add_stat(Statistic::Entry("Total Rules Before",
                                    std::to_string(total_rules())));;
      for(unsigned int i = 0; i < rulesets_.size();++i){
        std::string next_nt_set = rulesets_.back()->get_name();
        std::string next_set = rulesets_.back()->get_name();
        if(i < rulesets_.size() - 1)
          next_nt_set = rulesets_[i+1]->get_name();
        else
          next_nt_set = "FINAL_SET";
        if(rulesets_[i]->transformable()){
          std::vector<std::shared_ptr<UPF_Ruleset>> split_sets(
            RulesetSplitter::equal_split(*rulesets_[i], opt.block_size_));
          for(uint64_t j = 0; j < split_sets.size(); ++j){
            if (j < split_sets.size() - 1)
              next_set = split_sets[j+1]->get_name();
            else
              next_set = next_nt_set;
            std::vector<std::unique_ptr<UPF_Ruleset>>* collect_set_ptr =
              new std::vector<std::unique_ptr<UPF_Ruleset>>();
            collect_sets.push_back(collect_set_ptr);
            if(thread_available()){
              open_thread();
              std::unique_ptr<std::thread> new_thread(new std::thread(
                &hypersplit_threaded, std::ref(*this), std::move(split_sets[j]),
                                opt, next_set, std::ref(*collect_set_ptr)));
              threads.push_back(std::move(new_thread));
            }
            else
              hypersplit_single(std::move(split_sets[j]),
                                opt, next_set,*collect_set_ptr);
          }
        }
        else{
          if(i < rulesets_.size() - 1){
            std::unique_ptr<UPF_Rule> new_jump_rule(new UPF_Rule());
            new_jump_rule->set_action(Jump(next_nt_set));
            rulesets_[i]->add_rule(std::move(new_jump_rule));
          }
          std::vector<std::unique_ptr<UPF_Ruleset>>* collect_set_ptr =
              new std::vector<std::unique_ptr<UPF_Ruleset>>();
          collect_set_ptr->push_back(std::move(rulesets_[i]));
          collect_sets.push_back(collect_set_ptr);
        }
      }
      for (auto& thread : threads)
        thread->join();
      for(auto& set_vec : collect_sets){
        for(auto iter = set_vec->begin(); iter != set_vec->end(); ++iter)
          temp_sets.push_back(std::move(*iter));
        delete(set_vec);
      }
      replace_rulesets(temp_sets);
    stats.add_stat(Statistic::Entry("Total Rules After",
                                    std::to_string(total_rules())));;
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
    std::cout << "Processing Time (in ms) " << elapsed.count() << std::endl;
    stats.add_stat(Statistic::Entry("Processing Time (in ms)", std::to_string(elapsed.count())));
}

void UPF_Manager::saxpac_hypersplit(const SAXPAC& opt, Statistic& stats){

      typedef std::vector<std::unique_ptr<UPF_Ruleset>>* Collect_Ptr;
      typedef std::vector<std::unique_ptr<UPF_Ruleset>> RulesetVec;
      typedef std::vector<std::shared_ptr<UPF_Ruleset>> SplitSetVec;

      std::vector<std::unique_ptr<std::thread>> threads;
      auto start = std::chrono::system_clock::now();
      RulesetVec temp_sets;
      SplitSetVec split_sets;
      std::vector<Collect_Ptr> collect_sets;
      stats.add_stat(Statistic::Entry("Total Rules Before",
                                    std::to_string(total_rules())));;
      for(unsigned int i = 0; i < rulesets_.size();++i){
        if(rulesets_[i]->transformable() && rulesets_[i]->size() > 1){
          SplitSetVec curr_sets(RulesetSplitter::saxpac_split(*rulesets_[i],
                                                              opt.threshold_));
          split_sets.insert(split_sets.end(),
                            std::make_move_iterator(curr_sets.begin()),
                            std::make_move_iterator(curr_sets.end()));
        }
        else{
          std::shared_ptr<UPF_Ruleset> new_ptr(new UPF_Ruleset(std::move(*rulesets_[i])));
          split_sets.push_back(std::move(new_ptr));
        }
      }
      for(uint64_t j = 0; j < split_sets.size() ; ++j){
        const std::string& curr_set_name = split_sets[j]->get_name();
        std::string next_set = split_sets.back()->get_name();
        if (j < split_sets.size() - 1)
          next_set = split_sets[j+1]->get_name();
        Collect_Ptr ptr(new RulesetVec());
        collect_sets.push_back(ptr);
        if(curr_set_name.find("FP") != std::string::npos ||
           curr_set_name.find("FR") != std::string::npos){
          for(uint64_t k = j; k < split_sets.size(); ++k){
            next_set = split_sets[k]->get_name();
            if(next_set.find("FR") == std::string::npos &&
               next_set.find("FP") == std::string::npos)
                break;
          }
        }
        if(curr_set_name.find("FR") != std::string::npos){
          saxpac_single(std::move(split_sets[j]), opt, next_set, *ptr);
        }
        else{
          if(j < split_sets.size() - 1){
              std::unique_ptr<UPF_Rule> new_jump_rule(new UPF_Rule());
              new_jump_rule->set_action(Jump(next_set));
              split_sets[j]->add_rule(std::move(new_jump_rule));
            }
          std::unique_ptr<UPF_Ruleset> new_ptr(new UPF_Ruleset(std::move(*split_sets[j])));
          ptr->push_back(std::move(new_ptr));
        }

      }
      for(auto& set_vec : collect_sets){
        for(auto iter = set_vec->begin(); iter != set_vec->end(); ++iter){
          temp_sets.push_back(std::move(*iter));
        }
        delete(set_vec);
      }
      replace_rulesets(temp_sets);
    stats.add_stat(Statistic::Entry("Total Rules After",
                                    std::to_string(total_rules())));;
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
    std::cout << "Processing Time (in ms) " << elapsed.count() << std::endl;
    stats.add_stat(Statistic::Entry("Processing Time (in ms)", std::to_string(elapsed.count())));
}


void UPF_Manager::saxpac_hypersplit_mt(const SAXPAC& opt, Statistic& stats){

      typedef std::vector<std::unique_ptr<UPF_Ruleset>>* Collect_Ptr;
      typedef std::vector<std::unique_ptr<UPF_Ruleset>> RulesetVec;
      typedef std::vector<std::shared_ptr<UPF_Ruleset>> SplitSetVec;
      auto start = std::chrono::system_clock::now();
      RulesetVec temp_sets;
      SplitSetVec split_sets;
      std::vector<Collect_Ptr> collect_sets;
      std::vector<std::unique_ptr<std::thread>> threads;
      stats.add_stat(Statistic::Entry("Total Rules Before",
                                    std::to_string(total_rules())));;
      for(unsigned int i = 0; i < rulesets_.size();++i){
        if(rulesets_[i]->transformable() && rulesets_[i]->size() > 1){
          SplitSetVec curr_sets(RulesetSplitter::saxpac_split(*rulesets_[i],
                                                              opt.threshold_));
          split_sets.insert(split_sets.end(),
                            std::make_move_iterator(curr_sets.begin()),
                            std::make_move_iterator(curr_sets.end()));
        }
        else{
          std::shared_ptr<UPF_Ruleset> new_ptr(new UPF_Ruleset(std::move(*rulesets_[i])));
          split_sets.push_back(std::move(new_ptr));
        }
      }
      for(uint64_t j = 0; j < split_sets.size() ; ++j){
        const std::string& curr_set_name = split_sets[j]->get_name();
        std::string next_set = split_sets.back()->get_name();
        if (j < split_sets.size() - 1)
          next_set = split_sets[j+1]->get_name();
        Collect_Ptr ptr(new RulesetVec());
        collect_sets.push_back(ptr);
        if(curr_set_name.find("FP") != std::string::npos ||
           curr_set_name.find("FR") != std::string::npos){
          for(uint64_t k = j; k < split_sets.size(); ++k){
            next_set = split_sets[k]->get_name();
            if(next_set.find("FR") == std::string::npos &&
               next_set.find("FP") == std::string::npos)
                break;
          }
        }
        if(curr_set_name.find("FR") != std::string::npos){
          if(thread_available()){
            open_thread();
            std::unique_ptr<std::thread> new_thread(new std::thread(
              &saxpac_threaded, std::ref(*this), std::move(split_sets[j]),
                                  opt, next_set, std::ref(*ptr)));
              threads.push_back(std::move(new_thread));
          }
          else
            saxpac_single(std::move(split_sets[j]), opt, next_set, *ptr);
        }
        else{
          if(j < split_sets.size() - 1){
              std::unique_ptr<UPF_Rule> new_jump_rule(new UPF_Rule());
              new_jump_rule->set_action(Jump(next_set));
              split_sets[j]->add_rule(std::move(new_jump_rule));
            }
          std::unique_ptr<UPF_Ruleset> new_ptr(new UPF_Ruleset(std::move(*split_sets[j])));
          ptr->push_back(std::move(new_ptr));
        }

      }
      for (auto& thread : threads)
        thread->join();
      for(auto& set_vec : collect_sets){
        for(auto iter = set_vec->begin(); iter != set_vec->end(); ++iter){
          temp_sets.push_back(std::move(*iter));
        }
        delete(set_vec);
      }
      replace_rulesets(temp_sets);
    stats.add_stat(Statistic::Entry("Total Rules After",
                                    std::to_string(total_rules())));;
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
    std::cout << "Processing Time (in ms) " << elapsed.count() << std::endl;
    stats.add_stat(Statistic::Entry("Processing Time (in ms)", std::to_string(elapsed.count())));
}

void UPF_Manager::saxpac(const SAXPAC& opt, Statistic& stats){

      typedef std::vector<std::unique_ptr<UPF_Ruleset>>* Collect_Ptr;
      typedef std::vector<std::unique_ptr<UPF_Ruleset>> RulesetVec;
      typedef std::vector<std::shared_ptr<UPF_Ruleset>> SplitSetVec;

      auto start = std::chrono::system_clock::now();

      RulesetVec temp_sets;
      std::vector<Collect_Ptr> collect_sets;
      SplitSetVec split_sets;
      stats.add_stat(Statistic::Entry("Total Rules Before",
                                    std::to_string(total_rules())));;
      for(unsigned int i = 0; i < rulesets_.size();++i){
        if(rulesets_[i]->transformable() && rulesets_[i]->size() > 1){
          SplitSetVec curr_sets(RulesetSplitter::saxpac_split(*rulesets_[i],
                                                            opt.threshold_));
          split_sets.insert(split_sets.end(),
                          std::make_move_iterator(curr_sets.begin()),
                          std::make_move_iterator(curr_sets.end()));
        }
        else{
          std::shared_ptr<UPF_Ruleset> new_ptr(new UPF_Ruleset(std::move(*rulesets_[i])));
          split_sets.push_back(std::move(new_ptr));
        }
      }
      for(uint64_t j = 0; j < split_sets.size(); ++j){
        const std::string& curr_set_name = split_sets[j]->get_name();
        std::string next_set = split_sets.back()->get_name();
        if (j < split_sets.size() - 1)
          next_set = split_sets[j+1]->get_name();
        Collect_Ptr collect_set_ptr(new RulesetVec());
        collect_sets.push_back(collect_set_ptr);
        if(curr_set_name.find("FP") != std::string::npos||
           curr_set_name.find("FR") != std::string::npos){
          for(uint64_t k = j; k < split_sets.size(); ++k){
            next_set = split_sets[k]->get_name();
            if(next_set.find("FR") == std::string::npos &&
               next_set.find("FP") == std::string::npos)
                break;
          }
        }
        if(j < split_sets.size() - 1){
          std::unique_ptr<UPF_Rule> new_jump_rule(new UPF_Rule());
          new_jump_rule->set_action(Jump(next_set));
          split_sets[j]->add_rule(std::move(new_jump_rule));
        }
        std::unique_ptr<UPF_Ruleset> new_ptr(new UPF_Ruleset(std::move(*split_sets[j])));
        collect_set_ptr->push_back(std::move(new_ptr));
      }
      for(auto& set_vec : collect_sets){
        for(auto iter = set_vec->begin(); iter != set_vec->end(); ++iter)
          temp_sets.push_back(std::move(*iter));
        delete(set_vec);
      }
      replace_rulesets(temp_sets);
    stats.add_stat(Statistic::Entry("Total Rules After",
                                    std::to_string(total_rules())));;
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
    std::cout << "Processing Time (in ms) " << elapsed.count() << std::endl;
    stats.add_stat(Statistic::Entry("Processing Time", std::to_string(elapsed.count())));
}

void fw_compress_threaded(UPF_Manager& manager,
                          UPF_Ruleset& ruleset,
                          UPF_Ruleset& merge_set){
  FW_Compressor fw;
  fw.compress_ruleset(ruleset);
  merge_set.append(ruleset);
  manager.close_thread();
}

void fw_compress_single(UPF_Ruleset& ruleset,
                        UPF_Ruleset& merge_set){
  FW_Compressor fw;
  fw.compress_ruleset(ruleset);
  merge_set.append(ruleset);

}

void UPF_Manager::fw_compress(const FirewallCompressor& opt, Statistic& stats){

  typedef std::vector<std::unique_ptr<UPF_Ruleset>> RulesetVec;
  typedef std::vector<std::shared_ptr<UPF_Ruleset>> SplitSetVec;
  auto start = std::chrono::system_clock::now();
  RulesetVec temp_sets;
  SplitSetVec split_sets;
  std::vector<UPF_Ruleset*> merge_sets;
  std::vector<std::unique_ptr<std::thread>> threads;
  stats.add_stat(Statistic::Entry("Total Rules Before",
                                  std::to_string(total_rules())));
  std::string set_name = rulesets_[0]->get_name();
  for(unsigned int i = 0; i < rulesets_.size(); ++i){
    if(rulesets_[i]->transformable() && rulesets_[i]->size() > 1){
      SplitSetVec curr_sets(RulesetSplitter::equal_split(*rulesets_[i],
                                                          opt.block_size_));
      split_sets.insert(split_sets.end(),
        std::make_move_iterator(curr_sets.begin()),
        std::make_move_iterator(curr_sets.end()));
    }
    else{
      std::shared_ptr<UPF_Ruleset> new_ptr(new UPF_Ruleset(std::move(*rulesets_[i])));
      split_sets.push_back(std::move(new_ptr));
    }
  }
  for(uint64_t j = 0; j < split_sets.size(); ++j){
    UPF_Ruleset* merge_set = new UPF_Ruleset();
    merge_sets.push_back(merge_set);
    if(thread_available()){
      open_thread();
        std::unique_ptr<std::thread> new_thread(new std::thread(
          &fw_compress_threaded,std::ref(*this),std::ref(*split_sets[j]),
                               std::ref(*merge_set)));
        threads.push_back(std::move(new_thread));
    }
    else
      fw_compress_single(*split_sets[j], *merge_set);
    }
    for (auto& thread : threads)
        thread->join();
    std::unique_ptr<UPF_Ruleset> merged_set(new UPF_Ruleset(set_name, freeset));
    for (auto merge_set : merge_sets)
      merged_set->append(*merge_set);
    temp_sets.push_back(std::move(merged_set));

    replace_rulesets(temp_sets);
    stats.add_stat(Statistic::Entry("Total Rules After",
                                    std::to_string(total_rules())));;
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
    std::cout << "Processing Time (in ms) " << elapsed.count() << std::endl;
    stats.add_stat(Statistic::Entry("Processing Time", std::to_string(elapsed.count())));
}

void UPF_Manager::simple_redundancy_removal(const Simple_Redundancy& opt, Statistic& stats){

  std::vector<std::unique_ptr<UPF_Ruleset>> temp_sets;
  stats.add_stat(Statistic::Entry("Total Rules Before",
                                  std::to_string(total_rules())));
  auto start = std::chrono::system_clock::now();
  for(unsigned int i = 0; i < rulesets_.size(); ++i){

    std::vector<std::shared_ptr<UPF_Ruleset>> split_sets(
      RulesetSplitter::equal_split(*rulesets_[i], opt.block_size_));
    for(unsigned int j = 0; j < split_sets.size(); ++j){
      split_sets[j]->remove_redundancy();
      temp_sets.push_back(std::unique_ptr<UPF_Ruleset>(
        new UPF_Ruleset(std::move(*split_sets[j]))));
    }
  }
  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << "Processing Time (in ms) " << elapsed.count() << std::endl;
  replace_rulesets(temp_sets);
  stats.add_stat(Statistic::Entry("Total Rules After",
                                      std::to_string(total_rules())));
  stats.add_stat(Statistic::Entry("Processing Time", std::to_string(elapsed.count())));

}

void UPF_Manager::simple_redundancy_removal_mt(const Simple_Redundancy& opt, Statistic& stats){

  std::vector<std::unique_ptr<UPF_Ruleset>> temp_sets;
  stats.add_stat(Statistic::Entry("Total Rules Before",
                                  std::to_string(total_rules())));
  auto start = std::chrono::system_clock::now();
  for(unsigned int i = 0; i < rulesets_.size(); ++i){
     std::vector<std::shared_ptr<UPF_Ruleset>> split_sets(
      RulesetSplitter::equal_split(*rulesets_[i], opt.block_size_));
    for(unsigned int j = 0; j < split_sets.size(); ++j){
      split_sets[j]->remove_redundancy_mt();
      temp_sets.push_back(std::unique_ptr<UPF_Ruleset>(
        new UPF_Ruleset(std::move(*split_sets[j]))));
    }
    }
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
    std::cout << "Processing Time (in ms) " << elapsed.count() << std::endl;
    replace_rulesets(temp_sets);
    stats.add_stat(Statistic::Entry("Total Rules After",
                                      std::to_string(total_rules())));
    stats.add_stat(Statistic::Entry("Processing Time", std::to_string(elapsed.count())));
}

bool operator<(const SAXPAC_Block& lhs, const SAXPAC_Block& rhs){
    return lhs.start_index < rhs.start_index;
}
