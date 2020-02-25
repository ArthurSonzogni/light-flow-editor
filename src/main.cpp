#include <smk/Color.hpp>
#include <smk/Window.hpp>
#include "editor/Board.hpp"
#include "editor/Blueprint.hpp"

namespace editor {
namespace blueprint {

auto type_float = Type{{0.7f, 0.7f, 1.f, 1.f}};
auto type_vec3 = Type{{1.0f, 0.7f, 0.7f, 1.f}};
auto type_sdf = Type{{1.0f, 1.f, 1.f, 1.f}};

Board my_board = {{{
                    "number",
                    {},
                    {
                        {"out", type_float},
                    },
                },
                {
                    "time",
                    {},
                    {
                        {"out", type_float},
                    },
                },
                {
                    "vec3",
                    {
                        {"x", type_float},
                        {"y", type_float},
                        {"z", type_float},
                    },
                    {
                        {"out", type_vec3},
                    },
                },
                {
                    "translate",
                    {
                        {"in", type_sdf},
                        {"translate", type_float},
                    },
                    {
                        {"out", type_sdf},
                    },
                }}};
}  // namespace blueprint
}  // namespace editor


int main() {
  // Open a new window.
  auto window = smk::Window(512, 512, "test");

  editor::Board board(editor::blueprint::my_board);
  window.ExecuteMainLoop([&] {
    window.Clear({0.2, 0.2, 0.2, 1.0});
    window.PoolEvents();
    board.Step(&window, &window.input());
    board.Draw(&window);
    window.Display();
  });
  return EXIT_SUCCESS;
}
