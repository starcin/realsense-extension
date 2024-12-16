#pragma once

#include <godot_cpp/classes/node.hpp>
#include <librealsense2/rs.hpp>
#include <thread>
using namespace godot;

class RealSense : public Node {
	GDCLASS(RealSense, Node);

private:
	struct DepthCroppingConfig {
		float near_distance = 0.0f;
		float far_distance = 0.0f;
		float down_distance = 0.0f;
		float up_distance = 0.0f;
		float left_distance = 0.0f;
		float right_distance = 0.0f;
	} depth_cropping_config;

	bool is_initialized = false;
	bool is_running = false; // Flag to control the loop
	bool is_depth_enabled = false;
	bool is_color_enabled = false;
	bool will_crop_depth_data = false;
	bool will_reorient_before_cropping = false;

	rs2::pipeline pipeline;
	rs2::pointcloud pointcloud;
	rs2::config configuration;
	PackedByteArray depth_byte_array;
	PackedByteArray color_byte_array;
	std::thread loop_thread; // Thread for the infinite loop
	uint32_t number_of_pixels = 0;
	void start();
	void fetch_data();

protected:
	static void _bind_methods();

public:
	void initialize(int width, int height, bool enable_color = true, bool enable_depth = true);
	void configure_depth_cropping( float nearDistance, float farDistance, float downDistance, float upDistance, float leftDistance, float rightDistance, bool is_local = false);
	Vector2i get_size();
	PackedByteArray get_depth_data();
	PackedByteArray get_color_data();
	void start_capturing();
	void stop_capturing();
	RealSense();
	~RealSense();
};
