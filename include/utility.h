#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <sstream>
#include "types.h"
#include "Field.h"

namespace Utility{

class CIDR{
public:
  bool operator==(const CIDR& rhs) const{
    return addr_ == rhs.addr_ && mask_ == rhs.mask_;
  }

  CIDR(uint32_t addr, uint16_t mask)
    : addr_(addr), mask_(mask) {}
  uint32_t addr_;
  uint16_t mask_;
};

//bool operator==(const CIDR& lhs, const CIDR& rhs){
//  return lhs.addr_ == rhs.addr_ && lhs.mask_ == rhs.mask_;
//}

bool validateIPv4(const std::string& str);
bool validateIPv6(const std::string& str);
IPTypes getIPType(const std::string& ip);
unsigned int ipv4_to_int(const std::string& ip);
Range mac_to_range(const std::string mac);
Range interface_to_range(const std::string& iface);
Range cidr_to_range(const uint64_t ip, const unsigned short bits);
Range ipv4_to_range(const std::string& str);
Range ipv4_single_to_range(const std::string& str);
Range ipv4_cidr_to_range(const std::string& str);
Range ipv4_range_to_range(const std::string& str);
Range ipv6_to_range(const std::string& str);
Range port_to_range(const std::string& str);
Range icmp_to_range(const std::string& str);

Range ipv4_wildcard();
Range port_wildcard();
Range l4_prot_wildcard();

std::string int_to_ipv4 (const uint64_t ip_integer);
std::string range_to_ipv4_range_score(const Range& range);
std::string range_to_ipv4_range_colon(const Range& range);
std::string range_to_ipv4_range_cidr(const Range& range);
std::string range_to_port_score(const Range& range);
std::string range_to_port_colon(const Range& range);
std::string range_to_l4_protocol(const Range& range);
std::string range_to_interface(const Range& range);
std::string range_to_icmptype(const Range& range);
bool is_valid_protocol_range(const Range& range);

bool isPowerOfTwo(const uint64_t number);
std::string zeropad(unsigned int num);
//std::string iptables_flags(const std::string, const std::string);

std::vector<CIDR> ipv4_range_to_cidr_list(const Range& range);

}
#endif //UTILITY_HPP
