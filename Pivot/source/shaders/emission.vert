R"(////////// SHADER BEGIN /////////

in vec4 aPosition;
in vec2 aTexCoord;

out vec2 outTexCoord;
out vec4 pos;

uniform mat4 uPerspective;

void main(void) {
    gl_Position = uPerspective * aPosition;
    pos = gl_Position;
    outTexCoord = aTexCoord;
}

/////////// SHADER END //////////)"


