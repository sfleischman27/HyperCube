R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
// This one line is all the difference
precision mediump float;
#endif

// Color result from vertex shader
in vec4 outColor;
in vec4 pos;

// The output color
out vec4 frag_color;

/**
 * Performs the main fragment shading.
 */
void main(void) {
	frag_color = vec4(0, 0, 0, 1);
}

/////////// SHADER END //////////)"
