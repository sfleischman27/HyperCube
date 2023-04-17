R"(////////// SHADER BEGIN /////////

in vec4 aPosition;
in vec2 aTexCoord;

out vec4 pos;
out vec2 outTexCoord;

uniform mat4 uPerspective;
uniform int flipX;

void main(void) {
    gl_Position = uPerspective * aPosition;
    pos = aPosition;
    outTexCoord = aTexCoord;
    if (flipX == 1) {
        outTexCoord.x = 1.0 - outTexCoord.x;
    }
}

/////////// SHADER END //////////)"


