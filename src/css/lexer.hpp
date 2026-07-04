#pragma once

#include <string>
#include <vector>

namespace css::lexer {

enum class TokenType {
  Identifier,

  Hash,
  Dot,
  LeftBrace,
  RightBrace,
  Colon,
  Semicolon,

  DeclKey,
  DeclValue,
  DeclUnit,
};

struct Token {
  TokenType type;
  std::string content;
};

std::vector<Token> lexString(std::string &input);

} // namespace css::lexer
