#include "tile_map_2d.h"
#include "Core/common.h"
#include "Core/json_loader.h"
#include "tile_map_layer.h"
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/tile_data.hpp>
#include <godot_cpp/classes/tile_map_layer.hpp>
#include <godot_cpp/classes/file_access.hpp>

using namespace godot;

Error BLTileMap2D::init()
{
  if (_config_path.is_empty())
    return ERR_INVALID_PARAMETER;
  auto json = JsonLoader::load(_config_path);
  if (json.get_type() != Variant::DICTIONARY)
    return ERR_INVALID_DATA;
  if (!json.has_key("layers") || !json.has_key("layer_order"))
    return ERR_INVALID_DATA;
  Dictionary layers_dict = json.get("layers");
  Array layer_order_list = json.get("layer_order");
  for (int idx = 0; idx < layer_order_list.size(); idx++)
  {
    String name = layer_order_list[layer_order_list.size() - 1 - idx];
    if (!layers_dict.has(name))
      continue;
    Dictionary layer_prop = layers_dict[name];
    if (!layer_prop.has("path"))
      continue;
    String layer_path = layer_prop["path"];
    if (layer_path.is_relative_path())
      layer_path = _config_path.get_base_dir().path_join(layer_path);
    if (!FileAccess::file_exists(layer_path))
      continue;
    auto terrain = BLTerrain::create(layer_path);
    if (!terrain.is_valid())
      continue;
    auto tile_set = BLTileSet::create(terrain);
    if (!tile_set.is_valid())
      continue;
    BLTileMapLayer *new_layer = memnew(BLTileMapLayer);
    new_layer->set_name(name);
    new_layer->set_tile_set(tile_set);
    if (layer_prop.has("noise_texture") && get_material().is_valid())
    {
      String noise_texture_path = layer_prop["noise_texture"];
      if (noise_texture_path.is_relative_path())
        noise_texture_path =
            _config_path.get_base_dir().path_join(noise_texture_path);
      auto texture =
          ResourceLoader::get_singleton()->load(noise_texture_path);
      if (!texture.is_null())
      {
        Ref<ShaderMaterial> material = get_material()->duplicate();
        material->set_shader_parameter("texture_noise", texture);
        new_layer->set_material(material);
      }
    }
    _terrain_layer_index[name] = idx + 1;
    _layers[idx + 1] = new_layer;
    add_child(new_layer);
  }
  _terrain_layer_index[""] = 0;

  auto terrain_region = _region.grow(2);
  _terrain_img =
      Image::create(terrain_region.get_size().width,
                    terrain_region.get_size().height, false, Image::FORMAT_R8);
  for (auto layer_pair : this->_layers)
    layer_pair.value->set_region(_region);
  _dirty.terrains.clear();
  return OK;
}

String BLTileMap2D::get_terrain_name_by_coord(const Vector2i &p_coord) const
{
  auto t = _get_terrain_pixelv(p_coord);
  if (t == 0)
    return "";
  return _layers[t]->get_name();
}

void BLTileMap2D::set_terrains(const TypedArray<Vector2i> &p_coords,
                               const String &p_terrain,
                               bool p_force /* = false */)
{
  ERR_FAIL_COND(!_terrain_layer_index.has(p_terrain));
  for (auto coord_id = 0; coord_id < p_coords.size(); coord_id++)
  {
    const Vector2i &coord = p_coords[coord_id];
    if (!_region.has_point(coord))
      continue;
    auto pre_terrain = get_terrain_name_by_coord(coord);
    if (!p_force && pre_terrain == p_terrain)
      continue;
    if (!_dirty.terrains.has(coord))
      _dirty.terrains[coord] = Pair(pre_terrain, p_terrain);
    _terrain_img->set_pixelv(coord - _region.get_position(),
                             Color(_terrain_layer_index[p_terrain] / 255.0, 0, 0, 0));
    _dirty.terrains[coord].second = p_terrain;
  }
}

void BLTileMap2D::update()
{
  HashMap<String, TypedArray<Vector2i>> pre_dirty_map;
  HashMap<String, TypedArray<Vector2i>> cur_dirty_map;
  for (auto pair : _dirty.terrains)
  {
    auto coord = pair.key;
    auto terrain_pair = pair.value;
    if (terrain_pair.first != "")
      pre_dirty_map[terrain_pair.first].push_back(coord);
    if (terrain_pair.second != "")
      cur_dirty_map[terrain_pair.second].push_back(coord);
  }
  for (auto dirty : pre_dirty_map)
  {
    auto coords = dirty.value;
    auto layer = _layers[_terrain_layer_index[dirty.key]];
    layer->erase_terrains(coords);
  }
  Vector<Ref<Thread>> threads;
  for (auto dirty : cur_dirty_map)
  {
    auto layer = _layers[_terrain_layer_index[dirty.key]];
    auto coords = dirty.value;
    // layer->set_terrains(coords);
    Ref<Thread> thread = memnew(Thread);
    thread->start(Callable(layer, "set_terrains").bind(coords));
    threads.append(thread);
  }
  for (auto thread : threads)
    thread->wait_to_finish();
  threads.clear();
  _dirty.terrains.clear();
}

int32_t BLTileMap2D::_get_terrain_pixelv(const Vector2i &p_coord) const
{
  return _terrain_img->get_pixelv(p_coord - _region.get_position()).get_r8();
}

void BLTileMap2D::_bind_methods()
{
  ClassDB::bind_method(D_METHOD("get_layer", "name"), &BLTileMap2D::get_layer);
}
