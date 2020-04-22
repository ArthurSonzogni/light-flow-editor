// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include <glm/gtc/matrix_transform.hpp>
#include <smk/Color.hpp>
#include <smk/Framebuffer.hpp>
#include <smk/Shader.hpp>
#include <smk/Shape.hpp>
#include <smk/Sprite.hpp>
#include <smk/Window.hpp>

int main() {
  // Open a new window.
  auto window = smk::Window(512, 512, "test");

  // Shader
  auto vertex_shader = smk::Shader::FromString(R"(
    layout(location = 0) in vec2 space_position;
    layout(location = 1) in vec2 texture_position;
    uniform mat4 projection;
    uniform mat4 view;
    out vec2 screen_position;
    void main() {
      gl_Position = projection * view * vec4(space_position, 0.0, 1.0);
      screen_position = gl_Position.xy;
    }
  )",
                                               GL_VERTEX_SHADER);

  auto fragment_shader = smk::Shader::FromString(R"(
    in vec2 screen_position;
    uniform vec4 color;
    uniform mat4 square_rotation;
    uniform mat4 sphere_rotation;
    uniform float time;
    out vec4 out_color;

    struct Value {
      vec3 color;
      float distance;
    };

    Value sphere(float radius, vec3 pos) {
      Value value;
      value.distance = length(pos) - radius;
      value.color = vec3(1.0, 0.0, 0.0);
      return value;
    }

    vec3 repeat(vec3 pos, vec3 size) {
      return mod(pos+0.5* size, size)-0.5* size;
    }

    float oignon(float width, float distance) {
      return abs(distance + width) - width;
    }

    Value Cube(vec3 size, vec3 pos) {
      vec3 q = abs(pos) - size;
      vec3 clamped = clamp(pos, - size * 0.5, size * 0.5);
      Value value;
      value.distance = length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
      value.color = vec3(0.0, 1.0, 0.0);
      return value;
    }

    Value plane(vec3 direction, float distance, vec3 pos) {
      Value value;
      value.distance = dot(direction, pos) + distance;
      value.color = vec3(1.0, 1.0, 1.0);
      return value;
    }


    Value plane1(vec3 pos) {
      Value value;
      value.distance = pos.y + 0.8f;
      value.color = vec3(1.0, 0.0, 1.0);
      return value;
    }

    Value plane2(vec3 pos) {
      Value value;
      value.distance = -pos.z + 15.f;
      value.color = vec3(0.0, 1.0, 1.0);
      return value;
    }

    vec3 move(vec3 diff, vec3 position) {
      return position - diff;
    }

    Value Intersection(Value a, Value b) {
      Value value;
      value.distance = max(a.distance, b.distance);
      float lambda = clamp(a.distance / (a.distance + b.distance), 0.f, 1.f);
      value.color = mix(a.color, b.color, lambda);
      return value;
    }

    Value Substract(Value a, Value b) {
      Value value;
      value.distance = max(a.distance, -b.distance);
      float lambda = clamp(a.distance / (a.distance + b.distance), 0.f, 1.f);
      value.color = mix(a.color, b.color, lambda);
      return value;
    }

    Value Union(Value a, Value b) {
      Value value;
      value.distance = min(a.distance, b.distance);
      float lambda = clamp(abs(a.distance) / (abs(a.distance) + abs(b.distance)), 0.f, 1.f);
      value.color = mix(a.color, b.color, lambda);
      return value;
    }

    Value SmoothedUnion(float k, Value a, Value b) {
      Value value;
      float lambda = clamp(0.5 + 0.5 * (b.distance - a.distance) / k, 0.0, 1.0);
      value.distance = mix(b.distance, a.distance, lambda) - k*lambda*(1.0-lambda);
      value.color = mix(b.color, a.color, lambda);
      return value;
    }

    /*
    Value other_sdf(vec3 position) {

      float sphere_size = 2.0 + 2.0 * sin(time * 0.01f);

      position = move(vec3(0.0, -1.0, 3.0), position);
      vec3 static_position = move(vec3(0.f, -1.f, 3.f), position);
      vec3 sphere_position = (sphere_rotation * vec4(move(vec3(0.f, -1.f, 3.f), position), 1.f)).xyz;
      vec3 cube_position = (square_rotation * vec4(static_position, 1.f)).xyz;

      Value cube_wall = Cube(vec3(0.8), repeat(cube_position, vec3(2.6f, 2.6f, 100.f)));
      Value cube_wall2 = Cube(vec3(0.8), repeat(cube_position, vec3(2.6f, 100.f, 2.6f)));
      cube_wall = Union(cube_wall, cube_wall2);

      const float sin_size = 15.f;
      vec3 s = sin(cube_position * sin_size);
      cube_wall.distance += s.x * s.y * s.z * 0.05f;

      Value my_sphere = sphere(3.2, cube_position);
      //Value cube_wall = Cube(vec3(0.8), repeat(cube_position, vec3(3.f, 100.f, 3.f)));

      my_sphere.distance = oignon(1.2f, my_sphere.distance);
      my_sphere.distance = oignon(0.5f, my_sphere.distance);
      my_sphere.distance = oignon(0.2f, my_sphere.distance);
      //my_sphere.distance = oignon(0.1f, my_sphere.distance);

      Value value = Substract(cube_wall, my_sphere);

      Value v2 = my_sphere;
      v2.color = vec3(1.0, 1.0, 1.0);
      v2 = Intersection(v2, plane(vec3(-1.0,0.0,0.0), 0.0, sphere_position));
      v2 = Intersection(v2, plane(vec3(+0.8,0.0,0.2), 0.0, sphere_position));
      value = Union(value, v2);

      value = Union(value, plane1(static_position));
      value = Union(value, plane2(static_position));

      value.distance;
      return value;
    }
    */

    Value sdf(vec3 position) {
      vec3 p = move(vec3(0.0, 0.0, 10.0), position);
      vec3 b = (sphere_rotation * vec4(p, 1.f)).xyz;
      float d = 1.f;
      vec3 e = b / d;
      Value c = Cube(vec3(1.f,1.f,1.f),e);
      c.distance  *= d;
      return c;
    }

    vec3 differential(vec3 position) {
      float d = 0.001f;
      float b = sdf(position).distance;
      float fx = sdf(position + vec3(+d, +0, +0)).distance;
      float fy = sdf(position + vec3(+0, +d, +0)).distance;
      float fz = sdf(position + vec3(+0, +0, +d)).distance;
      return normalize(vec3(fx,fy, fz) - vec3(b));
    }

    vec3 ray(vec3 pos, vec3 direction) {
      float distance = 0.f;
      for(int i = 0; i<64; ++i) {
        distance = sdf(pos).distance;
        pos += direction * distance;
        if (distance < 0.001f)
          break;
      }

      return pos;
    }

    float softshadow(vec3 ro, vec3 rd, float mint, float maxt, float k ) {
        float res = 1.0;
        float ph = 1e20;
        for( float t=mint; t<maxt; )
        {
            float h = sdf(ro + rd*t).distance;
            if( h<0.001 )
                return 0.0;
            float y = h*h/(2.0*ph);
            float d = sqrt(h*h-y*y);
            res = min( res, k*d/max(0.0,t-y) );
            ph = h;
            t += h * 0.1;
        }
        return res;
    }

    void main() {
      vec3 direction = normalize(vec3(screen_position, 1.f));
      vec3 position = vec3(0.f);

      position = ray(position, direction);
      if (position.z > 100.f) {
        out_color = vec4(1.f, 0.f, 0.f, 1.f);

      } else {

        vec3 light_position = vec3(1.f, 3.1, +3.1);
        vec3 light_direction = normalize(light_position - position);
        vec3 surface_direction = differential(position);
        vec3 reflection_direction = -reflect(direction, surface_direction);

        float ambient_color = 0.2;
        float diffuse_color = 0.6 * max(0.f, dot(surface_direction, light_direction));
        float specular_color = 0.2 * max(0.f, pow(dot(reflection_direction, light_direction), 3.0));

        float phong = ambient_color + diffuse_color + specular_color;
        vec3 color = sdf(position).color;
        
        position += 0.01f * surface_direction;
        direction = light_direction;

        float shadow = 0.2+0.8*softshadow(position, light_direction,
            0.1f, distance(position, light_position), 5.f );

        phong *= shadow;
        color *= phong;
        out_color = vec4(color, 1.f);
      }
    }
  )",
                                                 GL_FRAGMENT_SHADER);

  auto shader_program = smk::ShaderProgram();
  shader_program.AddShader(vertex_shader);
  shader_program.AddShader(fragment_shader);
  shader_program.Link();

  size_t size = 512;
  auto framebuffer = smk::Framebuffer(size, size);
  framebuffer.SetShaderProgram(&shader_program);

  auto view = smk::View();
  view.SetCenter(0.f, 0.f);
  view.SetSize(1.f, 1.f);
  framebuffer.SetView(view);

  auto square = smk::Shape::Square();
  square.SetPosition(-0.5f, -0.5f);
  square.SetScale(1.0f, 1.f);

  window.ExecuteMainLoop([&] {
    float time = window.time();
    framebuffer.Clear({0.2, 0.2, 0.2, 1.0});
    // if (true)
    {
      auto view = smk::View();
      view.SetCenter(0.f, 0.f);
      view.SetSize(1.f, 1.f);
      framebuffer.SetView(view);

      shader_program.Use();
      shader_program.SetUniform("time", time);
      shader_program.SetUniform(
          "square_rotation",
          glm::rotate(glm::mat4(1.0), time * 0.04f, glm::vec3(0.f, 1.f, 0.f)));
      shader_program.SetUniform(
          "sphere_rotation",
          glm::rotate(glm::mat4(1.0), -time * 0.4f, glm::vec3(0.f, 1.f, 0.f)));
      framebuffer.Draw(square);
    }

    window.Clear({0.2, 0.2, 0.2, 1.0});
    window.PoolEvents();
    {
      window.shader_program_2d()->Use();
      auto square = smk::Sprite(framebuffer);
      square.SetScale(std::min(window.width(), window.height()) / float(size));
      window.Draw(square);
    }
    window.Display();
  });
  return EXIT_SUCCESS;
}
