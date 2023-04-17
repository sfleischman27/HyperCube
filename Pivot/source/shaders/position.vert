R"(////////// SHADER BEGIN /////////

in vec4 aPosition;
in vec2 aTexCoord;

out vec4 pos;
out vec2 outTexCoord;

uniform mat4 uPerspective;

void main(void) {
    gl_Position = uPerspective * aPosition;
    pos = aPosition;
    outTexCoord = aTexCoord;
}

/////////// SHADER END //////////)"


