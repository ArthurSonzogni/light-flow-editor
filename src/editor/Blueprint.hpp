#ifndef EDITOR_BLUEPRINT_HPP
#define EDITOR_BLUEPRINT_HPP

#include <glm/glm.hpp>

namespace editor {
namespace blueprint {

struct Type {
  glm::vec4 color;
};

struct Slot {
  std::string label;
  Type type;
};

struct Node {
  std::string label;
  std::vector<Slot> input;
  std::vector<Slot> output;
};

struct Board {
  std::vector<Node> nodes;
};

}  // namespace blueprint
}  // namespace editor

#endif /* end of include guard: EDITOR_BLUEPRINT_HPP */
