#include "renderer.h"

// --- THE DETAILED TAG MODIFIER REGISTRY ---
std::unordered_map<std::string, NodeModifier> TAG_MODIFIERS = {
    // --- HEADERS ---
    {"h1",     [](Element e, const AttributeMap&) { return e | bold | color(Color::Red); }},
    {"h2",     [](Element e, const AttributeMap&) { return e | bold | color(Color::Blue); }},
    {"h3",     [](Element e, const AttributeMap&) { return e | bold | color(Color::Cyan); }},
    {"h4",     [](Element e, const AttributeMap&) { return e | bold | color(Color::Green); }},
    {"h5",     [](Element e, const AttributeMap&) { return e | bold | color(Color::Yellow); }},
    {"h6",     [](Element e, const AttributeMap&) { return e | bold | color(Color::Magenta); }},

    // --- INLINE TEXT STYLES ---
    {"b",      [](Element e, const AttributeMap&) { return e | bold; }},
    {"strong", [](Element e, const AttributeMap&) { return e | bold; }},
    {"i",      [](Element e, const AttributeMap&) { return e | dim; }}, 
    {"em",     [](Element e, const AttributeMap&) { return e | dim; }},
    {"u",      [](Element e, const AttributeMap&) { return e | underlined; }},
    {"ins",    [](Element e, const AttributeMap&) { return e | underlined; }},
    {"del",    [](Element e, const AttributeMap&) { return e | strikethrough; }},
    {"s",      [](Element e, const AttributeMap&) { return e | strikethrough; }},
    {"small",  [](Element e, const AttributeMap&) { return e | dim; }},
    {"mark",   [](Element e, const AttributeMap&) { return e | color(Color::Black) | bgcolor(Color::Yellow); }},

    // --- ANCHORS (LINKS) ---
    {"a",      [](Element e, const AttributeMap& attrs) { 
                    if (attrs.find("href") != attrs.end()) {
                        return e | color(Color::BlueLight) | underlined;
                    }
                    return e; 
                }},

    // --- CODE & MONOSPACE ---
    {"code",   [](Element e, const AttributeMap&) { return e | bgcolor(Color::GrayDark) | color(Color::White); }},
    {"kbd",    [](Element e, const AttributeMap&) { return e | borderLight | bgcolor(Color::GrayLight) | color(Color::Black); }},
    {"samp",   [](Element e, const AttributeMap&) { return e | color(Color::GreenLight); }},
    {"pre",    [](Element e, const AttributeMap&) { return e | border | bgcolor(Color::Black); }},

    // --- STRUCTURAL & LAYOUT BLOCKS ---
    {"hr",         [](Element, const AttributeMap&)   { return separator(); }},
    {"blockquote", [](Element e, const AttributeMap&) { return hbox(text(" ┃ ") | color(Color::GrayLight), e | dim); }},
    {"fieldset",   [](Element e, const AttributeMap& attrs) {
                        auto it = attrs.find("legend");
                        std::string label = (it != attrs.end()) ? " " + it->second + " " : "";
                        return window(text(label), e);
                    }},

    // --- LISTS & LIST ITEMS ---
    {"ul",     [](Element e, const AttributeMap&) { return e; }}, 
    {"ol",     [](Element e, const AttributeMap&) { return e; }},
    {"li",     [](Element e, const AttributeMap&) { return hbox(text("  • ") | color(Color::GrayLight), e); }},

    // --- FORMS & INTERACTIVE COMPONENTS ---
    {"button", [](Element e, const AttributeMap& attrs) {
                    if (attrs.find("disabled") != attrs.end()) {
                        return e | borderHeavy | color(Color::GrayDark);
                    }
                    return e | borderDouble | color(Color::Cyan) | bold;
                }},
                
    {"input",  [](Element e, const AttributeMap& attrs) {
                    auto type_it = attrs.find("type");
                    if (type_it != attrs.end() && type_it->second == "checkbox") {
                        return e | color(Color::GreenLight);
                    }
                    return e | borderLight | bgcolor(Color::GrayDark);
                }},

    {"textarea", [](Element e, const AttributeMap&) { return e | borderLight; }},

    // --- SEMANTIC ALERTS / UTILITY TAGS ---
    {"alert-success", [](Element e, const AttributeMap&) { return e | borderLight | color(Color::Green) | bgcolor(Color::Black); }},
    {"alert-danger",  [](Element e, const AttributeMap&) { return e | borderLight | color(Color::Red) | bgcolor(Color::Black); }},
    {"alert-warning", [](Element e, const AttributeMap&) { return e | borderLight | color(Color::Yellow) | bgcolor(Color::Black); }}
};

// --- GLOBAL STYLE MODIFIER PARSER ---
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

    // Apply the tag specific structural or layout adjustments from our map
    auto mod_it = TAG_MODIFIERS.find(node->tag_name);
    if (mod_it != TAG_MODIFIERS.end()) {
        current_element = mod_it->second(current_element, node->attributes);
    }

    // Wrap with inline overrides last to ensure hierarchy priority
    current_element = apply_inline_styles(current_element, node->attributes);

    return current_element;
}