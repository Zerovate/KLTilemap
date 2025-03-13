#!/bin/bash

# 使用 SCons 构建
scons target=editor platform=macos arch=x86_64
# scons target=template_debug platform=macos arch=x86_64

# 如果需要，你可以启用其他 SCons 目标
# scons target=template_release platform=macos arch=x86_64

# 复制生成的文件到目标目录
# cp ./bin/* ../../alive/Extensions/bin/
# cp ./KLTileMap.gdextension ../../alive/Extensions/
