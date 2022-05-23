#version 460 core
out vec4 FragColor;

uniform vec2 u_resolution;
uniform float u_time;
uniform vec4 u_bg;

in vec2 UV;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D Texture;

vec4 basicLight(){
    vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);

    vec3 lightVec = vec3(10.0) - FragPos;

    // intensity of light with respect to distance
    float dist = length(lightVec);
    float a = 0.05;
    float b = 0.02;
    float inten = 0.0 / (a * dist * dist + b * dist + 1.0);

    // ambient lighting
    float ambient = u_time;
//    float ambient = 0.2;

    // diffuse lighting
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightVec);
    float diffuse = max(dot(normal, lightDirection), 0.0);

    return (diffuse * inten + ambient) * lightColor;
}

float near = 0.1;
float far = 5.0;

float linearizeDepth(float depth){
    return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float logisticDepth(float depth, float steepness, float offset){
    float zVal = linearizeDepth(depth);
    return 1.0 / (1.0 + exp(-steepness * (zVal - offset)));
}

void main(){
    vec4 color = texture(Texture, UV) * basicLight();
    float depth = logisticDepth(gl_FragCoord.z, 5.15, 4.5);
//    float depth = 0.0f;
    FragColor = color * (1.0 - depth) + depth * u_bg;
}