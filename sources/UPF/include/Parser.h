#ifndef PARSER_H
#define PARSER_H
#include "Tokenizer.h"

class Parser{
public:
  virtual void parse(std::string& string, std::vector<Token>& tokens) = 0;
  Parser(std::unordered_map<std::string,std::string> dict);
  virtual ~Parser(){}
protected:
  Tokenizer tokenizer_;
  std::unordered_map<std::string,std::string> token_dict_;
  std::unordered_map<std::string,std::set<std::string>> next_tokens_dict_;
};

class cbParser : public Parser{
public:
  virtual void parse(std::string& string, std::vector<Token>& tokens);
  cbParser();
};

class pfParser : Parser{
public:
  virtual void parse(std::string& str, std::vector<Token>& tokens);
  void normalize(std::istream& filepath, std::stringstream& str);
  pfParser();
};

class ipfParser : Parser{
public:
  virtual void parse(std::string& str, std::vector<Token>& tokens);
  void normalize(std::istream& filepath, std::stringstream& str);
  ipfParser();
};


class ipfwParser : Parser{
public:
  virtual void parse(std::string& str, std::vector<Token>& tokens);
  ipfwParser();
};

class IPTablesParser : public Parser{
public:
  virtual void parse(std::string& str, std::vector<Token>& tokens);
  IPTablesParser();
};

#endif // PARSER_H
