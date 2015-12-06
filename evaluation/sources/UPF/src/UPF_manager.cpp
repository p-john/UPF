#include "UPF_manager.h"
#include "dec_tree.h"
#include "graph.h"
#include "fdd.h"
#include "saxpac.h"
#include "fw_compressor.h"
#include "UPF_exporter.h"
#include <thread>



#include <cstdio>
#include <chrono>






// UPF_Manager -----------------------------------------------------------------
UPF_Manager::UPF_Manager()
 :  multithreading_(false),
    cores_(1){}

UPF_Manager::UPF_Manager(bool multithreading, uint16_t cores)
  : multithreading_(multithreading),
    cores_(cores < std::thread::hardware_concurrency() ?
           cores : std::thread::hardware_concurrency()){}


std::vector<std::unique_ptr<UPF_Ruleset>> add_iptables_file(
  const std::string& filepath){

    IPTablesParser parser;
    RuleGenerator generator;
    std::ifstream myfile;
    myfile.open(filepath.c_str());
    std::string str;
    std::vector<std::unique_ptr<UPF_Ruleset>> rulesets;
    bool filter_reached = false;
    while (std::getline(myfile, str)){
      std::stringstream ss(str);
      // if comment, skip line
      if (str == "*filter")
        filter_reached = true;
      if(filter_reached){
        // collect Chains
        if(str[0] == ':'){
          std::string chain;
          ss >> chain;
          std::string current_ruleset = chain.substr(1,chain.size() -1);
          if(current_ruleset == "INPUT")
            continue;
          std::unique_ptr<UPF_Ruleset> ruleset (
            new UPF_Ruleset(current_ruleset,freeset));
          ruleset->set_transformable(true);
          rulesets.push_back(std::move(ruleset));
          std::cout << current_ruleset << std::endl;
          continue;
        }
        else{
        std::vector<Token> result_tokens;
        parser.parse(str,result_tokens);
        std::string set_name_of_rule = result_tokens[14].value_;
//        std::cout << "SET NAME OF RULE: " << set_name_of_rule << std::endl;
        for(unsigned int i = 0; i < rulesets.size(); ++i){
          if(set_name_of_rule == rulesets[i]->get_name()){
            rulesets[i]->add_rule(std::unique_ptr<UPF_Rule>(generator.generate(
                                                            result_tokens)));
            break;
          }
        }
        }
      }
      if(str == "COMMIT")
        filter_reached = false;
    }
//        for(unsigned int i = 0; i < rulesets.size(); ++i){
////          std::cout << rulesets[i]->size();
////          rulesets[i]->print();
//        }

  return std::move(rulesets);
}

std::vector<std::unique_ptr<UPF_Ruleset>> add_ipfw_file(
  const std::string& filepath){

  ipfwParser parser;
  RuleGenerator generator;
  std::ifstream myfile;
  myfile.open(filepath.c_str());
  std::string str;
  std::vector<std::unique_ptr<UPF_Ruleset>> rulesets;
  std::unique_ptr<UPF_Ruleset> ruleset (new UPF_Ruleset("IPFW",freeset));
  ruleset->set_transformable(true);
  rulesets.push_back(std::move(ruleset));
  while (std::getline(myfile, str)){
    // if comment, skip line
    // collect sets
    if(str[0] != '#'){
      std::vector<Token> result_tokens;
      parser.parse(str,result_tokens);
      rulesets[0]->add_rule(std::unique_ptr<UPF_Rule>(generator.generate(
                                                      result_tokens)));
    }
  }
  return std::move(rulesets);
}

std::vector<std::unique_ptr<UPF_Ruleset>> add_pf_file(
  const std::string& filepath){

  pfParser parser;
  RuleGenerator generator;
  std::ifstream myfile;
  myfile.open(filepath.c_str());
  std::string str;
  std::vector<std::unique_ptr<UPF_Ruleset>> rulesets;
  std::stringstream normal_ss;
  parser.normalize(myfile,normal_ss);
  std::unique_ptr<UPF_Ruleset> ruleset (new UPF_Ruleset("PF",freeset));
  ruleset->set_transformable(true);
  rulesets.push_back(std::move(ruleset));
  while (std::getline(normal_ss, str)){
    // if comment, skip line
    // collect sets
    if(str[0] != '#'){
      std::vector<Token> result_tokens;
      parser.parse(str,result_tokens);
      rulesets[0]->add_rule(std::unique_ptr<UPF_Rule>(generator.generate(
                                                      result_tokens)));
    }
  }
  return std::move(rulesets);
}

std::vector<std::unique_ptr<UPF_Ruleset>> add_cb_file(
  const std::string& filepath){

  cbParser parser;
  RuleGenerator generator;
  std::vector<std::unique_ptr<UPF_Ruleset>> rulesets;
  std::ifstream file;
  file.open(filepath.c_str());
  std::string str;
  std::unique_ptr<UPF_Ruleset> ruleset (new UPF_Ruleset("CB",freeset));
  ruleset->set_transformable(true);
  rulesets.push_back(std::move(ruleset));
  while(std::getline(file,str)){
    std::vector<Token> result_tokens;
    parser.parse(str,result_tokens);
    rulesets[0]->add_rule(std::unique_ptr<UPF_Rule>(generator.generate(
                                                result_tokens)));
  }
  std::unique_ptr<UPF_Ruleset> new_set(new UPF_Ruleset("FINAL", freeset));
  new_set->set_transformable(true);
  std::unique_ptr<UPF_Rule> new_rule(new UPF_Rule());
  new_rule->set_action(ACCEPT);
  new_set->add_rule(std::move(new_rule));
  rulesets.push_back(std::move(new_set));
  return std::move(rulesets);
}

void UPF_Manager::add_ruleset_from_file(const std::string& filepath,
                                        const EngineType type){

    typedef std::vector<std::unique_ptr<UPF_Ruleset>> RuleVec;

    switch(type){
    case IPTables:{
      RuleVec rulesets = add_iptables_file(filepath);
      append_rulesets(rulesets); break;
    }
    case ipfw:{
      RuleVec rulesets = add_ipfw_file(filepath);
      append_rulesets(rulesets);break;
    }
     case pf:{
      RuleVec rulesets = add_pf_file(filepath);
      append_rulesets(rulesets);break;
    }
    case cb:{
      RuleVec rulesets = add_cb_file(filepath);
      append_rulesets(rulesets); break;
    }
    default:
      break;
  }
}



void UPF_Manager::split_transformable(){

  std::vector<std::unique_ptr<UPF_Ruleset>> rulesets;
  std::vector<std::unique_ptr<UPF_Ruleset>> temp_sets;
  for(auto iter = rulesets_.begin(); iter != rulesets_.end();++iter){
    UPF_Ruleset& ruleset_ = **iter;
    const std::string& rset_name = ruleset_.get_name();
    std::unique_ptr<UPF_Ruleset> new_set(new UPF_Ruleset(rset_name,freeset));
    bool transformable_mode = false;
    uint64_t transformable_counter = 1;
    uint64_t non_transformable_counter = 1;
    for (unsigned int i = 0; i < ruleset_.size();  ++i){
        const UPF_Rule& curr_rule = ruleset_.get_rule(i);
        if(!transformable_mode){
            ProtocolType proto = curr_rule.get_field(l4protocol)->get_protocol();
            if((proto != TRANSPORT)){
              if(new_set->size() > 0){
                rulesets.push_back(std::move(new_set));
              }
              std::string new_name = rset_name + "_T_" + std::to_string(transformable_counter);
              new_set = std::unique_ptr<UPF_Ruleset>(new UPF_Ruleset(new_name,freeset));
              new_set->set_transformable(true);
              transformable_mode = true;
              transformable_counter++;
//              new_set->clear();
              new_set->add_rule(std::move(ruleset_.draw_rule(i)));
            }
            else
              new_set->add_rule(std::move(ruleset_.draw_rule(i)));
        }
        else if(transformable_mode){
            ProtocolType proto = curr_rule.get_field(l4protocol)->get_protocol();
            if(proto != TRANSPORT)
              new_set->add_rule(std::move(ruleset_.draw_rule(i)));
            else{
              if(new_set->size() > 0){
                  rulesets.push_back(std::move(new_set));
              }

              std::string new_name = rset_name + "_NT_" + std::to_string(non_transformable_counter);
              new_set = std::unique_ptr<UPF_Ruleset>(new UPF_Ruleset(new_name,freeset));
              new_set->set_transformable(false);
              transformable_mode = false;
              non_transformable_counter++;
//              new_set->clear();
             new_set->add_rule(std::move(ruleset_.draw_rule(i)));
            }
        }
    }
    rulesets.push_back(std::move(new_set));
  }
  replace_rulesets(rulesets);
}


void UPF_Manager::optimize_simple(const Simple_Redundancy& opt, Statistic& stats){

    if(!opt.complete_transform_)
      split_transformable();

      if(opt.threading_)
        simple_redundancy_removal_mt(opt,stats);
      else
        simple_redundancy_removal(opt,stats);
}

void UPF_Manager::optimize_fw_compressor(const FirewallCompressor& opt,
                                         Statistic& stats){

      if(!opt.complete_transform_)
        split_transformable();

      fw_compress(opt,stats);


  }

void UPF_Manager::optimize_hypersplit(const Hypersplit& opt, Statistic& stats){

    if(!opt.complete_transform_)
      split_transformable();

    if(opt.threading_)
      hypersplit_mt(opt, stats);
    else
      hypersplit(opt,stats);

}

void UPF_Manager::optimize_saxpac(const SAXPAC& opt, Statistic& stats){

    if(!opt.complete_transform_)
      split_transformable();

    if(opt.threading_){
      if(opt.hypersplit_)
        saxpac_hypersplit_mt(opt,stats);
      else
        saxpac(opt,stats);
    }
    else{
      if(opt.hypersplit_)
        saxpac_hypersplit(opt,stats);
      else
        saxpac(opt,stats);
    }
}



void UPF_Manager::optimize(const std::vector<std::unique_ptr<Optimization>>& opts,
                           Statistic& stats){

  stats.add_stat(Statistic::Entry("Threads: ", std::to_string(cores_)));

  for(const auto& opt : opts){

    switch(opt->opt_type){
    case no_opt: break;
    case simple_redundancy:{
        std::cout << "Optimization : Simple Redundancy started" << std::endl;
        Simple_Redundancy* opti = static_cast<Simple_Redundancy*>(opt.get());
        if(opti->block_size_ < std::numeric_limits<uint64_t>::max())
          std::cout << "Block Size =  " << opti->block_size_ <<  std::endl;
        if(opti->threading_)
          std::cout << "Using " << cores_ << " Threads" << std::endl;
        if (opti->complete_transform_)
          std::cout << "Complete Transformation (potentially very expensive!!!)" << std::endl;
        optimize_simple(*opti,stats);
        break;
      }
    case fdd_redundancy:{
      std::cout << "Optimization : FDD Redundancy started" << std::endl;
      FDD_Redundancy* opti = static_cast<FDD_Redundancy*>(opt.get());
      if(opti->block_size_ < std::numeric_limits<uint64_t>::max())
        std::cout << "Block Size =  " << opti->block_size_ <<  std::endl;
      if(opti->threading_)
          std::cout << "Using " << cores_ << " Threads" << std::endl;
      if (opti->complete_transform_)
          std::cout << "Complete Transformation (potentially very expensive!!!)" << std::endl;
      if(opti->threading_)
        optimize_fdd_mt(*opti, stats);
      else
        optimize_fdd(*opti, stats);
      break;
      }
    case fw_compressor:{
      std::cout << "Optimization : Firewall Compressor started" << std::endl;
      FirewallCompressor* opti = static_cast<FirewallCompressor*>(opt.get());
      if(opti->block_size_ < std::numeric_limits<uint64_t>::max())
        std::cout << "Block Size =  " << opti->block_size_ <<  std::endl;
      if(opti->threading_)
        std::cout << "Using " << cores_ << " Threads" << std::endl;
      if (opti->complete_transform_)
        std::cout << "Complete Transformation (potentially very expensive!!!)" << std::endl;
      optimize_fw_compressor(*opti, stats);
      break;
    }
    case disjunct:{
      std::cout << "Optimization : Disjunction started" << std::endl;
      std::vector<std::unique_ptr<UPF_Ruleset>> collection;
      for(unsigned int i = 0; i < rulesets_.size(); ++i){
        FDD fdd;
        std::unique_ptr<UPF_Ruleset> result(new UPF_Ruleset());
        fdd.create_eff_sets(*rulesets_[i], *result);
        collection.push_back(std::move(result));
      }
      replace_rulesets(collection);
      break;
    }
    case OptimizeType::hypersplit:{
      Hypersplit* opti = static_cast<Hypersplit*>(opt.get());
      std::cout << "Optimization : Hypersplit" << std::endl;
      if(opti->threading_)
        std::cout << "Using " << cores_ << " Threads" << std::endl;
      if(opti->block_size_ < std::numeric_limits<uint64_t>::max())
        std::cout << "Block Size =  " << opti->block_size_ <<  std::endl <<
      "Cutting Type = " << cuttingtype_to_string(opti->cutting_type_) << std::endl <<
      "Binth = " << opti->binth_ << std::endl;
      if (opti->complete_transform_)
        std::cout << "Complete Transformation (potentially very expensive!!!)" << std::endl;
      optimize_hypersplit(*opti, stats);
      break;
    }
    case sax_pac:{
      SAXPAC* opti = static_cast<SAXPAC*>(opt.get());
      std::cout << "Optimization : SAXPAC" << std::endl;
      std::cout << "Threshold " << opti->threshold_ << std::endl;
      if(opti->block_size_ < std::numeric_limits<uint64_t>::max())
        std::cout << "Block Size =  " << opti->block_size_ <<  std::endl;
      if(opti->threading_)
        std::cout << "Using " << cores_ << " Threads" << std::endl;
      if(opti->hypersplit_)
        std::cout << "Hypersplit partioning of SAXPAC Blocks" << std::endl;
      if (opti->complete_transform_)
        std::cout << "Complete Transformation (potentially very expensive!!!)" << std::endl;
      optimize_saxpac(*opti, stats);
      break;
    }
    }
  }
  std::cout << "Optimization Done" << std::endl;
}



void UPF_Manager::export_set_to_file(const std::string& filepath,
                                     const EngineType type,
                                     const bool cross_translate,
                                     bool enable_log) const{

    if(type == IPTables){
        IPTables_Exporter exporter;
        exporter.upf_export(rulesets_,filepath,cross_translate, enable_log);
    }
    else if(type == ipfw){
        ipfw_Exporter exporter;
        exporter.upf_export(rulesets_,filepath,cross_translate, enable_log);
    }
    else if(type == pf){
        pf_Exporter exporter;
        exporter.upf_export(rulesets_,filepath,cross_translate, enable_log);
    }
    else if(type == ipf){
        ipf_Exporter exporter;
        exporter.upf_export(rulesets_,filepath,cross_translate, enable_log);
    }
    else if(type == dump){
      dump_ruleset(filepath);
    }
}

void UPF_Manager::replace_rulesets(std::vector<std::unique_ptr<UPF_Ruleset>>& rhs){

  rulesets_.clear();
  for(unsigned int i = 0; i < rhs.size();++i)
    rulesets_.push_back(std::move(rhs[i]));
}

void UPF_Manager::append_rulesets(std::vector<std::unique_ptr<UPF_Ruleset>>& rhs){

  for(unsigned int i = 0; i < rhs.size();++i)
    rulesets_.push_back(std::move(rhs[i]));
}

void UPF_Manager::dump_ruleset(const std::string& filepath) const{

  std::stringstream out;
  std::ofstream file;
  file.open(filepath);
  for(auto iter = rulesets_.begin(); iter != rulesets_.end(); ++iter){
    out << (*iter)->get_name() << std::endl;
    out << "{" << std::endl;
    out << **iter;
    out << "}" << std::endl;

  }
  file << out.str();
  file.close();
}

uint64_t UPF_Manager::total_rules()const {

  uint64_t count = 0;
  for(auto& ruleset : rulesets_)
    count += ruleset->size();

  return count;
}
uint64_t UPF_Manager::get_threads() const{
  return current_threads_;
}

void UPF_Manager::open_thread(){
  ++current_threads_;
}

void UPF_Manager::close_thread(){
  --current_threads_;
}

bool UPF_Manager::thread_available() const{
  return multithreading_ && current_threads_ < cores_;
}


std::ostream& operator<<(std::ostream& out, const SAXPAC_Block& block){

  out << "SAXPAC_Block: [" + std::to_string(block.start_index) + " - " +
  std::to_string(block.end_index) + "]" <<  std::endl;
  return out;
}

/* For Compatibility issues */

//bool operator<(const SAXPAC_Block& lhs, const SAXPAC_Block& rhs){
//    return lhs.start_index < rhs.start_index;
//}

