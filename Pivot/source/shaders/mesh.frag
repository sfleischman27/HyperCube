R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;
in vec3 outNormal;

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 frag_replace; // if r == 0, then replace, else if r == 1.0 keep same
layout (location = 2) out vec4 frag_normal;
layout (location = 3) out vec4 frag_depth; // stored in r

uniform sampler2D uTexture;
uniform vec3 uDirection;
uniform float farPlaneDist;

// Editable parameter to determine cutting. Set to 0.0 for cuts, set to 999.0 for visualization
const float cullOutside = 0.0;

vec4 EncodeFloatRGBA(float v) {
  vec4 enc = vec4(1.0, 255.0, 65025.0, 16581375.0) * v;
  enc = fract(enc);
  enc -= enc.yzww * vec4(1.0/255.0, 1.0/255.0, 1.0/255.0, 0.0);
  return enc;
}

void main(void) {

	// encode color and replace based on culling
	if (dot(uDirection, outNormal) <= cullOutside) {
        frag_color = vec4(1.0, 0.0, 1.0, 1.0); // this should never be seen
		frag_replace = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
		frag_color = texture(uTexture, outTexCoord);
		frag_color.a = 1.0;
		frag_replace = vec4(1.0, 0.0, 0.0, 1.0);
	}
	// always want to encode depth and normal as-is
	frag_depth = EncodeFloatRGBA(gl_FragCoord.z);
	frag_normal = vec4((outNormal + vec3(1.0)) / 2.0, 1.0);
}

/////////// SHADER END //////////)"
