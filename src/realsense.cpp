#include "realsense.h"
#include <godot_cpp/core/class_db.hpp>

void RealSense::_bind_methods() {
	ClassDB::bind_method(D_METHOD("initialize", "width", "height"), &RealSense::initialize);
	ClassDB::bind_method(D_METHOD("get_size"), &RealSense::get_size);
	ClassDB::bind_method(D_METHOD("get_depth_data"), &RealSense::get_depth_data);
	ADD_SIGNAL(MethodInfo("initialized"));
}

RealSense::RealSense() {
}

// RealSense::~RealSense() {
// 	pipeline.stop();
// }

void RealSense::initialize(int width, int height) {
	configuration.enable_stream(RS2_STREAM_DEPTH, width, height);
	configuration.enable_stream(RS2_STREAM_ACCEL, RS2_FORMAT_MOTION_XYZ32F);
	depth_data.resize(width * height);
	pipeline.start(configuration);
	// pipeline.wait_for_frames(); // Crashes the app
	emit_signal("initialized");
}

Vector2i RealSense::get_size() {
	rs2::frameset frames = pipeline.wait_for_frames();
	rs2::depth_frame depth_frame = frames.get_depth_frame();
	return Vector2i(depth_frame.get_width(), depth_frame.get_height());
}

PackedInt32Array RealSense::get_depth_data() {
	rs2::frameset frames = pipeline.wait_for_frames();
	rs2::depth_frame depth_frame = frames.get_depth_frame();

	const int16_t* depth_data = (int16_t*)depth_frame.get_data();
	int size = depth_frame.get_width() * depth_frame.get_height();

	PackedInt32Array result;
	result.resize(size);
	for (int i = 0; i < size; i++) {
		result[i] = depth_data[i];
	}
	return result;
}
