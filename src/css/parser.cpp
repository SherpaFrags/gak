#include "parser.hpp"
#include "lexer.hpp"

#include <optional>

namespace css::parser {

enum class SelectorPartType { TAG, ID, CLASS };

class Parser {
public:
  Parser(std::vector<lexer::Token> &tokens) : tokens(tokens) {}

  std::optional<Rule> next() {
    if (index >= tokens.size())
      return std::nullopt;

    Rule rule{};
    rule.selector = nextSelector();

    std::optional<Declaration> decl;
    while ((decl = nextDeclaration()) != std::nullopt) {
      rule.decls.push_back(*decl);
    }

    return rule;
  }

private:
  std::vector<lexer::Token> &tokens;
  size_t index = 0;

  Selector nextSelector() {
    Selector selector{};
    auto currentType = SelectorPartType::TAG;

    lexer::Token token;

    while (index < tokens.size() && token.type != lexer::TokenType::LeftBrace) {
      token = tokens[index];

      switch (token.type) {
      case lexer::TokenType::Dot:
        currentType = SelectorPartType::CLASS;
        break;
      case lexer::TokenType::Hash:
        currentType = SelectorPartType::ID;
        break;
      case lexer::TokenType::Identifier: {
        switch (currentType) {
        case SelectorPartType::TAG:
          selector.tag = token.content;
          break;
        case SelectorPartType::ID:
          selector.id = token.content;
          break;
        case SelectorPartType::CLASS:
          selector.classNames.push_back(token.content);
          break;
        }
      }
      default:;
      }

      index++;
    }

    return selector;
  }

  std::optional<Declaration> nextDeclaration() {
    auto token = tokens[index++];
    if (token.type == lexer::TokenType::RightBrace) {
      return std::nullopt;
    }

    Declaration decl{};
    decl.property = token.content;

    index++; // skip colon
    decl.value.value = tokens[index++].content;

    lexer::Token maybeUnit = tokens[index++];
    if (maybeUnit.type == lexer::TokenType::DeclUnit) {
      decl.value.unit = maybeUnit.content;
      index++; // skip semicolon
    }

    return decl;
  }
};

Stylesheet parse(std::vector<lexer::Token> tokens) {
  Stylesheet output{};
  Parser parser{tokens};

  std::optional<Rule> next;
  while ((next = parser.next()) != std::nullopt) {
    output.rules.push_back(*next);
  }

  return output;
}

}; // namespace css::parser
