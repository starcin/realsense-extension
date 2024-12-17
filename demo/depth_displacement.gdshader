shader_type spatial;
//render_mode unshaded;

uniform sampler2D depth_texture : source_color,repeat_enable;

void vertex() {
	// Called for every vertex the material is visible on.
	vec4 depth_value = texture(depth_texture, vec2((float(VERTEX.x) + 0.8f) / 1.6f, (1.2f - float(VERTEX.y) + 0.6f) / 1.2f));
	float distance = (depth_value.g * 256.0f + depth_value.r) / 10.0f;
	float alpha = 1.f;
	if (VERTEX.x < 0.05f || VERTEX.x > 1.55f || VERTEX.y < 0.05f || VERTEX.y > 1.15f) alpha = 0.f;
	if (distance < 0.15f) alpha = 0.f;


	//float distance = depth_value.r / 100.f;
	//if (distance == 0.f) distance = 10000.0f;
	VERTEX.z -= distance;
	COLOR.xyz = vec3(distance);
	COLOR.a = alpha;


}

void fragment() {
	// Called for every pixel the material is visible on.
	//COLOR.xyz = texture(depth_texture, vec2(VERTEX.x, VERTEX.y)).xyz;
	//ALBEDO = vec3(texture(depth_texture, vec2(VERTEX.x, VERTEX.y)).xyz);
	//ALBEDO = vec3(texture(depth_texture, vec2(0.5f, 0.1f)).xyz);
	//ALBEDO = texture(depth_texture, UV).rgb;
	ALBEDO = COLOR.xyz;
	ALPHA = COLOR.a;
	//ALBEDO = vec3(texelFetch(depth_texture, ivec2(28, 28), 0).xyz);
	//ALBEDO = vec3(1.0f, 1.0f, 1.0f);
}

//void light() {
	// Called for every pixel for every light affecting the material.
	// Uncomment to replace the default light processing function with this one.
//}
