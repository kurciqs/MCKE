#version 460 core

out vec4 FragColor;
layout (location = 0) in vec2 fragCoord;

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

float circle(vec2 st_, float radius, float outer_radius, vec2 position) {
    float dist_to_center = distance(st_, position);
    return 1.-smoothstep(radius, outer_radius, dist_to_center);
}

float box(vec2 st_, vec2 size, vec2 pos){
    float dx = abs(st_.x - pos.x);
    float dy = abs(st_.y - pos.y);
    return (step(vec2(dx, dy), size).x * step(vec2(dx, dy), size).y);
}

void main(){
    vec2 st = fragCoord.xy/u_resolution;
    st.x *= u_resolution.x/u_resolution.y;

    vec3 color;
    float r = 0.2 + abs(sin(u_time)/10.);
	vec3 box1 = vec3(1.0, 1.0, 0.0)*box(st, vec2(0.2, 0.5), vec2(0.8));
    vec3 box2 = vec3(1.000,0.413,0.346)*box(st, vec2(0.010,0.50), vec2(0.580,0.800));
    vec3 box3 = vec3(1.000,0.452,0.648)*box(st, vec2(0.260,0.370), vec2(0.740,-0.080));
    vec3 box4 = vec3(0.727,1.000,0.892)*box(st, vec2(0.130,0.500), vec2(0.430,0.800));
    vec3 box5 = vec3(1.000,0.922,0.118)*box(st, vec2(0.310,0.660), vec2(0.160,-0.37));
    vec3 box6 = vec3(0.314,1.000,0.479)*box(st, vec2(0.180,0.160), vec2(0.110,0.860));
    vec3 box7 = vec3(0.340,0.396,1.000)*box(st, vec2(0.250,0.195), vec2(0.040,0.495));
    color = box1 + box2 + box3 + box4 + box5 + box6 + box7;
    FragColor = vec4( color, 1.0 );
}
