#include <stack>
#include "Parser.h"
#include "utility.h"


static std::unordered_map<std::string,std::string> getIPTablesToken();
static std::unordered_map<std::string,std::string> getipfwToken();
static std::unordered_map<std::string,std::string> getpfToken();
static std::unordered_map<std::string,std::string> getcbToken();

Parser::Parser(std::unordered_map<std::string,std::string> dict)
  :  tokenizer_(dict){}

cbParser::cbParser()
  :  Parser(getcbToken()){}

void cbParser::parse(std::string& str, std::vector<Token>& tokens){
  std::string new_str = str;
  if(str[0] == '@' && str.size() >= 2)
    new_str = str.substr(1,str.size() -1);
  std::vector<Token> result;

  Token l2_src("[L2_SRC]","WILDCARD");
  Token l2_dst("[L2_DST]","WILDCARD");
  Token iface_in("[INTERFACE_IN]","WILDCARD");
  Token iface_out("[INTERFACE_OUT]","WILDCARD");
  Token l3_proto("[L3_PROTOCOL]","IPv4");
  Token l3_src("[L3_SRC]","WILDCARD");
  Token l3_dst("[L3_DST]","WILDCARD");
  Token l4_proto("[L4_PROTOCOL]","WILDCARD");
  Token l4_src("[L4_SRC]","WILDCARD");
  Token l4_dst("[L4_DST]","WILDCARD");
  Token tcpflags("[TCPFLAGS]","WILDCARD");
  Token tcpstates("[TCPSTATES]","WILDCARD");
  Token action("[ACTION]","ACCEPT");
  Token orig("[ORIGINAL]","UPF_RULE");
  Token ruleset_name("[RULESET]","DEFAULT");
  Token ruletype("[RULE]","FREE");
  Token unknown_match("[UNKNOWN_MATCHES]","FALSE");


  result = tokenizer_.tokenize(new_str);
  if(result.size() == 10){
    if(result[0].value_ != "0.0.0.0/0")
      l3_src.value_ = result[0].value_;
    if(result[1].value_ != "0.0.0.0/0")
      l3_dst.value_ = result[1].value_;

      l4_src.value_ = result[2].value_ + result[3].value_ + result[4].value_;
      l4_dst.value_ = result[5].value_ + result[6].value_ + result[7].value_;

      if(l4_src.value_ == "0:65535")
        l4_src.value_ = "WILDCARD";
      if(l4_dst.value_ == "0:65535")
        l4_dst.value_ = "WILDCARD";

      if(result[8].value_ == "0x06/0xFF")
        l4_proto.value_ = "tcp";
      else if(result[8].value_ == "0x11/0xFF")
        l4_proto.value_ = "udp";
      else if (result[8].value_ == "0x01/0xFF")
        l4_proto.value_ = "icmp";
      else if (result[8].value_ == "0x2f/0xFF")
        l4_proto.value_ = "gre";
      else if(result[8].value_ == "0x33/0xFF")
        l4_proto.value_=  "ah";

      if(result[9].value_ == "0x0000/0x0000")
        action.value_ = "ACCEPT";
      else if (result[9].value_ == "0x1000/0x1000")
        action.value_ = "DROP";
  }
  else if(result.size() == 9){
    if(result[0].value_ != "0.0.0.0/0")
      l3_src.value_ = result[0].value_;
    if(result[1].value_ != "0.0.0.0/0")
      l3_dst.value_ = result[1].value_;

    l4_src.value_ = result[2].value_ + result[3].value_ + result[4].value_;
    l4_dst.value_ = result[5].value_ + result[6].value_ + result[7].value_;

    if(l4_src.value_ == "0:65535")
      l4_src.value_ = "WILDCARD";
    if(l4_dst.value_ == "0:65535")
      l4_dst.value_ = "WILDCARD";

    if(result[8].value_ == "0x06/0xFF")
      l4_proto.value_ = "tcp";
    else if(result[8].value_ == "0x11/0xFF")
      l4_proto.value_ = "udp";
    else if (result[8].value_ == "0x01/0xFF")
      l4_proto.value_ = "icmp";
    else if (result[8].value_ == "0x2f/0xFF")
      l4_proto.value_ = "gre";
    else if(result[8].value_ == "0x33/0xFF")
      l4_proto.value_=  "ah";

    action.value_ = "ACCEPT";
}
  else
    throw std::string("UNKNOWN FILE");
//    ruletype.value_ = "LEGACY";

  tokens.push_back(l2_src);
  tokens.push_back(l2_dst);
  tokens.push_back(iface_in);
  tokens.push_back(iface_out);
  tokens.push_back(l3_proto);
  tokens.push_back(l3_src);
  tokens.push_back(l3_dst);
  tokens.push_back(l4_proto);
  tokens.push_back(l4_src);
  tokens.push_back(l4_dst);
  tokens.push_back(tcpflags);
  tokens.push_back(tcpstates);
  tokens.push_back(action);
  tokens.push_back(orig);
  tokens.push_back(ruleset_name);
  tokens.push_back(ruletype);
  tokens.push_back(unknown_match);
}

IPTablesParser::IPTablesParser()
  :   Parser(getIPTablesToken()){
}

void IPTablesParser::parse(std::string& str, std::vector<Token>& tokens){
  std::vector<Token> result;



  // Tokens for Rulegenerator
  Token l2_src("[L2_SRC]","WILDCARD");
  Token l2_dst("[L2_DST]","WILDCARD");
  Token iface_in("[INTERFACE_IN]","WILDCARD");
  Token iface_out("[INTERFACE_OUT]","WILDCARD");
  Token l3_proto("[L3_PROTOCOL]","WILDCARD");
  Token l3_src("[L3_SRC]","WILDCARD");
  Token l3_dst("[L3_DST]","WILDCARD");
  Token l4_proto("[L4_PROTOCOL]","WILDCARD");
  Token l4_src("[L4_SRC]","WILDCARD");
  Token l4_dst("[L4_DST]","WILDCARD");
  Token tcpflags("[TCPFLAGS]","WILDCARD");
  Token tcpstates("[TCPSTATES]","WILDCARD");
  Token action("[ACTION]","WILDCARD");
  Token orig("[ORIGINAL]",str);
  Token ruleset_name("[RULESET]","DEFAULT");
  Token ruletype("[RULE]","FREE");
  Token unknown_match("[UNKNOWN_MATCHES]","FALSE");

  result = tokenizer_.tokenize(str);
  std::string original;

  for (unsigned int i = 2; i < result.size(); ++i){
      original += result[i].value_;
      if(i < result.size() - 1)
        original += " ";
  }

  // get fields from tokenstream
  for (unsigned int i = 0; i < result.size(); ++i){




     // Rule Stage

    if(result[i].value_ == "-A")
      ruleset_name.value_ = result[++i].value_;
    else if(result[i].value_ == "-i" || result[i].value_=="--in-interface")
      iface_in.value_ = result[++i].value_;
    else if(result[i].value_ == "-o" || result[i].value_=="--out-interface")
      iface_out.value_ = result[++i].value_;
    else if(result[i].value_ == "-p" || result[i].value_ == "-protocol")
      l4_proto.value_ = result[++i].value_;
    else if(result[i].value_ == "-m"){
      Token next_token = result[++i];
      if(next_token.value_ == "state"){
        if(result[i+1].value_ == "--state")
          tcpstates.value_ = result[++(++i)].value_;
      }
      else if(next_token.value_ == "tcp"){
        if(result[i+1].value_ == "--sport")
          l4_src.value_ = result[++(++i)].value_;
            else if (result[i+1].value_ == "--dport")
              l4_dst.value_ = result[++(++i)].value_;
            else if (result[i+1].value_ == "--tcp-flags")
              tcpflags.value_ = result[++(++(++i))].value_;
      }
      else if(next_token.value_ == "udp"){
        if(result[i+1].value_ == "--sport")
          l4_src.value_ = result[++(++i)].value_;
        else if (result[i+1].value_ == "--dport")
          l4_dst.value_ = result[++(++i)].value_;
      }
      else if(next_token.value_ == "icmp"){
        if(result[i+1].value_ == "--icmp-type")
          l4_src.value_ = result[++(++i)].value_;
      }
      else if(next_token.value_ == "iprange"){
        l3_proto.value_ = "IPv4";
        if(result[i+1].value_ == "--src-range")
          l3_src.value_ = result[++(++i)].value_;
        else if(result[i+1].value_ == "--dst-range")
          l3_dst.value_ = result[++(++i)].value_;
      }
      else
        unknown_match.value_ = "TRUE";
    }
    else if(result[i].value_ == "-s" || result[i].value_ == "--src" ||
      result[i].value_ == "--source"){
      if(Utility::validateIPv4(result[i+1].value_)){
        l3_proto.value_ = "IPv4";
        l3_src.value_ = result[++i].value_;
      }
      else{
        l3_proto.value_ = "IPv6";
        l3_src.value_ = result[++i].value_;
      }
    }
    else if(result[i].value_ == "-d" || result[i].value_ == "--dst" ||
      result[i].value_ == "--destination"){
      if(Utility::validateIPv4(result[i+1].value_)){
        l3_proto.value_ = "IPv4";
        l3_dst.value_ = result[++i].value_;
      }
      else{
        l3_proto.value_ = "IPv6";
        l3_dst.value_ = result[++i].value_;
      }
    }
    else if(result[i].value_ == "-j")
      action.value_ = result[++i].value_;
    else if(result[i].value_ == "--sport")
      l4_src.value_ = result[++i].value_;
    else if (result[i].value_ == "--dport")
      l4_dst.value_ = result[++i].value_;
    else if (result[i].value_ == "--tcp-flags")
      tcpflags.value_ = result[++i].value_;
//    else
//      ruletype.value_ = "LEGACY";

  }

  orig.value_ = original;

  // Push tokens into token vector to pass to Rulegenerator

  tokens.push_back(l2_src);
  tokens.push_back(l2_dst);
  tokens.push_back(iface_in);
  tokens.push_back(iface_out);
  tokens.push_back(l3_proto);
  tokens.push_back(l3_src);
  tokens.push_back(l3_dst);
  tokens.push_back(l4_proto);
  tokens.push_back(l4_src);
  tokens.push_back(l4_dst);
  tokens.push_back(tcpflags);
  tokens.push_back(tcpstates);
  tokens.push_back(action);
  tokens.push_back(orig);
  tokens.push_back(ruleset_name);
  tokens.push_back(ruletype);
  tokens.push_back(unknown_match);
}

ipfwParser::ipfwParser()
    :   Parser(getipfwToken()){}

void ipfwParser::parse(std::string& str, std::vector<Token>& tokens){
  std::vector<Token> result;

  // Tokens for Rulegenerator
  Token l2_src("[L2_SRC]","WILDCARD");
  Token l2_dst("[L2_DST]","WILDCARD");
  Token iface_in("[INTERFACE_IN]","WILDCARD");
  Token iface_out("[INTERFACE_OUT]","WILDCARD");
  Token l3_proto("[L3_PROTOCOL]","WILDCARD");
  Token l3_src("[L3_SRC]","WILDCARD");
  Token l3_dst("[L3_DST]","WILDCARD");
  Token l4_proto("[L4_PROTOCOL]","WILDCARD");
  Token l4_src("[L4_SRC]","WILDCARD");
  Token l4_dst("[L4_DST]","WILDCARD");
  Token tcpflags("[TCPFLAGS]","WILDCARD");
  Token tcpstates("[TCPSTATES]","WILDCARD");
  Token action("[ACTION]","WILDCARD");
  Token orig("[ORIGINAL]",str);
  Token ruleset_name("[RULESET]","DEFAULT");
  Token ruletype("[RULE]","FREE");
  Token unknown_match("[UNKNOWN_MATCHES]","FALSE");

  result = tokenizer_.tokenize(str);

  for (unsigned int i = 0; i < result.size(); ++i){

    unsigned int end = result.size() -1;

    if(result[i].value_ == "allow")
      action.value_ = "ACCEPT";
    else if(result[i].value_ == "deny")
      action.value_ = "BLOCK";
//    else if(result[i].value_ == "in")
//      ruleset_name.value_ = "IN";
//    else if(result[i].value_ == "out")
//      ruleset_name.value_ = "OUT";
    else if(result[i].value_ == "tcp")
      l4_proto.value_ = "tcp";
    else if(result[i].value_ == "udp")
      l4_proto.value_ = "udp";
    else if(result[i].value_ == "icmp")
      l4_proto.value_ = "icmp";
    else if(result[i].value_ == "from"){
      l3_src.value_ = result[++i].value_;
      if(Utility::validateIPv4(l3_src.value_)){
        l3_proto.value_ = "IPv4";
      }
      else
        l3_proto.value_ = "IPv6";
        if(result[i+1].value_.find_first_not_of("1234567890") == std::string::npos)
          l4_src.value_ = result[++i].value_;

    }
    else if(result[i].value_ == "to"){
      l3_dst.value_ = result[++i].value_;
      if(Utility::validateIPv4(l3_dst.value_)){
        l3_proto.value_ = "IPv4";
      }
      else
        l3_proto.value_ = "IPv6";
      if(i != end){
        if(result[i+1].value_.find_first_not_of("1234567890") != std::string::npos)
          l4_dst.value_ = result[++i].value_;
      }
      }
    else if(!(result[i].value_ != "ipfw" || result[i].value_ != "-q"
            || result[i].value_ != "all" || result[i].value_ != "in"
            || result[i].value_ != "out" || result[i].value_ != "add"
            || result[i].value_.find_first_not_of("1234567890") == std::string::npos)){
      ruletype.value_ = "LEGACY";
      unknown_match.value_ = "TRUE";
    }
  }
//  std::cout << "L4_DST : " + l4_dst.value_ << std::endl;
//  std::cout << "TYPE : " + ruletype.value_ << std::endl;
  tokens.push_back(l2_src);
  tokens.push_back(l2_dst);
  tokens.push_back(iface_in);
  tokens.push_back(iface_out);
  tokens.push_back(l3_proto);
  tokens.push_back(l3_src);
  tokens.push_back(l3_dst);
  tokens.push_back(l4_proto);
  tokens.push_back(l4_src);
  tokens.push_back(l4_dst);
  tokens.push_back(tcpflags);
  tokens.push_back(tcpstates);
  tokens.push_back(action);
  tokens.push_back(orig);
  tokens.push_back(ruleset_name);
  tokens.push_back(ruletype);
  tokens.push_back(unknown_match);
}

pfParser::pfParser()
    :   Parser(getpfToken()){}


void pfParser::normalize(std::istream& input,std::stringstream& iss){
  std::string str;
  std::vector<std::string> quick_queue;
  std::stack<std::string> non_quick_stack;

  while(std::getline(input,str)){
    if(str[0] != '#'){
      if(str.find("quick") != std::string::npos)
        quick_queue.push_back(str);
      else
        non_quick_stack.push(str);
    }
    else
      iss << str << std::endl;
  }

  // Sort quick rules to top
  for (unsigned int i = 0; i < quick_queue.size(); ++i)
    iss << quick_queue[i] << std::endl;

  // add quicked remaining rules in reversed order
  while(!non_quick_stack.empty()){
    std::string str = non_quick_stack.top();
    if(str.find("in") != std::string::npos)
      str.insert(str.find("in")+3, "quick ");
    else if (str.find("out") != std::string::npos)
      str.insert(str.find("out")+4, "quick ");
    else if (str.find("pass") != std::string::npos)
      str.insert(str.find("pass")+5, "quick ");
    else if (str.find("block") != std::string::npos)
      str.insert(str.find("block")+6, "quick ");
    iss << str << std::endl;
    non_quick_stack.pop();
  }
}

void pfParser::parse(std::string& str, std::vector<Token>& tokens){

  std::vector<Token> result;

  // Tokens for Rulegenerator
  Token l2_src("[L2_SRC]","WILDCARD");
  Token l2_dst("[L2_DST]","WILDCARD");
  Token iface_in("[INTERFACE_IN]","WILDCARD");
  Token iface_out("[INTERFACE_OUT]","WILDCARD");
  Token l3_proto("[L3_PROTOCOL]","WILDCARD");
  Token l3_src("[L3_SRC]","WILDCARD");
  Token l3_dst("[L3_DST]","WILDCARD");
  Token l4_proto("[L4_PROTOCOL]","WILDCARD");
  Token l4_src("[L4_SRC]","WILDCARD");
  Token l4_dst("[L4_DST]","WILDCARD");
  Token tcpflags("[TCPFLAGS]","WILDCARD");
  Token tcpstates("[TCPSTATES]","WILDCARD");
  Token action("[ACTION]","WILDCARD");
  Token orig("[ORIGINAL]",str);
  Token ruleset_name("[RULESET]","DEFAULT");
  Token ruletype("[RULE]","FREE");
  Token unknown_match("[UNKNOWN_MATCHES]","FALSE");

  result = tokenizer_.tokenize(str);

  for (unsigned int i = 0; i < result.size(); ++i){

    unsigned int end = result.size() -1;

    if(result[i].value_ == "pass")
      action.value_ = "ACCEPT";
    else if(result[i].value_ == "block")
      action.value_ = "BLOCK";
//    else if(result[i].value_ == "in")
//      ruleset_name.value_ = "IN";
//    else if(result[i].value_ == "out")
//      ruleset_name.value_ = "OUT";
    else if(result[i].value_ == "proto"){
      Token next_token = result[++i];
      if (next_token.value_ == "tcp")
        l4_proto.value_ = "tcp";
      else if (next_token.value_ == "udp")
        l4_proto.value_ = "udp";
      else if (next_token.value_ == "icmp")
        l4_proto.value_ = "icmp";
    }
    else if(result[i].value_ == "from"){
      l3_src.value_ = result[++i].value_;
      if(Utility::validateIPv4(l3_src.value_)){
        l3_proto.value_ = "IPv4";
      }
      else
        l3_proto.value_ = "IPv6";
        if(result[i+1].value_ == "port")
          l4_src.value_ = result[i+2].value_;

    }
    else if(result[i].value_ == "to"){
      l3_dst.value_ = result[++i].value_;
      if(Utility::validateIPv4(l3_dst.value_)){
        l3_proto.value_ = "IPv4";
      }
      else
        l3_proto.value_ = "IPv6";
      if(i != end){
        if(result[i+1].value_ == "port")
          l4_dst.value_ = result[i+2].value_;
      }
      }
    else if(!(result[i].value_ != "quick" || result[i].value_ != "all"
            || result[i].value_ != "any" || result[i].value_ != "in"
            || result[i].value_ != "out")){
      ruletype.value_ = "LEGACY";
      unknown_match.value_ = "TRUE";
    }
  }
//  std::cout << "L4_DST : " + l4_dst.value_ << std::endl;
//  std::cout << "TYPE : " + ruletype.value_ << std::endl;
  tokens.push_back(l2_src);
  tokens.push_back(l2_dst);
  tokens.push_back(iface_in);
  tokens.push_back(iface_out);
  tokens.push_back(l3_proto);
  tokens.push_back(l3_src);
  tokens.push_back(l3_dst);
  tokens.push_back(l4_proto);
  tokens.push_back(l4_src);
  tokens.push_back(l4_dst);
  tokens.push_back(tcpflags);
  tokens.push_back(tcpstates);
  tokens.push_back(action);
  tokens.push_back(orig);
  tokens.push_back(ruleset_name);
  tokens.push_back(ruletype);
  tokens.push_back(unknown_match);

}

ipfParser::ipfParser()
    :   Parser(getpfToken()){}


void ipfParser::normalize(std::istream& input,std::stringstream& iss){
  std::string str;
  std::vector<std::string> quick_queue;
  std::stack<std::string> non_quick_stack;

  while(std::getline(input,str)){
    if(str[0] != '#'){
      if(str.find("quick") != std::string::npos)
        quick_queue.push_back(str);
      else
        non_quick_stack.push(str);
    }
    else
      iss << str << std::endl;
  }

  // Sort quick rules to top
  for (unsigned int i = 0; i < quick_queue.size(); ++i)
    iss << quick_queue[i] << std::endl;

  // add quicked remaining rules in reversed order
  while(!non_quick_stack.empty()){
    std::string str = non_quick_stack.top();
    if(str.find("in") != std::string::npos)
      str.insert(str.find("in")+3, "quick ");
    else if (str.find("out") != std::string::npos)
      str.insert(str.find("out")+4, "quick ");
    else if (str.find("pass") != std::string::npos)
      str.insert(str.find("pass")+5, "quick ");
    else if (str.find("block") != std::string::npos)
      str.insert(str.find("block")+6, "quick ");
    iss << str << std::endl;
    non_quick_stack.pop();
  }
}

void ipfParser::parse(std::string& str, std::vector<Token>& tokens){

  std::vector<Token> result;

  // Tokens for Rulegenerator
  Token l2_src("[L2_SRC]","WILDCARD");
  Token l2_dst("[L2_DST]","WILDCARD");
  Token iface_in("[INTERFACE_IN]","WILDCARD");
  Token iface_out("[INTERFACE_OUT]","WILDCARD");
  Token l3_proto("[L3_PROTOCOL]","WILDCARD");
  Token l3_src("[L3_SRC]","WILDCARD");
  Token l3_dst("[L3_DST]","WILDCARD");
  Token l4_proto("[L4_PROTOCOL]","WILDCARD");
  Token l4_src("[L4_SRC]","WILDCARD");
  Token l4_dst("[L4_DST]","WILDCARD");
  Token tcpflags("[TCPFLAGS]","WILDCARD");
  Token tcpstates("[TCPSTATES]","WILDCARD");
  Token action("[ACTION]","WILDCARD");
  Token orig("[ORIGINAL]",str);
  Token ruleset_name("[RULESET]","DEFAULT");
  Token ruletype("[RULE]","FREE");
  Token unknown_match("[UNKNOWN_MATCHES]","FALSE");

  result = tokenizer_.tokenize(str);

  for (unsigned int i = 0; i < result.size(); ++i){

    unsigned int end = result.size() -1;

    if(result[i].value_ == "pass")
      action.value_ = "ACCEPT";
    else if(result[i].value_ == "block")
      action.value_ = "BLOCK";
//    else if(result[i].value_ == "in")
//      ruleset_name.value_ = "IN";
//    else if(result[i].value_ == "out")
//      ruleset_name.value_ = "OUT";
    else if(result[i].value_ == "proto"){
      Token next_token = result[++i];
      if (next_token.value_ == "tcp")
        l4_proto.value_ = "tcp";
      else if (next_token.value_ == "udp")
        l4_proto.value_ = "udp";
      else if (next_token.value_ == "icmp")
        l4_proto.value_ = "icmp";
    }
    else if(result[i].value_ == "from"){
      l3_src.value_ = result[++i].value_;
      if(Utility::validateIPv4(l3_src.value_)){
        l3_proto.value_ = "IPv4";
      }
      else
        l3_proto.value_ = "IPv6";
        if(result[i+1].value_ == "port")
          l4_src.value_ = result[++(++i)].value_;
    }
    else if(result[i].value_ == "to"){
      l3_dst.value_ = result[++i].value_;
      if(Utility::validateIPv4(l3_dst.value_)){
        l3_proto.value_ = "IPv4";
      }
      else
        l3_proto.value_ = "IPv6";
      if(i != end){
        if(result[i+1].value_ == "port")
          l4_dst.value_ = result[++(++i)].value_;
      }
      }
    else if(!(result[i].value_ != "quick" || result[i].value_ != "all"
            || result[i].value_ != "any" || result[i].value_ != "in"
            || result[i].value_ != "out")){
      ruletype.value_ = "LEGACY";
      unknown_match.value_ = "TRUE";
    }
  }
//  std::cout << "L4_DST : " + l4_dst.value_ << std::endl;
//  std::cout << "TYPE : " + ruletype.value_ << std::endl;
  tokens.push_back(l2_src);
  tokens.push_back(l2_dst);
  tokens.push_back(iface_in);
  tokens.push_back(iface_out);
  tokens.push_back(l3_proto);
  tokens.push_back(l3_src);
  tokens.push_back(l3_dst);
  tokens.push_back(l4_proto);
  tokens.push_back(l4_src);
  tokens.push_back(l4_dst);
  tokens.push_back(tcpflags);
  tokens.push_back(tcpstates);
  tokens.push_back(action);
  tokens.push_back(orig);
  tokens.push_back(ruleset_name);
  tokens.push_back(ruletype);
  tokens.push_back(unknown_match);

}


static std::unordered_map<std::string,std::string> getIPTablesToken(){

  std::unordered_map<std::string,std::string> tokens;

   // Table Commands
  tokens.emplace("-t","[TABLE COMMAND]");
  tokens.emplace("--table","[TABLE COMMAND]");

  // Tables
  tokens.emplace("filter","[TABLE]");
  tokens.emplace("nat","[TABLE]");
  tokens.emplace("mangle","[TABLE]");
  tokens.emplace("raw","[TABLE]");

  // Chains
  tokens.emplace("INPUT","[CHAIN]");
  tokens.emplace("OUTPUT","[CHAIN]");
  tokens.emplace("FORWARD","[CHAIN]");
  tokens.emplace("PREROUTING","[CHAIN]");
  tokens.emplace("POSTROUTING","[CHAIN]");

  // Chain Commands (short)
  tokens.emplace("-A","[CHAIN COMMAND]");
  tokens.emplace("-D","[CHAIN COMMAND]");
  tokens.emplace("-I","[CHAIN COMMAND]");
  tokens.emplace("-R","[CHAIN COMMAND]");
  tokens.emplace("-L","[CHAIN COMMAND]");
  tokens.emplace("-F","[CHAIN COMMAND]");
  tokens.emplace("-Z","[CHAIN COMMAND]");
  tokens.emplace("-N","[CHAIN COMMAND]");
  tokens.emplace("-X","[CHAIN COMMAND]");
  tokens.emplace("-P","[CHAIN COMMAND]");
  tokens.emplace("-E","[CHAIN COMMAND]");

  // Chain Commands (long)
  tokens.emplace("--append","[CHAIN COMMAND]");
  tokens.emplace("--delete","[CHAIN COMMAND]");
  tokens.emplace("--emplace","[CHAIN COMMAND]");
  tokens.emplace("--replace","[CHAIN COMMAND]");
  tokens.emplace("--list","[CHAIN COMMAND]");
  tokens.emplace("--flush","[CHAIN COMMAND]");
  tokens.emplace("--zero","[CHAIN COMMAND]");
  tokens.emplace("--new-chain","[CHAIN COMMAND]");
  tokens.emplace("--delete-chain","[CHAIN COMMAND]");
  tokens.emplace("--policy","[CHAIN COMMAND]");
  tokens.emplace("--rename-chain","[CHAIN COMMAND]");

  // Actions
  tokens.emplace("ACCEPT","[ACTION]");
  tokens.emplace("DROP","[ACTION]");
  tokens.emplace("QUEUE","[ACTION]");
  tokens.emplace("RETURN","[ACTION]");

  // Rule Parameters (short)
  tokens.emplace("-s","[PARAMETER]");
  tokens.emplace("-d","[PARAMETER]");
  tokens.emplace("-g","[PARAMETER]");
  tokens.emplace("-j","[PARAMETER]");
  tokens.emplace("-i","[PARAMETER]");
  tokens.emplace("-o","[PARAMETER]");
  tokens.emplace("-f","[PARAMETER]");
  tokens.emplace("-c","[PARAMETER]");
  tokens.emplace("-v","[PARAMETER]");
  tokens.emplace("-n","[PARAMETER]");
  tokens.emplace("-x","[PARAMETER]");
  tokens.emplace("-p","[PARAMETER]");
  tokens.emplace("-m","[MATCH_EXTENSION]");

  // Rule Parameters (long)
  tokens.emplace("--protocol","[PARAMETER]");
  tokens.emplace("--source","[PARAMETER]");
  tokens.emplace("--destination","[PARAMETER]");
  tokens.emplace("--goto","[PARAMETER]");
  tokens.emplace("--jump","[PARAMETER]");
  tokens.emplace("--in-interface","[PARAMETER]");
  tokens.emplace("--out-interface","[PARAMETER]");
  tokens.emplace("--fragment","[PARAMETER]");
  tokens.emplace("--set-counters","[PARAMETER]");
  tokens.emplace("--verbose","[PARAMETER]");
  tokens.emplace("--numeric","[PARAMETER]");
  tokens.emplace("--exact","[PARAMETER]");
  tokens.emplace("--line-numbers","[PARAMETER]");
  tokens.emplace("--match","[MATCH_EXTENSION]");

  // Match Extensions when -p (tcp,udp) or -m (match) is specified
  tokens.emplace("iprange","[OPTION]");
  tokens.emplace("--sport","[OPTION]");
  tokens.emplace("--source-port","[OPTION]");
  tokens.emplace("--sports","[OPTION]");
  tokens.emplace("--source-ports","[OPTION]");
  tokens.emplace("--dport","[OPTION]");
  tokens.emplace("--destination-port","[OPTION]");
  tokens.emplace("--dports","[OPTION]");
  tokens.emplace("--destination-ports","[OPTION]");
  tokens.emplace("--ports","[OPTION]");
  tokens.emplace("--src-range","[OPTION]");
  tokens.emplace("--dst-range","[OPTION]");
  tokens.emplace("--tcp-flags","[OPTION]");
  tokens.emplace("--syn","[OPTION]");
  tokens.emplace("state","[STATE]");
  tokens.emplace("--state","[OPTION]");

  // Match Fields
  tokens.emplace("tcp","[PROTOCOL]");
  tokens.emplace("udp","[PROTOCOL]");
  tokens.emplace("icmp","[PROTOCOL]");
  tokens.emplace("all","[PROTOCOL]");

  // States
  tokens.emplace("NEW","[STATES]");
  tokens.emplace("ESTABLISHED","[STATES]");
  tokens.emplace("RELATED","[STATES]");
  tokens.emplace("INVALID","[STATES]");

  return tokens;
}

//static std::unordered_map<std::string,std::set<std::string>> next_tokens_IPTables(){
//
//    typedef std::unordered_map<std::string, std::set<std::string>> TokenMap;
//
//    TokenMap ipmap;
//    std::set<std::string> ipset;
//
//    // CHAIN
//
//    ipset.clear();
//    ipset.emplace("[CHAIN]");
//    ipmap.emplace("-A", ipset);
//
//    // Interface
//
//    ipset.clear();
//    ipset.emplace("[UNKNOWN]");
//    ipmap.emplace("-i", ipset);
//    ipmap.emplace("-o", ipset);
//
//    // IP
//
//    ipset.clear();
//    ipset.emplace("[IP]");
//    ipset.emplace("[IPCIDR]");
//    ipset.emplace("[IPRANGE]");
//    ipmap.emplace("-s", ipset);
//    ipmap.emplace("-d", ipset);
//
//    //Protocol
//
//    ipset.clear();
//    ipset.emplace("[PROTOCOL]");
//    ipmap.emplace("-p",ipset);
//
//    // Match Extensions
//
//    ipset.clear();
//    ipset.emplace("[PROTOCOL]");
//    ipset.emplace("[STATE]");
//    ipmap.emplace("-m",ipset);
//
//    // Ports
//
//    ipset.clear();
//    ipset.emplace("[NUMBER]");
//    ipmap.emplace("--dport",ipset);
//    ipmap.emplace("--sport",ipset);
//
//    // State
//
//    ipset.clear();
//    ipset.emplace("[OPTION]");
//    ipmap.emplace("state", ipset);
//
//    ipset.clear();
//    ipset.emplace("[STATES]");
//    ipmap.emplace("--state",ipset);
//
//    // Action
//
//    ipset.clear();
//    ipset.emplace("[ACTION]");
//    ipmap.emplace("-j",ipset);
//
//
//    return ipmap;
//
//}

static std::unordered_map<std::string,std::string> getipfwToken(){
  std::unordered_map<std::string,std::string> tokens;

  // General
  tokens.emplace("ipfw","[COMMAND PREFIX]");
  tokens.emplace("from","[FROM]");
  tokens.emplace("to","[TO]");
  tokens.emplace("any","[ANY]");
  tokens.emplace("all","[ALL]");
  tokens.emplace("not","[NOT]");
  tokens.emplace("or","[OR]");

  // Set Commands
  tokens.emplace("set","[COMMAND]");
  tokens.emplace("add","[COMMAND]");

  // Commands
  tokens.emplace("add","[COMMAND]");
  tokens.emplace("delete","[COMMAND]");
  tokens.emplace("flush","[COMMAND]");
  tokens.emplace("show","[COMMAND]");
  tokens.emplace("list","[COMMAND]");
  tokens.emplace("zero","[COMMAND]");
  tokens.emplace("resetlog","[COMMAND]");

  // Command Options
  tokens.emplace("-a","[COMMAND OPTION]");
  tokens.emplace("-b","[COMMAND OPTION]");
  tokens.emplace("-c","[COMMAND OPTION]");
  tokens.emplace("-d","[COMMAND OPTION]");
  tokens.emplace("-e","[COMMAND OPTION]");
  tokens.emplace("-f","[COMMAND OPTION]");
  tokens.emplace("-i","[COMMAND OPTION]");
  tokens.emplace("-n","[COMMAND OPTION]");
  tokens.emplace("-N","[COMMAND OPTION]");
  tokens.emplace("-q","[COMMAND OPTION]");
  tokens.emplace("-S","[COMMAND OPTION]");
  tokens.emplace("-t","[COMMAND OPTION]");
  tokens.emplace("-T","[COMMAND OPTION]");

  // Actions
  tokens.emplace("allow","[ACTION]");
  tokens.emplace("accept","[ACTION]");
  tokens.emplace("pass","[ACTION]");
  tokens.emplace("permit","[ACTION]");
  tokens.emplace("check-state","[ACTION]");
  tokens.emplace("count","[ACTION]");
  tokens.emplace("deny","[ACTION]");
  tokens.emplace("drop","[ACTION]");
  tokens.emplace("divert","[ACTION]");
  tokens.emplace("fwd","[ACTION]");
  tokens.emplace("forward","[ACTION]");
  tokens.emplace("nat","[ACTION]");
  tokens.emplace("pipe","[ACTION]");
  tokens.emplace("queue","[ACTION]");
  tokens.emplace("reject","[ACTION]");
  tokens.emplace("reset","[ACTION]");
  tokens.emplace("reset6","[ACTION]");
  tokens.emplace("skipto","[ACTION]");
  tokens.emplace("call","[ACTION]");
  tokens.emplace("return","[ACTION]");
  tokens.emplace("tee","[ACTION]");
  tokens.emplace("unreach","[ACTION]");
  tokens.emplace("unreach6","[ACTION]");
  tokens.emplace("netgraph","[ACTION]");
  tokens.emplace("ngtee","[ACTION]");
  tokens.emplace("setfib","[ACTION]");
  tokens.emplace("setdscp","[ACTION]");
  tokens.emplace("reass","[ACTION]");

  // Match Options (some backward compatibility with older FreeBSD)
  tokens.emplace("ip","[PROTO]");
  tokens.emplace("ip4","[PROTO]");
  tokens.emplace("ip6","[PROTO]");
  tokens.emplace("ipv4","[PROTO]");
  tokens.emplace("ipv6","[PROTO]");
  tokens.emplace("ipv6","[PROTO]");

  // TODO

  return tokens;
}

static std::unordered_map<std::string,std::string> getpfToken(){

  std::unordered_map<std::string,std::string> tokens;
  return tokens;
}

static std::unordered_map<std::string,std::string> getcbToken(){

  std::unordered_map<std::string,std::string> tokens;
  tokens.emplace(":","[COLON]");
  tokens.emplace("0x00/0x00","[NOPROT]");
  tokens.emplace("0x01/0xFF","[ICMP]");
  tokens.emplace("0x06/0xFF","[TCP]");
  tokens.emplace("0x11/0xFF","[UDP]");
  tokens.emplace("0x2f/0xFF","[GRE]");

  return tokens;
}
