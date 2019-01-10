shader_type canvas_item;

uniform float radius : hint_range(0.0, 500.0) = 100.0;
uniform float x = 100.0;
uniform float y = 100.0;

void fragment() {
	float dx = (x - UV.x / SCREEN_PIXEL_SIZE.x);
	float dy = (y - UV.y / SCREEN_PIXEL_SIZE.y);
	float alpha = (dx * dx + dy * dy < radius * radius) ? 0.0 : 0.2;
	COLOR = vec4(0.0, 0.0, 0.0, alpha);
}