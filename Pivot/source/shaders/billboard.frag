R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 outColor;
in vec4 pos;

out vec4 frag_color;

uniform mat4 Mv;

void main(void) {

	// TODO added depth stuff here but should use a depth buffer in FSQ
	float d = -(Mv * pos).z;
	float maxDepth = 35.0;
	float ratio = d / maxDepth;
	vec4 fadeColor = vec4(0.1, 0.1, 0.1, 1.0);
	frag_color = outColor - fadeColor * ratio;
	frag_color.a = 1.0;
}

/////////// SHADER END //////////)"
