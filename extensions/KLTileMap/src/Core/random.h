#pragma once
#include <godot_cpp/classes/random_number_generator.hpp>

using namespace godot;

class BLRandom : public RandomNumberGenerator {
  GDCLASS(BLRandom, RandomNumberGenerator)
public:
  static Ref<BLRandom> get_instance() {
    static Ref<BLRandom> instance = memnew(BLRandom);
    return instance;
  }

  Vector2 randVec2() { return Vector2(randf(), randf()); }
  Vector2 randVec2_range(const Rect2 &range) {
    return Vector2(
        randf_range(range.position.x, range.size.x + range.position.x),
        randf_range(range.position.y, range.size.y + range.position.y));
  }

protected:
  static void _bind_methods() {
    ClassDB::bind_static_method("BLRandom", D_METHOD("get_instance"),
                                &BLRandom::get_instance);
  }

public:
  BLRandom() = default;
  ~BLRandom() = default;
};
