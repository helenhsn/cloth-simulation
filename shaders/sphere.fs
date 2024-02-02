#version 330 core

in vec3 p_pos;
in vec3 p_normal;
in vec2 p_uv;
in vec3 p_color;

out vec4 fragCol;

uniform vec3 camera_pos;

vec3 lightPos = vec3(-100.0, 50.0, 100.0);

void main()
{
    vec3 v = normalize(camera_pos - p_pos);

    vec4 col = vec4(p_color, 1.0);
    vec3 n = normalize(p_normal);
    
    float f = dot(n, v);
    float diff = max(f, 0.0);
    float spec = pow(max(f, 0.0), 64.0);

    fragCol = col * ( 0.8 + diff + spec);

}