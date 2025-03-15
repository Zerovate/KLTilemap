#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/core/type_info.hpp>
#include <godot_cpp/templates/hash_map.hpp>

using namespace godot;

class BLTerrain : public Resource {
	GDCLASS(BLTerrain, Resource)

public:
	enum TileFlag {
		LEFTTOP = 1 << 0, // 0b000000001
		RIGHTTOP = 1 << 1, // 0b000000010
		RIGHTBOTTOM = 1 << 2, // 0b000000100
		LEFTBOTTOM = 1 << 3, // 0b000001000
		MIDTOP = 1 << 4, // 0b000010000
		MIDRIGHT = 1 << 5, // 0b000100000
		MIDBOTTOM = 1 << 6, // 0b001000000
		MIDLEFT = 1 << 7, // 0b010000000
		CENTER = 1 << 8 // 0b100000000
	};

	static Ref<BLTerrain> create(const String &json_path);

	_FORCE_INLINE_ Ref<Texture2D> get_texture() const noexcept { return _texture; }
	_FORCE_INLINE_ Vector2i get_region_size() const noexcept { return _region_size; }
	Vector<int64_t> get_all_flags() const;

	_FORCE_INLINE_ Vector<Vector2i> get_atlas_coords(int64_t flag) const {
		ERR_FAIL_COND_V_MSG(!_atlas_coords.has(flag), Vector<Vector2i>(),
				vformat("Invalid flag: %d", flag));
		return _atlas_coords[flag];
	}

	_FORCE_INLINE_ int64_t get_flag_from_atlas_coord(const Vector2i &atlas_coord) const {
		ERR_FAIL_COND_V_MSG(!_atlas_coords_inv.has(atlas_coord), 0,
				vformat("Invalid coord: (%d, %d)", atlas_coord.x, atlas_coord.y));
		return _atlas_coords_inv[atlas_coord];
	}

	_FORCE_INLINE_ Vector2i get_margins() const noexcept { return _margins; }
	_FORCE_INLINE_ Vector2i get_separation() const noexcept { return _separation; }

	int get_tile_id_from_flag(BitField<TileFlag> flag);
	Ref<Texture2D> get_icon() const;

protected:
	static void _bind_methods();

private:
	Ref<Texture2D> _texture;
	HashMap<int64_t, Vector<Vector2i>> _atlas_coords;
	HashMap<Vector2i, int64_t> _atlas_coords_inv;
	Vector2i _region_size;
	Vector2i _margins{ 4, 4 };
	Vector2i _separation{ 8, 8 };
};

VARIANT_BITFIELD_CAST(BLTerrain::TileFlag);