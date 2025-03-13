#pragma once
#include "Core/common.h"
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/node3d.hpp>

using namespace godot;

namespace godot {
class MeshInstance3D;
class SubViewport;
} // namespace godot
class BLTileSet;
class BLTileMapLayer;

class BLTileMap3D : public Node3D {
  GDCLASS(BLTileMap3D, Node3D)
public:
  _FORCE_INLINE_ void set_config_path(const String &p_config_path) {
    _config_path = p_config_path;
  }
  _FORCE_INLINE_ String get_config_path() const { return _config_path; }
  _FORCE_INLINE_ void set_region(const Rect2i &p_region) { _region = p_region; }
  _FORCE_INLINE_ Rect2i get_region() const { return _region; }
  _FORCE_INLINE_ void set_meter_per_tile(const Vector2 &p_meter_per_tile) {
    _meter_per_tile = p_meter_per_tile;
  }
  _FORCE_INLINE_ Vector2 get_meter_per_tile() const { return _meter_per_tile; }
  _FORCE_INLINE_ void set_slice_size(const Vector2i &p_slice_size) {
    _slice_size = p_slice_size;
  }
  _FORCE_INLINE_ Vector2i get_slice_size() const { return _slice_size; }

  TypedArray<String> get_terrain_names() const;
  String get_terrain(const Vector2i &p_coords) const;
  void set_terrains(const TypedArray<Vector2i> &p_coords,
                    const String &p_terrain, bool p_force = false);

  Error init();
  void update();

private:
  String _config_path;
  Rect2i _region = Rect2i(-64, -64, 128, 128);
  Vector2 _meter_per_tile = Vector2(2.0, 2.0);
  Vector2i _slice_size = Vector2i(4096, 4096);
  Ref<Image> _terrain_img;
  HashMap<String, int> _terrain_layer_index;
  struct Dirty {
    HashMap<Vector2i, Pair<String, String>> terrains;
  };
  Dirty _dirty;

  struct TileMapSlice {
    Rect2i region;
    MeshInstance3D *mesh_instance;
    SubViewport *subviewport;
    HashMap<int, BLTileMapLayer *> layers;
  };
  HashMap<Vector2i, TileMapSlice> _slices;
  Vector2i _slice_tile_shape;

  _FORCE_INLINE_ int32_t _get_terrain_pixelv(const Vector2i &p_coords) const {
    return _terrain_img->get_pixelv(p_coords - _region.position).get_r8();
  }

protected:
  static void _bind_methods();

public:
  BLTileMap3D() = default;
  ~BLTileMap3D() = default;
};
