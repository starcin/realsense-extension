#pragma once

#include <godot_cpp/classes/node.hpp>
#include <librealsense2/rs.hpp>
#include <thread>
using namespace godot;

class RealSense : public Node {
	GDCLASS(RealSense, Node);

private:
	bool is_initialized = false;
	bool is_running = false; // Flag to control the loop
	bool is_depth_enabled = false;
	bool is_color_enabled = false;
	rs2::pipeline pipeline;
	rs2::config configuration;
	PackedByteArray depth_byte_array;
	PackedByteArray color_byte_array;
	std::thread loop_thread; // Thread for the infinite loop
	void start();
	void fetch_data();

protected:
	static void _bind_methods();

public:
	void initialize(int width, int height, bool enable_depth = true, bool enable_color = true);
	Vector2i get_size();
	PackedByteArray get_depth_data();
	PackedByteArray get_color_data();
	void start_capturing();
	void stop_capturing();
	RealSense();
	~RealSense();
};
