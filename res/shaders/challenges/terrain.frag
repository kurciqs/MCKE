#version 460 core
out vec4 FragColor;
layout (location = 0) in vec2 fragCoord;
layout (location = 1) in vec3 fragCoord3;

uniform vec2 u_resolution;
uniform float u_time;
uniform vec2 u_mouse;
uniform vec3 background;
in vec4 col;
in vec3 Normal;

void main(){
    vec2 st = fragCoord.xy/u_resolution;
    st.x *= u_resolution.x/u_resolution.y;
//    vec3 color = vec3(st.x, st.y, 1.0);
    vec3 color = col.xyz;

    vec3 lightColor = vec3(1., 1., 1.);
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    vec3 lightPos = vec3(0., 5., 2.);
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - fragCoord3);
    float diffStrength = 0.9;
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * diffStrength;

    FragColor = vec4(color * (ambient + diffuse), 1.0);
}