#ifndef EDITOR_CONNECTOR_HPP
#define EDITOR_CONNECTOR_HPP

#include <smk/RenderTarget.hpp>
#include <smk/Transformable.hpp>

namespace editor {

class Slot;

class Connector {
 public:
  Connector(Slot* A, Slot* B);
  ~Connector();
  bool IsConnected();

  void Disconnect();
  void DrawForeground(smk::RenderTarget* target);
  void DrawBackground(smk::RenderTarget* target);

 private:
  void RebuildVertex();

  Slot* A_ = nullptr;
  Slot* B_ = nullptr;

  smk::Transformable background_;
  smk::Transformable foreground_;
  glm::vec2 position_a_ = glm::vec2(0.f, 0.f);
  glm::vec2 position_b_ = glm::vec2(0.f, 0.f);
};

}  // namespace editor

#endif /* end of include guard: EDITOR_CONNECTOR_HPP */
