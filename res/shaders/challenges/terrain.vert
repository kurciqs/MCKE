#version 460 core

layout (location = 0) in vec4 aColor;
layout (location = 1) in vec3 aPos;

uniform mat4 u_trans;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform float u_dist;

layout (location = 0) out vec2 fragCoord;
layout (location = 1) out vec3 fragCoord3;
out vec4 col;
out vec3 Normal;
uniform float u_time;

float rand (vec2 st) {
    return fract(sin(dot(st.xy,
    vec2(12.9898,78.233)))*
    43758.5453123);
}

float rand (float x) {
    return fract(sin(x)*43758.5453123);
}

float noise(float x){
    float i = floor(x);  // integer
    float f = fract(x);  // fraction
    return mix(rand(i), rand(i + 1.0), smoothstep(0.,1.,f));
}

float noise(vec2 st){
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = rand(i);
    float b = rand(i + vec2(1.0, 0.0));
    float c = rand(i + vec2(0.0, 1.0));
    float d = rand(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
    (c - a)* u.y * (1.0 - u.x) +
    (d - b) * u.x * u.y;
}


#define OCTAVES 8
float fbm (in vec2 st) {
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;

    for (int i = 0; i < OCTAVES; i++) {
        value += amplitude * noise(st);
        st *= 2.;
        amplitude *= .5;
    }
    return value;
}

float fbm (float x) {
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;
    for (int i = 0; i < OCTAVES; i++) {
        value += amplitude * noise(x);
        x *= 2.;
        amplitude *= .5;
    }
    return value;
}

float height(vec2 st){
    return fbm(st)/3.;
}

void main(){
    col = aColor;
    fragCoord = aPos.xy + vec2(0.5);
    fragCoord3 = aPos.xyz + vec3(0.5);

    float intensity = 1.9;
    float y = height(aPos.xz*intensity);
    vec3 POS = vec3(aPos.x, y, aPos.z);
    vec4 pos = u_proj * u_view * (u_trans * vec4(POS, 1.0));

    // X-Unit vector scaled by the distance of two vertices and transformed
    vec3 I = (u_proj * u_view * (u_trans * vec4(vec3(1., 0., 0.), 1.0)) * u_dist).xyz;
    // Z-Unit vector scaled by the distance of two vertices and transformed
    vec3 K =(u_proj * u_view * (u_trans * vec4(vec3(0., 0., 1.), 1.0)) * u_dist).xyz;

    vec3 right = aPos + I;
    vec3 left = aPos - I;
    vec3 front = aPos + K;
    vec3 back = aPos - K;

    right.y = height(right.xz*intensity);
    left.y = height(left.xz*intensity);
    front.y = height(front.xz*intensity);
    back.y = height(back.xz*intensity);

    Normal = normalize(cross((right - pos.xyz), (front - pos.xyz)));
//    Normal = normalize(cross((left - pos.xyz), (back - pos.xyz)));

    gl_Position = pos;
}