#pragma once
#ifdef CTEST

#include <nlohmann/json.hpp>

#include "../parser/ast.hpp"

using json = nlohmann::json;

namespace nlohmann {
  template <typename T>
  struct adl_serializer<std::unique_ptr<T>> {
    static auto to_json(json &j, std::unique_ptr<T> const &ptr) -> void {
      if (ptr.get()) {
        j = *ptr;
      } else {
        j = nullptr;
      }
    }
  };
} // namespace nlohmann

namespace dao {
  inline auto to_json(json &j, dao::function_arg const &arg) {
    j = json{{"name", arg.name}};
  }

  inline auto from_json(json const &j, dao::function_arg &arg) {
    j.at("name").get_to(arg.name);
  }

  inline auto to_json(json &j, dao::function_proto const &proto) {
    j = json{{"id", proto.id}, {"args", proto.args}};
  }

  inline auto from_json(json const &j, dao::function_proto &proto) {
    j.at("id").get_to(proto.id);
    j.at("args").get_to(proto.args);
  }

} // namespace dao

#endif
