#pragma once

#include <godot_cpp/classes/object.hpp>

using namespace godot;

class RealSense : public Object {
	GDCLASS(RealSense, Object);

private:
	int count;

protected:
	static void _bind_methods();

public:
	void add(int p_value);
	int get_total() const;
	RealSense();
};
