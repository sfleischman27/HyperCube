R"(////////// SHADER BEGIN /////////

in vec4 aPosition;
in vec4 aColor;

out vec4 outColor;
out vec4 pos;

uniform mat4 uPerspective;

void main(void) {
    gl_Position = uPerspective * aPosition;
    pos = gl_Position;
    outColor = aColor;
}

/////////// SHADER END //////////)"


