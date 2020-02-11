#include "editor/Resource.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

std::string GetEnvironmentVariable(const char* env) {
  auto value = std::getenv(env);
  if (value)
    return value;
  return std::string();
}


std::string ResourcePath() {
  static bool initialized = false;
  static std::string resource_path;
  if (initialized)
    return resource_path;
  initialized = true;

  auto SNAP = GetEnvironmentVariable("SNAP");

  std::vector<std::string> path_list = {
    // Application installed using snapcraft.
    SNAP + "/usr/local/share/lightfloweditor/resources",

    // Application installed using "sudo make install"
    "/usr/local/share/lightfloweditor/resources",

    // Code build and run inside ${CMAKE_CURRENT_DIRECTORY}/build
    "../resources",

    // Code build and run inside ${CMAKE_CURRENT_DIRECTORY}
    "../resources",

    // Code build for WebAssembly.
    "/resources",

  };

  for (auto& path : path_list) {
    std::ifstream file(path + "/lightfloweditor");
    std::string line;
    if (std::getline(file, line) && line == "lightfloweditor") {
      resource_path = path;
    }
  }

  std::cerr << "Resource path = " << resource_path << std::endl;
  return resource_path;
}
