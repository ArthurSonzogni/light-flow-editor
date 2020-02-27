#include "editor/Connector.hpp"
#include <smk/Color.hpp>
#include <smk/Shape.hpp>
#include "editor/Slot.hpp"

namespace editor {

Connector::Connector(Slot* A, Slot* B) : A_(A), B_(B) {
  A_->Connect(this);
  B_->Connect(this);
}

Connector::~Connector() {
  Disconnect();
}

bool Connector::IsConnected() {
  return A_;
}

void Connector::Disconnect() {
  Slot* A = A_;
  Slot* B = B_;
  A_ = nullptr;
  B_ = nullptr;
  if (A)
    A->Disconnect(this);
  if (B)
    B->Disconnect(this);
}

void Connector::DrawBackground(smk::RenderTarget* target) {
  if (!IsConnected())
    return;
  RebuildVertex();
  target->Draw(background_);
}

void Connector::DrawForeground(smk::RenderTarget* target) {
  if (!IsConnected())
    return;
  RebuildVertex();
  target->Draw(foreground_);
}

void Connector::RebuildVertex() {
  glm::vec2 position_a = A_->GetPosition();
  glm::vec2 position_b = B_->GetPosition();

  if (position_a == position_a_ && position_b == position_b_)
    return;

  position_a_ = position_a;
  position_b_ = position_b;

  float d = glm::distance(position_a, position_b);
  glm::vec2 strength(d * 0.4, 0);

  auto bezier = smk::Shape::Bezier(
      {
          position_a,
          position_a + (A_->IsRight() ? +strength : -strength),
          position_b + (B_->IsRight() ? +strength : -strength),
          position_b,
      },
      16);

  background_ = smk::Shape::Path(bezier, 16);
  foreground_ = smk::Shape::Path(bezier, 10);

  background_.SetColor({0.0, 0.0, 0.0, 0.3});
  foreground_.SetColor(A_->GetColor());
}

}  // namespace editor
