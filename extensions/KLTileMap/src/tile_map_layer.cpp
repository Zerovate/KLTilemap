#include "tile_map_layer.h"
#include "Core/common.h"
#include "tile_set.h"
#include <godot_cpp/classes/tile_data.hpp>

void BLTileMapLayer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_terrains", "coords"),
			&BLTileMapLayer::set_terrains);
	ClassDB::bind_method(D_METHOD("erase_terrains", "coords"),
			&BLTileMapLayer::erase_terrains);
}

void BLTileMapLayer::set_region(const Rect2i &p_region) {
	_region = p_region.grow(1);
	_center = p_region.get_center();
	_terrain_flags.clear();
}

HashMap<TileSet::CellNeighbor, Vector2i>
BLTileMapLayer::_get_neighbors(const Vector2i &p_coord) const {
	HashMap<TileSet::CellNeighbor, Vector2i> out;
	Ref<BLTileSet> tile_set = get_tile_set();
	ERR_FAIL_COND_V(!tile_set.is_valid(), out);
	for (auto flag_pair : tile_set->get_flag_map()) {
		auto neighbor = flag_pair.key;
		auto n_coord = get_neighbor_cell(p_coord, neighbor);
		if (!_region.has_point(n_coord)) {
			continue;
		}
		out[neighbor] = n_coord;
	}
	return out;
}

int64_t BLTileMapLayer::_flag_from_neighbors(const Vector2i &p_coord) const {
	Ref<BLTileSet> tile_set = get_tile_set();
	auto neighbors = _get_neighbors(p_coord);
	auto flag_map = tile_set->get_flag_map();
	int64_t out = 0b0;
	for (auto n_pair : neighbors) {
		auto neighbor = n_pair.key;
		auto coord = n_pair.value;
		if (_terrains.has(coord)) {
			out |= flag_map[neighbor];
		}
	}
	return out;
}

Vector<Vector2i> BLTileMapLayer::_atlas_coord_from_flag(int64_t flag) const {
	Ref<BLTileSet> tile_set = get_tile_set();
	return tile_set->get_terrain()->get_atlas_coords(flag);
}

inline Vector2i _rand_coord(const Vector<Vector2i> &coords) {
	return coords[UtilityFunctions::randi_range(0, coords.size() - 1)];
}

void BLTileMapLayer::set_terrains(const TypedArray<Vector2i> &p_coords) {
	Vector<Vector2i> inside_coords;
	for (auto idx = 0; idx < p_coords.size(); idx++) {
		Vector2i coord = p_coords[idx];
		if (!_region.has_point(coord)) {
			continue;
		}
		_terrains.insert(coord);
		inside_coords.push_back(coord);
	}
	if (inside_coords.is_empty()) {
		return;
	}

	for (auto coord : inside_coords) {
		_terrain_flags[coord] = 0b111111111;
		auto atlas_coord = _atlas_coord_from_flag(0b111111111);
		set_cell(coord - _center, 0, _rand_coord(atlas_coord), 0);
	}

	HashSet<Vector2i> trans_coords;
	for (auto coord : inside_coords) {
		for (auto n_pair : _get_neighbors(coord)) {
			auto n_coord = n_pair.value;
			if (_terrains.has(n_coord)) {
				continue;
			}
			trans_coords.insert(n_coord);
		}
	}
	for (auto coord : trans_coords) {
		auto flag = _flag_from_neighbors(coord);
		if (_terrain_flags.has(coord) && _terrain_flags[coord] == flag) {
			continue;
		}
		_terrain_flags[coord] = flag;
		auto atlas_coord = _atlas_coord_from_flag(flag);
		set_cell(coord - _center, 0, _rand_coord(atlas_coord), 0);
	}
}

void BLTileMapLayer::erase_terrains(const TypedArray<Vector2i> &p_coords) {
	Vector<Vector2i> inside_coords;
	for (auto i = 0; i < p_coords.size(); i++) {
		auto coord = p_coords[i];
		if (!_region.has_point(coord)) {
			continue;
		}
		_terrains.erase(coord);
		inside_coords.push_back(coord);
	}
	if (inside_coords.is_empty()) {
		return;
	}

	HashSet<Vector2i> dirty_terrains;
	HashSet<Vector2i> erase_cells;
	for (auto coord : inside_coords) {
		bool cell_isolate = true;
		for (auto n_pair : _get_neighbors(coord)) {
			if (_terrains.has(n_pair.value)) {
				cell_isolate = false;
			} else {
				dirty_terrains.insert(n_pair.value);
			}
		}
		if (cell_isolate) {
			erase_cells.insert(coord);
		} else {
			dirty_terrains.insert(coord);
		}
	}
	HashSet<Vector2i> trans_coords;
	for (auto coord : dirty_terrains) {
		bool isolate = true;
		for (auto n_pair : _get_neighbors(coord)) {
			if (_terrains.has(n_pair.value)) {
				isolate = false;
				trans_coords.insert(coord);
				break;
			}
		}
		if (isolate) {
			erase_cells.insert(coord);
		}
	}
	for (auto coord : trans_coords) {
		auto flag = _flag_from_neighbors(coord);
		if (_terrain_flags.has(coord) && _terrain_flags[coord] == flag) {
			continue;
		}
		_terrain_flags[coord] = flag;
		auto atlas_coord = _atlas_coord_from_flag(flag);
		set_cell(coord - _center, 0, _rand_coord(atlas_coord), 0);
	}
	for (auto coord : erase_cells) {
		erase_cell(coord - _center);
		_terrain_flags.erase(coord);
	}
}
