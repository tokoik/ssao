/*
** �V�F�[�_�[�\�[�X�t�@�C���̓ǂݍ���
*/
#include <iostream>
#include <fstream>

#if defined(WIN32)
#  include <GL/glew.h>
#  include <GL/gl.h>
#elif defined(__APPLE__) || defined(MACOSX)
#  include <OpenGL/gl.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#endif

#include "glsl.h"

/*
** �V�F�[�_�[�̃\�[�X�v���O�������������ɓǂݍ���
*/
int readShaderSource(GLuint shader, const char *name)
{
  int ret; // �I���X�e�[�^�X�i�� 0 �Ȃ�G���[�j
    
  std::ifstream file(name, std::ios::binary);
  if (file.fail()) {
    std::cerr << "Can't open file: " << name << std::endl;
    ret = -1;
  }
  else {

    // �t�@�C���̖����Ɉړ������݈ʒu�i�܂�t�@�C���T�C�Y�j�𓾂�
    file.seekg(0L, std::ios::end);
    GLsizei length = static_cast<GLsizei>(file.tellg());
    
    // �t�@�C���T�C�Y�̃��������m��
    const GLchar *source = new GLchar[length];

    // �t�@�C����擪����ǂݍ���
    file.seekg(0L, std::ios::beg);
    file.read((char *)source, length);
    
    if (file.bad()) {

      // ���܂��ǂݍ��߂Ȃ�����
      std::cerr << "Could not read file: " << name << std::endl;
      ret = -1;
    }
    else {

      // �V�F�[�_�̃\�[�X�v���O�����̃V�F�[�_�I�u�W�F�N�g�ւ̓ǂݍ���
      glShaderSource(shader, 1, &source, &length);
      ret = 0;
    }
    
    // �ǂݍ��݂Ɏg�������������J������
    delete[] source;
  }
    
  return ret;
}

/*
** �V�F�[�_�̏���\������
*/
void printShaderInfoLog(GLuint shader)
{
  // �V�F�[�_�̃R���p�C�����̃��O�̒������擾����
  GLsizei bufSize;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &bufSize);
  
  if (bufSize > 1) {
    
    // �V�F�[�_�̃R���p�C�����̃��O�̓��e���擾����
    GLchar *infoLog = new GLchar[bufSize];
    GLsizei length;
    glGetShaderInfoLog(shader, bufSize, &length, infoLog);
    std::cerr << "InfoLog:\n" << infoLog << '\n' << std::endl;
    delete[] infoLog;
  }
}

/*
** �v���O�����̏���\������
 */
void printProgramInfoLog(GLuint program)
{
  // �V�F�[�_�̃����N���̃��O�̒������擾����
  GLsizei bufSize;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH , &bufSize);
  
  if (bufSize > 1) {
    
    // �V�F�[�_�̃����N���̃��O�̓��e���擾����
    GLchar *infoLog = new GLchar[bufSize];
    GLsizei length;
    glGetProgramInfoLog(program, bufSize, &length, infoLog);
    std::cerr << "InfoLog:\n" << infoLog << '\n' << std::endl;
    delete[] infoLog;
  }
}
