#include "RuleGenerator.h"
#include <cmath>
#include <bitset>
#include <cstring>


std::unique_ptr<UPF_Rule> RuleGenerator::generate(std::vector<Token>& token) const{

    if(token.empty())
      throw std::string("Empty Tokenstream, No Rule created");


    std::unique_ptr<UPF_Rule> rule(new UPF_Rule());
    if(token[15].value_ == "FREE"){
        // set rule to type freerule
        rule->set_type(freerule);

        using namespace Utility;
        ProtocolType l3 = ETHERNET;
        ProtocolType l4 = TRANSPORT;
        // Layer 2 Fields
        if(token[0].value_ != "WILDCARD"){
            Field* mac = new MacField(mac_to_range(token[0].value_));
            rule->set_field(l2_src,ETHERNET, mac);
            delete(mac);
        }
        if(token[1].value_ != "WILDCARD"){
            Field* mac = new MacField(mac_to_range(token[1].value_));
            rule->set_field(l2_dst,ETHERNET, mac);
            delete(mac);
        }
        // Interfaces
        if(token[2].value_ != "WILDCARD"){
            Field* iface = new InterfaceField(interface_to_range(token[2].value_));
            rule->set_field(interface_in_field,ETHERNET, iface);
            delete(iface);
        }
        if(token[3].value_ != "WILDCARD"){
            Field* iface = new InterfaceField(interface_to_range(token[3].value_));
            rule->set_field(interface_out_field, ETHERNET, iface);
            delete(iface);
        }
        // Layer 3 Protocol
        if(token[4].value_ == "IPv4"){
            Field* l3_prot = new L3ProtocolField(ProtocolType::IPV4, Range(4,4));
            rule->set_field(l3_proto, IPV4, l3_prot);
            l3 = IPV4;
            delete(l3_prot);
        }
        else if(token[4].value_ == "IPv6"){
            Field* l3_prot = new L3ProtocolField(ProtocolType::IPV6, Range(41,41));
            rule->set_field(l3_proto, IPV6, l3_prot);
            l3 = IPV6;
            delete(l3_prot);
        }
        // Layer 3 Fields
        if(token[5].value_ != "WILDCARD" || token[5].value_ != "any"){
            if(l3 == IPV4){
                Field* src_field = new IPv4Field(ipv4_to_range(token[5].value_));
                rule->set_field(l3_src, IPV4, src_field);
                delete(src_field);
            }
            else if(l3 == IPV6){
                Field* src_field = new IPv6Field(ipv6_to_range(token[5].value_));
                rule->set_field(l3_src, IPV6, src_field);
                delete(src_field);
            }
        }
        if(token[6].value_ != "WILDCARD" || token[5].value_ != "any"){
            if(l3 == IPV4){
                Field* dst_field = new IPv4Field(ipv4_to_range(token[6].value_));
                rule->set_field(l3_dst, IPV4, dst_field);
                delete(dst_field);
            }
            else if(l3 == IPV6){
                Field* dst_field = new IPv6Field(ipv6_to_range(token[6].value_));
                rule->set_field(l3_dst, IPV6, dst_field);
                delete(dst_field);
            }
        }


        // Layer 4 Protocol
        if(token[7].value_ == "udp"){
            Field* l4_prot = new L4ProtocolField(ProtocolType::UDP, Range(17,17));
            rule->set_field(l4_proto, UDP, l4_prot);
            l4 = UDP;
            delete(l4_prot);
        }
        else if(token[7].value_ == "tcp"){
            Field* l4_prot = new L4ProtocolField(ProtocolType::TCP, Range(6,6));
            rule->set_field(l4_proto, TCP, l4_prot);
            l4 = TCP;
            delete(l4_prot);
        }
        else if(token[7].value_ == "icmp" || token[7].value_ == "1"){
            Field* l4_prot = new L4ProtocolField(ProtocolType::ICMP, Range(1,1));
            rule->set_field(l4_proto, ICMP, l4_prot);
            l4 = ICMP;
            delete(l4_prot);
        }
        else if(token[7].value_ == "gre" || token[7].value_ == "47"){
            Field* l4_prot = new L4ProtocolField(ProtocolType::GRE, Range(47,47));
            rule->set_field(l4_proto, GRE, l4_prot);
            l4 = ICMP;
            delete(l4_prot);
        }
        // Layer 4 Fields
        if(token[8].value_ != "WILDCARD"){
            if(l4 == UDP){
                Field* src_port = new PortField(port_to_range(token[8].value_), UDP);
                rule->set_field(l4_src, UDP, src_port);
                delete(src_port);
            }
            else if(l4 == TCP){
                Field* src_port = new PortField(port_to_range(token[8].value_), TCP);
                rule->set_field(l4_src, TCP, src_port);
                delete(src_port);
            }
            else if(l4 == ICMP){
                Field* icmp_type = new ICMPTypeField(icmp_to_range(token[8].value_));
                rule->set_field(l4_src, ICMP, icmp_type);
                delete(icmp_type);
            }
        }
        if(token[9].value_ != "WILDCARD"){
            if(l4 == UDP){
                Field* dst_port = new PortField(port_to_range(token[9].value_), UDP);
                rule->set_field(l4_dst, UDP, dst_port);
                delete(dst_port);
            }
            else if(l4 == TCP){
                Field* dst_port = new PortField(port_to_range(token[9].value_), TCP);
                rule->set_field(l4_dst, TCP, dst_port);
                delete(dst_port);
            }
            else if(l4 == ICMP){
                Field* icmp_code = new ICMPCodeField(icmp_to_range(token[9].value_));
                rule->set_field(l4_dst, ICMP, icmp_code);
                delete(icmp_code);
            }
        }
        // TCP Flags and States
        if(token[10].value_ != "WILDCARD"){
            Field* flags = new TCPFlagField(std::bitset<6>(token[10].value_));
            rule-> set_field(tcpflagfield,TCP, flags);
            delete(flags);
        }
        // Action
        if(token[12].value_ == "ACCEPT")
            rule->set_action(ACCEPT);
        else if (token[12].value_ == "DROP" || token[12].value_ == "BLOCK")
            rule->set_action(BLOCK);
        else
            rule->set_action(Jump(token[12].value_));

        if(token[16].value_ == "TRUE")
           rule->set_unknown_match("TRUE");

        rule->set_original_rule(token[13].value_);
        
        return std::move(rule);

    }
    else{
        rule->set_type(legacyrule);
        rule->set_original_rule(token[13].value_);
        return std::move(rule);
    }
}
