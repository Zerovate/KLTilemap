extends Control

@export var camera: Camera2D = null
@onready var _itemlist: ItemList = $ItemList
var _tilemap: BLTileMap

func load_tilemap(tilemap: BLTileMap):
    _tilemap = tilemap
    _itemlist.clear()
    for terrain_name in tilemap.get_terrain_names():
        var terrain: BLTerrain = tilemap.get_terrain(terrain_name)
        _itemlist.add_item(terrain_name, terrain.get_icon(), true)

func _unhandled_input(event: InputEvent) -> void:
    var selected := _itemlist.get_selected_items()
    if selected.is_empty() or camera == null:
        return
    var paint = false
    var erase = false
    var mouse_pos: Vector2
    var mb: InputEventMouseButton = event as InputEventMouseButton
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
        var map_pos = camera.get_canvas_transform().affine_inverse() * mouse_pos
        _tilemap.set_terrains([_tilemap.get_tile_coords(map_pos)], "" if erase else terrain_name)
        _tilemap.update()
