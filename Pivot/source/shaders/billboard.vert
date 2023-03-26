R"(////////// SHADER BEGIN /////////

in vec4 aPosition;
in vec4 aColor;
in vec2 aTexCoord;
in int aTexBindPoint;

out vec4 outColor;
out vec4 pos;
out vec2 outTexCoord;
flat out int texBindPoint;

uniform mat4 uPerspective;

void main(void) {
    gl_Position = uPerspective * aPosition;
    pos = aPosition;
    outColor = aColor;
    texBindPoint = aTexBindPoint;
    outTexCoord = aTexCoord;
}

/////////// SHADER END //////////)"


