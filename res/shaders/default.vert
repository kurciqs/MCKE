#version 460 core
layout (location = 0) in int xz;
layout (location = 1) in uint y;
layout (location = 2) in int texPos;
layout (location = 3) in int aData;

out vec2 UV;
out vec3 Normal;
out vec3 FragPos;
flat out int light;

uniform mat4 u_trans;
uniform mat4 u_cam;

uniform vec2 worldPosition;
float maxTextureAtlasPos = 16.;

#define FRONT_FACE 0
#define BACK_FACE 1
#define BOTTOM_FACE 2
#define TOP_FACE 3
#define LEFT_FACE 4
#define RIGHT_FACE 5

int extractX(int com){
    // 240 = 0b11110000
    return ((com & 240) >> 4);
}

int extractZ(int com){
    // 15 = 0b00001111
    return (com & 15);
}

float map(float X, float A, float B, float C, float D){
    return (X-A)/(B-A) * (D-C) + C;
}

vec3 get_normal(uint face){
    if (face == FRONT_FACE){
        return vec3(0.0, 0.0, -1.0);
    }
    if (face == BACK_FACE){
        return vec3(0.0, 0.0, 1.0);
    }
    if (face == BOTTOM_FACE){
        return vec3(0.0, -1.0, 0.0);
    }
    if (face == TOP_FACE){
        return vec3(0.0, 1.0, 0.0);
    }
    if (face == LEFT_FACE){
        return vec3(1.0, 0.0, 0.0);
    }
    else {
        return vec3(-1.0, 0.0, 0.0);
    }
}

void main(){
    int face = ((aData >> 5) & 7); // 7 = 0b00000111
    Normal = get_normal(face);

    int lightLvl = ((aData >> 1) & 15); // 15 = 0b00001111
    light = lightLvl;

    UV = vec2(map(extractX(texPos), 0., maxTextureAtlasPos, 0., 1.), map(extractZ(texPos), 0., maxTextureAtlasPos, 0., 1.));

    int x = extractX(xz);
    int z = extractZ(xz);
    vec3 position = vec3(x + worldPosition.x, y, z + worldPosition.y);
    gl_Position = u_cam * (u_trans * vec4(position, 1.0));
    FragPos = (u_trans * vec4(position, 1.0)).xyz;
}