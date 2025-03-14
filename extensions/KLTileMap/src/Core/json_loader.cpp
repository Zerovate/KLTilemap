#include "json_loader.h"
#include <godot_cpp/classes/file_access.hpp>

// void JsonLoader::_bind_methods() {
//   ClassDB::bind_static_method("JsonLoader", D_METHOD("load", "path"),
//                               &JsonLoader::load);
// }

Variant JsonLoader::load(const String &path) {
	auto file = FileAccess::open(path, FileAccess::READ);
	if (!file.is_valid()) {
		ERR_FAIL_COND_V_MSG(ERR_FILE_CANT_OPEN, Variant(), "Can't open file");
	}
	auto file_content = file->get_as_text();
	file->close();
	return JSON::parse_string(file_content);
}
