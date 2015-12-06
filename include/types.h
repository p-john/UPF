#ifndef TYPES_H
#define TYPES_H
#include <iostream>

enum IPTypes{

    none = 0,
    ipv4_single,
    ipv4_range_colon,
    ipv4_range_score,
    ipv4_cidr,
    ipv6_single,
    ipv6_range_colon,
    ipv6_range_score,
    ipv6_cidr
};

enum ProtocolType{

    ETHERNET = 0,
    TRANSPORT,
    IPV4,
    IPV6,
    UDP,
    TCP,
    ICMP,
    GRE,
    AH
};

enum FieldType{

    l2_src = 0,
    l2_dst,
    interface_in_field,
    interface_out_field,
    l3_proto,
    l3_src,
    l3_dst,
    l4_proto,
    l4_src,
    l4_dst,
    tcpflagfield,
    macfield,
    ipv4field,
    ipv6field,
    portfield,
    icmptypefield,
    icmpcodefield
};

enum RuleTypes{

    l2rule = 0,
    l3rule,
    l4rule,
    macrule,
    ipv4rule,
    ipv6rule,
    tcprule,
    udprule,
    icmprule
};

enum UPF_Type{

    legacyrule = 0,
    freerule
};

enum UPF_setType{

    default_set = 0,
    freeset,
    legacyset,
};

enum ActionTypes{

    NONE = 0,
    ACCEPT,
    BLOCK,
    JUMP

};

enum EngineType{

    cb,
    IPTables,
    ipfw,
    pf,
    ipf,
    dump,
    invalid
};

enum PartitionType{

  equal = 0,
  order_independent
};

enum OptimizeType{

    no_opt = 0,
    simple_redundancy,
    fdd_redundancy,
    disjunct,
    hypersplit,
    sax_pac,
    fw_compressor
};

enum CuttingType{

    equi_dist = 0,
    equi_seg,
    equi_rule,
    weighted_seg
};

enum DimensionType{

    ipv4src = 0,
    ipv4dst,
    l4protocol,
    tcpsrcport,
    tcpdstport,
    udpsrcport,
    udpdstport,
    macsrc,
    macdst,
    ipv6src,
    ipv6dst
};

static inline std::string fieldtype_to_string(const FieldType field_key){

  std::string name_;
  switch(field_key){
    case l2_src : name_ = "L2_Src"; break;
    case l2_dst : name_ = "L2_Dst"; break;
    case interface_in_field : name_ = "Interface_In"; break;
    case interface_out_field : name_ ="Interface_Out"; break;
    case l3_proto : name_ = "L3_Protocol"; break;
    case l3_src : name_ = "L3_Src"; break;
    case l3_dst : name_ = "L3_Dst"; break;
    case l4_proto : name_ ="L4_Protocol"; break;
    case l4_src : name_ = "L4_Src"; break;
    case l4_dst : name_ = "L4_Dst"; break;
    case tcpflagfield : name_ = "TCPFlagField"; break;
  default: name_ = "";
  }
  return name_;
}

static inline std::string dimensiontype_to_string(const DimensionType field_key){

  std::string name_;
  switch(field_key){
    case macsrc : name_ = "MacSrc"; break;
    case macdst : name_ = "MacDst"; break;
    case ipv4src : name_ = "IPv4Src"; break;
    case ipv4dst : name_ = "IPv4Dst"; break;
    case ipv6src : name_ = "IPv6Src"; break;
    case ipv6dst : name_ = "IPv6Dst"; break;
    case l4protocol : name_ ="L4_Protocol"; break;
    case tcpsrcport : name_ = "TCPSrcPort"; break;
    case tcpdstport : name_ = "TCPDstPort"; break;
    case udpsrcport : name_ = "UDPSrcPort"; break;
    case udpdstport : name_ = "UDPDstPort"; break;

  default: name_ = "";
  }
  return name_;
}


static inline FieldType next_field_key(const FieldType field_key){

  unsigned int counter = (int)field_key;
  counter++;
  return (FieldType)counter;

}

static inline std::string action_to_string(const ActionTypes act_type){

  std::string name_;
  switch(act_type){
    case ACCEPT: name_ = "ACCEPT"; break;
    case BLOCK: name_ = "BLOCK"; break;
    case JUMP: name_ = "JUMP"; break;
    case NONE: name_ = "NONE"; break;
    default: name_ = "";
  }
  return name_;
}

static inline std::string cuttingtype_to_string(const CuttingType cut_type){

  std::string type;
  switch(cut_type){
    case weighted_seg: type = "Weighted Segments"; break;
    case equi_dist: type = "Equal Distances"; break;
    case equi_rule: type = "Equal Rules"; break;
    case equi_seg: type = "Equal Segments"; break;
  }
  return type;
}

static inline std::string partitiontype_to_string(const PartitionType part_type){

  std::string type;
  if(part_type == order_independent)
    return std::string("Order Independent");
  else
    return std::string("Equal");

}

static inline std::string protocoltype_to_string(const ProtocolType type){

  switch(type){
    case ETHERNET: return "ETHERNET";
    case TRANSPORT: return "TRANSPORT";
    case IPV4:  return "IPV4";
    case IPV6: return "IPV6";
    case TCP: return "TCP";
    case UDP: return "UDP";
    case ICMP: return "ICMP";
    case GRE: return "GRE";
    case AH: return "AH";
    default: return "";
    }


}


#endif // TYPES_H
