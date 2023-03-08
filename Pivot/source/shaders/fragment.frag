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

uniform mat4 Mv;

/**
 * Performs the main fragment shading.
 */
void main(void) {
	float d = -(Mv * pos).z;
	float div = 100;
	frag_color = vec4(0, 0, 0, 1);// - vec4(d / div, d / div, d / div, 0);
}

/////////// SHADER END //////////)"
