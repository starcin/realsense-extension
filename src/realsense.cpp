#include "realsense.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void RealSense::_bind_methods() {
	ClassDB::bind_method(D_METHOD("initialize", "width", "height"), &RealSense::initialize);
	ClassDB::bind_method(D_METHOD("get_size"), &RealSense::get_size);
	ClassDB::bind_method(D_METHOD("get_depth_data"), &RealSense::get_depth_data);
	ClassDB::bind_method(D_METHOD("get_color_data"), &RealSense::get_color_data);
	ClassDB::bind_method(D_METHOD("start_capturing"), &RealSense::start_capturing);
	ClassDB::bind_method(D_METHOD("stop_capturing"), &RealSense::stop_capturing);
	ADD_SIGNAL(MethodInfo("new_depth_data", PropertyInfo(Variant::PACKED_BYTE_ARRAY, "data")));
	ADD_SIGNAL(MethodInfo("new_color_data", PropertyInfo(Variant::PACKED_BYTE_ARRAY, "data")));
	ADD_SIGNAL(MethodInfo("new_frame_ready"));
	ADD_SIGNAL(MethodInfo("initialized"));
}

RealSense::RealSense() {
}

RealSense::~RealSense() {
// 	stop_capturing();
// 	pipeline.stop();
}

void RealSense::start() {
	UtilityFunctions::print("Starting camera");
	pipeline.start(configuration);
	is_initialized = true;
	UtilityFunctions::print("Started camera");
	call_deferred("emit_signal", "initialized");
}

void RealSense::initialize(int width, int height, bool will_capture_depth, bool will_capture_color) {
	UtilityFunctions::print("Starting RealSense");

	if (will_capture_depth) {
		configuration.enable_stream(RS2_STREAM_DEPTH, width, height);
		configuration.enable_stream(RS2_STREAM_ACCEL, RS2_FORMAT_MOTION_XYZ32F);
		depth_byte_array.resize(width * height * 2);
		is_depth_enabled = true;
	}
	if (will_capture_color) {
		configuration.enable_stream(RS2_STREAM_COLOR, width, height, RS2_FORMAT_RGB8, 30);
		color_byte_array.resize(width * height * 3);
		is_color_enabled = true;
	}
	// Start the camera thread
	std::thread camera_thread([this]() { start(); });
	camera_thread.detach();

	UtilityFunctions::print("RealSense started");
}

Vector2i RealSense::get_size() {
	rs2::frameset frames = pipeline.wait_for_frames();
	rs2::depth_frame depth_frame = frames.get_depth_frame();
	return Vector2i(depth_frame.get_width(), depth_frame.get_height());
}

PackedByteArray RealSense::get_depth_data() {
	return depth_byte_array;
}

PackedByteArray RealSense::get_color_data() {
	return color_byte_array;
}

void RealSense::fetch_data() {
	UtilityFunctions::print("Starting data loop");

	while (is_running) {
		try {
			// Capture a frame
			rs2::frameset frames = pipeline.wait_for_frames();

			if (is_depth_enabled) {
				rs2::depth_frame depth_frame = frames.get_depth_frame();
				// Extract depth data
				const uint16_t* depth_data = (uint16_t*)depth_frame.get_data();
				int size = depth_frame.get_width() * depth_frame.get_height();
				// Convert depth data to byte array
				for (int i = 0; i < size; i++) {
					depth_byte_array[i * 2] = depth_data[i] & 255;
					depth_byte_array[i * 2 + 1] = (depth_data[i] >> 8) & 255;
				}
				// Emit the signals
				call_deferred("emit_signal", "new_depth_data", depth_byte_array);
			}

			if (is_color_enabled) {
				rs2::video_frame color_frame = frames.get_color_frame();
				const uint8_t* color_data = (uint8_t*)color_frame.get_data();
				int size = color_frame.get_width() * color_frame.get_height() * 3;
				for (int i = 0; i < size; i++) {
					color_byte_array[i] = color_data[i];
				}
				call_deferred("emit_signal", "new_color_data", color_byte_array);
			}

			call_deferred("emit_signal", "new_frame_ready");
		} catch (const std::exception& e) {
			UtilityFunctions::print("Error in depth loop: " + String(e.what()));
		}

		// Sleep to avoid high CPU usage (adjust as necessary)
		std::this_thread::sleep_for(std::chrono::milliseconds(30)); // <30 FPS
	}

	UtilityFunctions::print("Depth data loop stopped");
}

void RealSense::start_capturing() {
	if (is_running) {
		UtilityFunctions::print("Depth loop is already running");
		return;
	}
	is_running = true;
	loop_thread = std::thread([this]() { fetch_data(); });
}

void RealSense::stop_capturing() {
	if (!is_running) {
		UtilityFunctions::print("Depth loop is not running");
		return;
	}
	is_running = false;
	if (loop_thread.joinable()) {
		loop_thread.join(); // Wait for the thread to finish
	}
}

