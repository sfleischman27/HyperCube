R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

out vec4 frag_color;

uniform float farPlaneDist;
uniform sampler2D depthTexture;
uniform sampler2D replaceTexture;

// Editable parameter for depth falloff. Higher = fades out sooner
const float severity = 50.0;

void main(void) {
    // Do not calculate if this area is cut
    if (texture(replaceTexture, outTexCoord).r == 0.0) {
        discard;
    }
	float d = texture(depthTexture, outTexCoord).r * farPlaneDist * severity;
	frag_color.xyz = vec3(1.0) * d;
	frag_color.a = 1.0;
}

/////////// SHADER END //////////)"
