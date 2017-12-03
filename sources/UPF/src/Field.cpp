#include "Field.h"


uint128_t::uint128_t() : low_(0), high_(0){}

uint128_t::uint128_t(uint64_t value) : low_(value), high_(0){}

void uint128_t::upshift(unsigned int shift){
  high_ <<= shift;
  low_ <<= shift;
}

void uint128_t::downshift(unsigned int shift){
  high_ >>= shift;
  low_ >>= shift;
}

void uint128_t::divide_by_two(){
  high_ = (uint64_t) high_ / 2;
  low_ = (uint64_t) low_ / 2;

}

//inline bool uint128_t::operator<(const uint128_t& rhs) const{
//  return(high_ < rhs.high_ || ((high_ == rhs.high_) && (low_ < rhs.low_)));
//}
//
//inline bool uint128_t::operator>(const uint128_t& rhs) const{
//  return(high_ > rhs.high_ || ((high_ == rhs.high_) && (low_ > rhs.low_)));
//}

//inline bool uint128_t::operator==(const uint128_t& rhs) const{
////  return(low_ == rhs.low_ && high_ == rhs.high_);
////}

//inline bool uint128_t::operator!=(const uint128_t& rhs) const{
//  return!(*this == rhs);
//}

//inline bool uint128_t::operator<=(const uint128_t& rhs) const{
//  return((*this < rhs) || (*this == rhs));
//}
//
//inline bool uint128_t::operator>=(const uint128_t& rhs) const{
//  return((*this > rhs) || (*this == rhs));
//}

uint128_t uint128_t::operator+(const uint128_t& rhs) const{
  uint128_t sum;
  sum.high_ = high_ + rhs.high_;
  sum.low_ = low_ + rhs.low_;
  if (sum.low_ < low_)
    ++sum.high_;
  return sum;
}

uint128_t uint128_t::operator-(const uint128_t& rhs) const{
  uint128_t dif;
  dif.high_ = high_ - rhs.high_;
  dif.low_ = low_ - rhs.low_;
  if (dif.low_ > low_)
      --dif.high_;
  return dif;
}

std::string uint128_t::print() const{
  std::ostringstream ss;
  ss <<  low_;
  return ss.str();
}

uint128_t uint128_t::max(){
  uint128_t res;
  res.low_ = 18446744073709551615U;
  res.high_ = 18446744073709551615U;
  return  res;
}

uint128_t uint128_t::min(){
  uint128_t res;
  res.low_ = 0;
  res.high_ = 0;
  return  res;
}

uint64_t uint128_t::get_low() const{
  return low_;
}

uint64_t uint128_t::get_high() const{
  return high_;
}

// Range

Range::Range()
  : lower_(uint128_t::min()),
    upper_(uint128_t::max()){}

Range::Range(const uint128_t& lower, const uint128_t& upper)
  : lower_(lower), upper_(upper){

    if(lower_ > upper_){
//      uint128_t temp(upper_);
//      upper_ = lower_;
//      lower_ = temp;
        std::swap(lower_,upper_);
    }
}

bool Range::is_wildcard() const{

  return (lower_ == uint128_t::min() && upper_ == uint128_t::max());
}
bool Range::equal_to(const Range& rhs) const{
  return ((lower_ == rhs.lower_) && (upper_ == rhs.upper_));
}

bool Range::covers(const Range& rhs) const{
  return (lower_ <= rhs.lower_ && rhs.upper_ <= upper_);
}

bool Range::intersect(const Range& rhs) const{
  if(*this == rhs)
    return true;
  else
    return !((upper_ < rhs.lower_ && lower_ < rhs.upper_) ||
            (lower_ > rhs.upper_ && upper_ > rhs.lower_));
}

bool Range::border(const Range& rhs) const{
  return (upper_ + 1 == rhs.lower_ || lower_ - 1 == rhs.upper_);
}

void Range::merge(const Range& rhs){

  if(this->border(rhs)){
    if (this->upper_ < rhs.lower_)
      this->upper_ = rhs.upper_;
    else
      this->lower_ = rhs.lower_;
  }
}


std::vector<Range> Range::diff(const Range& curr) const{
  std::vector<Range> intervals;
  // given range is outside left
  if (upper_ < curr.lower_)
    intervals.push_back(Range(lower_,upper_));
  // given range is outside right
  else if(lower_ > curr.upper_)
    intervals.push_back(Range(lower_,upper_));
  // given range envelopes left edge of interval
  else if(lower_ < curr.lower_ && upper_ <= curr.upper_)
    intervals.push_back(Range(lower_, curr.lower_-1));
  // given range envelopes right edge of interval
  else if(lower_ >= curr.lower_ && upper_ > curr.upper_)
    intervals.push_back(Range(curr.upper_+1, upper_));
  // given subtracted interval falls into given range
  else if(lower_ < curr.lower_ && upper_ > curr.upper_){
    intervals.push_back(Range(lower_, curr.lower_-1));
    intervals.push_back(Range(curr.upper_+1, upper_));
  }
  // given range falls entirely into subtracted interval
  // else if(result.lower_ >= curr.lower_ && result.upper_ =< curr.upper_)
  // intervals.reset();
  // return empty vector
  return intervals;
}

void Range::print() const {
  std::cout << "Range:[" << lower_ << "," << upper_ << "]"
            << std::endl;
}

Range Range::intersection(const Range& rhs) const{

  Range int_range;
  if (this->intersect(rhs)){
    if(this->lower_ >= rhs.lower_)
      int_range.lower_ = this->lower_;
    else
      int_range.lower_ = rhs.lower_;

    if(this->upper_ >= rhs.upper_)
      int_range.upper_ = rhs.upper_;
    else
      int_range.upper_ = this->upper_;

    return int_range;
  }
  else
    return Range(0,0);
}

Range& Range::operator=(const Range& rhs){

  if(this != &rhs){
    lower_ = rhs.lower_;
    upper_ = rhs.upper_;
  }
  return *this;
}

uint64_t Range::hash(){

  return std::hash<uint64_t>()(lower_.get_low() ^ upper_.get_high());

}


// L4Protocol Field

L4ProtocolField::L4ProtocolField(const Range& range)
  : range_(range),
    protocol_(TRANSPORT),
    type_(FieldType::l4_proto)
    {
    switch(range.lower_.get_low()){
    case 1 : protocol_ = ICMP; break;
    case 6 : protocol_ = TCP; break;
    case 17: protocol_ = UDP; break;
    case 47: protocol_ = GRE; break;
    case 51: protocol_ = AH; break;
    }

    }

L4ProtocolField::L4ProtocolField(const ProtocolType proto, const Range& range)
  : range_(range),
    protocol_(proto),
    type_(FieldType::l4_proto)
    {}

L4ProtocolField* L4ProtocolField::clone()const{
  return new L4ProtocolField(*this);
}

bool L4ProtocolField::equal_to(const L4ProtocolField& rhs) const{
  return(protocol_ == rhs.protocol_);
}

bool L4ProtocolField::covers(Field* field) const{
  FieldType type = field->get_type();
  if(type == l4_proto)
    return protocol_ == static_cast<L4ProtocolField*>(field)->get_protocol();
  else
    return false;
}

bool L4ProtocolField::intersect(Field* field)const{
  return range_.intersect(field->get_range());
}

ProtocolType L4ProtocolField::get_protocol() const{
  return protocol_;
}

const Range& L4ProtocolField::get_range() const{
  return range_;
}

void L4ProtocolField::set_range(const Range& range){
    range_ = range;
}

void L4ProtocolField::subtract(std::vector<Range>& ranges_union,
                               std::vector<Range>& intervals) const{

  std::vector<Range> working_set;
  Range def = range_;
  working_set.push_back(def);

  for(auto iter = ranges_union.begin(); iter != ranges_union.end(); ++iter){

    Range curr = (*iter);
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

FieldType L4ProtocolField::get_type() const{
  return type_;
}

void L4ProtocolField::set_protocol(const ProtocolType type){
  protocol_ = type;
  if(type == TCP)
    range_ = Range(6,6);
  if(type == UDP)
    range_ = Range(17,17);
  if(type == ICMP)
    range_ = Range(1,1);
  if(type == TRANSPORT)
    range_ = Range(0,255);
}

// TCPField

TCPFlagField::TCPFlagField(const Range& range)
  : range_(range),
    type_(FieldType::tcpflagfield)
    {}


TCPFlagField::TCPFlagField(const std::bitset<6>& flags)
  : flags_(flags),
    range_(Range(0,0)),
    type_(FieldType::tcpflagfield)
    {}

TCPFlagField* TCPFlagField::clone() const{ return new TCPFlagField(*this);}

bool TCPFlagField::equal_to(const TCPFlagField &rhs) const{
  return (flags_== rhs.flags_);
}

bool TCPFlagField::covers(Field* field) const{
  if(field->get_type() == tcpflagfield)
    return(flags_ == static_cast<TCPFlagField*>(field)->get_flags());
  else
    return false;
}

bool TCPFlagField::intersect(Field* field)const{
  return this->covers(field);
}

FieldType TCPFlagField::get_type() const {
  return type_;
}

ProtocolType TCPFlagField::get_protocol() const {
  return TCP;
}

void TCPFlagField::set_flag(std::string flag){
  if (flag.compare("SYN") == 0)
    flags_.set(5,1);
  else if (flag.compare("ACK") == 0)
    flags_.set(4,1);
  else if (flag.compare("PSH") == 0)
    flags_.set(3,1);
  else if (flag.compare("URG") == 0)
    flags_.set(2,1);
  else if (flag.compare("FIN") == 0)
    flags_.set(1,1);
  else if (flag.compare("RST") == 0)
    flags_.set(0,1);
  else
    throw "Invalid Flag, no Flags set";
}

void TCPFlagField::unset_flag(std::string flag){
  if (flag.compare("SYN") == 0)
    flags_.set(5,0);
  else if (flag.compare("ACK") == 0)
    flags_.set(4,0);
  else if (flag.compare("PSH") == 0)
    flags_.set(3,0);
  else if (flag.compare("URG") == 0)
    flags_.set(2,0);
  else if (flag.compare("FIN") == 0)
    flags_.set(1,0);
  else if (flag.compare("RST") == 0)
    flags_.set(0,0);
  else
    throw "Invalid Flag, no Flags unset";
}

void TCPFlagField::set_flags(std::bitset<6> bitvec) {
  flags_ = (flags_|bitvec);
}

std::bitset<6> TCPFlagField::get_flags() const {
  return flags_;
}

void TCPFlagField::reset_flags(){
  flags_.reset();
}

bool TCPFlagField::covers(const TCPFlagField &rhs) const{
  if (flags_ == rhs.flags_)
    return true;
  else
   return false;
}

bool TCPFlagField::intersect(const TCPFlagField& ) const{
  return false;
}

const Range& TCPFlagField::get_range() const{
  return range_;
}

void TCPFlagField::set_range(const Range& range){
  range_ = range;
}

void TCPFlagField::subtract(std::vector<Range>& ranges_union,
                            std::vector<Range>& intervals) const{

  Range def = this->range_;
  std::vector<Range> working_set;
  working_set.push_back(def);

  for(auto iter = ranges_union.begin(); iter != ranges_union.end(); ++iter){
    Range curr = (*iter);
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

// PortFields

PortField::PortField(Range range, ProtocolType protocol)
  : range_(range),
    protocol_(protocol)
    {}

PortField* PortField::clone() const {
  return new PortField(*this);
}

bool PortField::equal_to(const PortField &rhs) const{
  return (range_ == rhs.range_);
}

FieldType PortField::get_type() const {
  return type_;
}

ProtocolType PortField::get_protocol() const {
  return protocol_;
}

bool PortField::covers(const PortField &rhs) const{
  return (range_.covers(rhs.range_));
}

bool PortField::covers(Field* field) const{
  if(field->get_type() == portfield)
    return (range_.covers(static_cast<PortField*>(field)->get_range()));
  else
    return false;
}

bool PortField::intersect(const PortField& rhs) const{
  return range_.intersect(rhs.range_);
}

bool PortField::intersect(Field* field) const{
  return range_.intersect(field->get_range());
}

void PortField::set_range(const Range& new_range){
  range_ = new_range;
}

const Range& PortField::get_range() const{
  return range_;
}

void PortField::subtract(std::vector<Range>& ranges_union,
                         std::vector<Range>& intervals) const{

  Range def = range_;
  std::vector<Range> working_set;
  working_set.push_back(def);
  for(auto iter = ranges_union.begin(); iter != ranges_union.end(); ++iter){
    Range curr = (*iter);
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

// ICMP Fields

ICMPCodeField::ICMPCodeField(unsigned char icmpcode)
  : range_(Range(icmpcode,icmpcode)),
    type_(icmpcodefield)
    {}

ICMPCodeField::ICMPCodeField(const Range& range)
  : range_(range),
    type_(icmpcodefield)
    {}

ICMPCodeField* ICMPCodeField::clone() const{
  return new ICMPCodeField(*this);
}

bool ICMPCodeField::equal_to(const ICMPCodeField &rhs) const{
  return (range_ == rhs.range_);
}

bool ICMPCodeField::covers(const ICMPCodeField &rhs) const{
  return (range_ == rhs.range_);
}

bool ICMPCodeField::covers(Field* field) const {
  if(field->get_type() == icmpcodefield)
    return (range_.covers(static_cast<ICMPCodeField*>(field)->get_range()));
  else if (field->get_type() == icmpcodefield)
    return (range_.covers(static_cast<ICMPCodeField*>(field)->get_range()));
  else
    return false;
}

bool ICMPCodeField::intersect(const ICMPCodeField& rhs) const{
  return this->covers(rhs);
}

bool ICMPCodeField::intersect(Field* field) const{
  return this->covers(field);
}

void ICMPCodeField::set_icmpcode(const unsigned char code){
  range_ = Range(code,code);
}


const Range& ICMPCodeField::get_range() const{
  return range_;
}

void ICMPCodeField::set_range(const Range& range){
  range_ = range;
}

FieldType ICMPCodeField::get_type() const {
  return type_;
}

ProtocolType ICMPCodeField::get_protocol() const {
  return ICMP;
}

void ICMPCodeField::subtract(std::vector<Range>& ranges_union,
                             std::vector<Range>& intervals)const{

  Range def = range_;
  std::vector<Range> working_set;
  working_set.push_back(def);
  for(auto iter = ranges_union.begin(); iter != ranges_union.end(); ++iter){
    Range curr = (*iter);
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

FieldType ICMPTypeField::get_type() const {
  return type_;
}

ICMPTypeField* ICMPTypeField::clone() const{
  return new ICMPTypeField(*this);
}

ICMPTypeField::ICMPTypeField(unsigned char icmptype)
  : range_(Range(icmptype,icmptype)),
    type_(icmptypefield)
    {}

ICMPTypeField::ICMPTypeField(const Range& range)
  : range_(range),
    type_(icmptypefield)
    {}

bool ICMPTypeField::equal_to(const ICMPTypeField &rhs) const{
  return (range_ == rhs.range_);
}

bool ICMPTypeField::covers(const ICMPTypeField &rhs) const{
  return (range_ == rhs.range_);
}

bool ICMPTypeField::covers(Field* field) const {
  if(field->get_type() == icmptypefield)
    return (range_.covers(static_cast<ICMPTypeField*>(field)->get_range()));
  else if (field->get_type() == icmptypefield)
    return (range_.covers(static_cast<ICMPTypeField*>(field)->get_range()));
  else
    return false;
}

bool ICMPTypeField::intersect(const ICMPTypeField& rhs ) const{
  return this->covers(rhs);
}

bool ICMPTypeField::intersect(Field* field) const{
  return this->covers(field);
}

void ICMPTypeField::set_icmptype(const unsigned char type){
  range_ = Range(type,type);
}

const Range& ICMPTypeField::get_range() const{
  return range_;
}

ProtocolType ICMPTypeField::get_protocol() const {
  return ICMP;
}

void ICMPTypeField::set_range(const Range& range){
  range_ = range;
}

void ICMPTypeField::subtract(std::vector<Range>& ranges_union,
                             std::vector<Range>& intervals) const{

  Range def = range_;
  std::vector<Range> working_set;
  working_set.push_back(def);
  for(auto iter = ranges_union.begin(); iter != ranges_union.end(); ++iter){
    Range curr = (*iter);
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

// L3ProtocolField

static ProtocolType range_to_l3prot(const Range& range){

  switch(range.lower_.get_low()){
    case 4: return ProtocolType::IPV4;
    case 41: return ProtocolType::IPV6;
    default: return ProtocolType::ETHERNET;
  }
}

L3ProtocolField::L3ProtocolField(const Range& range)
  : range_(range),
    protocol_(range_to_l3prot(range)),
    type_(l3_proto)
    {}

L3ProtocolField::L3ProtocolField(const ProtocolType proto, const Range& range)
  : range_(range),
    protocol_(proto),
    type_(l3_proto)
    {}

L3ProtocolField* L3ProtocolField::clone() const{
  return new L3ProtocolField(*this);
}

bool L3ProtocolField::equal_to(const L3ProtocolField& rhs) const{
  return(protocol_ == rhs.protocol_);
}

bool L3ProtocolField::covers(Field* field) const{
  if(field->get_type() == l3_proto)
    return (range_.covers(static_cast<L3ProtocolField*>(field)->get_range()));
  else if (field->get_type() == l4_dst)
    return (range_.covers(static_cast<L3ProtocolField*>(field)->get_range()));
  else
    return false;
}

bool L3ProtocolField::intersect(Field* field) const{
  return this->covers(field);
}

const Range& L3ProtocolField::get_range() const{
  return range_;
}

void L3ProtocolField::set_range(const Range& range){
  range_ = range;
}

void L3ProtocolField::subtract(std::vector<Range>& ranges_union,
                               std::vector<Range>& intervals) const{

  Range def = range_;
  std::vector<Range> working_set;
  working_set.push_back(def);
  for(auto iter = ranges_union.begin(); iter != ranges_union.end(); ++iter){
    Range curr = (*iter);
    std::vector<Range> sec_set;
    for (auto iterw = working_set.begin();iterw != working_set.end(); ++iterw){
      std::vector<Range> result;
      result = iterw->diff(curr);
      if(!result.empty())
        sec_set.insert(sec_set.end(), result.begin(), result.end());
    }
    working_set = sec_set;
  }
  intervals = working_set;
}

FieldType L3ProtocolField::get_type() const {
  return type_;
}

void L3ProtocolField::set_protocol(const ProtocolType type){
  protocol_ = type;
  if(type == IPV4)
    range_ = Range(4,4);
  if(type == IPV6)
    range_ = Range(41,41);
  if(type == ETHERNET)
    range_ = Range(0,100);
}

ProtocolType L3ProtocolField::get_protocol() const {
  return protocol_;
}

// IPv4Field

IPv4Field* IPv4Field::clone() const {return new IPv4Field(*this);}

IPv4Field::IPv4Field(const Range& range)
  : range_(range),
    type_(ipv4field)
    {}

bool IPv4Field::equal_to(const IPv4Field &rhs) const{
  return ((range_ == rhs.range_));
}

FieldType IPv4Field::get_type() const {
  return type_;
}

bool IPv4Field::covers(const Range& rhs)const {
  return (range_.covers(rhs));
}

bool IPv4Field::covers(const IPv4Field &rhs) const{
  return (range_.covers(rhs.range_));
}

bool IPv4Field::covers(Field* field) const{
  if(field->get_type() == ipv4field)
    return (range_.covers(static_cast<IPv4Field*>(field)->get_range()));
  else if (field->get_type() == ipv4field)
    return (range_.covers(static_cast<IPv4Field*>(field)->get_range()));
  else
    return false;
}

bool IPv4Field::intersect(const IPv4Field& rhs) const{
  return range_.intersect(rhs.range_);
}

bool IPv4Field::intersect(Field* field) const{
  return range_.intersect(field->get_range());
}

void IPv4Field::set_range(const Range& new_range){
  range_ = new_range;
}

const Range& IPv4Field::get_range() const{
  return range_;
}

ProtocolType IPv4Field::get_protocol() const {
  return IPV4;
}

void IPv4Field::subtract(std::vector<Range>& ranges_union,
                         std::vector<Range>& intervals) const{

  Range def = range_;
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

// IPv6Field

IPv6Field::IPv6Field(const Range& range)
  :  range_(range),
     type_(ipv6field)
     {}

IPv6Field* IPv6Field::clone() const {
  return new IPv6Field(*this);
}

bool IPv6Field::equal_to(const IPv6Field &rhs) const{
  return ((range_ == rhs.range_));
}

bool IPv6Field::covers(const Range& rhs)const {
  return (range_.covers(rhs));
}

bool IPv6Field::covers(const IPv6Field &rhs) const{
  return (range_.covers(rhs.range_));
}

bool IPv6Field::covers(Field* field) const{
  if(field->get_type() == ipv6field)
    return (range_.covers(static_cast<IPv6Field*>(field)->get_range()));
  else if (field->get_type() == ipv6field)
    return (range_.covers(static_cast<IPv6Field*>(field)->get_range()));
  else
    return false;
}

bool IPv6Field::intersect(const IPv6Field& rhs) const{
  return range_.intersect(rhs.range_);
}

bool IPv6Field::intersect(Field* field) const{
  return range_.intersect(field->get_range());
}

void IPv6Field::set_range(const Range& new_range){
  range_ = new_range;
}

const Range& IPv6Field::get_range() const{
  return range_;
}

void IPv6Field::subtract(std::vector<Range>& ranges_union,
                         std::vector<Range>& intervals) const{

  Range def = range_;
  std::vector<Range> working_set;
  working_set.push_back(def);
  for(auto iter = ranges_union.begin(); iter != ranges_union.end(); ++iter){
    Range curr = (*iter);
    std::vector<Range> sec_set;
    for (auto iterw = working_set.begin(); iterw != working_set.end(); ++iterw){
      std::vector<Range> result;
      result = iterw->diff(curr);
      if(!result.empty())
        sec_set.insert(sec_set.end(), result.begin(), result.end());
      }
    working_set = sec_set;
    }
  intervals = working_set;
}

FieldType IPv6Field::get_type() const {
  return type_;
}

ProtocolType IPv6Field::get_protocol() const {
  return IPV4;
}

// MacField

MacField::MacField(const Range& range)
  : range_(range),
    type_(macfield)
    {}

MacField* MacField::clone() const{
  return new MacField(*this);
}

bool MacField::equal_to(const MacField& rhs) const {
  return (range_ == rhs.range_);
}

bool MacField::covers(const MacField& rhs) const {
  return (range_.covers(rhs.range_));
}

bool MacField::covers(Field* field) const{
  if(field->get_type() == macfield)
    return (range_.covers(static_cast<MacField*>(field)->get_range()));
  else if (field->get_type() == macfield)
    return (range_.covers(static_cast<MacField*>(field)->get_range()));
  else
    return false;
}

bool MacField::intersect(const MacField& rhs) const{
  return range_.intersect(rhs.range_);
}

bool MacField::intersect(Field* field) const{
  return range_.intersect(field->get_range());
}

void MacField::set_range(const Range& new_range){
  range_ = new_range;
}

const Range& MacField::get_range() const{
  return range_;
}

FieldType MacField::get_type() const {
  return type_;
}

ProtocolType MacField::get_protocol() const {
  return ETHERNET;
}

void MacField::subtract(std::vector<Range>& ranges_union,
                       std::vector<Range>& intervals)const{

  Range def = range_;
  std::vector<Range> working_set;
  working_set.push_back(def);
  for(auto iter = ranges_union.begin(); iter != ranges_union.end(); ++iter){
    Range curr = (*iter);
    std::vector<Range> sec_set;
    for (auto iterw = working_set.begin(); iterw != working_set.end(); ++iterw){
      std::vector<Range> result;
      result = iterw->diff(curr);
      if(!result.empty())
        sec_set.insert(sec_set.end(), result.begin(), result.end());
    }
    working_set = sec_set;
  }
  intervals = working_set;
}

// InterfaceField

InterfaceField::InterfaceField(const Range& range)
  : range_(range),
    type_(interface_in_field)
     {}

InterfaceField* InterfaceField::clone() const{
  return new InterfaceField(*this);
}

bool InterfaceField::equal_to(const InterfaceField& rhs) const {
  return (range_ == rhs.get_range());
}

bool InterfaceField::covers(const InterfaceField& rhs) const {
  return (range_.covers(rhs.get_range()));
}

bool InterfaceField::covers(Field* field) const{
  if(field->get_type() == interface_in_field ||
     field->get_type() == interface_out_field)

     return (range_.covers(static_cast<InterfaceField*>(field)->get_range()));
  else
      return false;
}

bool InterfaceField::intersect(Field* field) const{
  return this->covers(field);
}

bool InterfaceField::intersect(const InterfaceField&) const{
  return false;
}

const Range& InterfaceField::get_range() const{
  return range_;
}

void InterfaceField::set_range(const Range& range){
    range_ = range;
}

FieldType InterfaceField::get_type() const {
  return type_;
}

ProtocolType InterfaceField::get_protocol() const {
  return ETHERNET;
}

void InterfaceField::subtract(std::vector<Range>& ranges_union,
                              std::vector<Range>& intervals)const{

  Range def = this->get_range();
  std::vector<Range> working_set;
  working_set.push_back(def);
  for(auto iter = ranges_union.begin(); iter != ranges_union.end(); ++iter){
    Range curr = (*iter);
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


/* Ostream Operator Overloading */

std::ostream& operator<<(std::ostream& out, const Range& range){
  out << "[" << range.lower_ << "," << range.upper_ << "] ";
  return out;
}
std::ostream& operator<<(std::ostream& out, const uint128_t& num){
  out << num.get_low();
  return out;
  }
