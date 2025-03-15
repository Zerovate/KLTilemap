extends PanelContainer

@export var camera: Camera3D

@onready var _itemlist: ItemList = $VBoxContainer/ItemList
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
        var ray_origin = camera.project_ray_origin(mouse_pos)
        var ray_dir = camera.project_ray_normal(mouse_pos)
        if ray_dir.y == 0:
            return
        var t = - ray_origin.y / ray_dir.y
        if t < 0:
            return
        var hit_point = ray_origin + ray_dir * t
        var tile_pos = _tilemap.get_tile_coords(Vector2(hit_point.x, hit_point.z))

        var item_idx := selected[0]
        var terrain_name = _itemlist.get_item_text(item_idx)
        _tilemap.set_terrains([tile_pos], "" if erase else terrain_name)
        _tilemap.update()
        # var layer = _tilemap.get_layer(terrain_name)
        # var map_pos = camera.get_canvas_transform().affine_inverse() * mouse_pos
        # var local_pos = layer.to_local(map_pos)
        # _tilemap.set_terrains([layer.local_to_map(local_pos)], "" if erase else terrain_name)
        # _tilemap.update()
