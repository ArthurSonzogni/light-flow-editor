#include <iostream>
#include "editor/Shape.hpp"

#include <smk/Shape.hpp>
#include <smk/Vertex.hpp>
#include <smk/VertexArray.hpp>

smk::Transformable RoundedRectangle(float width, float height, float radius) {
  width = width * 0.5 - radius;
  height = height * 0.5 - radius;
  std::vector<smk::Vertex> v;
  smk::Vertex p0 = {{0.f, 0.f}, {0.f, 0.f}};
  smk::Vertex p1 = {{width + radius, -height}, {0.f, 0.f}};
  smk::Vertex p2 = {{width + radius, height}, {0.f, 0.f}};

  v.push_back(p0);
  v.push_back(p1);
  v.push_back(p2);

  float angle_delta = radius * 0.01f;

  auto center = glm::vec2(width, radius);
  for (float angle = 0.f; angle < 2.f * M_PI; angle += angle_delta) {
    if (angle > 0.75 * 2.f * M_PI)
      center = glm::vec2(width, -height);
    else if (angle > 0.5 * 2.f * M_PI)
      center = glm::vec2(-width, -height);
    else if (angle > 0.25 * 2.f * M_PI)
      center = glm::vec2(-width, +height);
    else
      center = glm::vec2(+width, +height);

    p1 = p2;
    p2 = {center + radius * glm::vec2(cos(angle), sin(angle)), {0.f, 0.f}};

    v.push_back(p0);
    v.push_back(p1);
    v.push_back(p2);
  }

  p1 = p2;
  p2 = {{width + radius, -height}, {0.f, 0.f}};
  v.push_back(p0);
  v.push_back(p1);
  v.push_back(p2);

  return smk::Shape::FromVertexArray(smk::VertexArray(std::move(v)));
}

smk::Transformable Path(std::vector<glm::vec2> points, float thickness) {
  using namespace glm;
  std::vector<glm::vec3> planes_left;
  std::vector<glm::vec3> planes_right;

  thickness *= 0.5f;
  // Compute planes shifted by +/- |thickness| around lines:
  // points[i] -- points[i+1]

  for (size_t i = 1; i < points.size(); ++i) {
    glm::vec3 plane_left = cross(vec3(points[i - 1], 1.f), 
                                 vec3(points[i - 0], 1.f));
    glm::vec3 plane_right = plane_left;
    plane_left.z -= thickness * length(vec2(plane_left.x, plane_left.y));
    plane_right.z += thickness * length(vec2(plane_right.x, plane_right.y));
    planes_left.push_back(plane_left);
    planes_right.push_back(plane_right);
  }

  // Compute the intersection of plane[i] and plane[i+1]. It gives us the the
  // outline of the shape to be filled.
  std::vector<glm::vec2> points_left;
  std::vector<glm::vec2> points_right;

  // Cap begin.
  {
    glm::vec2 direction = normalize(points[1] - points[0]);
    glm::vec2 normal = {direction.y, -direction.x};
    points_left.push_back(points[0] - normal * thickness);
    points_right.push_back(points[0] + normal * thickness);
  }

  const float epsilon = 0.01;
  int i = 0;
  for (size_t j = 1; j < points.size()-1; ++j) {
    vec3 intersection_left = cross(planes_left[i], planes_left[j]);
    vec3 intersection_right = cross(planes_right[i], planes_right[j]);
    if (abs(intersection_left.z) < epsilon ||
        abs(intersection_right.z) < epsilon) {
      continue;
    }
    intersection_left /= intersection_left.z;
    intersection_right /= intersection_right.z;
    points_left.push_back({intersection_left.x, intersection_left.y});
    points_right.push_back({intersection_right.x, intersection_right.y});
    i = j;
  }

  // Cap end.
  {
    glm::vec2 direction =
        normalize(points[points.size() - 2] - points.back());
    glm::vec2 normal = {direction.y, -direction.x};
    points_left.push_back(points.back() + normal * thickness);
    points_right.push_back(points.back() - normal * thickness);
  }


  std::vector<smk::Vertex> v;
  //v.push_back(smk::Vertex{points.front(), {0.0, 0.0}});
  //v.push_back(smk::Vertex{points_right.front(), {0.0, 0.0}});
  //v.push_back(smk::Vertex{points_left.front(), {0.0, 0.0}});

  //v.push_back(smk::Vertex{points.back(), {0.0, 0.0}});
  //v.push_back(smk::Vertex{points_left.back(), {0.0, 0.0}});
  //v.push_back(smk::Vertex{points_right.back(), {0.0, 0.0}});

  // Fill using rectangles.
  // ...-A--C-...  A = points_left[i]
  //     |\ | ...  B = points_right[i]
  //     | \| ...  C = points_left[i + 1]
  // ...-B--D-...  D = points_right[i + 1];
  for (size_t i = 1; i < points_left.size(); ++i) {
    glm::vec2& A = points_left[i-1];
    glm::vec2& B = points_right[i-1];
    glm::vec2& C = points_left[i];
    glm::vec2& D = points_right[i];

    v.push_back(smk::Vertex{A, {0.0, 0.0}});
    v.push_back(smk::Vertex{B, {0.0, 0.0}});
    v.push_back(smk::Vertex{D, {0.0, 0.0}});

    v.push_back(smk::Vertex{A, {0.0, 0.0}});
    v.push_back(smk::Vertex{D, {0.0, 0.0}});
    v.push_back(smk::Vertex{C, {0.0, 0.0}});
  }

  return smk::Shape::FromVertexArray(smk::VertexArray(std::move(v)));
}

std::vector<glm::vec2> Bezier(const std::vector<glm::vec2>& points,
                              size_t subdivision) {
  std::vector<glm::vec2> path;
  for (size_t index = 0; index < subdivision + 1; ++index) {
    std::vector<glm::vec2> data = points;
    float x = float(index) / subdivision;
    while (data.size() >= 2) {
      for (size_t i = 0; i < points.size() - 1; ++i)
        data[i] = glm::mix(data[i], data[i + 1], x);
      data.resize(data.size() - 1);
    }
    path.push_back(data[0]);
  }
  return path;
}
