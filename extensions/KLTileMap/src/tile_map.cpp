#include "tile_map.h"

TypedArray<String> BLTileMap::get_terrain_names() const {
	TypedArray<String> out;
	for (auto kv : _terrain_layer_index) {
		if (kv.value == 0) {
			continue;
		}
		out.append(kv.key);
	}
	return out;
}

void BLTileMap::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_config_path", "p_config_path"),
			&BLTileMap::set_config_path);
	ClassDB::bind_method(D_METHOD("get_config_path"),
			&BLTileMap::get_config_path);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "config_path"), "set_config_path",
			"get_config_path");
	ClassDB::bind_method(D_METHOD("set_region", "p_region"),
			&BLTileMap::set_region);
	ClassDB::bind_method(D_METHOD("get_region"), &BLTileMap::get_region);
	ADD_PROPERTY(PropertyInfo(Variant::RECT2I, "region"), "set_region",
			"get_region");
	ClassDB::bind_method(D_METHOD("set_noise_material", "p_material"), &BLTileMap::set_noise_material);
	ClassDB::bind_method(D_METHOD("get_noise_material"), &BLTileMap::get_noise_material);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "noise_material", PROPERTY_HINT_RESOURCE_TYPE, "Material"), "set_noise_material", "get_noise_material");

	ClassDB::bind_method(D_METHOD("init"), &BLTileMap::init);
	ClassDB::bind_method(D_METHOD("get_terrain_names"),
			&BLTileMap::get_terrain_names);
	ClassDB::bind_method(D_METHOD("get_terrain_name_by_coord", "p_coords"),
			&BLTileMap::get_terrain_name_by_coord);
	ClassDB::bind_method(
			D_METHOD("set_terrains", "p_coords", "p_terrain", "p_force"),
			&BLTileMap::set_terrains, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("update"), &BLTileMap::update);
	ClassDB::bind_method(D_METHOD("get_terrain", "p_name"), &BLTileMap::get_terrain);
	ClassDB::bind_method(D_METHOD("get_tile_coords", "p_pixel_coords"), &BLTileMap::get_tile_coords);
}
