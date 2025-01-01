shader_type spatial;
render_mode unshaded;

uniform sampler2D vertices_data : source_color,repeat_enable;

void vertex() {
	vec2 uv = vec2((float(VERTEX.x) + 0.8) / 1.6, (1.2 - float(VERTEX.y) + 0.6) / 1.2);
	vec4 vertex_data = texture(vertices_data, uv);

	// Skip points at origin
	bool is_valid = length(vertex_data.xyz) > 0.001;

	VERTEX.x = vertex_data.r;
	VERTEX.y = vertex_data.g;
	VERTEX.z = vertex_data.b;

	// Add some basic coloring based on depth
	COLOR.rgb = vec3(vertex_data.z * 0.1);
	COLOR.a = is_valid ? 1.0 : 0.0;  // Mark invalid triangles

	// Optional: make points closer to camera appear larger
	float point_size = 2.0;  // Base size
	POINT_SIZE = 2.0;  // Scale based on depth
}

void fragment() {
	// Discard fragment if any vertex in the triangle was invalid
	if (COLOR.a < 0.5) {
		discard;
	}

	ALBEDO = COLOR.rgb;
}
