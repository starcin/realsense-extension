#include "realsense.h"

#include <godot_cpp/core/class_db.hpp>

void RealSense::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add", "value"), &RealSense::add);
	ClassDB::bind_method(D_METHOD("get_total"), &RealSense::get_total);
}

void RealSense::add(int p_value) {
	count += p_value;
}

int RealSense::get_total() const {
	return count;
}

RealSense::RealSense() {
	count = 0;
}
