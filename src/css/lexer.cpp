#include "lexer.hpp"
#include <cctype>
#include <optional>
#include <vector>

namespace css::lexer {

inline bool isIdentifierPart(char character) {
  return isalnum(character) || character == '-' || character == '_';
}

class SourceIterator {
public:
  SourceIterator(std::string &src) : text(src), position(0) {}

  std::optional<Token> next() {
    skipSpaces();
    if (position > text.length())
      return std::nullopt;

    char character = text[position];

    position++;

    switch (character) {
    case '#':
      return Token{TokenType::Hash, "#"};
    case '.':
      return Token{TokenType::Dot, "."};
    case '{': {
      nextTokenType = TokenType::DeclKey;
      return Token{TokenType::LeftBrace, "{"};
    }
    case ':': {
      nextTokenType = TokenType::DeclValue;
      return Token{TokenType::Colon, ":"};
    }
    case ';': {
      nextTokenType = TokenType::DeclKey;
      return Token{TokenType::Semicolon, ";"};
    }
    case '}': {
      nextTokenType = std::nullopt;
      return Token{TokenType::RightBrace, "}"};
    }
    }

    position--;

    if (nextTokenType.has_value()) {
      Token token;
      if (*nextTokenType == TokenType::DeclValue) {
        token = nextNumberValue();
        nextTokenType = TokenType::DeclUnit;
      } else {
        token = nextText(*nextTokenType);
        nextTokenType = std::nullopt;
      }

      return token;
    }

    if (isIdentifierPart(character)) {
      return nextText(TokenType::Identifier);
    }

    return std::nullopt;
  }

private:
  std::string &text;
  std::optional<TokenType> nextTokenType;
  size_t position;

  void skipSpaces() {
    while (position < text.length() && isspace(text[position])) {
      position++;
    }
  }

  Token nextText(TokenType type) {
    size_t start = position;
    while (position < text.length() && isIdentifierPart(text[position])) {
      position++;
    }

    return {type, text.substr(start, position - start)};
  }

  Token nextNumberValue() {
    size_t start = position;
    while (position < text.length() && isdigit(text[position])) {
      position++;
    }

    return {TokenType::DeclValue, text.substr(start, position - start)};
  }
};

std::vector<Token> lexString(std::string &input) {
  SourceIterator iter{input};
  std::vector<Token> output{};

  std::optional<Token> token;
  while (true) {
    token = iter.next();
    if (token == std::nullopt)
      break;
    output.push_back(*token);
  }

  return output;
}

} // namespace css::lexer
