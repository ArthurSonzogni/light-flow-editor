#ifndef EDITOR_CONNECTOR_HPP
#define EDITOR_CONNECTOR_HPP

#include <smk/RenderTarget.hpp>

namespace editor {

class Slot;

class Connector {
 public:
  Connector(Slot* A, Slot* B);
  ~Connector();
  bool IsConnected();

  void Disconnect();
  void Draw(smk::RenderTarget* target);

 private:
  Slot* A_ = nullptr;
  Slot* B_ = nullptr;
};

}  // namespace editor

#endif /* end of include guard: EDITOR_CONNECTOR_HPP */
