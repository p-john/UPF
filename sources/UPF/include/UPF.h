#ifndef UPF_H_
#define UPF_H_

#include <algorithm>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>
#include "Field.h"


template<typename T> class Rule_CRTP {
    friend bool operator==(T const &a, T const &b) { return  a.equal_to (b); }
    friend bool operator!=(T const &a, T const &b) { return  !a.equal_to (b); }
    bool covers(T const &a, T const &b) { return  a.covers(b); }
public:
    virtual ~Rule_CRTP(){}
};


class Action : public Rule_CRTP<Action>{

public:

    bool equal_to(const Action& rhs) const;
    virtual std::ostream& print(std::ostream& out) const;
    virtual ~Action(){}
    ActionTypes get_type() const;
    std::string get_target() const;
    void set_target(const std::string target);
    Action(const ActionTypes type);

   friend std::ostream& operator<<(std::ostream& out, const Action& action);

protected:
    ActionTypes type_;
    std::string target_;

};

class Accept : public Action, public Rule_CRTP<Accept>{

public:
    bool equal_to(const Accept& rhs) const;
    Accept();


};

class Block : public Action, public Rule_CRTP<Block>{

public:
    bool equal_to(const Block& rhs) const;
    Block();

};

class UPF_Rule;

class Jump : public Action, public Rule_CRTP<Jump>{

public:
    bool equal_to(const Jump& rhs) const;
    bool equal_to(const Action& rhs) const;
    Jump(const std::string target);
    ~Jump();

};

// Rule

//class UPF_Rule{
//
//public:
//    virtual UPF_Rule* clone() const;
////    virtual void print() const;
//    virtual bool covers(const UPF_Rule& rhs) const;
//    void set_original_rule(const std::string orig);
//    std::string get_original_rule() const;
//    virtual UPF_Type get_type() const;
//    virtual void print() const;
//
//    UPF_Rule(std::string original_rule, const UPF_Type type);
//    UPF_Rule(const UPF_Rule& copy);
//    virtual ~UPF_Rule(){}
//
//protected:
//
//};

class L2Rule;
class L3Rule;
class L4Rule;

class UPF_Rule : public Rule_CRTP<UPF_Rule>{

public:
    static std::vector<FieldType> get_fieldkeys();
    static std::vector<DimensionType> get_dimkeys();

    UPF_Rule* clone() const;
    bool covers(const UPF_Rule& rhs) const;
    bool intersect(const UPF_Rule& rhs) const;
    bool equal_to(const UPF_Rule& rhs) const;
    void replace(const FieldType type, Field* field);
    Action get_action() const;
    void set_field(const FieldType field, const Range& range);
    void set_action(const ActionTypes& action);
    void set_action(const Jump& jump);
    void set_field(const DimensionType type, const Range& range);
    void set_field(const FieldType type, const Field* field);
    void set_field(const FieldType type,const ProtocolType proto, const Field* field);
    void set_range_on_field(const FieldType type, const Range& range);
    void set_unknown_match(bool unknown_match);
    bool has_unknown_match() const;
    void reset_field(FieldType type);
    Field* get_field(const FieldType field) const;
    Field* get_field(const DimensionType dim) const;
    std::vector<Range> get_ranges_next_from(const FieldType field) const;
    void print() const;
    std::ostream& print(std::ostream& out) const;
    UPF_Type get_type() const;
    ProtocolType get_l4_protocol() const;
    ProtocolType get_l3_protocol() const;
    void set_type(UPF_Type);
    void set_original_rule(const std::string& orig);
    const std::string& get_original_rule() const;

    friend std::ostream& operator<<(std::ostream& out, const UPF_Rule& rule);

    UPF_Rule();
    UPF_Rule(const UPF_Rule& copy);
    ~UPF_Rule();
private:
    Action action_;
    L2Rule* l2rule_;
    std::string original_rule_;
    UPF_Type type_;
    bool unknown_match_;

};


//class UPF_LegacyRule : public UPF_Rule, public Rule_CRTP<UPF_LegacyRule>{
//
//public:
//
//    bool equal_to(const UPF_LegacyRule& rhs) const;
//
//    UPF_LegacyRule();
//    UPF_LegacyRule(std::string original_rule);
//    ~UPF_LegacyRule(){}
//
//};


// Layer 2 Rules

class L2Rule : public Rule_CRTP<L2Rule>{

public:
    virtual L2Rule* clone() const;
    virtual RuleTypes get_type() const;
    virtual bool equal_to(const L2Rule& rhs) const;
    virtual bool covers(const L2Rule& rhs) const;
    virtual void set_field(const FieldType field, const Range& range);
    void set_interface_in(const InterfaceField&);
    void set_interface_out(const InterfaceField&);
    Field* get_interface_in() const;
    Field* get_interface_out() const;
    L2Rule();
    L2Rule(const L2Rule& copy);
    virtual ~L2Rule();
//protected:
    L3Rule* l3rule_;
    InterfaceField* interface_in_;
    InterfaceField* interface_out_;
};

class MacRule : public L2Rule, public Rule_CRTP<MacRule>{

public:
    MacRule* clone() const;
    RuleTypes get_type() const;
    virtual bool equal_to(const MacRule& rhs) const;
    virtual bool covers(const MacRule& rhs) const;
    virtual void set_field(const FieldType field, const Range& range);
    Field* get_src_mac() const;
    Field* get_dst_mac() const;

    void set_src_mac(const MacField& src);
    void set_dst_mac(const MacField& dst);

    void add_l3_rule(const L3Rule* l3rule);
    void remove_l3_rule();

    MacRule();
    MacRule(const MacRule& copy);
    ~MacRule();

protected:
    MacField* src_mac_field_;
    MacField* dst_mac_field_;
    RuleTypes type_;

};


// Layer 3 Rules

class L3Rule : public Rule_CRTP<L3Rule>{

public:
    virtual L3Rule* clone() const;
    virtual RuleTypes get_type() const;
    virtual bool equal_to(const L3Rule& rhs) const;
    virtual bool covers(const L3Rule& rhs) const;
    virtual void set_field(const FieldType field, const Range& range);
    Field* get_proto_field() const;
    L3Rule();
    L3Rule(const L3Rule& copy);
    virtual ~L3Rule();
    L4Rule* l4rule_ ;
    L3ProtocolField* l3prot_;


};

class IPv4Rule : public L3Rule, public Rule_CRTP<IPv4Rule>{

public:
    virtual RuleTypes get_type() const;
    IPv4Rule* clone() const;

    void replace(const FieldType type, Field* field);
    virtual bool equal_to(const IPv4Rule& rhs) const;
    virtual bool covers(const IPv4Rule& rhs) const;
    virtual void set_field(const FieldType field, const Range& range);
    Field* get_src_ip() const;
    Field* get_dst_ip() const;
    void set_src_ip(const IPv4Field& src);
    void set_dst_ip(const IPv4Field& dst);

    IPv4Rule();
    IPv4Rule(const IPv4Rule& copy);
    IPv4Rule(const L3Rule& copy);
    ~IPv4Rule();

private:
    IPv4Field* src_ip_field_;
    IPv4Field* dst_ip_field_;
    RuleTypes type_;

};

class IPv6Rule : public L3Rule, public Rule_CRTP<IPv4Rule>{

public:
    RuleTypes get_type() const;
    IPv6Rule* clone() const;

    void replace(const FieldType type, Field* field);

    virtual bool equal_to(const IPv6Rule& rhs) const;
    virtual bool covers(const IPv6Rule& rhs) const;
    virtual void set_field(const FieldType field, const Range& range);

    Field* get_src_ip() const;
    Field* get_dst_ip() const;
    void set_src_ip(const IPv6Field& src);
    void set_dst_ip(const IPv6Field& dst);

    IPv6Rule();
    IPv6Rule(const IPv6Rule& copy);
    IPv6Rule(const L3Rule& copy);
    ~IPv6Rule();

private:
    IPv6Field* src_ip_field_;
    IPv6Field* dst_ip_field_;
    RuleTypes type_;

};


//Layer 4 Rules

class L4Rule : public Rule_CRTP<L4Rule>{

public:
    virtual L4Rule* clone() const;
    virtual RuleTypes get_type() const;
    virtual bool covers(const L4Rule& rhs) const;
    virtual bool equal_to(const L4Rule& rhs) const;
    virtual void set_field(const FieldType field, const Range& range);
    Field* get_proto_field() const;
    L4Rule();
    L4Rule(const L4Rule& copy);
    virtual ~L4Rule();

    L4ProtocolField* l4prot_;

};

class TCPRule : public L4Rule, public Rule_CRTP<TCPRule>{

public:
    RuleTypes get_type() const;
    bool operator==(const L4Rule& rhs) const;
    TCPRule* clone() const;
    virtual bool equal_to(const TCPRule& rhs) const;
    virtual bool covers(const TCPRule& rhs) const;
    virtual void set_field(const FieldType field, const Range& range);

    void replace(const FieldType type, Field* field);
    Field* get_src_port() const;
    Field* get_dst_port() const;
    Field* get_flags() const;
    void set_src_port(const PortField& field);
    void set_dst_port(const PortField& field);
    void set_flags(const TCPFlagField& flags);

    TCPRule();
    TCPRule(const TCPRule& copy);
    ~TCPRule();

private:
    PortField* src_port_field_;
    PortField* dst_port_field_;
    TCPFlagField* flags_field_;
    RuleTypes type_;


};

class UDPRule : public L4Rule, public Rule_CRTP<UDPRule>{

public:
    RuleTypes get_type() const;
    UDPRule* clone() const;

    void replace(const FieldType type, Field* field);

    virtual bool equal_to(const UDPRule& rhs) const;
    virtual bool covers(const UDPRule& rhs) const;
    virtual void set_field(const FieldType field, const Range& range);

    Field* get_src_port() const;
    Field* get_dst_port() const;
    void set_src_port(const PortField& field);
    void set_dst_port(const PortField& field);

    UDPRule();
    UDPRule(const UDPRule& copy);
    ~UDPRule();

private:
    PortField* src_port_field_;
    PortField* dst_port_field_;
    RuleTypes type_;


};

class ICMPRule : public L4Rule, public Rule_CRTP<ICMPRule>{

public:
    ICMPRule* clone() const;
    RuleTypes get_type() const;
    virtual bool equal_to(const ICMPRule& rhs) const;
    virtual bool covers(const ICMPRule& rhs) const;
    virtual void set_field(const FieldType field, const Range& range);
    Field* get_icmptype() const;
    Field* get_icmpcode() const;

    Range get_range() const;
    void set_icmptype(const ICMPTypeField&);
    void set_icmpcode(const ICMPCodeField&);

    ICMPRule();
    ICMPRule(ICMPCodeField code, ICMPTypeField type);
    ICMPRule(const ICMPRule& copy);
    ~ICMPRule();

private:

    ICMPCodeField* icmpcode_field_;
    ICMPTypeField* icmptype_field_;
    RuleTypes type_;
};



#endif // UPF_HPP


