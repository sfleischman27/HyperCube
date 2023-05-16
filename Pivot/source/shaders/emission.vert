R"(////////// SHADER BEGIN /////////

in vec4 aPosition;
in vec2 aTexCoord;

out vec2 outTexCoord;

uniform mat4 uPerspective;

void main(void) {
    gl_Position = uPerspective * aPosition;
    outTexCoord = aTexCoord;
}

/////////// SHADER END //////////)"


