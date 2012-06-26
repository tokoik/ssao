#version 120
//
// pass1.frag
//
varying vec4 diffspec;                                                // �g�U���ˌ� D + ���ʔ��ˌ� S
varying vec4 ambient;                                                 // �����̔��ˌ� A
varying vec3 position;                                                // ���̕\�ʏ�̈ʒu P
varying vec3 normal;                                                  // ���̓_�̖@���x�N�g�� N

void main(void)
{
  gl_FragData[0] = diffspec;
  gl_FragData[1] = ambient;
  gl_FragData[2] = vec4(position, 1.0);
  gl_FragData[3] = vec4(normal, 0.0);
}
