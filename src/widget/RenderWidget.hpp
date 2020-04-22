// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#ifndef WIDGET_RENDER_WIDGET_HPP
#define WIDGET_RENDER_WIDGET_HPP

#include <smk/Framebuffer.hpp>
#include <smk/Shader.hpp>
#include <smk/Sprite.hpp>
#include <smk/Transformable.hpp>
#include <smkflow/widget/Widget.hpp>

extern float g_time;

class RenderWidget : public smkflow::Widget {
 public:
  static smkflow::WidgetFactory Create();
  static RenderWidget* From(smkflow::Widget* node);
  RenderWidget(smkflow::Widget::Delegate* delegate);
  glm::vec2 ComputeDimensions() override;
  void Draw(smk::RenderTarget* target) override;
  void Build(std::string new_code);

 private:
  float size_ = 256;
  std::string code_;

  smk::Framebuffer framebuffer_;

  smk::ShaderProgram shader_program_;
  smk::Shader vertex_shader_;
  smk::Shader fragment_shader_;

  smk::Sprite sprite_;

  smk::Transformable square_;
};

#endif /* end of include guard: WIDGET_RENDER_WIDGET_HPP */
