#include "tile_set.h"
#include <godot_cpp/classes/tile_data.hpp>
#include <godot_cpp/classes/tile_set_atlas_source.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

HashMap<TileSet::CellNeighbor, BLTerrain::TileFlag> BLTileSet::_flag_map;

void BLTileSet::_bind_methods()
{
  ClassDB::bind_static_method("BLTileSet", D_METHOD("create", "json_path"),
                              &BLTileSet::create);
  ClassDB::bind_method(D_METHOD("get_terrain"), &BLTileSet::get_terrain);
}

Ref<BLTileSet> BLTileSet::create(const Ref<BLTerrain> &p_blterrain)
{
  ERR_FAIL_COND_V(!p_blterrain.is_valid(), nullptr);
  Ref<BLTileSet> out = memnew(BLTileSet);
  out->set_tile_size(p_blterrain->get_region_size());

  Ref<TileSetAtlasSource> source = memnew(TileSetAtlasSource);
  source->set_texture(p_blterrain->get_texture());
  source->set_margins(p_blterrain->get_margins());
  source->set_separation(p_blterrain->get_separation());
  source->set_texture_region_size(p_blterrain->get_region_size());
  auto flags = p_blterrain->get_all_flags();
  for (auto flag : flags)
    for (auto atlas_coord : p_blterrain->get_atlas_coords(flag))
      source->create_tile(atlas_coord);
  out->add_source(source);
  out->_terrain = p_blterrain;
  return out;
}

BLTileSet::BLTileSet()
{
  if (_flag_map.is_empty())
  {
    _flag_map[CELL_NEIGHBOR_RIGHT_SIDE] = BLTerrain::TileFlag(
        BLTerrain::MIDRIGHT | BLTerrain::RIGHTTOP | BLTerrain::RIGHTBOTTOM);
    _flag_map[CELL_NEIGHBOR_BOTTOM_RIGHT_CORNER] = BLTerrain::RIGHTBOTTOM;
    _flag_map[CELL_NEIGHBOR_BOTTOM_SIDE] = BLTerrain::TileFlag(
        BLTerrain::MIDBOTTOM | BLTerrain::RIGHTBOTTOM | BLTerrain::LEFTBOTTOM);
    _flag_map[CELL_NEIGHBOR_BOTTOM_LEFT_CORNER] = BLTerrain::LEFTBOTTOM;
    _flag_map[CELL_NEIGHBOR_LEFT_SIDE] = BLTerrain::TileFlag(
        BLTerrain::MIDLEFT | BLTerrain::LEFTTOP | BLTerrain::LEFTBOTTOM);
    _flag_map[CELL_NEIGHBOR_TOP_LEFT_CORNER] = BLTerrain::LEFTTOP;
    _flag_map[CELL_NEIGHBOR_TOP_SIDE] = BLTerrain::TileFlag(
        BLTerrain::MIDTOP | BLTerrain::RIGHTTOP | BLTerrain::LEFTTOP);
    _flag_map[CELL_NEIGHBOR_TOP_RIGHT_CORNER] = BLTerrain::RIGHTTOP;
  }
}
