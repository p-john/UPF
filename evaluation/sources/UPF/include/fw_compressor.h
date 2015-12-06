#ifndef FW_COMPRESSOR_H_INCLUDED
#define FW_COMPRESSOR_H_INCLUDED
#include "Field.h"
#include "fdd.h"

class Segment{
public:

  void set_level(const uint64_t level);
  uint64_t get_level() const;
  Node* get_target() const;
  const Range& get_range() const;
  void merge(const Segment& rhs);
  Segment(const Range& range, Node* target_);
  Segment(const Range& range, Node* target_, const uint64_t level);

  friend std::ostream& operator<<(std::ostream& out, const Segment& rhs);


private:
  Range range_;
  Node* target_;
  uint64_t level_;

};

class SegmentSorter{
public:
  void add_segment(const Segment& segment);
  FieldType get_fieldkey() const;
  void find_minimal_order();
  const std::vector<Segment>& get_segments() const;
  Segment& find_lowest_seg_level(Node* target, uint64_t minimum_level);
  SegmentSorter(const FieldType fieldkey);
  SegmentSorter(const std::vector<Edge*>& edges,const FieldType fieldkey);

  friend std::ostream& operator<<(std::ostream& out, const SegmentSorter& rhs);
private:
  std::vector<Segment> segments_;
  FieldType field_key_;
  SegmentSorter* next_sorter_;

};


class FW_Compressor{
public:

//  UPF_Ruleset* compress_ruleset(const UPF_Ruleset& ruleset);
    void compress_ruleset(UPF_Ruleset& ruleset);


private:
  void find_ordering(Node* node);
  void reduce_fdd(Tree& fdd_tree);


};


#endif // FW_COMPRESSOR_H_INCLUDED
