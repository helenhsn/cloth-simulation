#version 330 core

in vec3 p_pos;
in vec3 p_normal;
in vec2 p_uv;

out vec4 fragColor;

void main()
{
    float val = step(0.0, fract(p_pos.x*0.05)-0.5);
    float val2 = step(0.0, fract(p_pos.z*0.05)-0.5);
    fragColor = vec4(vec3(max(val, val2) - min(val, val2)), 1.0);
}