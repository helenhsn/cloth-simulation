#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 n;
layout (location = 2) in vec2 uv;


uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

out vec3 p_pos;
out vec3 p_normal;
out vec2 p_uv;

void main()
{
    p_normal = n;
    p_uv = uv;
    p_pos = pos;
    gl_Position = projection*view*model*vec4(pos, 1.0);
}