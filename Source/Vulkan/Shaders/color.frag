#version 440

// This code is used from the qt vulkan examples

layout(location = 0) in vec3 v_color;

layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor = vec4(v_color, 1.0);
}