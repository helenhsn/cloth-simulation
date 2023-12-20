#version 330 core

in vec3 p_normal;
in vec2 p_uv;

out vec4 fragCol;

void main()
{
    fragCol = vec4(0.8, 0.8, 0.8, 1.0);
}