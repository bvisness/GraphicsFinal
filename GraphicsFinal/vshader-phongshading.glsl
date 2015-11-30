#version 150

in vec4 vPosition;
in vec4 vAmbientDiffuseColor;
in vec3 vNormal;
in vec3 vTangent;
in float vBitangentSign;
in vec2 vTexCoord;
in float vDiffuseAmount;
in float vSpecularAmount;
in float vSpecularExponent;

vec4 vSpecularColor = vec4(1, 1, 1, 1);

out vec4 fPosEye;
out vec3 fNormal;
out vec3 fTangent;
flat out float fBitangentSign;
out vec2 fTexCoord;
out vec4 fAmbientDiffuseColor;
out vec4 fSpecularColor;
out float fDiffuseAmount;
out float fSpecularAmount;
out float fSpecularExponent;

uniform mat4 uModelView;
uniform mat4 uProjection;

void main()
{
	fPosEye = uModelView * vPosition;
	fNormal = (uModelView * vec4(vNormal, 0)).xyz;
	fTangent = (uModelView * vec4(vTangent, 0)).xyz;
	fBitangentSign = vBitangentSign;
	fTexCoord = vTexCoord;
	fAmbientDiffuseColor = vAmbientDiffuseColor;
	fSpecularColor = vSpecularColor;
    fDiffuseAmount = vDiffuseAmount;
    fSpecularAmount = vSpecularAmount;
    fSpecularExponent = vSpecularExponent;

	gl_Position = uProjection * fPosEye;
}
