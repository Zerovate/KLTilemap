#pragma once
#include "terrain.h"
#include <godot_cpp/classes/tile_set.hpp>

using namespace godot;
namespace godot {
class TileSet;
}

class BLTileSet : public TileSet {
  GDCLASS(BLTileSet, TileSet)
public:
  static const int TRANSPARENT = 0;
  static const int BL_TERRAIN = 1;
  static Ref<BLTileSet> create(const Ref<BLTerrain> &p_blterrain);

  _FORCE_INLINE_ Ref<BLTerrain> get_terrain() const { return _terrain; }
  _FORCE_INLINE_ HashMap<CellNeighbor, BLTerrain::TileFlag>
  get_flag_map() const {
    return _flag_map;
  }

private:
  static HashMap<CellNeighbor, BLTerrain::TileFlag> _flag_map;
  Ref<BLTerrain> _terrain;

protected:
  static void _bind_methods();

public:
  BLTileSet();
  ~BLTileSet() = default;
};
