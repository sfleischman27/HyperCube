R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

layout (location = 0) out vec4 frag_color;
layout (location = 2) out vec4 frag_normal;
layout (location = 3) out vec4 frag_depth; // stored in r

uniform sampler2D billTex;
uniform sampler2D normTex;
uniform int useNormTex;
uniform int flipXfrag;
uniform vec3 uDirection;
uniform int farPlaneDist;

void main(void) {

	frag_color = texture(billTex, outTexCoord);
	if (frag_color.a < 0.5) {
		discard;
	}
	frag_color.a = 1.0;
	frag_depth = vec4(gl_FragCoord.z / farPlaneDist, 0.0, 0.0, 1.0);
	// Set normal, if it exists
	frag_normal = vec4(0.0, 0.0, 0.0, 1.0);
	if (useNormTex == 1) {
		frag_normal.xyz = texture(normTex, outTexCoord).xzy; // normal now is x right, y up, z forward
		// Need to transform normal accordingly
		if (flipXfrag == 1) {
			frag_normal.x = 1.0 - frag_normal.x;
		}
		mat2 R = mat2(uDirection.y, uDirection.x, -uDirection.x, uDirection.y);
		frag_normal.xy = frag_normal.xy * R;
	}
}

/////////// SHADER END //////////)"
