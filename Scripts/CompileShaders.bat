@echo off

setlocal

SET PATH=%PATH%;"D:\vcpkg\installed\x64-windows\tools\"

cd ..\Source\Vulkan\Shaders\

glslangvalidator -V color.vert -o color_vert.spv
glslangvalidator -V color.frag -o color_frag.spv

pause