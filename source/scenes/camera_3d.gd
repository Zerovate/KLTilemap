extends Camera3D

@export var move_speed = 10

func _process(delta: float) -> void:
    var dir_x = Input.get_axis("move_left", "move_right")
    var dir_z = Input.get_axis("move_up", "move_down")
    if not is_zero_approx(dir_x):
        global_position.x += dir_x * delta * move_speed
    if not is_zero_approx(dir_z):
        global_position.z += dir_z * delta * move_speed