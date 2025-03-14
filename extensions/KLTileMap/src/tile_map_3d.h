#pragma once

#include "tile_map.h"
#include "Core/common.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/node3d.hpp>

using namespace godot;

// Forward declarations
namespace godot
{
    class MeshInstance3D;
    class SubViewport;
}

class BLTileSet;
class BLTileMapLayer;

class BLTileMap3D : public BLTileMap
{
    GDCLASS(BLTileMap3D, BLTileMap)

public:
    // region Configuration Interface
    _FORCE_INLINE_ void set_meter_per_tile(const Vector2 &p_meter_per_tile)
    {
        _meter_per_tile = p_meter_per_tile;
    }
    _FORCE_INLINE_ Vector2 get_meter_per_tile() const
    {
        return _meter_per_tile;
    }

    _FORCE_INLINE_ void set_slice_size(const Vector2i &p_slice_size)
    {
        _slice_size = p_slice_size;
    }
    _FORCE_INLINE_ Vector2i get_slice_size() const
    {
        return _slice_size;
    }
    // endregion

    // region Core Functionality
    Error init() final;

    String get_terrain_name_by_coord(const Vector2i &p_coords) const final;

    void set_terrains(
        const TypedArray<Vector2i> &p_coords,
        const String &p_terrain,
        bool p_force = false) final;

    void update() final;

    Ref<BLTerrain> get_terrain(const String &p_name) const;
    // endregion

private:
    // region Internal Structures
    /// Manages a single terrain slice with rendering components
    struct TileMapSlice
    {
        /// Valid area in grid coordinates
        Rect2i region;

        /// 3D mesh representation
        MeshInstance3D *mesh_instance;

        /// Rendering viewport for this slice
        SubViewport *subviewport;

        /// Layer components by Z-index
        HashMap<int, BLTileMapLayer *> layers;
    };
    // endregion

    // region Private Members
    /// Tile size in 3D world units (meters)
    Vector2 _meter_per_tile = Vector2(2.0, 2.0);

    /// Optimal slice dimensions in tile units
    Vector2i _slice_size = Vector2i(4096, 4096);

    /// Active terrain slices by origin coordinates
    HashMap<Vector2i, TileMapSlice> _slices;

    /// Cached slice dimension in tiles
    Vector2i _slice_tile_shape;
    // endregion

    // region Terrain Helpers
    /// Get terrain data from cached image texture
    _FORCE_INLINE_ int32_t _get_terrain_pixelv(const Vector2i &p_coords) const final
    {
        return _terrain_img->get_pixelv(p_coords - _region.position).get_r8();
    }
    // endregion

protected:
    static void _bind_methods();

public:
    BLTileMap3D() = default;
    ~BLTileMap3D() = default;
};