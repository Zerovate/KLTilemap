extends Control

@export var camera:Camera2D = null
@onready var _itemlist :ItemList = $TileEditor/ItemList
var _tilemap :BLTileMap

func load_tilemap(tilemap: BLTileMap):
    _tilemap = tilemap
    _itemlist.clear()
    for name in tilemap.get_terrain_names():
        var tile_layer :BLTileMapLayer= tilemap.get_layer(name)
        var tile_set :BLTileSet= tile_layer.tile_set
        var terrain :BLTerrain= tile_set.get_terrain()
        _itemlist.add_item(name, terrain.get_icon(), true)

func _unhandled_input(event: InputEvent) -> void:
    var selected := _itemlist.get_selected_items()
    if selected.is_empty() or camera == null:
        return
    var paint = false
    var erase = false
    var mouse_pos:Vector2
    var mb :InputEventMouseButton = event as InputEventMouseButton
    if mb != null and mb.is_pressed():
        var index = mb.get_button_index()
        match index:
            MOUSE_BUTTON_LEFT:
                paint = true
                mouse_pos = mb.get_position()
            MOUSE_BUTTON_RIGHT:
                erase = true
                mouse_pos = mb.get_position()
    var mm := event as InputEventMouseMotion
    if mm != null:
        var mask = mm.get_button_mask()
        if mask & MOUSE_BUTTON_LEFT:
            paint = true
            mouse_pos = mm.get_position()
        elif mask & MOUSE_BUTTON_RIGHT:
            erase = true
            mouse_pos = mm.get_position()
    if paint or erase:
        var item_idx := selected[0]
        var terrain_name = _itemlist.get_item_text(item_idx)
        var layer = _tilemap.get_layer(terrain_name)
        var map_pos = camera.get_global_transform_with_canvas().affine_inverse() * mouse_pos
        var local_pos = layer.to_local(map_pos)
        _tilemap.set_terrains([layer.local_to_map(local_pos)], "" if erase else terrain_name)
