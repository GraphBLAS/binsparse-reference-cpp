#pragma once

#include <filesystem>
#include <string>
#include <vector>

inline std::vector<std::string> files({"data/1138_bus/1138_bus.mtx",
                                       "data/chesapeake/chesapeake.mtx",
                                       "data/mouse_gene/mouse_gene.mtx"});

inline std::string find_prefix(std::string file_name) {
  if (std::filesystem::exists("../../" + file_name)) {
    return "../../";
  } else if (std::filesystem::exists("build/" + file_name)) {
    return "build/";
  } else {
    return "";
  }
}
