#ifndef RENDERER_H
#define RENDERER_H

#include "dom_node.h"
#include <ftxui/dom/elements.hpp>
#include <functional>

using namespace ftxui;

using NodeModifier = std::function<Element(Element, const AttributeMap&)>;
extern std::unordered_map<std::string, NodeModifier> TAG_MODIFIERS;

Element apply_inline_styles(Element e, const AttributeMap& attrs);
Element compile_dom_to_ftxui(const std::shared_ptr<DOMNode>& node);

#endif // RENDERER_H