#ifndef RULEGENERATOR_H_
#define RULEGENERATOR_H_

#include "UPF.h"
#include "Tokenizer.h"

class RuleGenerator{
public:
  std::unique_ptr<UPF_Rule> generate(std::vector<Token>& token) const;
};

#endif //RULEGENERATOR_H
