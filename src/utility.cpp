#include "utility.h"

#include <cmath>
#include <bitset>
#include <algorithm>
#include <iomanip>



bool Utility::validateIPv4(const std::string& str){
    bool valid = false;
//    std::stringstream s(str);
//    std::string token;
//    std::vector<std::string> tokens;

//    while(getline(s,token,'.'))
//        tokens.push_back(token);
//
//           for(auto iter = tokens.begin(); iter != tokens.end(); iter++){
          if(str.empty()){
            std::cout << "Parse Error, Couldn't read file contents" << std::endl;
            exit(EXIT_FAILURE);
            }
          if(str.find_first_of("abcdef") != std::string::npos)
             valid = false;

//            else if(atoi(iter->c_str()) < 0 || atoi(iter->c_str()) > 255){
//                valid = false;
//                break;
//            }
            else
                valid = true;
    return valid;
}

bool Utility::validateIPv6(const std::string& str){
//    return false;           // Function incomplete
    bool valid = false;
    std::stringstream s(str);
    std::string token;
    std::vector<std::string> tokens;
    while(getline(s,token,':'))
        tokens.push_back(token);
    if(tokens.size() <= 8){
        for(auto iter = tokens.begin(); iter != tokens.end(); iter++){
            if(iter->find_first_not_of("0123456789abcdef-:/") != std::string::npos){
                valid = false;
                break;
            }
            else if(atoi(iter->c_str()) < 0 || atoi(iter->c_str()) > 255){
                valid = false;
                break;
            }
            else
                valid = true;
        }
    }
    return valid;
}

IPTypes Utility::getIPType(const std::string& ip){
    using namespace Utility;

    IPTypes type = none;
    std::stringstream s(ip);
    std::string token;
    std::vector<std::string> tokens;
    if(validateIPv4(ip)){
        if(ip.find_first_not_of("0123456789.") == std::string::npos)
            type = ipv4_single;
        else if(ip.find_first_not_of("0123456789.-") == std::string::npos
            && (ip.length() <= 32)){
            while(getline(s,token,'-'))
                tokens.push_back(token);
            if (validateIPv4(tokens[0]) && validateIPv4(tokens[1]))
                type = ipv4_range_score;
        }
        else if(ip.find_first_not_of("0123456789.:") == std::string::npos
                && (ip.length() <= 32)){
            while(getline(s,token,':'))
               tokens.push_back(token);
            if (validateIPv4(tokens[0]) && validateIPv4(tokens[1]))
                type = ipv4_range_colon;
        }
        else if(ip.find_first_not_of("0123456789./") == std::string::npos
                && (ip.length() <= 19)){
            while(getline(s,token,'/'))
               tokens.push_back(token);
            if (validateIPv4(tokens[0]) && atoi(tokens[1].c_str()) >= 0
                && atoi(tokens[1].c_str()) <= 32)
                type = ipv4_cidr;
        }
    }
    else if(validateIPv6(ip)){
        if(ip.find_first_not_of("0123456789abcdef:") == std::string::npos)
            type = ipv6_single;
        else if(ip.find_first_not_of("0123456789abcdef:-") == std::string::npos
                && (ip.length() <= 79)){
            while(getline(s,token,'-'))
                tokens.push_back(token);
            if (validateIPv6(tokens[0]) && validateIPv6(tokens[1]))
                type = ipv6_range_score;
        }
        else if(ip.find_first_not_of("0123456789abcdef:/") == std::string::npos
                && (ip.length() <= 43)){
            while(getline(s,token,'/'))
               tokens.push_back(token);
            if (validateIPv6(tokens[0]) && atoi(tokens[1].c_str()) >= 0
                && atoi(tokens[1].c_str()) <= 128)
                type = ipv6_cidr;
        }
    }
    //
    //
    // TODO IPv6 Ranges in ipfw
    //
    //
    return type;
}

Range Utility::mac_to_range(std::string mac){
    using namespace std;

    remove(mac.begin(), mac.end(), '-');
    remove(mac.begin(), mac.end(), ':');
    mac.erase(12);
    unsigned long long sum;
    sscanf(mac.c_str(),"%llx",&sum);
    return Range(sum,sum);
}

Range Utility::interface_to_range(const std::string& iface){
    std::stringstream ss;
    for(unsigned int i = 0; i < iface.length(); ++i){
        ss << static_cast<int>(iface[i]);
    }
    uint64_t number;
    ss >> number;
    return Range(number,number);
}

Range Utility::ipv4_cidr_to_range(const std::string& ipcidr){
    std::stringstream ss(ipcidr);
    int a,b,c,d;
    short mask;
    char dot, slash;
    ss >> a >> dot >> b >> dot >> c >> dot >> d >> slash >> mask;
    uint64_t sum = a*pow(256,3) + b * pow(256,2) + c * 256 + d;
    return cidr_to_range(sum,mask);
}

Range Utility::cidr_to_range(const uint64_t ip, const unsigned short bits){
    std::bitset<32> netmask;
    std::bitset<32> mybitset(ip);
    for(unsigned int i = 0; i <= 31; ++i){
        (i < bits) ? netmask.set(31-i,true) : netmask.set(31-i,false);
    }
    mybitset = (netmask&mybitset);
    uint64_t upper = (mybitset|netmask.flip()).to_ulong();
    return Range(mybitset.to_ulong(),upper);
}


Range Utility::ipv4_to_range(const std::string& str){
    // Decide if single, cidr or range
    IPTypes type = Utility::getIPType(str);
    Range range;
    if(type == ipv4_single)
        range = ipv4_single_to_range(str);
    else if (type == ipv4_cidr)
        range = ipv4_cidr_to_range(str);
    else if (type == ipv4_range_score || type == ipv4_range_colon)
        range = ipv4_range_to_range(str);
    return range;
}

Range Utility::ipv4_single_to_range(const std::string& str){
    std::stringstream ss(str);
    int a,b,c,d;
    char dot;
    ss >> a >> dot >> b >> dot >> c >> dot >> d;
    uint64_t sum = a*pow(256,3) + b * pow(256,2) + c * 256 + d;
    return Range(sum,sum);
}

Range Utility::ipv4_range_to_range(const std::string& str){
    std::stringstream ss(str);
    std::string a,b;
    a = str.substr(0, str.find(":"));
    if (a == str){
        a = str.substr(0,str.find("-"));
        b = str.substr(str.find("-")+1, str.size() - str.find("-"));
    }
    else
        b = str.substr(str.find(":")+1, str.size() - str.find(":"));
    uint64_t lower = ipv4_single_to_range(a).lower_.get_low();
    uint64_t upper = ipv4_single_to_range(b).upper_.get_low();;
    return Range(lower,upper);
}

Range Utility::ipv6_to_range(const std::string& ){
    // TODO
    return Range(0,0);
}


Range Utility::port_to_range(const std::string& str){
    // Singe Ports, Port Ranges and Port Lists (ipfw)
    // Check if single number
    if (str.find_first_not_of("0123456789") == std::string::npos)
        return Range(atoi(str.c_str()), atoi(str.c_str()));
    // Check if Range denoted with '-' or ':' between numbers
    else if (str.find_first_not_of("0123456789-:") == std::string::npos){
        std::stringstream ss(str);
        unsigned int a,b;
        char dot;
        ss >> a >> dot >> b;
        return Range(a,b);
    }
    // TODO: Add Port Lists
    return Range(0,0);
}

Range Utility::icmp_to_range(const std::string &str){
    return Range(atoi(str.c_str()),atoi(str.c_str()));
}

Range Utility::ipv4_wildcard(){
    return Range(0,4294967295);
}

Range Utility::port_wildcard(){
    return Range(0,65535);
}

Range Utility::l4_prot_wildcard(){
    return Range(0,255);
}

std::string Utility::int_to_ipv4(const uint64_t ip){
    unsigned char bytes[4];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;
    std::stringstream ss;
    ss << static_cast<unsigned>(bytes[3]) << "." <<
    static_cast<unsigned>(bytes[2]) << "." <<
    static_cast<unsigned>(bytes[1]) << "." <<
    static_cast<unsigned>(bytes[0]);
    return ss.str();
}

std::string Utility::range_to_ipv4_range_score(const Range& range){
    uint32_t lower_limit = range.lower_.get_low();
    uint32_t upper_limit = range.upper_.get_low();
    std::stringstream ss;
    ss << Utility::int_to_ipv4(lower_limit) << "-" <<
    Utility::int_to_ipv4(upper_limit);
    return ss.str();
}

std::string Utility::range_to_ipv4_range_colon(const Range& range){
    uint32_t lower_limit = range.lower_.get_low();
    uint32_t upper_limit = range.upper_.get_low();
    std::stringstream ss;
    ss << Utility::int_to_ipv4(lower_limit) << ":" <<
    Utility::int_to_ipv4(upper_limit);
    return ss.str();
}

bool Utility::isPowerOfTwo (uint64_t x){
   return ((x != 0) && !(x & (x - 1)));
}

std::string Utility::range_to_ipv4_range_cidr(const Range& range){
    uint32_t lower = range.lower_.get_low();
    uint32_t upper = range.upper_.get_low();
    std::stringstream result;
    unsigned int diff = upper-lower;
    if(isPowerOfTwo(diff+1)){
        if(upper != lower)
            result << int_to_ipv4(range.lower_.get_low()) <<
            "/" << 32 - log2(diff+1);
        else
            result << int_to_ipv4(range.lower_.get_low());
    }
    return result.str();
}

std::string Utility::range_to_port_colon(const Range& range){
    std::stringstream ss;
    if(range.lower_.get_low() != range.upper_.get_low())
        ss << (uint16_t) range.lower_.get_low() << ":"
        << (uint16_t) range.upper_.get_low();
    else
        ss << (uint16_t) range.lower_.get_low();
    return ss.str();
}

std::string Utility::range_to_port_score(const Range& range){
    std::stringstream ss;
    if(range.lower_.get_low() != range.upper_.get_low())
        ss << (uint16_t) range.lower_.get_low() << "-"
        << (uint16_t) range.upper_.get_low();
    else
        ss << (uint16_t) range.lower_.get_low();
    return ss.str();
}

bool Utility::is_valid_protocol_range(const Range& range){
    uint64_t lower = range.lower_.get_low();
    uint64_t upper = range.upper_.get_low();

    if(upper < 1 || lower > 47 )
      return false;
    else if (lower > 1 && upper < 6)
      return false;
    else if (lower > 6 && upper < 17)
      return false;
    else if (lower > 17 && upper < 47)
      return false;
    else
      return true;

}

std::string Utility::range_to_l4_protocol(const Range& range){
    uint64_t lower = range.lower_.get_low();
    uint64_t upper = range.upper_.get_low();
    std::string number = std::to_string(lower);
    if( lower == upper && lower != 0 && upper != 0){
     // Single protocol specified
      switch(lower){
        case 1: return "icmp";
        case 6: return "tcp";
        case 17: return "udp";
        case 47: return "47";
        default: return number;
      }
    }
    else{
      if(upper < 1)
        return "all";
      if(upper < 6)
        return "icmp";
      else if (upper < 17)
        return "tcp";
      else if (upper < 47 )
        return "udp";
      else
        return "all";
    }

    return "";
}

std::string Utility::range_to_interface(const Range& range){
    std::string s1 = std::to_string(range.lower_.get_low());
    std::stringstream fs;
    unsigned int i = 0;
    std::string first, second, third;
    std::string result = "";
    while(i < s1.size()){
    if(s1.size() - i >= 3){
        first = s1.at(i);
        first.push_back(s1.at(i+1));
        first.push_back(s1.at(i+2));
    }
    else if(s1.size() - i == 2){
        first = s1.at(i);
        first.push_back(s1.at(i+1));
    }
    else if(s1.size() - i == 1){
        first = s1.at(i);
    }
    result.push_back(static_cast<char>(std::stoi(first)));
        i = i+3;
    }
    return result;
}

std::string Utility::range_to_icmptype(const Range& range){
    return  std::to_string(range.lower_.get_low());
}


std::string Utility::zeropad(unsigned int num){
    std::ostringstream ss;
    ss << std::setw( 5 ) << std::setfill( '0' ) << num;
    return ss.str();
}

std::vector<Utility::CIDR> Utility::ipv4_range_to_cidr_list(const Range& range){

  std::vector<CIDR> cidr_list;

  uint32_t low = range.lower_.get_low();
  uint32_t high = range.upper_.get_low();
  uint64_t current = low;
//  std::cout << range << std::endl;

  while(current < high){
    for(unsigned int i = 0; i < 32; ++i){
      Range cidr_low = cidr_to_range(current, 32-(i+1));
      if(cidr_to_range(current,32-(i+1)).lower_ >= low &&
         cidr_to_range(current,32-(i+1)).upper_ <= high){
//        std::cout << i << std::endl;
        continue;
      }
      else if(cidr_to_range(current,32-(i+1)).upper_ > high && i != 0){
        cidr_list.push_back(CIDR(current,32-(i)));
        current = cidr_to_range(current,32-(i)).upper_.get_low() + 1;
        break;
      }
      else{
        cidr_list.push_back(CIDR(current,32-i));
        current += pow(2,i);
//        std::cout << int_to_ipv4(current) << std::endl;
        break;
      }
    }
  }
  return cidr_list;
}

//std::vector<Utility::CIDR> Utility::ipv4_range_to_cidr_list(const Range& range){
//
//  std::vector<CIDR> cidr_list;
//
//  uint32_t low = range.lower_.get_low();
//  uint32_t high = range.upper_.get_low();
//  uint64_t current = low;
////  std::cout << range << std::endl;
//
//  while(current < high){
//    for(unsigned int i = 0; i < 8; ++i){
//      Range cidr_low = cidr_to_range(current, 32-(i+1));
//      if(cidr_to_range(current,32-(i+1)).lower_ >= low &&
//         cidr_to_range(current,32-(i+1)).upper_ <= high){
////        std::cout << i << std::endl;
//        continue;
//      }
//      else if(cidr_to_range(current,32-(i+1)).upper_ > high && i != 0){
//        cidr_list.push_back(CIDR(current,32-(i)));
//        current = cidr_to_range(current,32-(i)).upper_.get_low() + 1;
//        break;
//      }
//      else{
//        cidr_list.push_back(CIDR(current,32-i));
//        current += pow(2,i);
////        std::cout << int_to_ipv4(current) << std::endl;
//        break;
//      }
//    }
//  }
//  return cidr_list;
//}
//

unsigned int Utility::ipv4_to_int(const std::string& ip){

    std::stringstream ss(ip);
    int a,b,c,d;
    char dot;
    ss >> a >> dot >> b >> dot >> c >> dot >> d;
    uint64_t sum = a*pow(256,3) + b * pow(256,2) + c * 256 + d;
    return sum;
}

