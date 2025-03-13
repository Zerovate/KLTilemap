#pragma once

#include "tile_set.h"
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/tile_map_layer.hpp>
#include <godot_cpp/templates/hash_map.hpp>

using namespace godot;

class BLTileMapLayer;

class BLTileMap : public Node2D {
  GDCLASS(BLTileMap, Node2D)
public:
  // virtual void _ready() override;
  void set_config_path(const String &p_config_path) {
    _config_path = p_config_path;
  }
  String get_config_path() const { return _config_path; }
  Error init();

  void set_region(const Rect2i &p_region);
  _FORCE_INLINE_ Rect2i get_region() const { return _region; }

  TypedArray<String> get_terrain_names() const;
  String get_terrain(const Vector2i &p_coords) const;
  void set_terrains(const TypedArray<Vector2i> &p_coords,
                    const String &p_terrain, bool p_force = false,
                    bool p_update_layers = true);

  _FORCE_INLINE_ BLTileMapLayer *get_layer(const String &p_name) const {
    return _layers[_terrain_index[p_name]];
  }
  void update_layers();

private:
  String _config_path;
  struct Dirty {
    HashMap<Vector2i, Pair<String, String>> terrains;
  };
  Ref<Image> _terrain_img;
  HashMap<String, int> _terrain_index;
  HashMap<int, BLTileMapLayer *> _layers;
  Rect2i _region = Rect2i(-50, -50, 100, 100);
  Dirty _dirty;

  int32_t _get_terrain_pixelv(const Vector2i &coords) const;

protected:
  static void _bind_methods();

public:
  BLTileMap() = default;
  ~BLTileMap() = default;
};
