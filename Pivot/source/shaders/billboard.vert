R"(////////// SHADER BEGIN /////////

in vec4 aPosition;
in vec2 aTexCoord;

out vec4 outColor;
out vec2 outTexCoord;

uniform mat4 uPerspective;
uniform int flipXvert;

void main(void) {
    gl_Position = uPerspective * aPosition;
    outTexCoord = aTexCoord;
    if (flipXvert != 1) {
        outTexCoord.x = 1.0 - outTexCoord.x;
    }
}

/////////// SHADER END //////////)"


