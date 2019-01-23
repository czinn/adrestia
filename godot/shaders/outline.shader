shader_type canvas_item;

uniform float width : hint_range(0.0, 30.0);
uniform vec4 outline_color : hint_color;
uniform float detail = 8;
uniform float PI = 3.141592653589;

// Adapted from outline shader tutorial at
// https://www.youtube.com/watch?v=LAa8UE3ItM8
// jim: ... but I made it a bit better
void fragment() {
	float size = width * 1.0 / float(textureSize(TEXTURE, 0).x);

	vec4 sprite_color = texture(TEXTURE, UV);
	float alpha = sprite_color.a;
	for (float i = 0.0; i < detail; i += 1.0) {
		float angle = 2.0 * i / detail * PI;
		float dx = cos(angle) * size;
		float dy = sin(angle) * size;
		alpha = max(alpha, texture(TEXTURE, UV + vec2(dx, dy)).a);
	}
	
	vec4 color = mix(outline_color, sprite_color, sprite_color.a);
	COLOR = vec4(color.rgb, alpha);
}
