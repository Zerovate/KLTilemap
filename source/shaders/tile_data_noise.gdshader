// NOTE: Shader automatically converted from Godot Engine 4.3.1.rc's CanvasItemMaterial.

shader_type canvas_item;
render_mode blend_mix;

varying vec2 uv1_triplanar_pos;

uniform float uv1_scale = 1.0;
uniform vec2 uv1_offset = vec2(0.0, 0.0);

uniform sampler2D texture_noise:source_color, filter_nearest, repeat_enable;

uniform vec2 vertex_offset = vec2(0.0, 0.0);

void vertex(){
    uv1_triplanar_pos = ((MODEL_MATRIX * vec4(VERTEX + vertex_offset, 0.0, 1.0)).xy) / vec2(128) * vec2(uv1_scale) + uv1_offset;
}

void fragment(){
    vec4 albedo_tex = texture(TEXTURE, UV);
    vec4 noise_tex = texture(texture_noise, uv1_triplanar_pos);
    COLOR = albedo_tex * noise_tex;
}