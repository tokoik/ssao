#version 120
//
// pass1.vert
//
#define AMBIENT_ONLY 0

varying vec4 diffspec;                                                // �g�U���ˌ� D + ���ʔ��ˌ� S
varying vec4 ambient;                                                 // �����̔��ˌ� A
varying vec3 position;                                                // ���̕\�ʏ�̈ʒu P
varying vec3 normal;                                                  // ���̓_�̖@���x�N�g�� N

void main(void)
{
  position = vec3(gl_ModelViewMatrix * gl_Vertex);
  normal = normalize(gl_NormalMatrix * gl_Normal);
  
  vec3 light = normalize(gl_LightSource[0].position.xyz - position);  // ���̓_��������Ɍ����������x�N�g�� L
  vec3 view = -normalize(position);                                   // ���̓_���王�_�Ɍ����������x�N�g�� V
  vec3 halfway = normalize(light + view);                             // ���ԃx�N�g�� H
  
  float nl = dot(normal, light);
  float nh = pow(dot(normal, halfway), gl_FrontMaterial.shininess);

  // �g�U���ˌ��{���ʔ��ˌ��� diffspec �Ɋi�[����
  diffspec = gl_FrontMaterial.diffuse * vec4(vec3(nl), 1.0) * gl_LightSource[0].diffuse + gl_FrontMaterial.specular * vec4(vec3(nh), 1.0) * gl_LightSource[0].specular;

  // ������ ambient �Ɋi�[����
  ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
  
  gl_Position = ftransform();                                         // ���_�ʒu�̍��W�ϊ�
}
