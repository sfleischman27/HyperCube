R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 outColor;
in vec4 pos;
in vec2 outTexCoord;

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 frag_data; // depth, tocut, N/A, 1.0
layout (location = 2) out vec4 frag_normal;
layout (location = 3) out vec4 frag_pos;

uniform mat4 Mv;
uniform sampler2D billTex;

void main(void) {

	frag_color = texture(billTex, outTexCoord);

	// TODO added depth stuff here but should use a depth buffer in FSQ
	//float d = -(Mv * pos).z;
	//float maxDepth = 35.0;
	//float ratio = d / maxDepth;
	//vec3 fadeColor = vec3(0.1, 0.1, 0.1);
	//frag_color.xyz = frag_color.xyz - fadeColor * ratio;

	if (frag_color.a < 0.5) {
		discard;
	}
	frag_color.a = 1.0;
	frag_data = vec4(gl_FragCoord.z * 50, 1.0, 0.0, 1.0);
	frag_normal = vec4(0.0, 0.0, 0.0, 1.0);
	frag_pos = pos / 1000;
	frag_pos.a = 1;
}

/////////// SHADER END //////////)"
