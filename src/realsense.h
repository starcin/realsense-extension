#pragma once

#include <godot_cpp/classes/object.hpp>
#include <librealsense2/rs.hpp>

using namespace godot;

class RealSense : public Object {
	GDCLASS(RealSense, Object);

private:
	rs2::pipeline pipeline;
	rs2::config configuration;

protected:
	static void _bind_methods();

public:
	void initialize(int width, int height);
	Vector2i get_size();
	RealSense();
	// ~RealSense();
};
