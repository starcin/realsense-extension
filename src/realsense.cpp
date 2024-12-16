#include "realsense.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#define M_PI 3.14159265358979323846

void RealSense::_bind_methods() {
	ClassDB::bind_method(D_METHOD("initialize", "width", "height"), &RealSense::initialize);
	ClassDB::bind_method(D_METHOD("configure_depth_cropping", "nearDistance", "farDistance", "downDistance", "upDistance", "leftDistance", "rightDistance", "is_local"), &RealSense::configure_depth_cropping);
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

void RealSense::initialize(int width, int height, bool will_capture_color, bool will_capture_depth) {
	UtilityFunctions::print("Starting RealSense");

	number_of_pixels = width * height;

	if (will_capture_depth) {
		configuration.enable_stream(RS2_STREAM_DEPTH, width, height);
		configuration.enable_stream(RS2_STREAM_ACCEL, RS2_FORMAT_MOTION_XYZ32F);
		depth_byte_array.resize(number_of_pixels * 2);
		is_depth_enabled = true;
	}
	if (will_capture_color) {
		configuration.enable_stream(RS2_STREAM_COLOR, width, height, RS2_FORMAT_RGB8, 30);
		color_byte_array.resize(number_of_pixels * 3);
		is_color_enabled = true;
	}
	// Start the camera thread
	std::thread camera_thread([this]() { start(); });
	camera_thread.detach();

	UtilityFunctions::print("RealSense started");
}

void RealSense::configure_depth_cropping(float nearDistance, float farDistance, float downDistance, float upDistance, float leftDistance, float rightDistance, bool is_local) {
	will_crop_depth_data = true;
	will_reorient_before_cropping = !is_local;
	depth_cropping_config.near_distance = nearDistance;
	depth_cropping_config.far_distance = farDistance;
	depth_cropping_config.down_distance = downDistance;
	depth_cropping_config.up_distance = upDistance;
	depth_cropping_config.left_distance = leftDistance;
	depth_cropping_config.right_distance = rightDistance;
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

				if (will_crop_depth_data) {
					rs2::points points = pointcloud.calculate(depth_frame);
					const rs2::vertex* vertices = points.get_vertices();

					if (will_reorient_before_cropping) {
						rs2::motion_frame motion_frame = frames.first(RS2_STREAM_ACCEL);
						rs2_vector motion_data = motion_frame.as<rs2::motion_frame>().get_motion_data();
						// Get number of vertices from points
						int numberOfPoints = points.size();
						// Calculate pitch and roll from accelerometer data
						float pitch = atan2(motion_data.y, motion_data.z);
						float roll = atan2(-motion_data.x, sqrt(motion_data.y*motion_data.y + motion_data.z*motion_data.z));
						// We want to rotate to level
						float target_pitch = -M_PI/2;
						float pitch_correction = pitch - target_pitch; // Negative pitch to level
						float roll_correction = roll; // Target roll is 0

							// Rotate each vertex
						for (int i = 0; i < numberOfPoints; i++) {
							rs2::vertex& vertex = const_cast<rs2::vertex&>(vertices[i]);
							float x = vertex.x;
							float y = vertex.y;
							float z = vertex.z;
							// First correct the roll (around Z axis)
							float x1 = x * cos(roll_correction) - y * sin(roll_correction);
							float y1 = x * sin(roll_correction) + y * cos(roll_correction);
							float z1 = z;
							// Then correct the pitch (around X axis)
							float y2 = y1 * cos(pitch_correction) - z1 * sin(pitch_correction);
							float z2 = y1 * sin(pitch_correction) + z1 * cos(pitch_correction);

							vertex.x = x1;
							vertex.y = y2;
							vertex.z = z2;
						}
					}
					std::vector<bool> cropMask(number_of_pixels, false);
					for (size_t i = 0; i < number_of_pixels; ++i)
					{
							const auto& vertex = vertices[i];
							cropMask[i] =
									vertex.z >= depth_cropping_config.near_distance &&
									vertex.z <= depth_cropping_config.far_distance &&
									vertex.y >= depth_cropping_config.down_distance &&
									vertex.y <= depth_cropping_config.up_distance &&
									vertex.x >= depth_cropping_config.left_distance &&
									vertex.x <= depth_cropping_config.right_distance;
					}
					for (int i = 0; i < size; i++) {
						depth_byte_array[i * 2] = cropMask[i] ? depth_data[i] & 255 : 0;
						depth_byte_array[i * 2 + 1] = cropMask[i] ? (depth_data[i] >> 8) & 255 : 0;
					}
				}
				else {
					// Convert depth data to byte array
					for (int i = 0; i < size; i++) {
						depth_byte_array[i * 2] = depth_data[i] & 255;
						depth_byte_array[i * 2 + 1] = (depth_data[i] >> 8) & 255;
					}
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

