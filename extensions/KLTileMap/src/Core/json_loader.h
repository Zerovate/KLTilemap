#pragma once
#include <godot_cpp/classes/json.hpp>

using namespace godot;

class JsonLoader {
public:
	static Variant load(const String &path);
};
