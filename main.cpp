#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cmath>

#if defined(WIN32)
#  pragma warning(disable:4996)
//#  pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#  pragma comment(lib, "glew32.lib")
#  include "glew.h"
#  include "glut.h"
#elif defined(__APPLE__) || defined(MACOSX)
#  include <GLUT/glut.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/glut.h>
#endif

#ifdef _DEBUG
static void CheckFramebufferStatus(void)
{
  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

  switch (status) {
  case GL_FRAMEBUFFER_COMPLETE_EXT:
    break;
  case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
    fprintf(stderr, "Unsupported framebuffer format\n");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
    fprintf(stderr, "Framebuffer incomplete, missing attachment\n");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
    fprintf(stderr, "Framebuffer incomplete, duplicate attachment\n");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
    fprintf(stderr, "Framebuffer incomplete, attached images must have same dimensions\n");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
    fprintf(stderr, "Framebuffer incomplete, attached images must have same format\n");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
    fprintf(stderr, "Framebuffer incomplete, missing draw buffer\n");
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
    fprintf(stderr, "Framebuffer incomplete, missing read buffer\n");
    break;
  default:
    fprintf(stderr, "Programming error; will fail on all hardware: %04x\n", status);
    break;
  }
}

static void CheckOpenGLError(const char *str)
{
  GLenum error = glGetError();

  switch (error) {
  case GL_NO_ERROR:
    break;
  case GL_INVALID_ENUM:
    fprintf(stderr, "%s: An unacceptable value is specified for an enumerated argument\n", str);
    break;
  case GL_INVALID_VALUE:
    fprintf(stderr, "%s: A numeric argument is out of range\n", str);
    break;
  case GL_INVALID_OPERATION:
    fprintf(stderr, "%s: The specified operation is not allowed in the current state\n", str);
    break;
  case GL_STACK_OVERFLOW:
    fprintf(stderr, "%s: This command would cause a stack overflow\n", str);
    break;
  case GL_STACK_UNDERFLOW:
    fprintf(stderr, "%s: This command would cause a a stack underflow\n", str);
    break;
  case GL_OUT_OF_MEMORY:
    fprintf(stderr, "%s: There is not enough memory left to execute the command\n", str);
    break;
  case GL_TABLE_TOO_LARGE:
    fprintf(stderr, "%s: The specified table exceeds the implementation's maximum supported table size\n", str);
    break;
#  ifndef GL_INVALID_FRAMEBUFFER_OPERATION
#    define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#  endif
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    fprintf(stderr, "%s: The specified operation is not allowed current frame buffer\n", str);
    break;
  default:
    fprintf(stderr, "%s: An OpenGL error has occured: 0x%04x\n", str, error);
    break;
  }
}
#else
#  define CheckFramebufferStatus()
#  define CheckOpenGLError(str)
#endif

/*
** �t���[���o�b�t�@�I�u�W�F�N�g
*/
static GLuint fbo;                            // �t���[���o�b�t�@�I�u�W�F�N�g��
static const GLenum bufs[] = {                // �o�b�t�@��
  GL_COLOR_ATTACHMENT0_EXT,                   //   �g�U���ˌ�
  GL_COLOR_ATTACHMENT1_EXT,                   //   ���ʔ��ˌ�
  GL_COLOR_ATTACHMENT2_EXT,                   //   �ʒu
  GL_COLOR_ATTACHMENT3_EXT,                   //   �@��
};
#define BUFNUM (sizeof bufs / sizeof bufs[0]) // �A�^�b�`�����g�̐�
#define BUFWIDTH 1024                         // �t���[���o�b�t�@�I�u�W�F�N�g�̕�
#define BUFHEIGHT 1024                        // �t���[���o�b�t�@�I�u�W�F�N�g�̍���
#define TEXNUM (BUFNUM + 3)                   // �e�N�X�`���̐� (buf �̗v�f + �[�x + �� + �w�i)
static GLuint tex[TEXNUM];                    // �e�N�X�`���� 
static GLint unit;                            // �e�N�X�`�����j�b�g�� uniform �ϐ�

/*
** �[�x�̃T���v�����O�Ɏg���_�Q
*/
#define MAXSAMPLES 256                        // �T���v���_�̍ő吔
#define SAMPLERADIUS 0.1f                     // �T���v���_�̎U�z���a
static GLfloat pointbuf[MAXSAMPLES][4];       // �T���v���_�̈ʒu
static GLint point;                           // �T���v���_�� uniform �ϐ�

/*
** �V�F�[�_�I�u�W�F�N�g
*/
#include "glsl.h"
static GLuint pass1, pass2;

/*
**�g���b�N�{�[������
*/
#include "Trackball.h"
static Trackball *tb;
static int btn = -1;

/*
** OBJ �t�@�C��
*/
#include "Obj.h"
static Obj *obj;
static char objfile[] = "HappyBudda.obj";

/*
** �ގ�
*/
static const GLfloat kd[] = { 0.5f, 0.5f, 0.1f, 0.6f };     // �g�U���ˌW�� (alpha > 0 �ɂ���)
static const GLfloat ks[] = { 0.2f, 0.2f, 0.2f, 0.0f };     // ���ʔ��ˌW�� (alpha = 0 �ɂ���)
static const GLfloat kshi = 40.0f;                          // �P���W��
static const GLfloat kr[] = { 0.2f, 0.2f, 0.2f, 0.0f };     // �f�荞�݂̔��˗� (alpha = 0 �ɂ���)
static const GLfloat keta = 0.67f;                          // ���ܗ��̔�
static const GLfloat kbgd = 0.1f;                           // �w�i�Ƃ̉��z�I�ȋ���
static GLint reflection;                                    // �f�荞�݂̔��˗���n�� uniform �ϐ�
static GLint refraction;                                    // ���ܗ��̔��n�� uniform �ϐ�
static GLint bgdistance;                                    // �w�i�Ƃ̋�����n�� uniform �ϐ�

/*
** ����
*/
static const GLfloat lpos[] = { 0.0f, 0.0f, 10.0f, 1.0f };  // �����ʒu (w �͖������Ă���)
static const GLfloat lcol[] = { 0.5f, 0.5f, 0.5f, 1.0f };   // �������x (alpha = 1 �ɂ���)
static const GLfloat lamb[] = { 0.8f, 0.8f, 0.8f, 0.0f };   // �������x (alpha = 0 �ɂ���)

/*
** ���_
*/
static GLdouble fovy = 60.0, zNear = 0.5, zFar = 3.0;
static GLdouble ex = 0.0, ey = 0.0, ez = -1.5;
static const GLdouble shiftstep = 0.1;  

/*
** �E�B���h�E�̕��ƍ���
*/
static int ww, wh;

/*
** �V�F�[�_�v���O�����̍쐬
*/
static GLuint loadShader(const char *vert, const char *frag)
{
  // �V�F�[�_�I�u�W�F�N�g�̍쐬
  GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  
  // �V�F�[�_�̃\�[�X�v���O�����̓ǂݍ���
  if (readShaderSource(vertShader, vert)) exit(1);
  if (readShaderSource(fragShader, frag)) exit(1);
  
  // �V�F�[�_�v���O�����̃R���p�C���^�����N���ʂ𓾂�ϐ�
  GLint compiled, linked;

  // �o�[�e�b�N�X�V�F�[�_�̃\�[�X�v���O�����̃R���p�C��
  glCompileShader(vertShader);
  glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
  printShaderInfoLog(vertShader);
  if (compiled == GL_FALSE) {
    std::cerr << "Compile error in vertex shader." << std::endl;
    exit(1);
  }
  
  // �t���O�����g�V�F�[�_�̃\�[�X�v���O�����̃R���p�C��
  glCompileShader(fragShader);
  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
  printShaderInfoLog(fragShader);
  if (compiled == GL_FALSE) {
    std::cerr << "Compile error in fragment shader." << std::endl;
    exit(1);
  }
  
  // �v���O�����I�u�W�F�N�g�̍쐬
  GLuint gl2Program = glCreateProgram();
  
  // �V�F�[�_�I�u�W�F�N�g�̃V�F�[�_�v���O�����ւ̓o�^
  glAttachShader(gl2Program, vertShader);
  glAttachShader(gl2Program, fragShader);
  
  // �V�F�[�_�I�u�W�F�N�g�̍폜
  glDeleteShader(vertShader);
  glDeleteShader(fragShader);
  
  // �V�F�[�_�v���O�����̃����N
  glLinkProgram(gl2Program);
  glGetProgramiv(gl2Program, GL_LINK_STATUS, &linked);
  printProgramInfoLog(gl2Program);
  if (linked == GL_FALSE) {
    std::cerr << "Link error." << std::endl;
    exit(1);
  }

  return gl2Program;
}

/*
** �e�N�X�`���t�@�C���̓ǂݍ���
*/
static GLuint loadImage(const char *name, int width, int height)
{
  std::ifstream file(name, std::ios::binary);

  if (file.fail()) {
    std::cerr << "Can't open file: " << name << std::endl;
    exit(1);
  }
  else {

    // �t�@�C���̖����Ɉړ�
    file.seekg(0L, std::ios::end);

    // ���݂̃t�@�C���̓ǂݍ��݈ʒu (���T�C�Y) ���擾
    GLsizei length = file.tellg();

    // �t�@�C���T�C�Y���̃��������m��
    GLubyte *image = new GLubyte[length];

    // �t�@�C���̐擪�Ɉړ�
    file.seekg(0L, std::ios::beg);

    // �t�@�C���̓ǂݍ���
    file.read((char *)image, length);

    // �e�N�X�`���I�u�W�F�N�g�̍쐬
    GLuint texname;
    glGenTextures(1, &texname);

    // �e�N�X�`���̌���
    glBindTexture(GL_TEXTURE_2D, texname);

    // �e�N�X�`���̊��蓖��
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // �e�N�X�`���p�����[�^
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // �ǂݍ��݂Ɏg�����������̊J��
    delete[] image;

    return texname;
  }
}

/*
** �t���[���o�b�t�@�I�u�W�F�N�g�̃A�^�b�`�����g�̍쐬
*/
static GLuint attachTexture(GLint format, GLsizei width, GLsizei height, GLenum attachment)
{
  // �e�N�X�`���I�u�W�F�N�g�̍쐬
  GLuint texname;
  glGenTextures(1, &texname);

  // �e�N�X�`���̌���
  glBindTexture(GL_TEXTURE_2D, texname);

  // �e�N�X�`���̊��蓖��
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
    (format == GL_DEPTH_COMPONENT) ? GL_DEPTH_COMPONENT : GL_RGBA, GL_UNSIGNED_BYTE, 0);

  // �e�N�X�`���p�����[�^
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // �t���[���o�b�t�@�I�u�W�F�N�g�Ɍ���
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment, GL_TEXTURE_2D, texname, 0);

  return texname;
}

/*
** ������
*/
static void init(void)
{
#if defined(WIN32)
  // GLEW �̏�����
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    std::cerr <<  "Error: " << glewGetErrorString(err) << std::endl;
    exit(1);
  }
#endif
  
  // OBJ �t�@�C���̓ǂݍ���
  obj = new Obj(objfile);

  // �g���b�N�{�[�������p�I�u�W�F�N�g�̐���
  tb = new Trackball;

  // �V�F�[�_�v���O�����̍쐬
  pass1 = loadShader("pass1.vert", "pass1.frag");
  pass2 = loadShader("pass2.vert", "pass2.frag");

  // pass2 �� uniform �ϐ��̎��ʎq�̎擾
  unit = glGetUniformLocation(pass2, "unit");
  point = glGetUniformLocation(pass2, "point");
  reflection = glGetUniformLocation(pass2, "reflection");
  refraction = glGetUniformLocation(pass2, "refraction");
  bgdistance = glGetUniformLocation(pass2, "bgdistance");

  /*
  ** �t���[���o�b�t�@�I�u�W�F�N�g���쐬���ă����_�����O�^�[�Q�b�g��p�ӂ���
  */

  // �t���[���o�b�t�@�I�u�W�F�N�g�̍쐬
  glGenFramebuffersEXT(1, &fbo);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

  // �g�U���ˌ� D + ���ʔ��ˌ� S
  tex[0] = attachTexture(GL_RGBA, BUFWIDTH, BUFHEIGHT, bufs[0]);

  // �����̔��ˌ� A
  tex[1] = attachTexture(GL_RGBA, BUFWIDTH, BUFHEIGHT, bufs[1]);

  // �ʒu P
  tex[2] = attachTexture(GL_RGBA32F_ARB, BUFWIDTH, BUFHEIGHT, bufs[2]);

  // �@�� N
  tex[3] = attachTexture(GL_RGBA32F_ARB, BUFWIDTH, BUFHEIGHT, bufs[3]);

  // �[�x
  tex[4] = attachTexture(GL_DEPTH_COMPONENT, BUFWIDTH, BUFHEIGHT, GL_DEPTH_ATTACHMENT_EXT);
  
  // �t���[���o�b�t�@�I�u�W�F�N�g�̌���������
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  // �����_�����O�^�[�Q�b�g���N���A����l
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // �� (�X�t�B�A�}�b�v)
  tex[5] = loadImage("room.raw", 256, 256);

  // �w�i (���܃}�b�s���O)
  tex[6] = loadImage("campus.raw", 1024, 1024);
  
  // �f�t�H���g�̃e�N�X�`���ɖ߂�
  glBindTexture(GL_TEXTURE_2D, 0);

  /*
  ** ���Օ��W�����Z�o����̂Ɏg������̓_�Q
  */

  // �T���v���_�̐���
  for (int i = 0; i < MAXSAMPLES; ++i) {
    float r = SAMPLERADIUS * (float)rand() / (float)RAND_MAX;
    float t = 6.2831853f * (float)rand() / ((float)RAND_MAX + 1.0f);
    float cp = 2.0f * (float)rand() / (float)RAND_MAX - 1.0f;
    float sp = sqrt(1.0f - cp * cp);
    float ct = cos(t), st = sin(t);

    pointbuf[i][0] = r * sp * ct;
    pointbuf[i][1] = r * sp * st;
    pointbuf[i][2] = r * cp;
    pointbuf[i][3] = 0.0f;
  }
}

/*
** �n��
*/
static void ground(void)
{
  static const GLfloat vertex[][3] = {
    { -1.0f, -0.6f, -1.0f },
    { -1.0f, -0.6f,  1.0f },
    {  1.0f, -0.6f,  1.0f },
    {  1.0f, -0.6f, -1.0f },
  };
  static const GLfloat normal[][3] = {
    {  0.0f,  1.0f,  0.0f },
    {  0.0f,  1.0f,  0.0f },
    {  0.0f,  1.0f,  0.0f },
    {  0.0f,  1.0f,  0.0f },
  };
  static const GLfloat color[][4] = {
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
  };

  // �ގ��̐ݒ�
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color[0]);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color[1]);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1.0f);

  // �n�ʂ̃|���S���̕`��
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vertex);
  glNormalPointer(GL_FLOAT, 0, normal);
  glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof vertex / sizeof vertex[0]);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

/*
** �V�[���̕`��
*/
static void scene(void)
{
  // ���f���r���[�ϊ��s��̕ۑ�
  glPushMatrix();

  // �n�ʂ�`��
  ground();

  // �g���b�N�{�[�����̉�]��^����
  glMultMatrixd(tb->rotation());

  glRotated(45.0, 0.0, 1.0, 0.0);

  // �ގ���ݒ肷��
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, kd);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, kshi);

#if 0
  glBegin(GL_POINTS);
  for (int i = 0; i < MAXSAPLES; ++i) glVertex3fv(pointbuf[i]);
  glEnd();
#else
  // �I�u�W�F�N�g��`��
  obj->draw();
#endif
  
  // ���f���r���[�ϊ��s��̕��A
  glPopMatrix();
}


/****************************
** GLUT �̃R�[���o�b�N�֐� **
****************************/

static void display(void)
{
  /*
  ** pass1: �����_�����O�^�[�Q�b�g�� D, S, P, N ����ѐ[�x���i�[����
  */

  // �e�N�X�`���S�̂��r���[�|�[�g�ɂ���
  glViewport(0, 0, BUFWIDTH, BUFHEIGHT);
  
  // �f�v�X�e�X�g��L���ɂ���
  glEnable(GL_DEPTH_TEST);
  
  // �t���[���o�b�t�@�I�u�W�F�N�g�ւ̃����_�����O�J�n
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
  glDrawBuffers(BUFNUM, bufs);
  
  // �J���[�o�b�t�@�ƃf�v�X�o�b�t�@���N���A
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // pass1 �V�F�[�_�[��L���ɂ���
  glUseProgram(pass1);
  
  // ���f���r���[�ϊ��s��̐ݒ�
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(ex, ey, ez);
  
  // ����
  glLightfv(GL_LIGHT0, GL_POSITION, lpos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lcol);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lcol);
  glLightfv(GL_LIGHT0, GL_AMBIENT, lamb);
  
  // �V�[���̕`��
  scene();
  
  /*
  ** pass2: �����_�����O�^�[�Q�b�g���}�b�s���O���ĕ\���̈�𕢂��|���S����`��
  */

  // �E�B���h�E�S�̂��r���[�|�[�g�ɂ���
  glViewport(0, 0, ww, wh);
  
  // �f�v�X�e�X�g�𖳌��ɂ���
  glDisable(GL_DEPTH_TEST);
  
  // �ʏ�̃t���[���o�b�t�@�ւ̃����_�����O�J�n
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  glDrawBuffer(GL_BACK);

  // pass2 �V�F�[�_�[��L���ɂ���
  glUseProgram(pass2);

  // pass2 �ŏ�������ގ����w�肷��
  glUniform4fv(reflection, 1, kr);
  glUniform1f(refraction, keta);
  glUniform1f(bgdistance, kbgd);

  // �e�N�X�`�����j�b�g���w�肷��
  GLint unitname[TEXNUM];
  for (int i = 0; i < TEXNUM; ++i) {
    unitname[i] = i;
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, tex[i]);
  }
  glUniform1iv(unit, TEXNUM, unitname);

  // �_�Q�̈ʒu�� uniform �ϐ��ɐݒ肷��
  glUniform4fv(point, MAXSAMPLES, pointbuf[0]);

  // �N���b�s���O��Ԃ����ς��̃|���S��
  static const GLfloat quad[][2] = {
    { -1.0f, -1.0f },
    {  1.0f, -1.0f },
    {  1.0f,  1.0f },
    { -1.0f,  1.0f },
  };

  // �\���̈�𕢂��|���S����`��
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, quad);
  glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof quad / sizeof quad[0]);
  glDisableClientState(GL_VERTEX_ARRAY);
  glBindTexture(GL_TEXTURE_2D, 0);

  // �Œ�@�\�V�F�[�_�[�ɖ߂�
  glUseProgram(0);
  
  // �_�u���o�b�t�@�����O
  glutSwapBuffers();
}

static void resize(int w, int h)
{
  // �E�B���h�E�̕��ƍ������o���Ă���
  ww = w;
  wh = h;

  // �����ϊ��s��̐ݒ�
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fovy, (double)w / (double)h, zNear, zFar);

  // �g���b�N�{�[������͈�
  tb->region(w, h);
}

static void idle(void)
{
  // ��ʂ̕`���ւ�
  glutPostRedisplay();
}

static void mouse(int button, int state, int x, int y)
{
  btn = button;

  switch (btn) {
  case GLUT_LEFT_BUTTON:
    if (state == GLUT_DOWN) {
      // �g���b�N�{�[���J�n
      tb->start(x, y);
      glutIdleFunc(idle);
    }
    else {
      // �g���b�N�{�[����~
      tb->stop(x, y);
      glutIdleFunc(0);
    }
    break;
  case GLUT_RIGHT_BUTTON:
    break;
  default:
    break;
  }
}

static void motion(int x, int y)
{
  switch (btn) {
  case GLUT_LEFT_BUTTON:
    // �g���b�N�{�[���ړ�
    tb->motion(x, y);
    break;
  case GLUT_RIGHT_BUTTON:
    break;
  default:
    break;
  }
}

static void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 'z':
    ez += shiftstep;
    glutPostRedisplay();
    break;
  case 'Z':
    ez -= shiftstep;
    glutPostRedisplay();
    break;
  case 'q':
  case 'Q':
  case '\033':
    // ESC �� q �� Q ���^�C�v������I��
    exit(0);
  default:
    break;
  }
}

static void function(int key, int x, int y)
{
  switch (key) {
      
  case GLUT_KEY_LEFT:
    ex -= shiftstep;
    break;
  case GLUT_KEY_RIGHT:
    ex += shiftstep;
    break;
  case GLUT_KEY_DOWN:
    ey -= shiftstep;
    break;
  case GLUT_KEY_UP:
    ey += shiftstep;
    break;
  default:
    break;
  }

  glutPostRedisplay();
}

/*
** ���C���v���O����
*/
int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitWindowSize(800, 800);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow("SSAO Sample");
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(function);
  init();
  glutMainLoop();
  return 0;
}
