R"(////////// SHADER BEGIN /////////

// Positions
in vec4 aPosition;

// Colors
in  vec4 aColor;
out vec4 outColor;
out vec4 pos;

// Matrices
uniform mat4 uPerspective;

// Transform and pass through
void main(void) {
    gl_Position = uPerspective * aPosition;
    pos = gl_Position;
    outColor = aColor;
}

/////////// SHADER END //////////)"


