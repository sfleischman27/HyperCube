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
uniform int farPlaneDist;

// Editable parameter to determine cutting. Set to 0.0 for cuts, set to 999.0 for visualization
const float cullOutside = 0.0;

// Packing and unpacking stored here for now
vec4 packFloat(const float value) {
	const vec4 bit_shift = vec4(256.0 * 256.0 * 256.0, 256.0 * 256.0, 256.0, 1.0);
    const vec4 bit_mask  = vec4(0.0, 1.0/256.0, 1.0/256.0, 1.0/256.0);
    vec4 res = fract(value * bit_shift);
    res -= res.xxyz * bit_mask;
    return res;
}

float unpackFloat(const vec4 value) {
    const vec4 bit_shift = vec4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1.0);
    float transF = dot(value, bit_shift);
    return transF * 50000.0 - 2500.0;
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
	frag_depth = vec4(gl_FragCoord.z / farPlaneDist, 0.0, 0.0, 1.0);
	frag_normal = vec4((outNormal + vec3(1.0)) / 2.0, 1.0);
}

/////////// SHADER END //////////)"
