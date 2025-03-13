#include "register_types.h"
#include "terrain.h"
#include "tile_map.h"
#include "tile_map_3d.h"
#include "tile_map_layer.h"

using namespace godot;

void initialize_tile_map_module(ModuleInitializationLevel p_level)
{
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    return;

  GDREGISTER_RUNTIME_CLASS(BLTerrain);
  GDREGISTER_RUNTIME_CLASS(BLTileSet);
  GDREGISTER_RUNTIME_CLASS(BLTileMapLayer);
  GDREGISTER_RUNTIME_CLASS(BLTileMap);
  GDREGISTER_RUNTIME_CLASS(BLTileMap3D);
}

void uninitialize_tile_map_module(ModuleInitializationLevel p_level)
{
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    return;
}

extern "C"
{
  GDExtensionBool GDE_EXPORT
  KLTileMap_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                         GDExtensionClassLibraryPtr *p_library,
                         GDExtensionInitialization *r_initialization)
  {
    GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library,
                                            r_initialization);
    init_obj.register_initializer(initialize_tile_map_module);
    init_obj.register_terminator(uninitialize_tile_map_module);
    init_obj.set_minimum_library_initialization_level(
        MODULE_INITIALIZATION_LEVEL_SCENE);
    return init_obj.init();
  }
}
