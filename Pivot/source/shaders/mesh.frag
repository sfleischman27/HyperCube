R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 outColor;
in vec4 pos;
in vec2 outTexCoord;
in vec3 outNormal;

out vec4 frag_color;

uniform mat4 Mv;
uniform sampler2D uTexture;
uniform vec3 uDirection;

void main(void) {
	//float d = -(Mv * pos).z;
	//float div = 100;
	//frag_color = vec4(0, 0, 0, 1);// - vec4(d / div, d / div, d / div, 0);
	//frag_color = vec4(outTexCoord, 0, 1);
	vec3 transNormal = normalize(outNormal.xyz * 2 - 1);
    int cullOutside = 0; // set to 0 for cuts, set to 999 for visualization
	if (dot(uDirection, transNormal) >= cullOutside) {
        frag_color = vec4(1, 0, 1, 1);
    } else {
		frag_color = texture(uTexture, outTexCoord);

		// TODO added depth stuff here but should use a depth buffer
		float d = -(Mv * pos).z;
		float maxDepth = 35;
		float ratio = d / maxDepth;
		vec4 fadeColor = vec4(0.1, 0.1, 0.1, 1);
		frag_color = frag_color - fadeColor * ratio;
		frag_color.a = 1;
	}
	//frag_color = vec4(normalize(outColor.xyz * 2 - 1), 1);
}

/////////// SHADER END //////////)"
