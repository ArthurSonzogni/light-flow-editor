#include <iostream>
#include "editor/Board.hpp"
#include <smk/Input.hpp>
#include "editor/Node.hpp"
#include "editor/Connector.hpp"
#include "editor/Slot.hpp"
#include "editor/Shape.hpp"

namespace editor {

Board::Board() {
  nodes_.push_back(std::make_unique<Node>());
  nodes_.back()->SetPosition({200,10});

  nodes_.push_back(std::make_unique<Node>());
  nodes_.back()->SetPosition({200,200});

  nodes_.push_back(std::make_unique<Node>());
  nodes_.back()->SetPosition({200,400});

  nodes_.push_back(std::make_unique<Node>());
  nodes_.back()->SetPosition({400,400});

  fake_node_ = std::make_unique<Node>();
}

Board::~Board() = default;

void Board::Step(smk::RenderTarget* target, smk::Input* input) {
  (void)target;
  auto cursor = input->cursor() + view_shiffting_;

  if (input->IsCursorPressed()) {
    start_slot_ = FindSlot(cursor);
    if (start_slot_)
      return;

    // Reorder moved node.
    for (auto& node : nodes_) {
      if (node->OnCursorPressed(cursor)) {
        selected_node_ = node.get();
        std::swap(node, nodes_.back());
        return;
      }
    }

    grab_point_ = view_shiffting_ + input->cursor();
  }

  if (input->IsCursorHold()) {
    if (start_slot_) {
      fake_slot_ = std::make_unique<Slot>(fake_node_.get(), cursor,
                                          !start_slot_->IsRight(),
                                          start_slot_->GetColor());
      if (Slot* end_slot = FindSlot(cursor)) {
        if (end_slot->GetColor() == start_slot_->GetColor()) {
        fake_slot_ = std::make_unique<Slot>(
            fake_node_.get(), end_slot->GetPosition(), end_slot->IsRight(),
            start_slot_->GetColor());
        }
      }
      fake_connector_ =
          std::make_unique<Connector>(start_slot_, fake_slot_.get());
      return;
    }

    if (selected_node_) {
      selected_node_->OnCursorMoved(cursor);
      return;
    }

    view_shiffting_ = grab_point_ - input->cursor();
    return;
  }

  if (input->IsCursorReleased()) {
    if (start_slot_) {
      if (Slot* end_slot = FindSlot(cursor)) {
        if (end_slot->GetColor() == start_slot_->GetColor()) {
          connectors_.push_back(
              std::make_unique<Connector>(start_slot_, end_slot));
        }
      }
      start_slot_ = nullptr;
      fake_connector_.reset();
      fake_slot_.reset();
    }
    selected_node_ = nullptr;
    return;
  }
}

void Board::Draw(smk::RenderTarget* target) {
  auto view = smk::View();
  view.SetCenter(target->dimension() * 0.5f + view_shiffting_);
  view.SetSize(target->dimension());
  target->SetView(view);

  for (const auto& node : nodes_)
    node->Draw(target);

  for (const auto& connector : connectors_)
    connector->Draw(target);

  if (fake_connector_)
    fake_connector_->Draw(target);
}

Slot* Board::FindSlot(const glm::vec2& position) {
  for (auto& node : nodes_) {
    Slot* slot = node->FindSlot(position);
    if (slot)
      return slot;
  }
  return nullptr;
}

}  // namespace editor
