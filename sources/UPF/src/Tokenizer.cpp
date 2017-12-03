#include "Tokenizer.h"
#include <cstdlib>
#include "utility.h"

Token::Token(std::string type, std::string value)
  : type_(type),
    value_(value)
    {}

void Token::print() const{
  std::cout << "Type: " << type_ << " Value: " << value_ << std::endl;
}

Tokenizer::Tokenizer(std::unordered_map<std::string,std::string> valid_tokens)
  : valid_tokens_(valid_tokens)
    {}

std::vector<Token> Tokenizer::tokenize(std::string sequence_to_tokenize) const{
  std::istringstream istr(sequence_to_tokenize);
  std::istream_iterator<std::string> istriter(istr), end;
  std::vector<std::string> sequence(istriter, end);
  std::vector<Token> readTokens;
  for(auto sitr = sequence.begin(); sitr != sequence.end(); sitr++){
    auto iter = this->valid_tokens_.find(*sitr);
    if( iter != this->valid_tokens_.end()){

      // exact string match
      Token token(iter->second, *sitr);
      readTokens.push_back(token);
    }

      // check if string contains anything but digits
      else if (!sitr->empty() &&
                sitr->find_first_not_of("0123456789") == std::string::npos){

        Token token("[NUMBER]",*sitr);
        readTokens.push_back(token);
      }

      // check if string is an IP-Address
      else{
        IPTypes address = Utility::getIPType(*sitr);
        switch(address){
          case ipv4_single:
            readTokens.push_back(Token("[IP]", *sitr));
            break;
          case ipv4_range_colon:
            readTokens.push_back(Token("[IPRANGE]", *sitr));
            break;
          case ipv4_range_score:
            readTokens.push_back(Token("[IPRANGE]", *sitr));
            break;
          case ipv4_cidr:
            readTokens.push_back(Token("[IPCIDR]", *sitr));
            break;
          default:
            readTokens.push_back(Token("[UNKNOWN]", *sitr));
        }
      }
  }
  return readTokens;
}


bool operator==(const Token& tok1, const Token& tok2){
  if(tok1.type_.compare(tok2.type_) == 0 &&
     tok1.value_.compare(tok2.value_) == 0)

    return true;
  else
    return false;
}

 bool operator!=(const Token& tok1, const Token& tok2){
  if(tok1.type_.compare(tok2.type_) != 0 ||
     tok1.value_.compare(tok2.value_) != 0)

    return true;
  else
    return false;
}

bool operator==(const std::list<Token>& tokenlist1,
                const std::list<Token>& tokenlist2){

  if(tokenlist1.size() != tokenlist2.size())
    return false;
  auto iter2 = tokenlist2.begin();
  for( auto iter1 = tokenlist1.begin(); iter1 != tokenlist1.end(); iter1++){
    if(*iter1 != *iter2)
      return false;
    iter2++;
  }
  return true;
}
