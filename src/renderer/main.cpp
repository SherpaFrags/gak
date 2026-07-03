#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <sstream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

// Structure to hold parsed attributes like style="color:red" or class="sidebar"
using AttributeMap = std::unordered_map<std::string, std::string>;

struct DOMNode {
    std::string tag_name;
    std::string text_content;
    AttributeMap attributes;
    std::vector<std::shared_ptr<DOMNode>> children;
    bool is_text = false;
};

// --- THE EXTENSIBLE REGISTRY ---
enum class LayoutType { Vertical, Horizontal, Leaf };

struct TagConfig {
    LayoutType layout;
};

const std::unordered_map<std::string, TagConfig> TAG_REGISTRY = {
    {"div",  {LayoutType::Vertical}},
    {"p",    {LayoutType::Vertical}},
    {"body", {LayoutType::Vertical}},
    {"h1",   {LayoutType::Vertical}},
    {"h2",   {LayoutType::Vertical}},
    {"span", {LayoutType::Horizontal}},
    {"b",    {LayoutType::Horizontal}},
    {"strong",{LayoutType::Horizontal}},
    {"hr",   {LayoutType::Leaf}}
};

// --- HELPER PARSERS ---
AttributeMap parse_attributes(const std::string& attr_str) {
    AttributeMap attrs;
    std::istringstream iss(attr_str);
    std::string pair;
    while (iss >> pair) {
        size_t delim = pair.find('=');
        if (delim != std::string::npos) {
            std::string key = pair.substr(0, delim);
            std::string val = pair.substr(delim + 1);
            // Strip quotes
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

            if (tag_content[0] == '/') { // Closing Tag
                return root; 
            }

            // Separate tag name from attributes
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
        } else { // Text Content node
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

using NodeModifier = std::function<Element(Element, const AttributeMap&)>;

std::unordered_map<std::string, NodeModifier> TAG_MODIFIERS = {
    {"h1",     [](Element e, const AttributeMap&) { return e | bold | color(Color::Red); }},
    {"h2",     [](Element e, const AttributeMap&) { return e | bold | color(Color::Blue); }},
    {"b",      [](Element e, const AttributeMap&) { return e | bold; }},
    {"a",      [](Element e, const AttributeMap& attrs) { 
                    auto it = attrs.find("href");
                    if (it != attrs.end()) {
                        return e | color(Color::Blue) | underlined;
                    }
                    return e; 
                }},
    {"strong", [](Element e, const AttributeMap&) { return e | bold; }},
    {"hr",     [](Element, const AttributeMap&)   { return separator(); }}
};

// Global style modifier parser (processes inline CSS-like properties seamlessly)
Element apply_inline_styles(Element e, const AttributeMap& attrs) {
    auto it = attrs.find("style");
    if (it == attrs.end()) return e;

    std::string style_str = it->second;
    if (style_str.find("color:red") != std::string::npos) e = e | color(Color::Red);
    if (style_str.find("color:green") != std::string::npos) e = e | color(Color::Green);
    if (style_str.find("color:yellow") != std::string::npos) e = e | color(Color::Yellow);
    if (style_str.find("font-weight:bold") != std::string::npos) e = e | bold;
    
    return e;
}

// --- THE ENGINE PIPELINE ---
Element compile_dom_to_ftxui(const std::shared_ptr<DOMNode>& node) {
    if (node->is_text) {
        return text(node->text_content);
    }

    // Process all children recursively
    Elements children_elements;
    for (const auto& child : node->children) {
        children_elements.push_back(compile_dom_to_ftxui(child));
    }

    Element current_element = text("");
    auto reg_it = TAG_REGISTRY.find(node->tag_name);
    LayoutType layout = (reg_it != TAG_REGISTRY.end()) ? reg_it->second.layout : LayoutType::Vertical;

    if (layout == LayoutType::Vertical) {
        current_element = vbox(std::move(children_elements));
    } else if (layout == LayoutType::Horizontal) {
        current_element = hbox(std::move(children_elements));
    }

    auto mod_it = TAG_MODIFIERS.find(node->tag_name);
    if (mod_it != TAG_MODIFIERS.end()) {
        current_element = mod_it->second(current_element, node->attributes);
    }

    current_element = apply_inline_styles(current_element, node->attributes);

    return current_element;
}

int main() {
    // Dynamically add a brand new tag variant or style property on the fly!
    TAG_MODIFIERS["custom-alert"] = [](Element e, const AttributeMap&) {
        return e | borderLight | color(Color::Yellow) | bgcolor(Color::Red);
    };

    std::string advanced_html = 
        "<div>"
        "  <h1>Scalable HTML Compiler Engine</h1>"
        "  <hr>"
        "  <p>Engine features dynamic lookup maps for tag rendering modifiers.</p>"
        "  <a href=\"#testid\">Visit Example</a>"
        "  <span>"
        "    <b style=\"color:green\">Inline Style Match: </b>"
        "    <span style=\"color:yellow\"> This text reads processed inline styles safely.</span>"
        "  </span>"
        "  <custom-alert id=\"testid\"> CRITICAL ALERT SYSTEM NODE </custom-alert>"
        "</div>";

    size_t pos = 0;
    auto parsed_dom = parse_html_to_dom(advanced_html, pos);

    auto my_renderer = Renderer([&]() {
        return compile_dom_to_ftxui(parsed_dom) | border;
    });

    auto screen = ScreenInteractive::TerminalOutput();
    screen.Loop(my_renderer);

    return 0;
}