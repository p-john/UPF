#include "UPF.h"

Action::Action(const ActionTypes action)
  :  type_(action),
     target_("")
     {}

ActionTypes Action::get_type()const {
  return type_;
}

bool Action::equal_to(const Action& rhs)const {
  return type_ == rhs.type_; /*&& target_ == rhs.target_;*/
}

std::ostream& Action::print(std::ostream& out) const{
    switch(type_){
      case NONE: out << "NONE" << std::endl; break;
      case ACCEPT: out << "ACCEPT" << std::endl; break;
      case BLOCK: out << "BLOCK" << std::endl; break;
      case JUMP: out << "JUMP to Target: " << target_ << std::endl; break;
    }
  return out;
}

std::string Action::get_target() const{
  return target_;
}

void Action::set_target(const std::string target){
  target_ = target;
}

Accept::Accept() : Action(ActionTypes::ACCEPT){}

bool Accept::equal_to(const Accept& rhs) const{
  return type_ == rhs.type_;
}

Block::Block(): Action(ActionTypes::BLOCK){}
bool Block::equal_to(const Block& rhs) const{
  return type_ == rhs.type_;
}

Jump::Jump(const std::string target)
  : Action(ActionTypes::JUMP){
      set_target(target);
    }

Jump::~Jump(){}
bool Jump::equal_to(const Jump& rhs)const{
  if (target_ == rhs.target_)
    return true;
  else
    return false;
}

// UPF Rule

UPF_Rule::UPF_Rule()
  :   action_(NONE),
      l2rule_(new L2Rule()),
      original_rule_("UPF_RULE"),
      type_(freerule),
      unknown_match_(false) {}

UPF_Rule::UPF_Rule(const UPF_Rule& copy)
  :   action_(copy.get_action()),
      l2rule_(copy.l2rule_->clone()),
      original_rule_(copy.get_original_rule()),
      type_(copy.get_type()),
      unknown_match_(copy.has_unknown_match()){}

UPF_Rule::~UPF_Rule(){
  delete(l2rule_);
}

UPF_Rule* UPF_Rule::clone() const {
  return new UPF_Rule(*this);
}

std::vector<FieldType> UPF_Rule::get_fieldkeys(){

    std::vector<FieldType> types;
    types.push_back(FieldType::l2_src);
    types.push_back(FieldType::l2_dst);
    types.push_back(FieldType::interface_in_field);
    types.push_back(FieldType::interface_out_field);
    types.push_back(FieldType::l3_proto);
    types.push_back(FieldType::l3_src);
    types.push_back(FieldType::l3_dst);
    types.push_back(FieldType::l4_proto);
    types.push_back(FieldType::l4_src);
    types.push_back(FieldType::l4_dst);
    types.push_back(FieldType::tcpflagfield);

    return types;
}

std::vector<DimensionType> UPF_Rule::get_dimkeys(){

    std::vector<DimensionType> types;
    types.push_back(DimensionType::macsrc);
    types.push_back(DimensionType::macdst);
    types.push_back(DimensionType::ipv4src);
    types.push_back(DimensionType::ipv4dst);
    types.push_back(DimensionType::ipv6src);
    types.push_back(DimensionType::ipv6dst);
    types.push_back(DimensionType::l4protocol);
    types.push_back(DimensionType::tcpsrcport);
    types.push_back(DimensionType::tcpdstport);
    types.push_back(DimensionType::udpdstport);
    types.push_back(DimensionType::udpsrcport);


    return types;
}

void UPF_Rule::set_unknown_match(bool unknown_match){
  unknown_match_ = unknown_match;
}

bool UPF_Rule::has_unknown_match() const{
  return unknown_match_;
}


const std::string& UPF_Rule::get_original_rule() const{
  return original_rule_;
}

void UPF_Rule::set_original_rule(const std::string& orig){
  original_rule_ = orig;
}

UPF_Type UPF_Rule::get_type() const {
  return type_;
}

void UPF_Rule::set_type(const UPF_Type type){
  type_ = type;
}

bool UPF_Rule::equal_to(const UPF_Rule& rhs) const{
  return (action_ == rhs.action_ && l2rule_->equal_to(*rhs.l2rule_));
}

bool UPF_Rule::covers(const UPF_Rule& rhs) const{
  bool covers = true;
  for (int cnt = l2_src; cnt < tcpflagfield; ++cnt){
    Field* this_field = this->get_field((FieldType)cnt);
    Field* other_field = rhs.get_field((FieldType)cnt);
    if(this_field != nullptr && other_field != nullptr){
      if(!(this_field->covers(other_field))){
        covers = false;
        break;
      }
    }
    else if (other_field == nullptr && this_field != nullptr){
      covers = false;
      break;
    }
  }
  return covers;
}

bool UPF_Rule::intersect(const UPF_Rule& rhs) const{
  bool intersect = false;
  for (int cnt = l2_src; cnt < tcpflagfield; ++cnt){
    Field* this_field = this->get_field((FieldType)cnt);
    Field* other_field = rhs.get_field((FieldType)cnt);
    if(this_field != nullptr && other_field != nullptr){
      if((this_field->intersect(other_field))){
        intersect = true;
        break;
      }
    }
    else
      intersect = true;
  }
  return intersect;
}

Action UPF_Rule::get_action() const {
  return action_;
}

void UPF_Rule::set_action(const ActionTypes& action){
  if( action == ActionTypes::ACCEPT)
    action_ = Accept();
  if(action == ActionTypes::BLOCK)
    action_ = Block();
  if(action == ActionTypes::JUMP)
    action_ = Jump("DEFAULT");
}

void UPF_Rule::set_action(const Jump& jump){
  action_ = Jump(jump.get_target());
}

ProtocolType UPF_Rule::get_l3_protocol() const{
  return l2rule_->l3rule_->get_proto_field()->get_protocol();
}

ProtocolType UPF_Rule::get_l4_protocol() const{
  return l2rule_->l3rule_->l4rule_->get_proto_field()->get_protocol();
}

Field* UPF_Rule::get_field(const DimensionType dim) const{
  switch(dim){
    case macsrc:{
      if(l2rule_->get_type() == macrule)
        return static_cast<MacRule*>(l2rule_)->get_src_mac();
      else
        return nullptr;
    }
    case macdst:{
      if(l2rule_->get_type() == macrule)
        return static_cast<MacRule*>(l2rule_)->get_dst_mac();
      else
        return nullptr;
    }
    case ipv4src:{
      RuleTypes l3r = l2rule_->l3rule_->get_type();
      if(l3r == ipv4rule)
        return static_cast<IPv4Rule*>(l2rule_->l3rule_)->get_src_ip();
      else
        return nullptr;
    }
    case ipv4dst:{
      RuleTypes l3r = l2rule_->l3rule_->get_type();
      if(l3r == ipv4rule)
        return static_cast<IPv4Rule*>(l2rule_->l3rule_)->get_dst_ip();
      else
        return nullptr;
    }
    case ipv6src:{
      RuleTypes l3r = l2rule_->l3rule_->get_type();
      if(l3r == ipv6rule)
        return static_cast<IPv6Rule*>(l2rule_->l3rule_)->get_src_ip();
      else
        return nullptr;
    }
    case ipv6dst:{
      RuleTypes l3r = l2rule_->l3rule_->get_type();
      if(l3r == ipv6rule)
        return static_cast<IPv6Rule*>(l2rule_->l3rule_)->get_dst_ip();
      else
        return nullptr;
    }
    case l4protocol:{
      return l2rule_->l3rule_->l4rule_->l4prot_;
    }
    case udpsrcport:{
      RuleTypes l4r = l2rule_->l3rule_->l4rule_->get_type();
      if (l4r == udprule)
        return static_cast<UDPRule*>(l2rule_->l3rule_->l4rule_)->get_src_port();
      else
        return nullptr;
    }
    case udpdstport:{
      RuleTypes l4r = l2rule_->l3rule_->l4rule_->get_type();
      if (l4r == udprule)
        return static_cast<UDPRule*>(l2rule_->l3rule_->l4rule_)->get_dst_port();
      else
        return nullptr;
    }
    case tcpsrcport:{
      RuleTypes l4r = l2rule_->l3rule_->l4rule_->get_type();
      if (l4r == tcprule)
        return static_cast<TCPRule*>(l2rule_->l3rule_->l4rule_)->get_src_port();
      else
        return nullptr;
    }
    case tcpdstport:{
      RuleTypes l4r = l2rule_->l3rule_->l4rule_->get_type();
      if (l4r == tcprule)
        return static_cast<TCPRule*>(l2rule_->l3rule_->l4rule_)->get_dst_port();
      else
        return nullptr;
    }
  }
  return nullptr;
}

Field* UPF_Rule::get_field(const FieldType field) const{
    switch(field){
      case l2_src:{
          if(l2rule_->get_type() == macrule)
          return static_cast<MacRule*>(l2rule_)->get_src_mac();
        else
          return nullptr;
      }
      case l2_dst:{
        if(l2rule_->get_type() == macrule)
          return static_cast<MacRule*>(l2rule_)->get_dst_mac();
        else
          return nullptr;
      }
      case l3_proto:{
        return l2rule_->l3rule_->get_proto_field();
      }
      case l3_src:{
        RuleTypes l3r = l2rule_->l3rule_->get_type();
        if(l3r == ipv4rule)
          return static_cast<IPv4Rule*>(l2rule_->l3rule_)->get_src_ip();
        else if(l3r == ipv6rule)
          return static_cast<IPv6Rule*>(l2rule_->l3rule_)->get_src_ip();
        else
          return nullptr;
      }
      case l3_dst:{
        RuleTypes l3r = l2rule_->l3rule_->get_type();
        if(l3r == ipv4rule)
          return static_cast<IPv4Rule*>(l2rule_->l3rule_)->get_dst_ip();
        else if(l3r == ipv6rule)
          return static_cast<IPv6Rule*>(l2rule_->l3rule_)->get_dst_ip();
        else
          return nullptr;
      }
      case l4_proto:{
        return l2rule_->l3rule_->l4rule_->get_proto_field();
      }
      case l4_src:{
        RuleTypes l4r = l2rule_->l3rule_->l4rule_->get_type();
        if (l4r == tcprule)
          return static_cast<TCPRule*>(l2rule_->l3rule_->l4rule_)->get_src_port();
        else if (l4r == udprule)
          return static_cast<UDPRule*>(l2rule_->l3rule_->l4rule_)->get_src_port();
        else if (l4r == icmprule)
          return static_cast<ICMPRule*>(l2rule_->l3rule_->l4rule_)->get_icmptype();
        else
          return nullptr;
      }
      case l4_dst:{
        RuleTypes l4r = l2rule_->l3rule_->l4rule_->get_type();
        if (l4r == tcprule)
          return static_cast<TCPRule*>(l2rule_->l3rule_->l4rule_)->get_dst_port();
        else if (l4r == udprule)
          return static_cast<UDPRule*>(l2rule_->l3rule_->l4rule_)->get_dst_port();
        else if (l4r == icmprule)
          return static_cast<ICMPRule*>(l2rule_->l3rule_->l4rule_)->get_icmpcode();
        else
          return nullptr;
      }
      case interface_in_field: {
        if(l2rule_->interface_in_ != nullptr)
          return l2rule_->interface_in_;
        else
          return nullptr;
      }
      case interface_out_field: {
        if(l2rule_->interface_out_ != nullptr)
          return l2rule_->interface_out_;
        else
          return nullptr;
      }
      case tcpflagfield: {
        L4Rule* l4r = l2rule_->l3rule_->l4rule_;
        if(l4r->get_type() == tcprule)
          return static_cast<const TCPRule*>(l4r)->get_flags();
        else
          return nullptr;
      }
      case icmptypefield: {
        L4Rule* l4r = l2rule_->l3rule_->l4rule_;
        if(l4r->get_type() == icmprule)
          return static_cast<const ICMPRule*>(l4r)->get_icmptype();
        else
          return nullptr;
      }
      case icmpcodefield:{
        L4Rule* l4r = l2rule_->l3rule_->l4rule_;
        if(l4r->get_type() == icmprule)
          return static_cast<const ICMPRule*>(l4r)->get_icmpcode();
        else
          return nullptr;
      }
      default: return nullptr;
    }
    return nullptr;
}

void UPF_Rule::set_field(const FieldType type, const ProtocolType protocol,
                         const Field* field){

  switch(type){
    case l2_src:{
      if(protocol == ETHERNET){
        const MacField* mac = static_cast<const MacField*>(field);
        if(l2rule_->get_type() != macrule){
          delete(l2rule_);
          l2rule_ = new MacRule();
        }
        static_cast<MacRule*>(l2rule_)->set_src_mac(*mac);
      }
      break;
    }
    case l2_dst:{
      if(protocol == ETHERNET){
        MacField mac = (*(static_cast<const MacField*>(field)));
        if(l2rule_->get_type() != macrule){
          delete(l2rule_);
          l2rule_ = new MacRule();
        }
        static_cast<MacRule*>(l2rule_)->set_dst_mac(mac);
      }
      break;
    }
    case l3_proto:{
      if(protocol == IPV4){
        delete(l2rule_->l3rule_->l3prot_);
        l2rule_->l3rule_->l3prot_ = new L3ProtocolField(IPV4,Range(4,4));
      }
      else if(protocol == IPV6){
        delete(l2rule_->l3rule_->l3prot_);
        l2rule_->l3rule_->l3prot_ = new L3ProtocolField(IPV6, Range(6,6));
      }
      break;
    }
    case l3_src:{
      if(protocol == IPV4){
        IPv4Field ipv4 = (*(static_cast<const IPv4Field*>(field)));
        if(l2rule_->l3rule_->get_type() != ipv4rule){
          L4Rule* temp = new L4Rule(*(l2rule_->l3rule_->l4rule_));
          delete(l2rule_->l3rule_);
          l2rule_->l3rule_ = new IPv4Rule();
          delete(l2rule_->l3rule_->l4rule_);
          l2rule_->l3rule_->l4rule_ = temp;
        }
        static_cast<IPv4Rule*>(l2rule_->l3rule_)->set_src_ip(ipv4);
      }
      else if(protocol == IPV6){
        IPv6Field ipv6 = (*(static_cast<const IPv6Field*>(field)));
        if(l2rule_->l3rule_->get_type() != ipv6rule){
          L4Rule* temp = new L4Rule(*(l2rule_->l3rule_->l4rule_));
          delete(l2rule_->l3rule_);
          l2rule_->l3rule_ = new IPv6Rule();
          delete(l2rule_->l3rule_->l4rule_);
          l2rule_->l3rule_->l4rule_ = temp;
        }
      static_cast<IPv6Rule*>(l2rule_->l3rule_)->set_src_ip(ipv6);
      }
      break;
    }
    case l3_dst:{
      if(protocol == IPV4){
        IPv4Field ipv4 = (*(static_cast<const IPv4Field*>(field)));
        if(l2rule_->l3rule_->get_type() != ipv4rule){
          L4Rule* temp = new L4Rule(*(l2rule_->l3rule_->l4rule_));
          delete(l2rule_->l3rule_);
          l2rule_->l3rule_ = new IPv4Rule();
          delete(l2rule_->l3rule_->l4rule_);
          l2rule_->l3rule_->l4rule_ = temp;
        }
        static_cast<IPv4Rule*>(l2rule_->l3rule_)->set_dst_ip(ipv4);
      }
      else if(protocol == IPV6){
        IPv6Field ipv6 = (*(static_cast<const IPv6Field*>(field)));
        if(l2rule_->l3rule_->get_type() != ipv6rule){
          L4Rule* temp = new L4Rule(*(l2rule_->l3rule_->l4rule_));
          delete(l2rule_->l3rule_);
          l2rule_->l3rule_ = new IPv6Rule();
          delete(l2rule_->l3rule_->l4rule_);
          l2rule_->l3rule_->l4rule_ = temp;
        }
        static_cast<IPv6Rule*>(l2rule_->l3rule_)->set_dst_ip(ipv6);
      }
      break;
    }
    case l4_proto:{
      if(protocol == TCP){
        delete(l2rule_->l3rule_->l4rule_->l4prot_);
        l2rule_->l3rule_->l4rule_->l4prot_ = new L4ProtocolField(TCP,Range(6,6));
      }
      else if(protocol == UDP){
        delete(l2rule_->l3rule_->l4rule_->l4prot_);
        l2rule_->l3rule_->l4rule_->l4prot_ = new L4ProtocolField(UDP,Range(17,17));
      }
      else if(protocol == ICMP){
        delete(l2rule_->l3rule_->l4rule_->l4prot_);
        l2rule_->l3rule_->l4rule_->l4prot_ = new L4ProtocolField(ICMP,Range(1,1));
      }
      else if(protocol == GRE){
        delete(l2rule_->l3rule_->l4rule_->l4prot_);
        l2rule_->l3rule_->l4rule_->l4prot_ = new L4ProtocolField(ICMP,Range(47,47));
      }
      break;
    }
    case l4_src:{
      if(protocol == UDP){
        PortField port = (*(static_cast<const PortField*>(field)));
        if(l2rule_->l3rule_->l4rule_->get_type() != udprule){
          delete(l2rule_->l3rule_->l4rule_);
          l2rule_->l3rule_->l4rule_ = new UDPRule();
        }
        static_cast<UDPRule*>(l2rule_->l3rule_->l4rule_)->set_src_port(port);
      }
      else if(protocol == TCP){
        PortField port = (*(static_cast<const PortField*>(field)));
        if(l2rule_->l3rule_->l4rule_->get_type() != tcprule){
          delete(l2rule_->l3rule_->l4rule_);
          l2rule_->l3rule_->l4rule_ = new TCPRule();
        }
        static_cast<TCPRule*>(l2rule_->l3rule_->l4rule_)->set_src_port(port);
      }
      else if(protocol == ICMP){
        ICMPTypeField icmptype = (*(static_cast<const ICMPTypeField*>(field)));
        if(l2rule_->l3rule_->l4rule_->get_type() != icmprule){
          delete(l2rule_->l3rule_->l4rule_);
          l2rule_->l3rule_->l4rule_ = new ICMPRule();
        }
        static_cast<ICMPRule*>(l2rule_->l3rule_->l4rule_)->set_icmptype(icmptype);
      }
      else if (protocol == GRE){
        PortField port = (*(static_cast<const PortField*>(field)));
        if(l2rule_->l3rule_->l4rule_->get_type() != udprule){
          delete(l2rule_->l3rule_->l4rule_);
          l2rule_->l3rule_->l4rule_ = new UDPRule();
        }
        static_cast<UDPRule*>(l2rule_->l3rule_->l4rule_)->set_src_port(port);
      }
      break;
    }
    case l4_dst:{
      if (protocol == UDP){
        PortField port = (*(static_cast<const PortField*>(field)));
        if(l2rule_->l3rule_->l4rule_->get_type() != udprule){
          delete(l2rule_->l3rule_->l4rule_);
          l2rule_->l3rule_->l4rule_ = new UDPRule();
        }
        static_cast<UDPRule*>(l2rule_->l3rule_->l4rule_)->set_dst_port(port);
      }
      else if(protocol == TCP){
        PortField port = (*(static_cast<const PortField*>(field)));
        if(l2rule_->l3rule_->l4rule_->get_type() != tcprule){
          delete(l2rule_->l3rule_->l4rule_);
          l2rule_->l3rule_->l4rule_ = new TCPRule();
        }
        static_cast<TCPRule*>(l2rule_->l3rule_->l4rule_)->set_dst_port(port);
      }
      else if(protocol == ICMP){
        ICMPCodeField icmpcode = (*(static_cast<const ICMPCodeField*>(field)));
        if(l2rule_->l3rule_->l4rule_->get_type() != icmprule){
          delete(l2rule_->l3rule_->l4rule_);
          l2rule_->l3rule_->l4rule_ = new ICMPRule();
        }
        static_cast<ICMPRule*>(l2rule_->l3rule_->l4rule_)->set_icmpcode(icmpcode);
      }
      else if(protocol == GRE){
        PortField port = (*(static_cast<const PortField*>(field)));
        if(l2rule_->l3rule_->l4rule_->get_type() != udprule){
          delete(l2rule_->l3rule_->l4rule_);
          l2rule_->l3rule_->l4rule_ = new UDPRule();
        }
        static_cast<UDPRule*>(l2rule_->l3rule_->l4rule_)->set_dst_port(port);
      }
      break;
    }
    case interface_in_field:{
      if(protocol == ETHERNET){
        InterfaceField iface = (*(static_cast<const InterfaceField*>(field)));
        l2rule_->set_interface_in(iface);
      }
      break;
    }
    case interface_out_field:{
      if(protocol == ETHERNET){
        InterfaceField iface = (*(static_cast<const InterfaceField*>(field)));
        l2rule_->set_interface_out(iface);
      }
      break;
    }
    case tcpflagfield:{
      if (protocol == TCP){
        TCPFlagField flag = (*(static_cast<const TCPFlagField*>(field)));
          if(l2rule_->l3rule_->l4rule_->get_type() != tcprule){
            delete (l2rule_->l3rule_->l4rule_);
            l2rule_->l3rule_->l4rule_ = new TCPRule();
          }
          static_cast<TCPRule*>(l2rule_->l3rule_->l4rule_)->set_flags(flag);
      }
      break;
    }
    case icmptypefield:{
      if (protocol == ICMP){
       ICMPTypeField icmp_type = (*(static_cast<const ICMPTypeField*>(field)));
        if(l2rule_->l3rule_->l4rule_->get_type() != icmprule){
          delete(l2rule_->l3rule_->l4rule_);
          l2rule_->l3rule_->l4rule_ = new ICMPRule();
        }
        static_cast<ICMPRule*>(l2rule_->l3rule_->l4rule_)
                               ->set_icmptype(icmp_type);
      }
      break;
    }
    case icmpcodefield:{
      if (protocol == ICMP){
        ICMPCodeField icmp_code = (*(static_cast<const ICMPCodeField*>(field)));
        if(l2rule_->l3rule_->l4rule_->get_type() != icmprule){
          delete(l2rule_->l3rule_->l4rule_);
          l2rule_->l3rule_->l4rule_ = new ICMPRule();
        }
        static_cast<ICMPRule*>(l2rule_->l3rule_->l4rule_)
                               ->set_icmpcode(icmp_code);
      }
      break;
    }
    default: {} // Silence Compiler
  }
}

void UPF_Rule::set_field(const FieldType type, const Field* field){
    ProtocolType protocol = field->get_protocol();

    switch(type){
    case l2_src:{
        if(protocol == ETHERNET){
            const MacField* mac = static_cast<const MacField*>(field);
            if(l2rule_->get_type() != macrule){
                delete(l2rule_);
                l2rule_ = new MacRule();
            }
            static_cast<MacRule*>(l2rule_)->set_src_mac(*mac);
        }
        break;
    }
    case l2_dst:{
        if(protocol == ETHERNET){
            MacField mac = (*(static_cast<const MacField*>(field)));
            if(l2rule_->get_type() != macrule){
                delete(l2rule_);
                l2rule_ = new MacRule();
            }
            static_cast<MacRule*>(l2rule_)->set_dst_mac(mac);
        }
        break;
    }
    case l3_proto:{

       if(protocol == IPV4){
            delete(l2rule_->l3rule_->l3prot_);
            l2rule_->l3rule_->l3prot_ = new L3ProtocolField(IPV4,Range(4,4));
        }
        else if(protocol == IPV6){
            delete(l2rule_->l3rule_->l3prot_);
            l2rule_->l3rule_->l3prot_ = new L3ProtocolField(IPV6, Range(6,6));
        }
        break;
    }
    case l3_src:{
        if(protocol == IPV4){
            IPv4Field ipv4 = (*(static_cast<const IPv4Field*>(field)));
            if(l2rule_->l3rule_->get_type() != ipv4rule){
                L4Rule* temp = new L4Rule(*(l2rule_->l3rule_->l4rule_));
                delete(l2rule_->l3rule_);
                l2rule_->l3rule_ = new IPv4Rule();
                delete(l2rule_->l3rule_->l4rule_);
                l2rule_->l3rule_->l4rule_ = temp;

            }
            static_cast<IPv4Rule*>(l2rule_->l3rule_)->set_src_ip(ipv4);
        }
        else if(protocol == IPV6){
            IPv6Field ipv6 = (*(static_cast<const IPv6Field*>(field)));
            if(l2rule_->l3rule_->get_type() != ipv6rule){
                L4Rule* temp = new L4Rule(*(l2rule_->l3rule_->l4rule_));
                delete(l2rule_->l3rule_);
                l2rule_->l3rule_ = new IPv6Rule();
                delete(l2rule_->l3rule_->l4rule_);
                l2rule_->l3rule_->l4rule_ = temp;

            }
            static_cast<IPv6Rule*>(l2rule_->l3rule_)->set_src_ip(ipv6);
        }
        break;
        }
    case l3_dst:{
        if(protocol == IPV4){
            IPv4Field ipv4 = (*(static_cast<const IPv4Field*>(field)));
            if(l2rule_->l3rule_->get_type() != ipv4rule){
                L4Rule* temp = new L4Rule(*(l2rule_->l3rule_->l4rule_));
                delete(l2rule_->l3rule_);
                l2rule_->l3rule_ = new IPv4Rule();
                delete(l2rule_->l3rule_->l4rule_);
                l2rule_->l3rule_->l4rule_ = temp;

            }
            static_cast<IPv4Rule*>(l2rule_->l3rule_)->set_dst_ip(ipv4);
        }
        else if(protocol == IPV6){
            IPv6Field ipv6 = (*(static_cast<const IPv6Field*>(field)));
            if(l2rule_->l3rule_->get_type() != ipv6rule){
                L4Rule* temp = new L4Rule(*(l2rule_->l3rule_->l4rule_));
                delete(l2rule_->l3rule_);
                l2rule_->l3rule_ = new IPv6Rule();
                delete(l2rule_->l3rule_->l4rule_);
                l2rule_->l3rule_->l4rule_ = temp;

            }
            static_cast<IPv6Rule*>(l2rule_->l3rule_)->set_dst_ip(ipv6);
        }
        break;
        }
    case l4_proto:{
        if(protocol == TCP){
            delete(l2rule_->l3rule_->l4rule_->l4prot_);
            l2rule_->l3rule_->l4rule_->l4prot_ = new L4ProtocolField(TCP,Range(6,6));
        }
        else if (protocol == UDP){
            delete(l2rule_->l3rule_->l4rule_->l4prot_);
            l2rule_->l3rule_->l4rule_->l4prot_ = new L4ProtocolField(UDP,Range(17,17));
        }
        else if (protocol == ICMP){
            delete(l2rule_->l3rule_->l4rule_->l4prot_);
            l2rule_->l3rule_->l4rule_->l4prot_ = new L4ProtocolField(ICMP,Range(1,1));
        }
        else if (protocol == GRE){
            delete(l2rule_->l3rule_->l4rule_->l4prot_);
            l2rule_->l3rule_->l4rule_->l4prot_ = new L4ProtocolField(ICMP,Range(47,47));
        }
        else
          delete(l2rule_->l3rule_->l4rule_->l4prot_);
          l2rule_->l3rule_->l4rule_->l4prot_ = new L4ProtocolField(field->get_range());
        break;
    }
    case l4_src:{
        if (protocol == UDP){
            PortField port = (*(static_cast<const PortField*>(field)));
            if(l2rule_->l3rule_->l4rule_->get_type() != udprule){
                delete(l2rule_->l3rule_->l4rule_);
                l2rule_->l3rule_->l4rule_ = new UDPRule();
            }
            static_cast<UDPRule*>(l2rule_->l3rule_->l4rule_)->set_src_port(port);
        }
        else if(protocol == TCP){
            PortField port = (*(static_cast<const PortField*>(field)));
            if(l2rule_->l3rule_->l4rule_->get_type() != tcprule){
                delete(l2rule_->l3rule_->l4rule_);
                l2rule_->l3rule_->l4rule_ = new TCPRule();
            }
            static_cast<TCPRule*>(l2rule_->l3rule_->l4rule_)->set_src_port(port);
        }
        else if(protocol == ICMP){
            ICMPTypeField icmptype = (*(static_cast<const ICMPTypeField*>(field)));
            if(l2rule_->l3rule_->l4rule_->get_type() != icmprule){
                delete(l2rule_->l3rule_->l4rule_);
                l2rule_->l3rule_->l4rule_ = new ICMPRule();
            }
            static_cast<ICMPRule*>(l2rule_->l3rule_->l4rule_)->set_icmptype(icmptype);
        }
        else if (protocol == GRE){
            PortField port = (*(static_cast<const PortField*>(field)));
            if(l2rule_->l3rule_->l4rule_->get_type() != udprule){
                delete(l2rule_->l3rule_->l4rule_);
                l2rule_->l3rule_->l4rule_ = new UDPRule();
            }
            static_cast<UDPRule*>(l2rule_->l3rule_->l4rule_)->set_src_port(port);
        }
        break;
    }
    case l4_dst:{
        if (protocol == UDP){
            PortField port = (*(static_cast<const PortField*>(field)));
            if(l2rule_->l3rule_->l4rule_->get_type() != udprule){
                delete(l2rule_->l3rule_->l4rule_);
                l2rule_->l3rule_->l4rule_ = new UDPRule();
            }
            static_cast<UDPRule*>(l2rule_->l3rule_->l4rule_)->set_dst_port(port);
        }
        else if(protocol == TCP){
            PortField port = (*(static_cast<const PortField*>(field)));
            if(l2rule_->l3rule_->l4rule_->get_type() != tcprule){
                delete(l2rule_->l3rule_->l4rule_);
                l2rule_->l3rule_->l4rule_ = new TCPRule();
            }
            static_cast<TCPRule*>(l2rule_->l3rule_->l4rule_)->set_dst_port(port);
        }
        else if(protocol == ICMP){
            ICMPCodeField icmpcode = (*(static_cast<const ICMPCodeField*>(field)));
            if(l2rule_->l3rule_->l4rule_->get_type() != icmprule){
                delete(l2rule_->l3rule_->l4rule_);
                l2rule_->l3rule_->l4rule_ = new ICMPRule();
            }
            static_cast<ICMPRule*>(l2rule_->l3rule_->l4rule_)->set_icmpcode(icmpcode);
        }
        else if(protocol == GRE){
            PortField port = (*(static_cast<const PortField*>(field)));
            if(l2rule_->l3rule_->l4rule_->get_type() != udprule){
                delete(l2rule_->l3rule_->l4rule_);
                l2rule_->l3rule_->l4rule_ = new UDPRule();
            }
            static_cast<UDPRule*>(l2rule_->l3rule_->l4rule_)->set_dst_port(port);
        }
        break;
    }
    case interface_in_field:{
        if(protocol == ETHERNET){
            InterfaceField iface = (*(static_cast<const InterfaceField*>(field)));
            l2rule_->set_interface_in(iface);
        }
        break;
    }
    case interface_out_field:{
        if(protocol == ETHERNET){
            InterfaceField iface = (*(static_cast<const InterfaceField*>(field)));
            l2rule_->set_interface_out(iface);
        }
        break;
    }
    case tcpflagfield:{
        if (protocol == TCP){
            TCPFlagField flag = (*(static_cast<const TCPFlagField*>(field)));
            if(l2rule_->l3rule_->l4rule_->get_type() != tcprule)
                delete (l2rule_->l3rule_->l4rule_);
                l2rule_->l3rule_->l4rule_ = new TCPRule();
            static_cast<TCPRule*>(l2rule_->l3rule_->l4rule_)->set_flags(flag);
        }
        break;
    }
    case icmptypefield:{
        if (protocol == ICMP){
            ICMPTypeField icmp_type = (*(static_cast<const ICMPTypeField*>(field)));
            if(l2rule_->l3rule_->l4rule_->get_type() != icmprule){
                delete(l2rule_->l3rule_->l4rule_);
                l2rule_->l3rule_->l4rule_ = new ICMPRule();
            }
            static_cast<ICMPRule*>(l2rule_->l3rule_->l4rule_)
                                    ->set_icmptype(icmp_type);
        }
        break;
    }
    case icmpcodefield:{
        if (protocol == ICMP){
            ICMPCodeField icmp_code = (*(static_cast<const ICMPCodeField*>(field)));
            if(l2rule_->l3rule_->l4rule_->get_type() != icmprule){
                delete(l2rule_->l3rule_->l4rule_);
                l2rule_->l3rule_->l4rule_ = new ICMPRule();
            }
            static_cast<ICMPRule*>(l2rule_->l3rule_->l4rule_)
            ->set_icmpcode(icmp_code);
        }
        break;
    }
    default: {} // Silence Compiler
    }
}

void UPF_Rule::set_field(const DimensionType type, const Range& range){
    switch(type){
      case macsrc:{
          Field* field = new MacField(range);
          this->set_field(l2_src,field);
          delete(field);
          break;
      }
      case macdst:{
          Field* field = new MacField(range);
          this->set_field(l2_dst,field);
          delete(field);
            break;
      }
      case ipv4src:{
          Field* field = new IPv4Field(range);
          this->set_field(l3_src,IPV4,field);
          delete(field);
          break;
      }
      case ipv4dst:{
          Field* field = new IPv4Field(range);
          this->set_field(l3_dst,IPV4,field);
          delete(field);
          break;
      }
      case l4protocol:{
          Field* l4field = new L4ProtocolField(range);
          this->set_field(l4_proto, l4field);
          delete(l4field);
          break;
      }
      case ipv6src:{
          Field* field = new IPv6Field(range);
          this->set_field(l3_src,IPV6, field);
          delete(field);
          break;
      }
      case ipv6dst:{
          Field* field = new IPv6Field(range);
          this->set_field(l3_dst,IPV6, field);
          delete(field);
          break;
      }
      case tcpsrcport:{
          Field* field = new PortField(range,TCP);
          this->set_field(l4_src,TCP,field);
          delete(field);
          break;
      }
      case tcpdstport:{
          Field* field = new PortField(range,TCP);
          this->set_field(l4_dst,TCP,field);
          delete(field);
          break;
      }
      case udpsrcport:{
          Field* field = new PortField(range,UDP);
          this->set_field(l4_src,UDP,field);
          delete(field);
          break;
      }
      case udpdstport:{
          Field* field = new PortField(range,UDP);
          this->set_field(l4_dst,UDP,field);
          delete(field);
          break;
      }
      default: break;
    }
}


void UPF_Rule::set_range_on_field(const FieldType field_key,
                                      const Range& range){

    switch(field_key){
    case l2_src:{
        if(l2rule_->get_type() == macrule){
            static_cast<MacRule*>(l2rule_)->get_src_mac()->set_range(range);
        }
        break;
    }
    case l2_dst:{
        if(l2rule_->get_type() == macrule){
            static_cast<MacRule*>(l2rule_)->get_dst_mac()->set_range(range);
        }
        break;
    }
    case l3_proto:{
            l2rule_->l3rule_->l3prot_->set_range(range);
            break;
    }
    case l3_src:{
        RuleTypes type = l2rule_->l3rule_->get_type();
        if(type == ipv4rule){
            static_cast<IPv4Rule*>(l2rule_->l3rule_)->get_src_ip()
            ->set_range(range);
        }
        else if(type == ipv6rule){
            static_cast<IPv6Rule*>(l2rule_->l3rule_)->get_src_ip()
            ->set_range(range);
        }
        break;
    }
    case l3_dst:{
        RuleTypes type = l2rule_->l3rule_->get_type();
        if(type == ipv4rule){
            static_cast<IPv4Rule*>(l2rule_->l3rule_)->get_dst_ip()
            ->set_range(range);
        }
        else if(type == ipv6rule){
            static_cast<IPv6Rule*>(l2rule_->l3rule_)->get_dst_ip()
            ->set_range(range);
        }
        break;
    }
    case l4_proto:{
            l2rule_->l3rule_->l4rule_->l4prot_->set_range(range);
            break;
    }
    case l4_src:{
        RuleTypes type = l2rule_->l3rule_->l4rule_->get_type();
        if(type == tcprule){
            static_cast<TCPRule*>(l2rule_->l3rule_->l4rule_)
                ->get_src_port()->set_range(range);
        }
        else if(type == udprule){
            static_cast<UDPRule*>(l2rule_->l3rule_->l4rule_)
                ->get_src_port()->set_range(range);
        }
        else if(type == icmprule){
            static_cast<ICMPRule*>(l2rule_->l3rule_->l4rule_)
                ->get_icmptype()->set_range(range);
        }
        break;
    }
    case l4_dst:{
        RuleTypes type = l2rule_->l3rule_->l4rule_->get_type();
        if(type == tcprule){
            static_cast<TCPRule*>(l2rule_->l3rule_->l4rule_)
                ->get_dst_port()->set_range(range);
        }
        else if(type == udprule){
            static_cast<UDPRule*>(l2rule_->l3rule_->l4rule_)
                ->get_dst_port()->set_range(range);
        }
        else if(type == icmprule){
            static_cast<ICMPRule*>(l2rule_->l3rule_->l4rule_)
                ->get_icmpcode()->set_range(range);
        }
        break;
    }
    default:{break;} // Silence Compiler Warnigns
    }
}

void UPF_Rule::set_field(const FieldType field, const Range& range){

  try{
  if (field == l2_src || field == l2_dst){
    if(l2rule_->get_type() != macrule)
      l2rule_ = static_cast<MacRule*>(l2rule_);
  }
  else
    l2rule_->set_field(field, range);
  }
  catch (std::string* error)
  {
    std::cout << " Exception in UPFRule: " << *error << std::endl;
  }
}



std::vector<Range> UPF_Rule::get_ranges_next_from(const FieldType field)const{

    std::vector<FieldType> types = UPF_Rule::get_fieldkeys();

    std::vector<Range> ranges;
    if(field <= tcpflagfield){
      for(auto iter = types.begin()+ field +1; iter != types.end(); ++iter){
          Field* current_field = this->get_field(*iter);
          if (current_field == nullptr){
              ranges.push_back(Range( uint128_t::min() , uint128_t::max() ));
          }
          else{
              ranges.push_back(current_field->get_range());
          }
      }
    }
    return ranges;
}
std::ostream& UPF_Rule::print(std::ostream& out) const{
    std::vector<FieldType> types;
    types.push_back(FieldType::l2_src);
    types.push_back(FieldType::l2_dst);
    types.push_back(FieldType::interface_in_field);
    types.push_back(FieldType::interface_out_field);
    types.push_back(FieldType::l3_proto);
    types.push_back(FieldType::l3_src);
    types.push_back(FieldType::l3_dst);
    types.push_back(FieldType::l4_proto);
    types.push_back(FieldType::l4_src);
    types.push_back(FieldType::l4_dst);
    types.push_back(FieldType::tcpflagfield);
    std::vector<Range> ranges;
    unsigned int counter = 0;
    for (auto iter = types.begin(); iter != types.end(); ++iter){
        out << "Field " << counter << ":    ";
        Field* current_field = this->get_field(*iter);
        if (current_field == nullptr)
            out << Range(uint128_t::min(), uint128_t::max()) << std::endl;
        else
            out << this->get_field(*iter)->get_range() << std::endl;
        counter++;
    }
    out << action_ << std::endl;
    if(this->unknown_match_)
      out << "Unknown Match = Yes" << std::endl;
    else
      out << "Unknown Match = No" << std::endl;
    return out;
}


// L2 Rule
L2Rule::L2Rule()
    :   l3rule_(new L3Rule()),interface_in_(nullptr),interface_out_(nullptr){}

L2Rule::L2Rule(const L2Rule& copy)
    :   l3rule_(copy.l3rule_->clone()),
        interface_in_(nullptr),
        interface_out_(nullptr){

    if(copy.interface_in_ != nullptr)
        interface_in_ = copy.interface_in_->clone();
    if(copy.interface_out_ != nullptr)
        interface_out_ = copy.interface_out_->clone();
}

L2Rule::~L2Rule(){
    if(l3rule_ != nullptr)
        delete l3rule_;
    if(interface_in_ != nullptr)
        delete interface_in_;
    if(interface_out_ != nullptr)
        delete interface_out_;
}

L2Rule* L2Rule::clone() const{return new L2Rule(*this);}

RuleTypes L2Rule::get_type() const{return RuleTypes::l2rule;}

void L2Rule::set_interface_in(const InterfaceField& interface){

    interface_in_ = new InterfaceField(interface);
}

void L2Rule::set_interface_out(const InterfaceField& interface){

    interface_out_ = new InterfaceField(interface);
}

Field* L2Rule::get_interface_in() const{

    return interface_in_;
}
Field* L2Rule::get_interface_out() const{

    return interface_out_;
}
bool L2Rule::equal_to(const L2Rule& rhs) const{

    return (l3rule_->equal_to(*rhs.l3rule_));
}

bool L2Rule::covers(const L2Rule& rhs) const{

    bool interface_in_match = false;
    bool interface_out_match = false;

    if ((interface_in_ == nullptr) && (rhs.interface_in_ == nullptr))
        interface_in_match = true;
    if ((interface_out_ == nullptr) && (rhs.interface_out_ == nullptr))
        interface_out_match = true;
    if((interface_in_ != nullptr) && (rhs.interface_in_ != nullptr)){
        if(*(interface_in_) == (*(rhs.interface_in_)))
            interface_in_match = true;
    }
    if((interface_out_ != nullptr) && (rhs.interface_out_ != nullptr)){
        if(*(interface_out_) == (*(rhs.interface_out_)))
            interface_out_match = true;
    }
    return (interface_in_match && interface_out_match);
}

void L2Rule::set_field(const FieldType field, const Range& range){

  if (field == l3_proto){
    if(range.lower_.get_low() == 4){
      IPv4Rule* new_rule = new IPv4Rule(*l3rule_);
      delete(l3rule_);
      l3rule_ = new_rule;
    }
    else if (range.lower_.get_low() == 41){
      IPv6Rule* new_rule = new IPv6Rule(*l3rule_);
      delete(l3rule_);
      l3rule_ = new_rule;
    }
    else{
      L3Rule* new_rule = new L3Rule();
      delete(l3rule_);
      l3rule_ = new_rule;
    }
  }
  else if (field == interface_in_field && !range.is_wildcard()){
    if(interface_in_ == nullptr)
      interface_in_ = new InterfaceField(range);
    else
      interface_in_->set_range(range);
  }
  else if (field == interface_out_field && !range.is_wildcard()){
    if(interface_out_ == nullptr)
      interface_out_ = new InterfaceField(range);
    else
      interface_out_->set_range(range);
  }
  else
    l3rule_->set_field(field, range);


}

// MacRule

MacRule::MacRule()
    :   src_mac_field_(nullptr), dst_mac_field_(nullptr),
        type_(RuleTypes::macrule){}

MacRule::MacRule(const MacRule& copy)
    :   L2Rule(copy),
        src_mac_field_(nullptr),
        dst_mac_field_(nullptr),
        type_(copy.type_){
        if(copy.src_mac_field_ != nullptr)
            src_mac_field_ = copy.src_mac_field_->clone();
        if (copy.dst_mac_field_ != nullptr)
            dst_mac_field_ = copy.dst_mac_field_->clone();
        }



MacRule::~MacRule(){
    if (src_mac_field_ != nullptr)
        delete(src_mac_field_);
    if  (dst_mac_field_ != nullptr)
        delete(dst_mac_field_);
}

MacRule* MacRule::clone() const{return new MacRule(*this);}

RuleTypes MacRule::get_type() const {return type_;}

bool MacRule::equal_to(const MacRule& rhs) const{
    bool src_mac_match = false;
    bool dst_mac_match = false;

    if(type_ != rhs.type_)
        return false;
    if ((src_mac_field_ == nullptr) && (rhs.src_mac_field_ == nullptr))
        src_mac_match = true;
    if ((dst_mac_field_ == nullptr) && (rhs.dst_mac_field_ == nullptr))
        dst_mac_match = true;
    if((src_mac_field_ != nullptr) && (rhs.src_mac_field_ != nullptr)){
        if(*(src_mac_field_) == (*(rhs.src_mac_field_)))
            src_mac_match = true;
    }
    if((dst_mac_field_ != nullptr) && (rhs.dst_mac_field_ != nullptr)){
        if(*(dst_mac_field_) == (*(rhs.dst_mac_field_)))
            dst_mac_match = true;
    }
    return (src_mac_match && dst_mac_match);
}

bool MacRule::covers(const MacRule& rhs) const{
    bool l3rule_cover = false;
    bool src_mac_cover = false;
    bool dst_mac_cover = false;
    if(type_ != rhs.type_)
        return false;
    if ((src_mac_field_ == nullptr) && (rhs.src_mac_field_ == nullptr))
        src_mac_cover = true;
    if ((dst_mac_field_ == nullptr) && (rhs.dst_mac_field_ == nullptr))
        dst_mac_cover = true;
    if((src_mac_field_ != nullptr) && (rhs.src_mac_field_ != nullptr)){
        if(src_mac_field_->covers(*(rhs.src_mac_field_)))
            src_mac_cover = true;
    }
    if((dst_mac_field_ != nullptr) && (rhs.dst_mac_field_ != nullptr)){
        if(dst_mac_field_->covers(*(rhs.dst_mac_field_)))
            dst_mac_cover = true;
    }
    if(l3rule_ == nullptr && rhs.l3rule_ != nullptr)
        l3rule_cover = true;

    if(l3rule_ == nullptr && rhs.l3rule_ == nullptr)
        l3rule_cover = true;
    else if (l3rule_ != nullptr && rhs.l3rule_ != nullptr){
        if(l3rule_->covers(*(rhs.l3rule_)))
            l3rule_cover = true;
    }
    return (src_mac_cover && dst_mac_cover && l3rule_cover);
}

void MacRule::set_src_mac(const MacField& field){
    if(src_mac_field_ != nullptr)
        src_mac_field_->set_range(field.get_range());
    else
        src_mac_field_ = new MacField(field);
}

void MacRule::set_dst_mac(const MacField& field){
    if(dst_mac_field_ != nullptr)
       dst_mac_field_->set_range(field.get_range());
    else
        dst_mac_field_ = new MacField(field);
}

Field* MacRule::get_src_mac() const{
    if(src_mac_field_ != nullptr)
        return src_mac_field_;
    else
        return nullptr;
}

Field* MacRule::get_dst_mac() const{
    if(dst_mac_field_ != nullptr)
        return dst_mac_field_;
    else
        return nullptr;
}

void MacRule::set_field(const FieldType field, const Range& range){

  if (field == l3_proto){
    if(range.lower_.get_low() == 4){
      IPv4Rule* new_rule = new IPv4Rule(*l3rule_);
      delete(l3rule_);
      l3rule_ = new_rule;
    }
    else if (range.lower_.get_low() == 41){
      IPv6Rule* new_rule = new IPv6Rule(*l3rule_);
      delete(l3rule_);
      l3rule_ = new_rule;
    }
    else{
      L3Rule* new_rule = new L3Rule();
      delete(l3rule_);
      l3rule_ = new_rule;
    }
    l3rule_->set_field(field, range);
  }

  switch(field){
  case l2_src:{
    if(src_mac_field_ == nullptr)
      src_mac_field_ = new MacField(range);
    else
      src_mac_field_->set_range(range);
    break;
  }
  case l2_dst:{
    if(dst_mac_field_ == nullptr)
      dst_mac_field_ = new MacField(range);
    else
      dst_mac_field_->set_range(range);
    break;
  }
  case interface_in_field:{
    if(interface_in_ == nullptr && !range.is_wildcard())
      interface_in_ = new InterfaceField(range);
    else
      interface_in_->set_range(range);
    break;
  }
  case interface_out_field:{
    if(interface_out_ == nullptr && !range.is_wildcard())
      interface_out_ = new InterfaceField(range);
    else
      interface_out_->set_range(range);
    break;
  }
  default:l3rule_->set_field(field, range); break;
  }
}

// L3 Rule
L3Rule::L3Rule()
    :   l4rule_(new L4Rule()),
        l3prot_(new L3ProtocolField(
                ProtocolType::ETHERNET,
                Range(uint128_t::min(), uint128_t::max()))){}

L3Rule::L3Rule(const L3Rule& copy)
    :   l4rule_(copy.l4rule_->clone()),
        l3prot_(copy.l3prot_->clone()){}

L3Rule::~L3Rule(){
        delete (l4rule_);
        delete (l3prot_);
}

L3Rule* L3Rule::clone() const {return new L3Rule(*this);}

bool L3Rule::equal_to(const L3Rule& rhs) const{
    return (l3prot_->equal_to(*rhs.l3prot_) && l4rule_->equal_to(*rhs.l4rule_));
}

bool L3Rule::covers(const L3Rule& rhs) const{
    return (l3prot_->equal_to(*rhs.l3prot_) && l4rule_->covers(*rhs.l4rule_));
}

RuleTypes L3Rule::get_type() const{
    return RuleTypes::l3rule;
}

Field* L3Rule::get_proto_field() const{return l3prot_;}

void L3Rule::set_field(const FieldType field, const Range& range){

  if (field == l4_proto){
    switch(range.lower_.get_low()){
      case 1:{
        delete(l4rule_);
        l4rule_ = new ICMPRule();
        break;
      }
      case 6:{
        delete(l4rule_);
        l4rule_ = new TCPRule();
        break;
      }
      case 17:{
        delete(l4rule_);
        l4rule_ = new UDPRule();
        break;
      }
    }
  }
}

// IPv4Rule

IPv4Rule::IPv4Rule()
    :   src_ip_field_(nullptr),
        dst_ip_field_(nullptr),
        type_(RuleTypes::ipv4rule){l3prot_->set_protocol(ProtocolType::IPV4);}

IPv4Rule::IPv4Rule(const L3Rule& copy)
  : L3Rule(copy),
    src_ip_field_(nullptr),
    dst_ip_field_(nullptr),
    type_(RuleTypes::ipv4rule){
      l3prot_->set_protocol(IPV4);
    }

IPv4Rule::IPv4Rule(const IPv4Rule& copy)
    :   L3Rule(copy), src_ip_field_(nullptr),
        dst_ip_field_(nullptr), type_(copy.type_)
    {
    if(copy.src_ip_field_ != nullptr)
        src_ip_field_ = copy.src_ip_field_->clone();
    if(copy.dst_ip_field_ != nullptr)
        dst_ip_field_ = copy.dst_ip_field_->clone();
}

IPv4Rule::~IPv4Rule(){
    if (src_ip_field_ != nullptr)
        delete(src_ip_field_);
    if (dst_ip_field_ != nullptr)
        delete(dst_ip_field_);
}

IPv4Rule* IPv4Rule::clone() const {return new IPv4Rule(*this);}
RuleTypes IPv4Rule::get_type() const{return type_;}

Field* IPv4Rule::get_src_ip() const {return src_ip_field_;}
Field* IPv4Rule::get_dst_ip() const {return dst_ip_field_;}

void IPv4Rule::set_src_ip(const IPv4Field& field){
    if(src_ip_field_ != nullptr)
        src_ip_field_->set_range(field.get_range());
    else
        src_ip_field_ = new IPv4Field(field);
}

void IPv4Rule::set_dst_ip(const IPv4Field& field){
    if(dst_ip_field_ != nullptr)
        dst_ip_field_->set_range(field.get_range());
    else
        dst_ip_field_ = new IPv4Field(field);
}

bool IPv4Rule::equal_to(const IPv4Rule& rhs) const{
    bool src_ip_match = false;
    bool dst_ip_match = false;

    if(type_ != rhs.type_)
        return false;
    if ((src_ip_field_ == nullptr) && (rhs.src_ip_field_ == nullptr))
        src_ip_match = true;
    if ((dst_ip_field_ == nullptr) && (rhs.dst_ip_field_ == nullptr))
        dst_ip_match = true;
    if((src_ip_field_ != nullptr) && (rhs.src_ip_field_ != nullptr)){
        if(*(src_ip_field_) == (*(rhs.src_ip_field_)))
            src_ip_match = true;
    }
    if((dst_ip_field_ != nullptr) && (rhs.dst_ip_field_ != nullptr)){
        if(*(dst_ip_field_) == (*(rhs.dst_ip_field_)))
            dst_ip_match = true;
    }
    return (src_ip_match && dst_ip_match);
}

bool IPv4Rule::covers(const IPv4Rule& rhs) const{
    bool l4rule_cover = false;
    bool src_ip_cover = false;
    bool dst_ip_cover = false;

    if(type_ != rhs.type_)
        return false;
    if ((src_ip_field_ == nullptr) && (rhs.src_ip_field_ == nullptr))
        src_ip_cover = true;
    if ((dst_ip_field_ == nullptr) && (rhs.dst_ip_field_ == nullptr))
        dst_ip_cover = true;
    if((src_ip_field_ != nullptr) && (rhs.src_ip_field_ != nullptr)){
        if(src_ip_field_->covers(*(rhs.src_ip_field_)))
            src_ip_cover = true;
    }
    if((dst_ip_field_ != nullptr) && (rhs.dst_ip_field_ != nullptr)){
        if(dst_ip_field_->covers(*(rhs.dst_ip_field_)))
            dst_ip_cover = true;
    }
    if(l4rule_ == nullptr && rhs.l4rule_ != nullptr)
        l4rule_cover = true;
    if(l4rule_ == nullptr && rhs.l4rule_ == nullptr)
        l4rule_cover = true;
    else if (l4rule_ != nullptr && rhs.l4rule_ != nullptr){
        if(l4rule_->covers(*(rhs.l4rule_)))
            l4rule_cover = true;
    }
    return (src_ip_cover && dst_ip_cover && l4rule_cover);
}

void IPv4Rule::set_field(const FieldType field, const Range& range){

  if (field == l4_proto){
    switch(range.lower_.get_low()){
      case 1:{
        delete(l4rule_);
        l4rule_ = new ICMPRule();
        break;
      }
      case 6:{
        delete(l4rule_);
        l4rule_ = new TCPRule();
        break;
      }
      case 17:{
        delete(l4rule_);
        l4rule_ = new UDPRule();
        break;
      }
    }
    l4rule_->set_field(field, range);
  }
  else if(field == l3_src){
    if (src_ip_field_ == nullptr)
      src_ip_field_ = new IPv4Field(range);
    else
      src_ip_field_->set_range(range);
  }
  else if(field == l3_dst){
    if (dst_ip_field_ == nullptr)
      dst_ip_field_ = new IPv4Field(range);
    else
      dst_ip_field_->set_range(range);
  }
  else
    l4rule_->set_field(field, range);
}

// IPv6Rule

IPv6Rule::IPv6Rule()
    :   src_ip_field_(nullptr),
        dst_ip_field_(nullptr),
        type_(RuleTypes::ipv6rule){l3prot_->set_protocol(ProtocolType::IPV6);}

IPv6Rule::IPv6Rule(const L3Rule& copy)
  : L3Rule(copy),
    src_ip_field_(nullptr),
    dst_ip_field_(nullptr),
    type_(RuleTypes::ipv6rule){
      l3prot_->set_protocol(IPV6);
    }


IPv6Rule::IPv6Rule(const IPv6Rule& copy)
    :   L3Rule(copy), src_ip_field_(nullptr),
        dst_ip_field_(nullptr), type_(copy.type_){
    if(copy.src_ip_field_ != nullptr)
        src_ip_field_ = copy.src_ip_field_->clone();
    if(copy.dst_ip_field_ != nullptr)
        dst_ip_field_ = copy.dst_ip_field_->clone();
}

IPv6Rule::~IPv6Rule(){
    if (src_ip_field_ != nullptr)
        delete(src_ip_field_);
    if (dst_ip_field_ != nullptr)
        delete(dst_ip_field_);
}

IPv6Rule* IPv6Rule::clone() const {return new IPv6Rule(*this);}
RuleTypes IPv6Rule::get_type() const{return type_;}
Field* IPv6Rule::get_src_ip() const {return src_ip_field_;}
Field* IPv6Rule::get_dst_ip() const {return dst_ip_field_;}

void IPv6Rule::set_src_ip(const IPv6Field& field){
    if (src_ip_field_ != nullptr)
        src_ip_field_->set_range(field.get_range());
    else
        src_ip_field_ = new IPv6Field(field);
}

void IPv6Rule::set_dst_ip(const IPv6Field& field){
    if(dst_ip_field_ != nullptr)
        dst_ip_field_->set_range(field.get_range());
    else
        dst_ip_field_ = new IPv6Field(field);
}

bool IPv6Rule::equal_to(const IPv6Rule& rhs) const{
    bool src_ip_match = false;
    bool dst_ip_match = false;

    if(type_ != rhs.type_)
        return false;
    if ((src_ip_field_ == nullptr) && (rhs.src_ip_field_ == nullptr))
        src_ip_match = true;
    if ((dst_ip_field_ == nullptr) && (rhs.dst_ip_field_ == nullptr))
        dst_ip_match = true;
    if((src_ip_field_ != nullptr) && (rhs.src_ip_field_ != nullptr)){
        if(*(src_ip_field_) == (*(rhs.src_ip_field_)))
            src_ip_match = true;
    }
    if((dst_ip_field_ != nullptr) && (rhs.dst_ip_field_ != nullptr)){
        if(*(dst_ip_field_) == (*(rhs.dst_ip_field_)))
            dst_ip_match = true;
    }
    return (src_ip_match && dst_ip_match);
}

bool IPv6Rule::covers(const IPv6Rule& rhs) const{
    bool l4rule_cover = false;
    bool src_ip_cover = false;
    bool dst_ip_cover = false;

    if(type_ != rhs.type_)
        return false;
    if ((src_ip_field_ == nullptr) && (rhs.src_ip_field_ == nullptr))
        src_ip_cover = true;
    if ((dst_ip_field_ == nullptr) && (rhs.dst_ip_field_ == nullptr))
        dst_ip_cover = true;
    if((src_ip_field_ != nullptr) && (rhs.src_ip_field_ != nullptr)){
        if(src_ip_field_->covers(*(rhs.src_ip_field_)))
            src_ip_cover = true;
    }
    if((dst_ip_field_ != nullptr) && (rhs.dst_ip_field_ != nullptr)){
        if(dst_ip_field_->covers(*(rhs.dst_ip_field_)))
            dst_ip_cover = true;
    }
    if(l4rule_ == nullptr && rhs.l4rule_ != nullptr)
        l4rule_cover = true;
    if(l4rule_ == nullptr && rhs.l4rule_ == nullptr)
        l4rule_cover = true;
    else if (l4rule_ != nullptr && rhs.l4rule_ != nullptr){
        if(l4rule_->covers(*(rhs.l4rule_)))
            l4rule_cover = true;
    }
    return (src_ip_cover && dst_ip_cover && l4rule_cover);
}

void IPv6Rule::set_field(const FieldType field, const Range& range){

  if (field == l4_proto){
    switch(range.lower_.get_low()){
      case 1:{
        delete(l4rule_);
        l4rule_ = new ICMPRule();
        break;
      }
      case 6:{
        delete(l4rule_);
        l4rule_ = new TCPRule();
        break;
      }
      case 17:{
        delete(l4rule_);
        l4rule_ = new UDPRule();
        break;
      }
    }
    l4rule_->set_field(field, range);
  }
  else if(field == l3_src){
    if (src_ip_field_ == nullptr)
      src_ip_field_ = new IPv6Field(range);
    else
      src_ip_field_->set_range(range);
  }
  else if(field == l3_dst){
    if (dst_ip_field_ == nullptr)
      dst_ip_field_ = new IPv6Field(range);
    else
      dst_ip_field_->set_range(range);
  }
  else
    l4rule_->set_field(field, range);
}

// L4Rule

L4Rule::L4Rule()
  :   l4prot_(new L4ProtocolField(
              ProtocolType::TRANSPORT,
              Range(uint128_t::min(), uint128_t::max()))){}

L4Rule::L4Rule(const L4Rule& copy)
  :   l4prot_(copy.l4prot_->clone()){}

L4Rule::~L4Rule(){
    delete l4prot_;
}

L4Rule* L4Rule::clone() const{return new L4Rule(*this);}

bool L4Rule::equal_to(const L4Rule& rhs) const{
    return (*l4prot_ == *rhs.l4prot_);

}

bool L4Rule::covers(const L4Rule& rhs) const{
    return (l4prot_ == rhs.l4prot_);

}

RuleTypes L4Rule::get_type() const { return RuleTypes::l4rule;}
Field* L4Rule::get_proto_field() const{return l4prot_;}

void L4Rule::set_field(const FieldType field, const Range& range){
  if(field == l4_proto)
      l4prot_->set_range(range);
//  else
//    throw new std::string("Tried to set invalid Field on L4Rule");
}
// TCP Rule

TCPRule::TCPRule()
    :   src_port_field_(nullptr),
        dst_port_field_(nullptr),
        flags_field_(nullptr),
        type_(RuleTypes::tcprule){
            l4prot_->set_protocol(ProtocolType::TCP);}

TCPRule::TCPRule(const TCPRule& copy)
    :   L4Rule(copy),
        src_port_field_ (nullptr),
        dst_port_field_(nullptr),
        flags_field_(nullptr),
        type_(copy.type_){
          if(copy.src_port_field_ != nullptr)
              src_port_field_ = copy.src_port_field_->clone();
          if(copy.dst_port_field_ != nullptr)
              dst_port_field_ = copy.dst_port_field_->clone();
          if(copy.flags_field_ != nullptr)
              flags_field_ = copy.flags_field_->clone();
}

TCPRule::~TCPRule(){
    if(src_port_field_ != nullptr)
        delete(src_port_field_);
    if(dst_port_field_ != nullptr)
        delete(dst_port_field_);
    if(flags_field_ != nullptr)
        delete(flags_field_);
}

TCPRule* TCPRule::clone() const{return new TCPRule(*this);}
Field* TCPRule::get_src_port() const{return src_port_field_;}
Field* TCPRule::get_dst_port() const{return dst_port_field_;}
Field* TCPRule::get_flags() const{return flags_field_;}

bool TCPRule::equal_to(const TCPRule& rhs) const{
    bool src_port_match = false;
    bool dst_port_match = false;
    bool flags_field_match = false;

    if (type_ != rhs.type_)
        return false;
    if(src_port_field_ != nullptr && rhs.src_port_field_ != nullptr){
        if( *(src_port_field_) == (*(rhs.src_port_field_)))
            src_port_match = true;
    }
    if(dst_port_field_ != nullptr && rhs.dst_port_field_ != nullptr){
        if( *(dst_port_field_) == (*(rhs.dst_port_field_)))
            dst_port_match = true;
    }
    if(flags_field_ != nullptr && rhs.flags_field_ != nullptr){
        if( *(flags_field_) == (*(rhs.flags_field_)))
            flags_field_match = true;
    }
    if ((src_port_field_ == nullptr) && (dst_port_field_ == nullptr)
        && (flags_field_ == nullptr))
            return true;
    return (src_port_match && dst_port_match && flags_field_match);
}

bool TCPRule::covers(const TCPRule& rhs) const{
    bool src_port_cover = false;
    bool dst_port_cover = false;
    bool flags_field_cover = false;

    if (type_ != rhs.type_)
        return false;
    if(src_port_field_ == nullptr && rhs.src_port_field_ == nullptr)
        src_port_cover = true;
    if(src_port_field_ != nullptr && rhs.src_port_field_ != nullptr){
        if(src_port_field_->covers(*(rhs.src_port_field_)))
            src_port_cover = true;
    }
    if(dst_port_field_ == nullptr && rhs.dst_port_field_ == nullptr)
        dst_port_cover = true;
    if(dst_port_field_ != nullptr && rhs.dst_port_field_ != nullptr){
        if( dst_port_field_->covers(*(rhs.dst_port_field_)))
            dst_port_cover = true;
    }
    if(flags_field_ != nullptr && rhs.flags_field_ != nullptr){
        if( flags_field_->covers(*(rhs.flags_field_)))
            flags_field_cover = true;
    }
    if ((src_port_field_ == nullptr) && (dst_port_field_ == nullptr)
        && (flags_field_ == nullptr))
            return true;
    return (src_port_cover && dst_port_cover && flags_field_cover);
}

RuleTypes TCPRule::get_type() const{
    return RuleTypes::tcprule;
}

void TCPRule::set_src_port(const PortField& field){
    if(src_port_field_ != nullptr)
        src_port_field_->set_range(field.get_range());
    else
        src_port_field_ = new PortField(field.get_range(), TCP);
}

void TCPRule::set_dst_port(const PortField& field){
    if(dst_port_field_ != nullptr)
        dst_port_field_->set_range(field.get_range());
    else
        dst_port_field_ = new PortField(field.get_range(), TCP);
}

void TCPRule::set_flags(const TCPFlagField& field){
    if(flags_field_ != nullptr){
        flags_field_->set_flags(field.get_flags());
        flags_field_->set_range(field.get_range());
    }
    else{
      flags_field_ = new TCPFlagField(field.get_flags());
      flags_field_->set_range(field.get_range());
    }

}

void TCPRule::set_field(const FieldType field, const Range& range){

  switch(field){
    case l4_proto: l4prot_->set_range(range); break;
    case l4_src:{
      if (src_port_field_ == nullptr)
        src_port_field_ = new PortField(range, ProtocolType::TCP);
      else
        src_port_field_->set_range(range);
      break;
    }
    case l4_dst:{
      if (dst_port_field_ == nullptr)
        dst_port_field_ = new PortField(range, ProtocolType::TCP);
      else
        dst_port_field_->set_range(range);
      break;
    }
    case tcpflagfield:{
      if (flags_field_ == nullptr)
        flags_field_ = new TCPFlagField(range);
      else
        flags_field_->set_range(range);
      break;
    }
    default: throw new std::string("Tried to set invalid Field in TCPRule"); break;
  }
}


//UDP Rule

UDPRule::UDPRule()
    :   src_port_field_(nullptr),
        dst_port_field_(nullptr),
        type_(RuleTypes::udprule){l4prot_->set_protocol(ProtocolType::UDP);}

UDPRule::UDPRule(const UDPRule& copy)
    :   L4Rule(copy),
        src_port_field_(nullptr),
        dst_port_field_(nullptr),
        type_(copy.type_){

        if(copy.src_port_field_ != nullptr)
            src_port_field_ = copy.src_port_field_->clone();
        if(copy.dst_port_field_ != nullptr)
            dst_port_field_ = copy.dst_port_field_->clone();
       }

UDPRule::~UDPRule(){
    if(src_port_field_ != nullptr)
        delete(src_port_field_);
    if(dst_port_field_ != nullptr)
        delete(dst_port_field_);
}

UDPRule* UDPRule::clone() const{return new UDPRule(*this);}
RuleTypes UDPRule::get_type() const {return type_;}
Field* UDPRule::get_src_port() const{return src_port_field_;}
Field* UDPRule::get_dst_port() const{return dst_port_field_;}

bool UDPRule::equal_to(const UDPRule& rhs) const{
    bool src_port_match = false;
    bool dst_port_match = false;

    if (type_ != rhs.type_)
        return false;
    if(src_port_field_ != nullptr && rhs.src_port_field_ != nullptr){
        if( *(src_port_field_) == (*(rhs.src_port_field_)))
            src_port_match = true;
    }
    if(dst_port_field_ != nullptr && rhs.dst_port_field_ != nullptr){
        if( *(dst_port_field_) == (*(rhs.dst_port_field_)))
            dst_port_match = true;
    }
    if ((src_port_field_ == nullptr) && (dst_port_field_ == nullptr))
        return true;
    return (src_port_match && dst_port_match);
}

bool UDPRule::covers(const UDPRule& rhs) const{
    bool src_port_cover = false;
    bool dst_port_cover = false;

    if (type_ != rhs.type_)
        return false;
    if(src_port_field_ != nullptr && rhs.src_port_field_ != nullptr){
        if(src_port_field_->covers(*(rhs.src_port_field_)))
            src_port_cover = true;
    }
    if(dst_port_field_ != nullptr && rhs.dst_port_field_ != nullptr){
        if( dst_port_field_->covers(*(rhs.dst_port_field_)))
            src_port_cover = true;
    }
    if ((src_port_field_ == nullptr) && (dst_port_field_ == nullptr))
            return true;
    return (src_port_cover && dst_port_cover);

}

void UDPRule::set_src_port(const PortField& field){
    if(src_port_field_ != nullptr)
        src_port_field_->set_range(field.get_range());
    else
        src_port_field_ = new PortField(field.get_range(), UDP);
}

void UDPRule::set_dst_port(const PortField& field){
    if(dst_port_field_ != nullptr)
        dst_port_field_->set_range(field.get_range());
    else
        dst_port_field_ = new PortField(field.get_range(), UDP);
}

void UDPRule::set_field(const FieldType field, const Range& range){

  switch(field){
    case l4_proto: l4prot_->set_range(range); break;
    case l4_src:{
      if (src_port_field_ == nullptr)
        src_port_field_ = new PortField(range, ProtocolType::UDP);
      else
        src_port_field_->set_range(range);
      break;
    }
    case l4_dst:{}
      if (dst_port_field_ == nullptr)
        dst_port_field_ = new PortField(range, ProtocolType::UDP);
      else
        dst_port_field_->set_range(range);
      break;
    default: break; //throw "Tried to set invalid Field in UDPRule"; break;
  }
}

// ICMPRule

ICMPRule::ICMPRule()
    :   icmpcode_field_(nullptr), icmptype_field_(nullptr),
        type_(RuleTypes::icmprule){l4prot_->set_protocol(ProtocolType::ICMP);}

ICMPRule::ICMPRule(const ICMPCodeField code, const ICMPTypeField type)
    :   icmpcode_field_(new ICMPCodeField(code)),
        icmptype_field_(new ICMPTypeField(type)),
        type_(RuleTypes::icmprule){}

ICMPRule::ICMPRule(const ICMPRule& copy)
    :   L4Rule(copy),
        icmpcode_field_(nullptr),
        icmptype_field_(nullptr),
        type_(copy.type_){

        if(copy.icmpcode_field_ != nullptr)
            icmpcode_field_ = copy.icmpcode_field_->clone();
        if(copy.icmptype_field_ != nullptr)
            icmptype_field_ = copy.icmptype_field_->clone();
        }

ICMPRule::~ICMPRule(){
    if(icmpcode_field_ != nullptr)
        delete(icmpcode_field_);
    if(icmptype_field_ != nullptr)
        delete(icmptype_field_);
}

RuleTypes ICMPRule::get_type() const{ return type_;}
ICMPRule* ICMPRule::clone() const {return new ICMPRule(*this);}
Field* ICMPRule::get_icmpcode() const {return icmpcode_field_;}
Field* ICMPRule::get_icmptype() const {return icmptype_field_;}

bool ICMPRule::equal_to(const ICMPRule& rhs) const{
    bool icmpcode_match = false;
    bool icmptype_match = false;

    if (type_ != rhs.type_)
        return false;
    if(icmpcode_field_ != nullptr && rhs.icmpcode_field_ != nullptr){
        if( *(icmpcode_field_) == (*(rhs.icmpcode_field_)))
            icmpcode_match = true;
    }
    if(icmptype_field_ != nullptr && rhs.icmptype_field_ != nullptr){
        if( *(icmptype_field_) == (*(rhs.icmptype_field_)))
            icmptype_match = true;
    }
    if ((icmpcode_field_ == nullptr) && (icmptype_field_ == nullptr))
        return true;
    return (icmpcode_match && icmptype_match);
}

bool ICMPRule::covers(const ICMPRule& rhs) const{
    bool icmpcode_cover = false;
    bool icmptype_cover = false;

    if (type_ != rhs.type_)
        return false;
    if(icmpcode_field_!= nullptr && rhs.icmpcode_field_ != nullptr){
        if(icmpcode_field_->covers(*(rhs.icmpcode_field_)))
            icmpcode_cover = true;
    }
    if(icmptype_field_ != nullptr && rhs.icmptype_field_ != nullptr){
        if( icmptype_field_->covers(*(rhs.icmptype_field_)))
            icmptype_cover = true;
    }
    if ((icmpcode_field_ == nullptr) && (icmptype_field_ == nullptr))
            return true;
    return (icmpcode_cover && icmptype_cover);

}

void ICMPRule::set_icmpcode(const ICMPCodeField& field){
    if(icmpcode_field_ != nullptr)
        icmpcode_field_->set_range(field.get_range());
    else
        icmpcode_field_ = new ICMPCodeField(field.get_range());
}

void ICMPRule::set_icmptype(const ICMPTypeField& field){
    if(icmptype_field_ != nullptr)
        icmptype_field_->set_range(field.get_range());
    else
        icmptype_field_ = new ICMPTypeField(field.get_range());
}

void ICMPRule::set_field(const FieldType field, const Range& range){

  switch(field){
    case l4_proto: l4prot_->set_range(range); break;
    case l4_src:{
      if (icmpcode_field_ == nullptr)
        icmpcode_field_ = new ICMPCodeField(range);
      else
        icmpcode_field_->set_range(range);
      break;
    }
    case l4_dst:{}
      if (icmptype_field_ == nullptr)
        icmptype_field_ = new ICMPTypeField(range);
      else
        icmptype_field_->set_range(range);
      break;
    default: break; //throw new std::string("Tried to set invalid Field in ICMPRule"); break;
  }
}


// << operator overload

std::ostream& operator<<(std::ostream& out, const UPF_Rule& rule){
  return rule.print(out);
}

std::ostream& operator<<(std::ostream& out, const Action& action){

  return action.print(out);

}
