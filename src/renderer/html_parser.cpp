#include "html_parser.h"
#include <sstream>

AttributeMap parse_attributes(const std::string& attr_str) {
    AttributeMap attrs;
    std::istringstream iss(attr_str);
    std::string pair;
    while (iss >> pair) {
        size_t delim = pair.find('=');
        if (delim != std::string::npos) {
            std::string key = pair.substr(0, delim);
            std::string val = pair.substr(delim + 1);
            if (!val.empty() && (val.front() == '"' || val.front() == '\'')) val.erase(0, 1);
            if (!val.empty() && (val.back() == '"' || val.back() == '\'')) val.pop_back();
            attrs[key] = val;
        }
    }
    return attrs;
}

std::shared_ptr<DOMNode> parse_html_to_dom(const std::string& html, size_t& pos) {
    auto root = std::make_shared<DOMNode>();
    root->tag_name = "div"; 

    while (pos < html.size()) {
        if (html[pos] == '<') {
            size_t close_bracket = html.find('>', pos);
            if (close_bracket == std::string::npos) break;

            std::string tag_content = html.substr(pos + 1, close_bracket - pos - 1);
            pos = close_bracket + 1;

            if (tag_content[0] == '/') { 
                return root; 
            }

            size_t first_space = tag_content.find(' ');
            std::string tag_name = (first_space == std::string::npos) ? tag_content : tag_content.substr(0, first_space);
            std::string attr_str = (first_space == std::string::npos) ? "" : tag_content.substr(first_space + 1);

            auto child = std::make_shared<DOMNode>();
            child->tag_name = tag_name;
            child->attributes = parse_attributes(attr_str);

            auto it = TAG_REGISTRY.find(tag_name);
            if (it != TAG_REGISTRY.end() && it->second.layout == LayoutType::Leaf) {
                root->children.push_back(child);
            } else {
                auto parsed_subtree = parse_html_to_dom(html, pos);
                child->children = parsed_subtree->children;
                child->text_content = parsed_subtree->text_content;
                root->children.push_back(child);
            }
        } else { 
            size_t next_open = html.find('<', pos);
            if (next_open == std::string::npos) next_open = html.size();
            
            std::string text_str = html.substr(pos, next_open - pos);
            pos = next_open;

            text_str.erase(0, text_str.find_first_not_of(" \t\n\r"));
            text_str.erase(text_str.find_last_not_of(" \t\n\r") + 1);

            if (!text_str.empty()) {
                auto text_node = std::make_shared<DOMNode>();
                text_node->is_text = true;
                text_node->text_content = text_str;
                root->children.push_back(text_node);
            }
        }
    }
    return root;
}