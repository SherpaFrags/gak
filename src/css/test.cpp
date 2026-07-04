#include "lexer.hpp"
#include "parser.hpp"
#include <iostream>

int main() {
  std::string source = R"(
body#main {
  padding: 4px;
  flex-direction: row;
}
  )";
  auto tokens = css::lexer::lexString(source);
  auto parsed = css::parser::parse(tokens);

  std::cout << parsed.rules.size();

  return 0;
}
