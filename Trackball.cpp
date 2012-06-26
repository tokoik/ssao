/*
** �ȈՃg���b�N�{�[������
*/
#include <cmath>

#include "Trackball.h"

#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

#define SCALE (2.0 * M_PI) // �}�E�X�̑��Έʒu����]�p�̊��Z�W��

/*
** r <- p x q
*/
static void qmul(double r[], const double p[], const double q[])
{
  r[0] = p[0] * q[0] - p[1] * q[1] - p[2] * q[2] - p[3] * q[3];
  r[1] = p[0] * q[1] + p[1] * q[0] + p[2] * q[3] - p[3] * q[2];
  r[2] = p[0] * q[2] - p[1] * q[3] + p[2] * q[0] + p[3] * q[1];
  r[3] = p[0] * q[3] + p[1] * q[2] - p[2] * q[1] + p[3] * q[0];
}

/*
** ��]�ϊ��s�� r <- �N�H�[�^�j�I�� q
*/
static void qrot(double r[], double q[])
{
  double x2 = q[1] * q[1] * 2.0;
  double y2 = q[2] * q[2] * 2.0;
  double z2 = q[3] * q[3] * 2.0;
  double xy = q[1] * q[2] * 2.0;
  double yz = q[2] * q[3] * 2.0;
  double zx = q[3] * q[1] * 2.0;
  double xw = q[1] * q[0] * 2.0;
  double yw = q[2] * q[0] * 2.0;
  double zw = q[3] * q[0] * 2.0;
  
  r[ 0] = 1.0 - y2 - z2;
  r[ 1] = xy + zw;
  r[ 2] = zx - yw;
  r[ 4] = xy - zw;
  r[ 5] = 1.0 - z2 - x2;
  r[ 6] = yz + xw;
  r[ 8] = zx + yw;
  r[ 9] = yz - xw;
  r[10] = 1.0 - x2 - y2;
  r[ 3] = r[ 7] = r[11] = r[12] = r[13] = r[14] = 0.0;
  r[15] = 1.0;
}

/*
** �g���b�N�{�[�������̏�����
** �@�@�v���O�����̏����������̂Ƃ���Ŏ��s����
*/
Trackball::Trackball(void)
{
  /* �h���b�O���ł͂Ȃ� */
  drag = false;

  /* �P�ʃN�H�[�^�[�j�I�� */
  cq[0] = 1.0;
  cq[1] = 0.0;
  cq[2] = 0.0;
  cq[3] = 0.0;
  
  /* ��]�s��̏����� */
  rt[ 0] = 1.0; rt[ 1] = 0.0; rt[ 2] = 0.0; rt[ 3] = 0.0;
  rt[ 4] = 0.0; rt[ 5] = 1.0; rt[ 6] = 0.0; rt[ 7] = 0.0;
  rt[ 8] = 0.0; rt[ 9] = 0.0; rt[10] = 1.0; rt[11] = 0.0;
  rt[12] = 0.0; rt[13] = 0.0; rt[14] = 0.0; rt[15] = 1.0;
}

/*
** �g���b�N�{�[������̈�
** �@�@Reshape �R�[���o�b�N (resize) �̒��Ŏ��s����
*/
void Trackball::region(int w, int h)
{
  /* �}�E�X�|�C���^�ʒu�̃E�B���h�E���̑��ΓI�ʒu�ւ̊��Z�p */
  sx = 1.0 / (double)w;
  sy = 1.0 / (double)h;
}

/*
** �h���b�O�J�n
** �@�@�}�E�X�{�^�����������Ƃ��Ɏ��s����
*/
void Trackball::start(int x, int y)
{
  /* �h���b�O�J�n */
  drag = true;

  /* �h���b�O�J�n�_���L�^ */
  cx = x;
  cy = y;
}

/*
** �h���b�O��
** �@�@�}�E�X�̃h���b�O���Ɏ��s����
*/
void Trackball::motion(int x, int y)
{
  if (drag) {
    double dx, dy, a;
    
    /* �}�E�X�|�C���^�̈ʒu�̃h���b�O�J�n�ʒu����̕ψ� */
    dx = (x - cx) * sx;
    dy = (y - cy) * sy;
    
    /* �}�E�X�|�C���^�̈ʒu�̃h���b�O�J�n�ʒu����̋��� */
    a = sqrt(dx * dx + dy * dy);
    
    if (a != 0.0) {
      double ar = a * SCALE * 0.5;
      double as = sin(ar) / a;
      double dq[4] = { cos(ar), dy * as, dx * as, 0.0 };
      
      /* �N�H�[�^�j�I�����|���ĉ�]������ */
      qmul(tq, dq, cq);
      
      /* �N�H�[�^�j�I�������]�̕ϊ��s������߂� */
      qrot(rt, tq);
    }
  }
}

/*
** ��~
** �@�@�}�E�X�{�^���𗣂����Ƃ��Ɏ��s����
*/
void Trackball::stop(int x, int y)
{
  /* �h���b�O�I���_�ɂ������]�����߂� */
  motion(x, y);

  /* ��]�̕ۑ� */
  cq[0] = tq[0];
  cq[1] = tq[1];
  cq[2] = tq[2];
  cq[3] = tq[3];

  /* �h���b�O�I�� */
  drag = false;
}

/*
** ��]�̕ϊ��s���߂�
** �@�@�߂�l�� glMultMatrixd() �ȂǂŎg�p���ăI�u�W�F�N�g����]����
*/
const double *Trackball::rotation(void)
{
  return rt;
}
