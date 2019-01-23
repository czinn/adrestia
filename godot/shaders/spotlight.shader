shader_type canvas_item;

uniform float radius : hint_range(0.0, 500.0) = 100.0;
uniform vec2 position = vec2(100.0, 100.0);

varying vec2 world_pos;

void vertex() {
	world_pos = VERTEX;
}

void fragment() {
	float alpha = (distance(position, world_pos) < radius) ? 0.0 : 0.2;
	COLOR = vec4(0.0, 0.0, 0.0, alpha);
}