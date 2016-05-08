#ifndef UPF_MANAGER_H
#define UPF_MANAGER_H

#include <fstream>
#include "UPF.h"
#include "Parser.h"
#include "Tokenizer.h"
#include "RuleGenerator.h"
#include "UPF_ruleset.h"

//#include "fw_compressor.h"

class Optimization{
public:
  Optimization(const OptimizeType& opt): opt_type(opt){}
  Optimization(const OptimizeType& opt,
               const uint64_t block_size = std::numeric_limits<uint64_t>::max(),
               const bool complete_transform = false,
               const bool threading = false)
    : opt_type(opt),
      block_size_(block_size),
      complete_transform_(complete_transform),
      threading_(threading){}
  virtual ~Optimization(){};

  OptimizeType opt_type;
  uint64_t block_size_;
  bool complete_transform_;
  bool threading_;
};

class Simple_Redundancy : public Optimization{
 public:
  Simple_Redundancy(const uint64_t block_size, bool complete,
                    const bool threading)
   :  Optimization(simple_redundancy, block_size,complete, threading){}
};

class FDD_Redundancy : public Optimization{
public:
  FDD_Redundancy(const uint64_t block_size, bool complete,
                 const bool threading)
    : Optimization(fdd_redundancy,block_size, complete, threading){}
};

class Disjunction : public Optimization{
public:
  Disjunction(const uint64_t block_size, bool complete,
              const bool threading)
    : Optimization(disjunct, block_size, complete, threading){}
};

class Hypersplit : public Optimization{
public:
  Hypersplit(const CuttingType cut, const unsigned int binth,
             const uint64_t block_size, const bool complete,
             const bool threading, const bool use_inline)
    : Optimization(hypersplit, block_size, complete, threading),
      cutting_type_(cut),
      binth_(binth),
      use_inline_(use_inline){}

  CuttingType cutting_type_;
  unsigned int binth_;
  bool use_inline_;
};

class FirewallCompressor : public Optimization{
public:
  FirewallCompressor(const uint64_t block_size,
                     const bool complete,
                     const bool threading)
    : Optimization(fw_compressor, block_size, complete, threading){}
};


class SAXPAC : public Optimization{
public:
  virtual OptimizeType get_opt_type() {return opt_type;}
  SAXPAC(const uint64_t threshold, const uint64_t binth, bool use_hypersplit,
         const bool complete, const bool threading,
         const uint64_t block_size = std::numeric_limits<uint64_t>::max())
  : Optimization(sax_pac, block_size, complete, threading),
    threshold_(threshold),
    binth_(binth),
    hypersplit_(use_hypersplit){}

  uint64_t threshold_;
  uint64_t binth_;
  bool hypersplit_;
};

class RulesetSplitter{
public:
  static std::vector<std::shared_ptr<UPF_Ruleset>> saxpac_split(
                            const UPF_Ruleset& ruleset, const uint64_t threshold);
  static std::vector<std::shared_ptr<UPF_Ruleset>> equal_split(
                            UPF_Ruleset& ruleset, const uint64_t split_number);
};

class SAXPAC_Block{
public:
  uint64_t start_index;
  uint64_t end_index;
  DimensionType field_key;
  SAXPAC_Block(uint64_t start, uint64_t end, DimensionType field_key)
  : start_index(start), end_index(end), field_key(field_key){}

  friend std::ostream& operator<<(std::ostream& out, const SAXPAC_Block& block);
  inline bool operator<(const SAXPAC_Block& rhs){
    return this->start_index < rhs.start_index;
  }
};


class UPF_Manager{



public:
    void add_ruleset_from_file(const std::string& filepath, const EngineType type);
    void optimize(const std::vector<std::unique_ptr<Optimization>>& opt,
                  Statistic& stats);
    void export_set_to_file(const std::string& filepath,
                            const EngineType type,
                            const bool cross_translate,
                            const bool enable_log) const;
    void replace_rulesets(std::vector<std::unique_ptr<UPF_Ruleset>>& rhs);
    void append_rulesets(std::vector<std::unique_ptr<UPF_Ruleset>>& rhs);
    void append_ruleset(UPF_Ruleset& ruleset);
    void dump_ruleset(const std::string& filepath) const;
    uint64_t total_rules() const;
    void open_thread();
    void close_thread();
    bool thread_available() const;
    uint64_t get_threads() const;
    UPF_Manager();
    UPF_Manager(bool multithreading, uint16_t cores);
private:
    void optimize_simple(const Simple_Redundancy& opt, Statistic& stats);
    void optimize_fdd(const FDD_Redundancy& opt, Statistic& stats);
    void optimize_fdd_mt(const FDD_Redundancy& opt, Statistic& stats);
    void optimize_hypersplit(const Hypersplit& opt, Statistic& stats);
    void optimize_saxpac(const SAXPAC& opt, Statistic& stats);
    void optimize_fw_compressor(const FirewallCompressor& opt, Statistic& stats);
    void simple_redundancy_removal(const Simple_Redundancy& opt, Statistic& stats);
    void simple_redundancy_removal_mt(const Simple_Redundancy& opt, Statistic& stats);
    void fw_compress(const FirewallCompressor& opt, Statistic& stats);
    void hypersplit(const Hypersplit& opt, Statistic& stats);
    void hypersplit_mt(const Hypersplit& opt, Statistic& stats);
    void saxpac_hypersplit(const SAXPAC& opt, Statistic& stats);
    void saxpac_hypersplit_mt(const SAXPAC& opt, Statistic& stats);
    void saxpac(const SAXPAC& opt, Statistic& stats);
    void saxpac_mt(const SAXPAC& opt, Statistic& stats);
    void split_transformable();

    uint64_t current_threads_;
    std::vector<std::unique_ptr<UPF_Ruleset>> rulesets_;
    bool multithreading_;
    uint16_t cores_;
 };

#endif // UPF_MANAGER_H
