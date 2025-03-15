#pragma once

#include "tile_map.h"
#include "tile_set.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/tile_map_layer.hpp>
#include <godot_cpp/templates/hash_map.hpp>

using namespace godot;

// Forward declaration
class BLTileMapLayer;

class BLTileMap2D : public BLTileMap {
    GDCLASS(BLTileMap2D, BLTileMap)

public:
    //region Core Functionality
    Error init() final;
    
    String get_terrain_name_by_coord(const Vector2i& p_coords) const final;
    
    void set_terrains(
        const TypedArray<Vector2i>& p_coords,
        const String& p_terrain,
        bool p_force = false
    ) final;
    
    void update() final;

    Ref<BLTerrain> get_terrain(const String& p_name) const final;

    Vector2i get_tile_coords(const Vector2& p_pixel_coords) const final;
    //endregion

    //region Layer Management
    /// Get layer by terrain type name
    _FORCE_INLINE_ BLTileMapLayer* get_layer(const String& p_name) const {
        return _layers[_terrain_layer_index[p_name]];
    }
    //endregion

protected:
    //region Internal Structures
    /// Track pending terrain modifications
    struct Dirty {
        HashMap<Vector2i, Pair<String, String>> terrains;
    };
    //endregion

    //region Protected Members
    
    /// Active layers by index
    HashMap<int, BLTileMapLayer*> _layers;
    //endregion

    //region Terrain Helpers
    /// Get pixel data from terrain texture
    int32_t _get_terrain_pixelv(const Vector2i& coords) const final;
    //endregion

    static void _bind_methods();

public:
    BLTileMap2D() = default;
    ~BLTileMap2D() = default;
};