R"(////////// SHADER BEGIN /////////

in vec4 aPosition;
in vec2 aTexCoord;
in vec3 aNormal;

out vec4 pos;
out vec2 outTexCoord;
out vec4 outNormal;

uniform vec3 uDirection;
uniform mat4 uPerspective;

void main(void) {
    gl_Position = uPerspective * aPosition;
    pos = aPosition;
    outTexCoord = aTexCoord;
    outNormal.xyz = aNormal + vec3(1.0, 1.0, 1.0);
    outNormal.x = outNormal.x / 2.0;
    outNormal.y = outNormal.y / 2.0;
    outNormal.z = outNormal.z / 2.0;
    outNormal.a = 1.0;
    if(dot(uDirection, aNormal) >= 0.0) {
        outNormal.a = 0.0;
    }
}

/////////// SHADER END //////////)"


