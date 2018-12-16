shader_type canvas_item;
render_mode blend_mix;

void fragment() {
	COLOR = texture(TEXTURE, UV);
	COLOR.rgb = vec3(dot(COLOR.rgb, vec3(0.299, 0.587, 0.114)));
}