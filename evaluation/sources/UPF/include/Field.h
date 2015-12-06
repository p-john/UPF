#ifndef FIELD_H
#define FIELD_H

#include <string>
#include <cstdint>
#include <vector>
#include <bitset>
#include <iostream>
#include <sstream>
#include <functional>
#include "types.h"
//#include "uint128_t.h"

class uint128_t{
public:
  uint128_t();
  uint128_t(uint64_t value);
  void downshift(unsigned int shift);
  void upshift(unsigned int shift);
  void divide_by_two();

  inline bool operator<(const uint128_t& rhs) const{
    return(high_ < rhs.high_ || ((high_ == rhs.high_) && (low_ < rhs.low_)));
  }

  inline bool operator>(const uint128_t& rhs) const{
    return(high_ > rhs.high_ || ((high_ == rhs.high_) && (low_ > rhs.low_)));
  }

  inline bool operator==(const uint128_t& rhs) const{
    return(low_ == rhs.low_ && high_ == rhs.high_);
  }
  inline bool operator!=(const uint128_t& rhs) const{
    return!(*this == rhs);
  }

  inline bool operator<=(const uint128_t& rhs) const{
    return((*this < rhs) || (*this == rhs));
  }

  inline bool operator>=(const uint128_t& rhs) const{
    return((*this > rhs) || (*this == rhs));
  }

  uint128_t operator+(const uint128_t& rhs) const;
  uint128_t operator-(const uint128_t& rhs) const;
  uint64_t get_low() const;
  uint64_t get_high() const;
  static uint128_t max();
  static uint128_t min();
  std::string print() const;

  friend std::ostream& operator<<(std::ostream& out, const uint128_t& num);


private:
  uint64_t low_;
  uint64_t high_;
};


/* Range Declarations ------------------------------------------------------- */

template<typename T> class Range_CRTP{
  friend bool operator==(T const &a, T const &b) { return  a.equal_to (b); }
  friend bool operator!=(T const &a, T const &b) { return  !a.equal_to (b); }
//  friend bool covers(T const &a, T const &b) { return  a.covers(b); }
public:
  virtual ~Range_CRTP(){}
};

class Range : public Range_CRTP<Range>{
public:
  uint64_t hash();
  Range& operator=(const Range& rhs);
  bool equal_to(const Range& rhs) const;
  bool covers(const Range& rhs) const;
  bool intersect(const Range& rhs) const;
  bool border(const Range& rhs) const;
  void merge(const Range& rhs);
  std::vector<Range> diff(const Range& rhs) const;
  Range intersection(const Range& rhs) const;
  bool is_wildcard() const;
  void print() const;
  Range();
  Range(const uint128_t& lower, const uint128_t& upper);
  uint128_t lower_;
  uint128_t upper_;

  friend std::ostream& operator<<(std::ostream& out, const Range& range);
};


//template<typename T> class Range{
//
//public:
//  template<typename K>
//  inline Range<T>& operator=(const Range<K>& rhs) const{
//    if(this != &rhs){
//      lower_ = rhs.lower_;
//      upper_ = rhs.upper_;
//    }
//    return *this;
//  }
//  template<typename K>
//  inline bool operator==(const Range<K>& rhs) const{
//    return ((lower_ == rhs.lower_) && (upper_ == rhs.upper_));
//  }
//  template<typename K>
//  inline bool operator!=(const Range<K>& rhs) const{
//    return (!(*this == rhs));
//  }
//  template<typename K>
//  inline bool covers(const Range<K>& rhs)const{
//    return (lower_ <= rhs.lower_ && rhs.upper_ <= upper_);
//  }
//  template<typename K>
//  inline bool intersect(const Range<K>& rhs) const{
//    if(*this == rhs)
//      return true;
//    else
//      return !((upper_ < rhs.lower_ && lower_ < rhs.upper_) ||
//              (lower_ > rhs.upper_ && upper_ > rhs.lower_));
//  }
//  template<typename K>
//  inline operator Range<T>(){ return Range<T>(*this);}
//
//  std::vector<Range<T>> diff(const Range<T>& rhs) const;
//  Range<T> intersection(const Range<T>& rhs) const;
//  Range<T>();
//
//  template<typename K>
//  Range(const K& lower, const K& upper)
//    : lower_(lower), upper_(upper){
//      if (lower_ > upper_)
//        std::swap(lower_, upper_);
//    }
//  template <typename K>
//  friend std::ostream& operator<<(std::ostream& out, const Range<K>& range);
//
//  T lower_;
//  T upper_;
//
//};


/* Field CRTP-----------------------------------------------------------------*/

template<typename T> class Field_CRTP {
  friend bool operator==(T const &a, T const &b) { return  a.equal_to (b); }
  friend bool operator!=(T const &a, T const &b) { return  !a.equal_to (b); }
//  friend bool covers(T const &a, T const &b) { return  a.covers(b); }
public:
  virtual ~Field_CRTP(){}
};

class Cloneable {
public:
  virtual Cloneable* clone() const = 0;
  virtual ~Cloneable(){}
};

class Field : public Cloneable{
public:
  virtual bool covers(Field* field) const = 0;
  virtual bool intersect(Field* field) const = 0;
  virtual FieldType get_type() const = 0;
  virtual ProtocolType get_protocol() const = 0;
  virtual void set_range(const Range& range) = 0;
  virtual const Range& get_range() const = 0;
  virtual void subtract(std::vector<Range>& ranges_union,
                          std::vector<Range>& intervals)const = 0;
  virtual ~Field(){};

  bool negated = false;
};

/* Layer 4 Fields ------------------------------------------------------------*/

class L4Field : public Field, public Field_CRTP<L4Field>{
public:

     virtual ~L4Field(){}
};

class TCPFlagField : public L4Field, public Field_CRTP <TCPFlagField>{
public:
  TCPFlagField* clone() const;
  bool equal_to(const TCPFlagField& rhs) const;
  bool covers(const Range& range) const;
  bool covers(const TCPFlagField &rhs) const;
  bool covers(Field* field) const;
  bool intersect(const TCPFlagField& rhs) const;
  bool intersect(Field* field) const;
  const Range& get_range() const;
  void set_range(const Range& range);
  FieldType get_type() const;
  ProtocolType get_protocol() const;
  void subtract(std::vector<Range>& ranges_union,
                std::vector<Range>& intervals) const;

  void set_flag(std::string flag);
  void unset_flag(std::string flag);
  void set_flags(std::bitset<6> flags);
  std::bitset<6> get_flags() const;
  void reset_flags();
  TCPFlagField(const Range& range);
  TCPFlagField(const std::bitset<6>& flags);
  ~TCPFlagField(){}
private:
    std::bitset<6> flags_;
    Range range_;
    FieldType type_;


};

class PortField : public L4Field, public Field_CRTP<PortField>{
public:
  PortField* clone() const;
  bool equal_to(const PortField& rhs) const;
  bool covers(const Range& portrange) const;
  bool covers(const PortField& portrange) const;
  bool covers(Field* field) const;
  bool intersect(const PortField& rhs) const;
  bool intersect(Field* field) const;
  const Range& get_range() const;
  FieldType get_type() const;
  ProtocolType get_protocol() const;
  void subtract(std::vector<Range>& ranges_union,
                std::vector<Range>& intervals) const;
  void set_range(const Range& rhs);

  PortField(const Range portrange, ProtocolType protocol);
  ~PortField(){}
private:
  Range range_;
  FieldType type_;
  ProtocolType protocol_;
};

class ICMPTypeField : public L4Field, public Field_CRTP<ICMPTypeField>{
public:
  ICMPTypeField* clone() const;
  bool equal_to(const ICMPTypeField& rhs) const;
  bool covers(const ICMPTypeField& rhs) const;
  bool covers(Field* field) const;
  bool intersect(const ICMPTypeField& rhs) const;
  bool intersect(Field* field) const;
  unsigned char get_icmptype() const;
  const Range& get_range() const;
  FieldType get_type() const;
  ProtocolType get_protocol() const;
  void set_range(const Range& range);
  void subtract(std::vector<Range>& ranges_union,
                std::vector<Range>& intervals) const;
  void set_icmptype(const unsigned char);
  ICMPTypeField(unsigned char icmptype_);
  ICMPTypeField(const Range& range);
  ~ICMPTypeField(){}
private:
  Range range_;
  FieldType type_;
};

class ICMPCodeField : public L4Field, public Field_CRTP<ICMPCodeField>{
public:
  ICMPCodeField* clone() const;
  bool equal_to(const ICMPCodeField& rhs) const;
  bool covers(const ICMPCodeField& rhs) const;
  bool covers(Field* field) const;
  bool intersect(const ICMPCodeField& rhs) const;
  bool intersect(Field* field) const;
  unsigned char get_icmpcode() const;
  const Range& get_range() const;
  FieldType get_type() const;
  ProtocolType get_protocol() const;
  void set_range(const Range& range);
  void subtract(std::vector<Range>& ranges_union,
                std::vector<Range>& intervals) const;
  void set_icmpcode(const unsigned char);
  ICMPCodeField(unsigned char icmpcode_);
  ICMPCodeField(const Range& range);
  ~ICMPCodeField(){}
private:
  Range range_;
  FieldType type_;
};

class L4ProtocolField : public L4Field, public Field_CRTP<L4ProtocolField>{
public:
  L4ProtocolField* clone() const;
  bool covers(Field* field) const;
  bool intersect(const L4ProtocolField& rhs) const;
  bool intersect(Field* field) const;
  bool equal_to(const L4ProtocolField& rhs) const;
  void subtract(std::vector<Range>& ranges_union,
                std::vector<Range>& intervals) const;
  const Range& get_range() const;
  void set_range(const Range& range);
  FieldType get_type() const;
  ProtocolType get_protocol() const;
  void set_protocol(const ProtocolType type);
  L4ProtocolField(const Range& range);
  L4ProtocolField(const ProtocolType type, const Range& range);
private:
  Range range_;
  ProtocolType protocol_;
  FieldType type_;
};

/* Layer 3 Fields ------------------------------------------------------------*/

class L3Field : public Field, public Field_CRTP<L3Field>{
public:
  virtual ~L3Field(){}
};

class IPv4Field : public L3Field, public Field_CRTP<IPv4Field>{
public:
  IPv4Field* clone() const;
  bool equal_to(const IPv4Field& rhs) const;
  bool covers(const IPv4Field& rhs) const;
  bool covers(const Range& range) const;
  bool covers(Field* field) const;
  bool intersect(const IPv4Field& rhs) const;
  bool intersect(Field* field) const;
  const Range& get_range() const;
  FieldType get_type() const;
  ProtocolType get_protocol() const;
  void subtract(std::vector<Range>& ranges_union,
                std::vector<Range>& intervals) const;
  void set_range(const Range& rhs);
  IPv4Field(const Range&);
  ~IPv4Field(){}
private:
  Range range_;
  FieldType type_;
};

class IPv6Field : public L3Field, public Field_CRTP<IPv6Field>{
public:
  IPv6Field* clone() const;
  bool equal_to(const IPv6Field& rhs) const;
  bool covers(const Range& rng) const;
  bool covers(const IPv6Field& rhs) const;
  bool covers(Field* field) const;
  bool intersect(const IPv6Field& rhs) const;
  bool intersect(Field* field) const;
  const Range& get_range() const;
  FieldType get_type() const;
  ProtocolType get_protocol() const;
  void subtract(std::vector<Range>& ranges_union,
                std::vector<Range>& intervals) const;
  void set_range(const Range& rhs);
  IPv6Field(const Range&);
  ~IPv6Field(){}
protected:
  Range range_;
  FieldType type_;
};

class L3ProtocolField : public L3Field, public Field_CRTP<L3ProtocolField>{
public:
  L3ProtocolField* clone() const;
  bool equal_to(const L3ProtocolField& rhs) const;
  bool covers(Field* field) const;
  bool intersect(const L3ProtocolField& rhs) const;
  bool intersect(Field* field) const;
  const Range& get_range() const;
  void set_range(const Range& range);
  void subtract(std::vector<Range>& ranges_union,
                std::vector<Range>& intervals) const;
  FieldType get_type() const;
  ProtocolType get_protocol() const;
  void set_protocol(const ProtocolType type);
  L3ProtocolField(const Range& range);
  L3ProtocolField(const ProtocolType type, const Range& range);
private:
  Range range_;
  ProtocolType protocol_;
  FieldType type_;
};

/* Layer 2 Fields ------------------------------------------------------------*/

class L2Field : public Field, public Field_CRTP<L2Field>{
public:
  virtual ~L2Field(){}
};

class MacField : public L2Field, public Field_CRTP<MacField>{
public:
  MacField* clone() const;
  bool equal_to(const MacField& rhs) const;
  bool covers(const MacField& rhs) const;
  bool covers(Field* field) const;
  bool intersect(const MacField& rhs) const;
  bool intersect(Field* field) const;
  const Range& get_range() const;
  FieldType get_type() const;
  ProtocolType get_protocol() const;
  void subtract(std::vector<Range>& ranges_union,
                std::vector<Range>& intervals) const;
  void set_range(const Range& rhs);
  MacField(const Range& range);
private:
  Range range_;
  FieldType type_;
};

class InterfaceField : public L2Field, public Field_CRTP<InterfaceField>{
public:
  InterfaceField* clone() const;
  bool equal_to(const InterfaceField& rhs) const;
  bool covers(const InterfaceField& rhs) const;
  bool covers(Field* field) const;
  bool intersect(const InterfaceField& rhs) const;
  bool intersect(Field* field) const;
  const Range& get_range() const;
  void set_range(const Range& range);
  FieldType get_type() const;
  ProtocolType get_protocol() const;
  void subtract(std::vector<Range>& ranges_union,
                std::vector<Range>& intervals) const;
  void set_interface() const;
  InterfaceField(const Range& range);
private:
  Range range_;
  FieldType type_;
};

#endif // FIELD_H
