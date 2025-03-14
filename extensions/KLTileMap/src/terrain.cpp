#include "terrain.h"
#include "Core/json_loader.h"
#include <godot_cpp/classes/atlas_texture.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void BLTerrain::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_icon"), &BLTerrain::get_icon);
}

Vector<int64_t> BLTerrain::get_all_flags() const {
	Vector<int64_t> out;
	for (const auto &E : _atlas_coords) {
		out.append(E.key);
	}
	return out;
}

namespace {
Dictionary setup_tile_flag_dict() {
	Dictionary out;
	out[0b0000] = 0;
	out[0b0001] = 1;
	out[0b0010] = 2;
	out[0b0100] = 3;
	out[0b1000] = 4;
	Dictionary tmp;
	tmp[0b0] = 5;
	tmp[0b1] = 6;
	out[0b0011] = tmp.duplicate();
	tmp.clear();
	out[0b0101] = 7;
	tmp[0b0] = 8;
	tmp[0b1000] = 9;
	out[0b1001] = tmp.duplicate();
	tmp.clear();
	tmp[0b0] = 10;
	tmp[0b10] = 11;
	out[0b0110] = tmp.duplicate();
	tmp.clear();
	out[0b1010] = 12;
	tmp[0b0] = 13;
	tmp[0b0100] = 14;
	out[0b1100] = tmp.duplicate();
	tmp.clear();
	tmp[0b00] = 15;
	tmp[0b01] = 16;
	tmp[0b10] = 17;
	tmp[0b11] = 18;
	Dictionary tmp2;
	tmp2[0b0011] = tmp.duplicate();
	out[0b0111] = tmp2.duplicate();
	tmp.clear();
	tmp2.clear();
	tmp[0b0000] = 19;
	tmp[0b0001] = 20;
	tmp[0b1000] = 21;
	tmp[0b1001] = 22;
	tmp2[0b1001] = tmp.duplicate();
	out[0b1011] = tmp2.duplicate();
	tmp.clear();
	tmp2.clear();
	tmp[0b0000] = 23;
	tmp[0b0100] = 24;
	tmp[0b1000] = 25;
	tmp[0b1100] = 26;
	tmp2[0b1100] = tmp.duplicate();
	out[0b1101] = tmp2.duplicate();
	tmp.clear();
	tmp2.clear();
	tmp[0b0000] = 27;
	tmp[0b0010] = 28;
	tmp[0b0100] = 29;
	tmp[0b0110] = 30;
	tmp2[0b0110] = tmp.duplicate();
	out[0b1110] = tmp2.duplicate();
	tmp.clear();
	tmp2.clear();
	tmp[0b0000] = 31;
	tmp[0b0001] = 32;
	tmp[0b0010] = 33;
	tmp[0b0100] = 34;
	tmp[0b1000] = 35;
	tmp[0b0011] = 36;
	tmp[0b0101] = 37;
	tmp[0b1001] = 38;
	tmp[0b0110] = 39;
	tmp[0b1010] = 40;
	tmp[0b1100] = 41;
	tmp[0b0111] = 42;
	tmp[0b1011] = 43;
	tmp[0b1101] = 44;
	tmp[0b1110] = 45;
	tmp[0b1111] = 46;
	tmp2[0b11111] = 47;
	tmp2[0b1111] = tmp.duplicate();
	out[0b1111] = tmp2.duplicate();
	return out;
}
} //namespace

int BLTerrain::get_tile_id_from_flag(BitField<TileFlag> flag) {
	static Dictionary tile_flag_dict = Dictionary();
	if (tile_flag_dict.is_empty()) {
		tile_flag_dict = setup_tile_flag_dict();
	}
	auto first_four = flag & 0b1111;
	auto level1 = tile_flag_dict[first_four];
	if (level1.get_type() == Variant::INT) {
		return level1;
	}
	Dictionary dict1 = level1;
	int64_t flag2 = flag >> 4;
	for (auto i = 0; i < dict1.size(); i++) {
		int64_t key = dict1.keys()[i];
		auto value = dict1[key];
		if (value.get_type() == Variant::INT) {
			if (key == flag2) {
				return value;
			}
		} else {
			Dictionary dict2 = value;
			return dict2[flag2 & key];
		}
	}
	return 0;
}

Ref<Texture2D> BLTerrain::get_icon() const {
	int w = this->_region_size.width;
	int h = this->_region_size.height;
	auto out_img = Image::create_empty(w * 2, h * 2, false, Image::FORMAT_RGBA8);

	auto fill_image_with_tile = [&](int64_t flag, Rect2i src_rect,
										Vector2i target) {
		auto coord = this->_atlas_coords[flag][0];
		auto atlas = memnew(AtlasTexture);
		atlas->set_atlas(_texture);
		atlas->set_region(Rect2i(coord.x, coord.y, w, h));
		out_img->blit_rect(atlas->get_image(), src_rect, target);
	};

	fill_image_with_tile(0b00000100, Rect2i(w / 2, h / 2, w / 2, h / 2),
			Vector2i(0, 0));
	fill_image_with_tile(0b01001100, Rect2i(0, h / 2, w, h / 2),
			Vector2i(w / 2, 0));
	fill_image_with_tile(0b00001000, Rect2i(0, h / 2, w / 2, h / 2),
			Vector2i(w * 3 / 2, 0));
	fill_image_with_tile(0b10001001, Rect2i(0, 0, w / 2, h),
			Vector2i(w * 3 / 2, h / 2));
	fill_image_with_tile(0b00000001, Rect2i(0, 0, w / 2, h / 2),
			Vector2i(w * 3 / 2, h * 3 / 2));
	fill_image_with_tile(0b00010011, Rect2i(0, 0, w, h / 2),
			Vector2i(w / 2, h * 3 / 2));
	fill_image_with_tile(0b00000010, Rect2i(w / 2, 0, w / 2, h / 2),
			Vector2i(0, h * 3 / 2));
	fill_image_with_tile(0b00100110, Rect2i(w / 2, 0, w / 2, h),
			Vector2i(0, h / 2));
	fill_image_with_tile(0b111111111, Rect2i(0, 0, w, h), Vector2i(w / 2, h / 2));
	return ImageTexture::create_from_image(out_img);
}

Ref<BLTerrain> BLTerrain::create(const String &path) {
	Ref<BLTerrain> out = memnew(BLTerrain);
	auto json = JsonLoader::load(path);
	if (json.get_type() != Variant::DICTIONARY) {
		return nullptr;
	}
	if (!json.has_key("name") || !json.has_key("size") || !json.has_key("tiles")) {
		return nullptr;
	}
	out->set_name(json.get("name"));
	Array _size = json.get("size");
	out->_region_size = Vector2(_size[0], _size[1]);
	auto image = Image::load_from_file(path.get_basename() + ".png");
	image->generate_mipmaps();
	out->_texture = ImageTexture::create_from_image(image);
	Array coords_array = json.get("tiles");
	for (int i = 0; i < coords_array.size(); i++) {
		Dictionary coords_dict = coords_array[i];
		int flag = coords_dict["flag"];
		int x = coords_dict["x"];
		int y = coords_dict["y"];
		Vector2i atlas_coord = (Vector2i(x, y) - out->_margins) /
				(out->_region_size + out->_separation);
		out->_atlas_coords[flag].append(atlas_coord);
		out->_atlas_coords_inv[atlas_coord] = flag;
	}
	return out;
}
