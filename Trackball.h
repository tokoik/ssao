/*
** �ȈՃg���b�N�{�[������
*/
#ifndef TRACKBALL_H
#define TRACKBALL_H

class Trackball {
  int cx, cy;                   // �h���b�O�J�n�ʒu
  double sx, sy;                // �}�E�X�̐�Έʒu���E�B���h�E���ł̑��Έʒu�̊��Z�W��
  double cq[4];                 // ��]�̏����l (�N�H�[�^�j�I��)
  double tq[4];                 // �h���b�O���̉�] (�N�H�[�^�j�I��)
  double rt[16];                // ��]�̕ϊ��s��
  bool drag;                    // �h���b�O�����ۂ�
public:
  Trackball(void);
  ~Trackball(void) {};
  void region(int w, int h);    // �g���b�N�{�[�������͈͎̔w��
  void start(int x, int y);     // �g���b�N�{�[�������̊J�n
  void motion(int x, int y);    // ��]�̕ϊ��s��̌v�Z
  void stop(int x, int y);      // �g���b�N�{�[�������̒�~
  const double *rotation(void); // ��]�̕ϊ��s��̎��o��
};

#endif
