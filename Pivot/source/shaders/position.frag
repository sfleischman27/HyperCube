R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 pos;
in vec2 outTexCoord;

layout (location = 0) out vec4 frag_pos_x;
layout (location = 1) out vec4 frag_pos_y;
layout (location = 2) out vec4 frag_pos_z;

uniform int removeA;
uniform sampler2D billTex;

vec4 packFloat(const float value) {
	float transF = (value + 2500.0) / 50000.0;
	const vec4 bit_shift = vec4(256.0 * 256.0 * 256.0, 256.0 * 256.0, 256.0, 1.0);
    const vec4 bit_mask  = vec4(0.0, 1.0/256.0, 1.0/256.0, 1.0/256.0);
    vec4 res = fract(transF * bit_shift);
    res -= res.xxyz * bit_mask;
    return res;
}

void main(void) {

	vec4 frag_color = texture(billTex, outTexCoord);

	if (removeA == 1) {
		if (frag_color.a < 0.5) {
			discard;
		}
	}

	frag_pos_x = packFloat(pos.x);
	frag_pos_y = packFloat(pos.y);
	frag_pos_z = packFloat(pos.z);
}

/////////// SHADER END //////////)"
