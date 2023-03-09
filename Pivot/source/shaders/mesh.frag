R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 outColor;
in vec4 pos;

out vec4 frag_color;

uniform mat4 Mv;

void main(void) {
	//float d = -(Mv * pos).z;
	//float div = 100;
	//frag_color = vec4(0, 0, 0, 1);// - vec4(d / div, d / div, d / div, 0);
	frag_color = vec4(0, 0, 0, 1);
}

/////////// SHADER END //////////)"
