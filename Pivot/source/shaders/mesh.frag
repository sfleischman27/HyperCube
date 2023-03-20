R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 pos;
in vec2 outTexCoord;
in vec3 outNormal;

out vec4 frag_color;

uniform mat4 Mv;
uniform sampler2D uTexture;
uniform vec3 uDirection;

void main(void) {
	vec3 transNormal = normalize(outNormal.xyz * 2.0 - vec3(1.0, 1.0, 1.0));
    float cullOutside = 0.0; // set to 0.0 for cuts, set to 999.0 for visualization
	if (dot(uDirection, transNormal) >= cullOutside) {
        frag_color = vec4(1.0, 0.0, 1.0, 1.0);
    } else {
		frag_color = texture(uTexture, outTexCoord) * .5 + vec4(.5, .5, .5, 1.0) * .5;

		// TODO added depth stuff here but should use a depth buffer in FSQ
		float d = -(Mv * pos).z;
		float maxDepth = 35.0;
		float ratio = d / maxDepth;
		vec4 fadeColor = vec4(0.1, 0.1, 0.1, 1.0);
		frag_color = frag_color - fadeColor * ratio;
		frag_color.a = 1.0;
	}
}

/////////// SHADER END //////////)"
