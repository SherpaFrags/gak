#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include "dom_node.h"
#include <string>
#include <memory>

AttributeMap parse_attributes(const std::string& attr_str);
std::shared_ptr<DOMNode> parse_html_to_dom(const std::string& html, size_t& pos);

#endif // HTML_PARSER_H