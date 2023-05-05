R"(////////// SHADER BEGIN /////////

in vec4 aPosition;
in vec2 aTexCoord;

out vec2 outTexCoord;

void main(void) {
    gl_Position = aPosition;
    outTexCoord = aTexCoord;
}

/////////// SHADER END //////////)"


