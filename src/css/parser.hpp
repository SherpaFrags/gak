#pragma once

#include <optional>
#include <string>
#include <vector>

#include "lexer.hpp"

namespace css::parser {

struct Selector {
  std::optional<std::string> tag;
  std::optional<std::string> id;
  std::vector<std::string> classNames;
};

struct Value {
  std::string value;
  std::optional<std::string> unit;
};

struct Declaration {
  std::string property;
  Value value;
};

struct Rule {
  Selector selector;
  std::vector<Declaration> decls;
};

struct Stylesheet {
  std::vector<Rule> rules;
};

Stylesheet parse(std::vector<lexer::Token> tokens);

} // namespace css::parser
