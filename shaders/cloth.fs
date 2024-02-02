#version 330 core

in vec3 p_pos;
in vec3 p_normal;
in vec2 p_uv;
in vec3 p_color;

uniform vec3 camera_pos;

out vec4 fragCol;

vec3 lightPos = vec3(-20.0, 20.0, 10.0);
void main()
{
    vec3 l = normalize(lightPos - p_pos);
    vec3 v = normalize(camera_pos - p_pos);
    vec3 h = normalize(l+v);

    vec4 col = vec4(p_color, 1.0);
    vec3 n = normalize(p_normal);
    
    float f = dot(n, v);
    float diff = max(f, 0.0);
    float spec = pow(max(dot(h, n), 0.0), 128.0);

    fragCol = col * ( 0.5 + diff + spec);
}