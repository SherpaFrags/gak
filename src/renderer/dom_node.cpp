#include "dom_node.h"

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