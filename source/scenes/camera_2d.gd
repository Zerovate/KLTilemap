extends Camera2D

@export var move_speed = 20
@export var zoom_speed = 1.1

func _process(delta: float) -> void:
    var direction = Vector2(
        Input.get_axis("move_left", "move_right"),
        Input.get_axis("move_up", "move_down"))
    if not direction.is_zero_approx():
        direction = direction.normalized()
        position += direction * move_speed / zoom
    
    var zoom_offset :float= Input.get_axis("zoom_in", "zoom_out")
    if not is_zero_approx(zoom_offset):
        if zoom_offset < 0:
            zoom *= zoom_speed
        else:
            zoom /= zoom_speed
