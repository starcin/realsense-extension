#pragma once

#include <godot_cpp/classes/node.hpp>
#include <librealsense2/rs.hpp>

using namespace godot;

class RealSense : public Node {
	GDCLASS(RealSense, Node);

private:
	bool is_initialized;
	rs2::pipeline pipeline;
	rs2::config configuration;
	PackedInt32Array depth_data;

protected:
	static void _bind_methods();

public:
	void initialize(int width, int height);
	Vector2i get_size();
	PackedInt32Array get_depth_data();
	RealSense();
	// ~RealSense();
};
