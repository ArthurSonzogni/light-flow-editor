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

void Connector::Draw(smk::RenderTarget* target) {
  if (!IsConnected())
    return;

  float d = glm::distance(A_->GetPosition(), B_->GetPosition());
  glm::vec2 strength(d * 0.4, 0);

  auto bezier = smk::Shape::Bezier(
      {
          A_->GetPosition(),
          A_->GetPosition() + (A_->IsRight() ? +strength : -strength),
          B_->GetPosition() + (B_->IsRight() ? +strength : -strength),
          B_->GetPosition(),
      },
      20);

  {
    auto path = smk::Shape::Path(bezier, 16);
    path.SetColor({0.0, 0.0, 0.0, 0.3});
    target->Draw(path);
  }
  {
    auto path = smk::Shape::Path(bezier, 10);
    path.SetColor(A_->GetColor());
    target->Draw(path);
  }
}

}  // namespace editor
