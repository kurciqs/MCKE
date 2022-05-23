#version 460 core

out vec4 FragColor;
layout (location = 0) in vec2 fragCoord;

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;
uniform vec3 background;

float circle(vec2 st_, vec2 pos, float r){
	float dist = distance(st_, pos);
    return 1.-step(r, dist);
}


vec2 rotate2D(float angle, vec2 st_){
    mat2 rmat = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    return rmat * st_;
}

void main() {
    vec2 st = fragCoord.xy/u_resolution.xy;
    st.x *= u_resolution.x/u_resolution.y;
	st -= 0.5;
    st = rotate2D(u_time * 6., st);
    st += 0.5;
    vec3 color = vec3(0.);
    vec3 rb = vec3(st.x, st.y, 0.5);
    color = circle(st, vec2(0.5), 0.352) * rb + (circle(st, vec2(0.5), 0.026))*vec3(0.460,0.460,0.460);
    if (color == vec3(0.0)){
        color = background;
    }

    FragColor = vec4(color,1.0);
}