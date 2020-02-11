#include <smk/Transformable.hpp>
#include <smk/VertexArray.hpp>

smk::Transformable RoundedRectangle(float width, float height, float radius);
smk::Transformable Path(std::vector<glm::vec2> points, float thickness);
smk::Transformable Spline(const std::vector<glm::vec2>& points,
                          size_t degree = 2,
                          float thickness = 1.0,
                          size_t subdivision = 10);
std::vector<glm::vec2> Bezier(const std::vector<glm::vec2>& points,
                              size_t subdivision = 10);
