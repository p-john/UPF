#ifndef UPF_RULESET_H_INCLUDED
#define UPF_RULESET_H_INCLUDED

#include <fstream>
#include "UPF.h"
#include "statistics.h"

// UPF_Ruleset


class UPF_Ruleset{

    class PositionVector{
    public:
      std::vector<unsigned int> positions;
      std::mutex mtx;
      void add(unsigned int x){positions.push_back(x);}
    };

public:
    UPF_Ruleset& operator=(UPF_Ruleset& other){
       ruleset_ = std::move(other.ruleset_);
       return *this;
    }

    void add_rule(std::unique_ptr<UPF_Rule> rule);
    UPF_Rule& get_rule(unsigned int position) const;
    void insert_rule(std::unique_ptr<UPF_Rule> rule, uint64_t position);
    std::unique_ptr<UPF_Rule> draw_rule(const unsigned int position);
    std::shared_ptr<UPF_Rule> get_upf_rule(unsigned int position) const;

    void split_and_remove_redundancy();
    void split_and_remove_redundancy_mt(Statistic& stats);
    void remove_redundancy();
    void remove_redundancy_mt();
    void remove_backward_thread(unsigned int thread_id,
                                  unsigned int threads,
                                  PositionVector& pos);
    void remove_forward_thread(unsigned int thread_id,
                                  unsigned int threads,
                                  PositionVector& pos);
    void append(std::shared_ptr<UPF_Ruleset> ruleset);
    void append(std::vector<UPF_Rule*>& rules);
    void append(UPF_Ruleset& ruleset);
    void replace_ruleset(std::vector<std::unique_ptr<UPF_Rule>>& rhs);
    unsigned int cut_intersections(const DimensionType dim, const uint128_t pos) const;
    void print() const;
    std::ostream& print(std::ostream& out) const;
    bool empty() const;
    void clear();
    unsigned long size() const;
    UPF_setType get_type() const;
    void set_type(UPF_setType);
    void set_name(std::string name);
    std::string get_name() const;
    bool transformable(){return transformable_;}
    void set_transformable(bool transformable){transformable_ = transformable;}
    UPF_Ruleset();
    UPF_Ruleset(const std::string name, const UPF_setType type);
    UPF_Ruleset(UPF_Ruleset&& rhs);

    friend std::ostream& operator<<(std::ostream& out, const UPF_Ruleset& ruleset);

private:
    std::string name_;
    UPF_setType type_;
    std::vector<std::unique_ptr<UPF_Rule>> ruleset_;
    bool transformable_;
};

#endif // UPF_RULESET_H_INCLUDED
