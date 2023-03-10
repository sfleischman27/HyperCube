R"(////////// SHADER BEGIN /////////

in vec4 aPosition;
in vec2 aTexCoord;

out vec4 outColor;
out vec4 pos;
out vec2 outTexCoord;

void main(void) {
    gl_Position = aPosition;
    pos = aPosition;
    outTexCoord = aTexCoord;
}

/////////// SHADER END //////////)"


