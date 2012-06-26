/*
** �O�p�`�������� Alias OBJ �`���t�@�C���̓ǂݍ���
*/
#ifndef OBJ_H
#define OBJ_H

class Obj {
  int nv, nf;                   // ���_�̐��C�ʂ̐�
  float (*vert)[3];             // ���_�̈ʒu
  float (*norm)[3];             // ���_�̖@���x�N�g��
  float (*fnorm)[3];            // �ʂ̖@���x�N�g��
  int (*face)[3];               // �ʃf�[�^�i���_�̃C���f�b�N�X�j
  void init();                  // ������
  void copy(const Obj &);       // �������̃R�s�[
  void free();                  // �������̉��

public:
  Obj();
  Obj(const char *name);
  Obj(const Obj &o);
  ~Obj();
  
  Obj &operator=(const Obj &o);
  
  bool load(const char *name);  // OBJ �t�@�C���̓ǂݍ���
  void draw();                  // �}�`�̕`��
};

#endif
