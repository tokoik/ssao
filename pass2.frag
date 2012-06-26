#version 120
//
// pass2.frag
//
#define MAXSAMPLES 256                  // �T���v���_�̍ő吔
#define JITTERING 0                     // jittering �̗L��

const int SAMPLES = 256;                // �T���v���_��
const float SAMPLING_RATIO = 2.0;       // �T���v���_�̐��̔�r�W��

uniform sampler2D unit[7];              // �e�N�X�`�����j�b�g
uniform vec4 point[MAXSAMPLES];         // �T���v���_�̈ʒu
uniform vec4 reflection;                // �f�荞�݂̔��˗�
uniform float refraction;               // ���ܗ���
uniform float bgdistance;               // �w�i�Ƃ̋���
varying vec2 texcoord;                  // �e�N�X�`�����W

void main(void)
{
  // unit[0] ����g�U���ˌ����x diffuse + specular ���擾
  vec4 diffspec = texture2D(unit[0], texcoord);
  
  // diffspec �̃A���t�@�l�� 0 �Ȃ�w�i (unit[6]) �̂�
  if (diffspec.a == 0.0) {
    gl_FragColor = texture2D(unit[6], texcoord);
    return;
  }
  
  // unit[2] ���珈���Ώۂ̉�f�̎��_���W�n��̈ʒu position ���擾
  vec4 p = texture2D(unit[2], texcoord);

  // �Օ�����Ȃ��|�C���g�̐�
  int count = 0;
  
#if JITTERING
  // �W�b�^�����O
  int offset = int(mod(gl_FragCoord.y, 4.0)) * 64 + int(mod(gl_FragCoord.x, 4.0)) * 16;
#endif
  
  // �X�̃T���v���_�ɂ���
  for (int i = 0; i < SAMPLES; ++i) {

    // �T���v���_�̈ʒu�� p ����̑��Έʒu�ɕ��s�ړ�������C���̓_�̃N���b�s���O���W�n��̈ʒu q �����߂�
#if JITTERING
    vec4 q = gl_ProjectionMatrix * (p + point[i + offset]);
#else
    vec4 q = gl_ProjectionMatrix * (p + point[i]);
#endif
    
    // �e�N�X�`�����W�ɕϊ�
    q = q * 0.5 / q.w + 0.5;
      
    // q �̐[�x�� unit[4] �̒l (�f�v�X�o�b�t�@�̒l) ��菬������΁C�Օ�����Ȃ��|�C���g�Ƃ��ăJ�E���g����
    if (q.z < texture2D(unit[4], q.xy).z) ++count;
  }

  // �Օ�����Ȃ��|�C���g�̐�������Օ��W�������߂�
  float a = clamp(float(count) * SAMPLING_RATIO / float(SAMPLES), 0.0, 1.0);
  
  // �����̔��ˌ� (unit[1]) �Ɋ��Օ��W����K�p
  vec4 color = diffspec + texture2D(unit[1], texcoord) * a;
  
  // unit[3] ����@���x�N�g�������o��
  vec3 normal = texture2D(unit[3], texcoord).xyz;

  // �� (���˃}�b�s���O)
  color += texture2D(unit[5], normal.xy * 0.5 + 0.5) * reflection;
  
  // �w�i (���܃}�b�s���O)
  vec4 bg = texture2D(unit[6], texcoord + refract(vec3(0.0, 0.0, 1.0), normal, refraction).xy * bgdistance);

  // �S�i�Ɣw�i���������ďo��
  gl_FragColor = mix(bg, color, color.a);
}
