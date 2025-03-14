extends Camera2D

@export var move_speed = 600
@export var zoom_speed = 1.1

func _process(delta: float) -> void:
    var direction = Vector2(
        Input.get_axis("move_left", "move_right"),
        Input.get_axis("move_up", "move_down"))
    if not direction.is_zero_approx():
        direction = direction.normalized()
        global_position += direction * delta * move_speed / zoom


func _unhandled_input(event: InputEvent) -> void:
    if event.is_action_pressed("zoom_in"):
        zoom *= zoom_speed
    if event.is_action_pressed("zoom_out"):
        zoom /= zoom_speed
