#ifndef DOM_NODE_H
#define DOM_NODE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

using AttributeMap = std::unordered_map<std::string, std::string>;

enum class LayoutType { Vertical, Horizontal, Leaf };

struct TagConfig {
    LayoutType layout;
};

struct DOMNode {
    std::string tag_name;
    std::string text_content;
    AttributeMap attributes;
    std::vector<std::shared_ptr<DOMNode>> children;
    bool is_text = false;
};

extern const std::unordered_map<std::string, TagConfig> TAG_REGISTRY;

#endif // DOM_NODE_H