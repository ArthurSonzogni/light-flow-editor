#ifndef EDITOR_BOARD_HPP
#define EDITOR_BOARD_HPP

#include <memory>
#include <smk/RenderTarget.hpp>
#include <smk/Input.hpp>

namespace editor {
class Node;
class Connector;
class Slot;

namespace blueprint {
struct Board;
}

class Board {
 public:
  Board(const blueprint::Board& blueprint);
  ~Board();
  void Step(smk::RenderTarget* target, smk::Input* input);
  void Draw(smk::RenderTarget* target);

  Slot* FindSlot(const glm::vec2& position);

 private:
  std::vector<std::unique_ptr<Node>> nodes_;
  std::vector<std::unique_ptr<Connector>> connectors_;
  Node* selected_node_ = nullptr;
  Slot* start_slot_ = nullptr;
  glm::vec2 view_shiffting_;
  glm::vec2 grab_point_;

  glm::vec2 connector_in_;
  glm::vec2 connector_in_pushed_;
  glm::vec2 connector_out_pushed_;
  glm::vec2 connector_out_;
};

}  // namespace editor

#endif /* end of include guard: EDITOR_BOARD_HPP */
