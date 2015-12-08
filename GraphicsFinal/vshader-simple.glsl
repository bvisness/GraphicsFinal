#version 430

in vec4 vPosition;
in vec4 vAmbientDiffuseColor;

out vec4 fPosEye;
out vec4 fAmbientDiffuseColor;

uniform mat4 uModelView;
uniform mat4 uProjection;

void main()
{
	fPosEye = uModelView * vPosition;
	fAmbientDiffuseColor = vAmbientDiffuseColor;

	gl_Position = uProjection * fPosEye;
}