R"(////////// SHADER BEGIN /////////

in vec4 aPosition;
in vec4 aColor;
in vec2 aTexCoord;
in vec3 aNormal;

out vec4 outColor;
out vec4 pos;
out vec2 outTexCoord;
out vec3 outNormal;

uniform mat4 uPerspective;

void main(void) {
    gl_Position = uPerspective * aPosition;
    pos = aPosition;
    outTexCoord = aTexCoord;
    outNormal = (aNormal + 1) / 2;
    outColor = aColor;
    //outColor = aColor;
    //outColor = vec4(uDirection, 1);
    //outColor = vec4((aNormal + 1) / 2, 1);
}

/////////// SHADER END //////////)"


