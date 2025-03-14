#pragma once

#include "tile_set.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/tile_map_layer.hpp>
#include <godot_cpp/templates/hash_map.hpp>

using namespace godot;

// Forward declaration
class BLTileMapLayer;

class BLTileMap : public Node2D {
    GDCLASS(BLTileMap, Node2D)

public:
    //region Configuration Interface
    _FORCE_INLINE_ void set_config_path(const String& p_config_path) { _config_path = p_config_path; }
    _FORCE_INLINE_ String get_config_path() const { return _config_path; }
    
    _FORCE_INLINE_ void set_region(const Rect2i& p_region) { _region = p_region; }
    _FORCE_INLINE_ Rect2i get_region() const { return _region; }
    //endregion

    //region Core Functionality
    virtual Error init() = 0;
    TypedArray<String> get_terrain_names() const;
    
    virtual String get_terrain_name_by_coord(const Vector2i& p_coords) const = 0;
    virtual void set_terrains(
        const TypedArray<Vector2i>& p_coords,
        const String& p_terrain,
        bool p_force = false
    ) = 0;
    
    virtual void update() = 0;
    //endregion

protected:
    //region Internal Structures
    /// Track pending terrain changes
    struct Dirty {
        HashMap<Vector2i, Pair<String, String>> terrains;
    };
    //endregion

    //region Protected Members
    /// Path to tile configuration resource
    String _config_path;
    
    /// Cached terrain data texture
    Ref<Image> _terrain_img;
    
    /// Mapping of terrain names to layer indices
    HashMap<String, int> _terrain_layer_index;
    
    /// Valid operation area in grid coordinates
    Rect2i _region = Rect2i(-64, -64, 128, 128);
    
    /// Pending terrain modifications
    Dirty _dirty;
    //endregion

    //region Internal Helpers
    virtual int32_t _get_terrain_pixelv(const Vector2i& coords) const = 0;
    //endregion

    static void _bind_methods();

public:
    BLTileMap() = default;
    ~BLTileMap() = default;
};
