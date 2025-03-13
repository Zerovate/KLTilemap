#pragma once
#include <godot_cpp/classes/tile_map_layer.hpp>
#include <godot_cpp/templates/hash_set.hpp>

using namespace godot;

class BLTileMapLayer : public TileMapLayer {
  GDCLASS(BLTileMapLayer, TileMapLayer)
public:
  void set_region(const Rect2i &p_region);
  void set_terrains(const TypedArray<Vector2i> &p_coords);
  void erase_terrains(const TypedArray<Vector2i> &p_coords);

private:
  Rect2i _region;
  HashMap<Vector2i, int64_t> _terrain_flags;
  HashSet<Vector2i> _terrains;
  Vector2i _center = Vector2i(0, 0);

  HashMap<TileSet::CellNeighbor, Vector2i>
  _get_neighbors(const Vector2i &p_coord) const;
  int64_t _flag_from_neighbors(const Vector2i &p_coord) const;
  Vector<Vector2i> _atlas_coord_from_flag(int64_t flag) const;

protected:
  static void _bind_methods();

public:
  BLTileMapLayer() = default;
  ~BLTileMapLayer() = default;
};
