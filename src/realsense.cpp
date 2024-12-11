#include "realsense.h"
#include <godot_cpp/core/class_db.hpp>

void RealSense::_bind_methods() {
	ClassDB::bind_method(D_METHOD("initialize", "width", "height"), &RealSense::initialize);
	ClassDB::bind_method(D_METHOD("get_size"), &RealSense::get_size);
}

RealSense::RealSense() {
}

// RealSense::~RealSense() {
// 	pipeline.stop();
// }

void RealSense::initialize(int width, int height) {
	configuration.enable_stream(RS2_STREAM_DEPTH, width, height);
	configuration.enable_stream(RS2_STREAM_ACCEL, RS2_FORMAT_MOTION_XYZ32F);
	pipeline.start(configuration);
}

Vector2i RealSense::get_size() {
	rs2::frameset frames = pipeline.wait_for_frames();
	rs2::depth_frame depth_frame = frames.get_depth_frame();
	return Vector2i(depth_frame.get_width(), depth_frame.get_height());
	// return Vector2i(10, 20);
}
