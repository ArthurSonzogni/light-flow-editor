#include "editor/Node.hpp"
#include <smk/Color.hpp>
#include <smk/Font.hpp>
#include <smk/Shape.hpp>
#include "editor/Board.hpp"
#include "editor/Resource.hpp"
#include "editor/Shape.hpp"
#include "editor/Slot.hpp"

namespace editor {

smk::Font font;
float width = 150;
float height = 150;
float title_height = 32;
float padding = 3;

Node::Node() {
  base_ = smk::Shape::RoundedRectangle(width, height, 10);
  base_.SetColor({0.5, 0.5, 0.5, 0.5});
  base_.SetCenter(-width * 0.5, -height * 0.5);

  title_base_ = smk::Shape::RoundedRectangle(width, title_height, 10);
  title_base_.SetColor({0.5, 0.5, 0.5, 0.5});
  title_base_.SetCenter(-width * 0.5, -title_height * 0.5);

  font = smk::Font(ResourcePath() + "/arial.ttf", 20);
  title_ = smk::Text(font, "Translate");
  title_.SetColor(smk::Color::Black);

  static int i = 0;
  glm::vec4 color[] = {
      {0.5, 0.5, 1.0, 1.0},
      {1.0, 0.5, 0.5, 1.0},
      smk::Color::Yellow,
      smk::Color::Green,
  };

  slots_.push_back(std::make_unique<Slot>(this, glm::vec2(0, 50), false, color[i++%3]));
  slots_.push_back(std::make_unique<Slot>(this, glm::vec2(width, 50), true, color[i++%3]));
  slots_.push_back(std::make_unique<Slot>(this, glm::vec2(0, 82), false, color[i++%3]));
  slots_.push_back(std::make_unique<Slot>(this, glm::vec2(width, 82), true, color[i++%3]));
  slots_.push_back(std::make_unique<Slot>(this, glm::vec2(0, 112), false, color[i++%3]));
  slots_.push_back(std::make_unique<Slot>(this, glm::vec2(width, 112), true, color[i++%3]));
}

Node::~Node() = default;
void Node::Draw(smk::RenderTarget* target) {
  base_.SetPosition(position_);
  title_base_.SetPosition(position_);
  title_.SetPosition(position_ + glm::vec2(padding, padding));

  target->Draw(base_);
  target->Draw(title_base_);
  target->Draw(title_);

  for (auto& slot : slots_)
    slot->Draw(target);
}

bool Node::OnCursorPressed(glm::vec2 cursor) {
  bool hover = cursor.x > position_.x && cursor.x < position_.x + width &&
               cursor.y > position_.y && cursor.y < position_.y + height;

  if (!hover)
    return false;

  cursor_drag_point = position_ - cursor;
  return true;
}

void Node::OnCursorMoved(glm::vec2 cursor) {
  position_ = cursor_drag_point + cursor;
}

void Node::OnCursorReleased(glm::vec2) {}

void Node::SetPosition(const glm::vec2& position) {
  position_ = position;
}

const glm::vec2& Node::GetPosition() {
  return position_;
}

Slot* Node::FindSlot(const glm::vec2& position) {
  for (auto& slot : slots_) {
    glm::vec2 slot_position = slot->GetPosition();
    if (glm::distance(position, slot_position) < 16)
      return slot.get();
  }
  return nullptr;
}

}  // namespace editor
