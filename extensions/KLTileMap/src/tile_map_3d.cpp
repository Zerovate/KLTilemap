#include "tile_map_3d.h"
#include "Core/json_loader.h"
#include "tile_map_layer.h"
#include "tile_set.h"
#include <godot_cpp/classes/camera2d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/plane_mesh.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include <godot_cpp/classes/file_access.hpp>

using namespace godot;

void BLTileMap3D::_bind_methods()
{
  ClassDB::bind_method(D_METHOD("set_meter_per_tile", "p_meter_per_tile"),
                       &BLTileMap3D::set_meter_per_tile);
  ClassDB::bind_method(D_METHOD("get_meter_per_tile"),
                       &BLTileMap3D::get_meter_per_tile);
  ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "meter_per_tile"),
               "set_meter_per_tile", "get_meter_per_tile");
  ClassDB::bind_method(D_METHOD("set_slice_size", "p_slice_size"),
                       &BLTileMap3D::set_slice_size);
  ClassDB::bind_method(D_METHOD("get_slice_size"),
                       &BLTileMap3D::get_slice_size);
  ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "slice_size"), "set_slice_size",
               "get_slice_size");
}

Error BLTileMap3D::init()
{
  auto json = JsonLoader::load(_config_path);
  if (json.get_type() != Variant::DICTIONARY)
    return ERR_INVALID_DATA;
  if (!json.has_key("layers") || !json.has_key("layer_order"))
    return ERR_INVALID_DATA;
  Dictionary layers_dict = json.get("layers");
  Array layer_order_list = json.get("layer_order");
  HashMap<String, Ref<BLTileSet>> tile_sets;
  HashMap<String, Ref<Material>> noise_materials;
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
    {
      layer_path = _config_path.get_base_dir().path_join(layer_path);
    }
    if (!FileAccess::file_exists(layer_path))
      continue;
    auto terrain = BLTerrain::create(layer_path);
    if (!terrain.is_valid())
      continue;
    auto tile_set = BLTileSet::create(terrain);
    if (!tile_set.is_valid())
      continue;
    tile_sets[name] = tile_set;
    _terrain_layer_index[name] = idx + 1;
    if (layer_prop.has("noise_texture"))
    {
      String noise_texture_path = layer_prop["noise_texture"];
      if (noise_texture_path.is_relative_path())
      {
        noise_texture_path =
            _config_path.get_base_dir().path_join(noise_texture_path);
      }
      auto texture =
          ResourceLoader::get_singleton()->load(noise_texture_path);
      if (!texture.is_null())
      {
        auto shader = ResourceLoader::get_singleton()->load(
            "res://Assets/Shader/tile_data_noise.gdshader");
        Ref<ShaderMaterial> material = memnew(ShaderMaterial);
        material->set_shader(shader);
        material->set_shader_parameter("texture_noise", texture);
        noise_materials[name] = material;
      }
    }
  }
  _terrain_layer_index[""] = 0;

  if (tile_sets.is_empty())
    return ERR_INVALID_DATA;
  Vector2i tile_size = tile_sets.begin()->value->get_tile_size();
  _slice_tile_shape = _slice_size / tile_size;
  for (int x = _region.position.x; x < _region.position.x + _region.size.x;
       x += _slice_tile_shape.x)
  {
    for (int y = _region.position.y; y < _region.position.y + _region.size.y;
         y += _slice_tile_shape.y)
    {
      TileMapSlice slice;
      slice.region = Rect2i(x, y, _slice_tile_shape.x, _slice_tile_shape.y);
      Vector2i slice_end = slice.region.get_end();
      if (slice_end.x > _region.position.x + _region.size.x)
        slice_end.x = _region.position.x + _region.size.x;
      if (slice_end.y > _region.position.y + _region.size.y)
        slice_end.y = _region.position.y + _region.size.y;
      slice.region.set_end(slice_end);
      slice.mesh_instance = memnew(MeshInstance3D);
      Ref<PlaneMesh> mesh = memnew(PlaneMesh);
      mesh->set_size(Vector2(slice.region.size) * _meter_per_tile);
      slice.mesh_instance->set_mesh(mesh);
      Vector2 slice_center = slice.region.get_center() * _meter_per_tile;
      slice.mesh_instance->set_position(
          Vector3(slice_center.x, 0, slice_center.y));
      Ref<StandardMaterial3D> material = memnew(StandardMaterial3D);
      material->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);
      material->set_specular(0.0);
      material->set_flag(BaseMaterial3D::FLAG_USE_TEXTURE_REPEAT, false);
      slice.mesh_instance->set_material_override(material);
      add_child(slice.mesh_instance);
      slice.subviewport = memnew(SubViewport);
      slice.subviewport->set_size(_slice_size);
      slice.subviewport->set_update_mode(SubViewport::UPDATE_ONCE);
      slice.subviewport->set_disable_3d(true);
      slice.subviewport->set_disable_input(true);
      add_child(slice.subviewport);
      material->set_texture(StandardMaterial3D::TEXTURE_ALBEDO,
                            slice.subviewport->get_texture());
      auto camera = memnew(Camera2D);
      slice.subviewport->add_child(camera);
      for (auto kv : tile_sets)
      {
        auto terrain_name = kv.key;
        auto tile_set = kv.value;
        BLTileMapLayer *new_layer = memnew(BLTileMapLayer);
        new_layer->set_name(terrain_name);
        new_layer->set_tile_set(tile_set);
        new_layer->set_region(slice.region);
        if (noise_materials.has(terrain_name))
        {
          Ref<ShaderMaterial> noise_material =
              noise_materials[terrain_name]->duplicate(true);
          noise_material->set_shader_parameter(
              "vertex_offset", slice.region.get_center() * tile_size);
          noise_material->set_shader_parameter("uv1_scale", 0.1);
          new_layer->set_material(noise_material);
        }
        slice.layers[_terrain_layer_index[terrain_name]] = new_layer;
        slice.subviewport->add_child(new_layer);
      }
      Vector2i pos = Vector2i(x, y) / _slice_tile_shape;
      _slices[pos] = slice;
    }
  }
  auto terrain_region = _region.grow(2);
  _terrain_img =
      Image::create(terrain_region.get_size().width,
                    terrain_region.get_size().height, false, Image::FORMAT_R8);
  _dirty.terrains.clear();
  return OK;
}

String BLTileMap3D::get_terrain_name_by_coord(const Vector2i &p_coord) const
{
  auto t = _get_terrain_pixelv(p_coord);
  if (t == 0 || _slices.is_empty())
    return "";
  return _slices.begin()->value.layers[t]->get_name();
}

void BLTileMap3D::set_terrains(const TypedArray<Vector2i> &p_coords,
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
    _terrain_img->set_pixelv(
        coord - _region.get_position(),
        Color(_terrain_layer_index[p_terrain] / 255.0, 0, 0, 0));
    _dirty.terrains[coord].second = p_terrain;
  }
}

void BLTileMap3D::update()
{
  HashMap<String, HashMap<Vector2i, TypedArray<Vector2i>>> pre_dirty_map;
  HashMap<String, HashMap<Vector2i, TypedArray<Vector2i>>> cur_dirty_map;
  HashSet<Vector2i> changed_slices;
  for (auto pair : _dirty.terrains)
  {
    auto coord = pair.key;
    auto pos = coord / _slice_tile_shape;
    auto terrain_pair = pair.value;
    if (terrain_pair.first != "")
      pre_dirty_map[terrain_pair.first][pos].push_back(coord);
    if (terrain_pair.second != "")
      cur_dirty_map[terrain_pair.second][pos].push_back(coord);
    auto mod = coord % _slice_tile_shape;
    changed_slices.insert(pos);
    if (mod.x == 0)
    {
      changed_slices.insert(pos + Vector2i(-1, 0));
      if (mod.y == 0)
        changed_slices.insert(pos + Vector2i(-1, -1));
      if (mod.y == _slice_tile_shape.y - 1)
        changed_slices.insert(pos + Vector2i(-1, 1));
    }
    if (mod.x == _slice_tile_shape.x - 1)
    {
      changed_slices.insert(pos + Vector2i(1, 0));
      if (mod.y == 0)
        changed_slices.insert(pos + Vector2i(1, -1));
      if (mod.y == _slice_tile_shape.y - 1)
        changed_slices.insert(pos + Vector2i(1, 1));
    }
    if (mod.y == 0)
      changed_slices.insert(pos + Vector2i(0, -1));
    if (mod.y == _slice_tile_shape.y - 1)
      changed_slices.insert(pos + Vector2i(0, 1));
  }

  for (auto dirty : pre_dirty_map)
  {
    auto coords = dirty.value;
    for (auto kv : _slices)
    {
      auto pos = kv.key;
      auto layer = kv.value.layers[_terrain_layer_index[dirty.key]];
      for (auto coord_kv : coords)
      {
        auto cur_pos = coord_kv.key;
        layer->erase_terrains(coord_kv.value);
      }
    }
  }
  // Vector<Ref<Thread>> threads;
  for (auto dirty : cur_dirty_map)
  {
    auto coords = dirty.value;
    for (auto slice_kv : _slices)
    {
      auto slice_pos = slice_kv.key;
      auto layer = slice_kv.value.layers[_terrain_layer_index[dirty.key]];
      for (auto coord_kv : coords)
      {
        auto cur_pos = coord_kv.key;
        layer->set_terrains(coord_kv.value);
        // Ref<Thread> thread = memnew(Thread);
        // thread->start(Callable(layer, "set_terrains").bind(coord_kv.value));
        // threads.append(thread);
      }
    }
  }
  // for (auto thread : threads)
  //   thread->wait_to_finish();
  // threads.clear();
  for (auto pos : changed_slices)
  {
    if (!_slices.has(pos))
      continue;
    _slices[pos].subviewport->set_update_mode(SubViewport::UPDATE_ONCE);
  }
  _dirty.terrains.clear();
}
