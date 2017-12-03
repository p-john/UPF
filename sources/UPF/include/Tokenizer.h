#ifndef TOKENIZER_H
#define TOKENIZER_H


#include <iterator>
#include <unordered_map>
#include <list>
#include <sstream>
#include <set>
#include "types.h"
#include "utility.h"

class Token{
public:
  void print() const;
  std::string type_;
  std::string value_;
  Token(std::string type, std::string value);
};

class Tokenizer{
public:
  Tokenizer(){};
  Tokenizer(std::unordered_map<std::string,std::string> validTokens);
  std::vector<Token> tokenize(std::string sequence_to_tokenize) const;
  std::unordered_map<std::string,std::string> valid_tokens_;
};

bool operator==(const Token& tok1, const Token& tok2);

bool operator==(const std::list<Token>& tokenlist1,
                const std::list<Token>& tokenlist2);

bool operator!=(const Token& tok1, const Token& tok2);

#endif //TOKENIZER_H
