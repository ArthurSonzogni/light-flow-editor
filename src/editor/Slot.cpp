#include "editor/Slot.hpp"
#include <smk/Color.hpp>
#include <smk/Shape.hpp>
#include <smk/Transformable.hpp>
#include "editor/Connector.hpp"
#include "editor/Node.hpp"

namespace editor {

smk::Transformable circle;
smk::Transformable circle_background;

void DrawEmptyConnector(smk::RenderTarget* target,
                        glm::vec2 position,
                        glm::vec4 color) {
  circle_background.SetPosition(position);
  circle_background.SetColor({0, 0, 0, 0.2});
  target->Draw(circle_background);

  circle.SetPosition(position);
  circle.SetColor(color);
  target->Draw(circle);
}

Slot::Slot(Node* node, glm::vec2 position, bool is_right, glm::vec4 color)
    : node_(node), position_(position), is_right_(is_right), color_(color) {
  circle_background = smk::Shape::Circle(8);
  circle = smk::Shape::Circle(5);
}

glm::vec2 Slot::GetPosition() {
  return node_->GetPosition() + position_;
}

void Slot::Draw(smk::RenderTarget* target) {
  DrawEmptyConnector(target, GetPosition(), color_);
}

void Slot::Connect(Connector* connector) {
  if (connector_)
    Disconnect(connector_);
  connector_ = connector;
}
void Slot::Disconnect(Connector* connector) {
  if (connector_ == connector) {
    connector_ = nullptr;
    connector->Disconnect();
  }
}

bool Slot::IsRight() {
  return is_right_;
}

glm::vec4 Slot::GetColor() {
  return color_;
}

}  // namespace editor
