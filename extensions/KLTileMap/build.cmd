@echo off

@REM scons target=editor platform=windows
@REM scons target=template_debug platform=windows
@REM scons target=template_release platform=windows
scons target=editor platform=windows dev_build=yes
@REM scons target=template_debug platform=windows dev_build=yes
@REM scons target=template_release platform=windows dev_build=yes

@REM xcopy .\bin ..\..\alive\Extensions\bin /Y
@REM xcopy .\KLTileMap.gdextension ..\..\alive\Extensions /Y
