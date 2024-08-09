#version 330 core

in vec3 p_pos;
in vec3 p_normal;
in vec2 p_uv;
in vec3 p_color;

uniform vec3 camera_pos;

out vec4 fragCol;

void main()
{
    vec3 l = normalize(vec3(0.0, 100.0, 0.0) - p_pos);
    vec3 v = normalize(camera_pos - p_pos);
    vec3 h = normalize(l+v);


    vec3 n = normalize(p_normal);
    // vec4 col = vec4(n*0.5+0.5,  1.0f);
    vec4 col = vec4(p_color, 1.0f);
    
    float f = dot(n, v);
    float diff = max(f, 0.0);
    float spec = pow(max(dot(h, n), 0.0), 64.0);

    fragCol = col * ( 0.3 + diff) + spec;
}