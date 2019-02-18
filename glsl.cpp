/*
** シェーダーソースファイルの読み込み
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
** シェーダーのソースプログラムをメモリに読み込む
*/
int readShaderSource(GLuint shader, const char *name)
{
  int ret; // 終了ステータス（非 0 ならエラー）
    
  std::ifstream file(name, std::ios::binary);
  if (file.fail()) {
    std::cerr << "Can't open file: " << name << std::endl;
    ret = -1;
  }
  else {

    // ファイルの末尾に移動し現在位置（つまりファイルサイズ）を得る
    file.seekg(0L, std::ios::end);
    GLsizei length = static_cast<GLsizei>(file.tellg());
    
    // ファイルサイズのメモリを確保
    const GLchar *source = new GLchar[length];

    // ファイルを先頭から読み込む
    file.seekg(0L, std::ios::beg);
    file.read((char *)source, length);
    
    if (file.bad()) {

      // うまく読み込めなかった
      std::cerr << "Could not read file: " << name << std::endl;
      ret = -1;
    }
    else {

      // シェーダのソースプログラムのシェーダオブジェクトへの読み込み
      glShaderSource(shader, 1, &source, &length);
      ret = 0;
    }
    
    // 読み込みに使ったメモリを開放する
    delete[] source;
  }
    
  return ret;
}

/*
** シェーダの情報を表示する
*/
void printShaderInfoLog(GLuint shader)
{
  // シェーダのコンパイル時のログの長さを取得する
  GLsizei bufSize;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &bufSize);
  
  if (bufSize > 1) {
    
    // シェーダのコンパイル時のログの内容を取得する
    GLchar *infoLog = new GLchar[bufSize];
    GLsizei length;
    glGetShaderInfoLog(shader, bufSize, &length, infoLog);
    std::cerr << "InfoLog:\n" << infoLog << '\n' << std::endl;
    delete[] infoLog;
  }
}

/*
** プログラムの情報を表示する
 */
void printProgramInfoLog(GLuint program)
{
  // シェーダのリンク時のログの長さを取得する
  GLsizei bufSize;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH , &bufSize);
  
  if (bufSize > 1) {
    
    // シェーダのリンク時のログの内容を取得する
    GLchar *infoLog = new GLchar[bufSize];
    GLsizei length;
    glGetProgramInfoLog(program, bufSize, &length, infoLog);
    std::cerr << "InfoLog:\n" << infoLog << '\n' << std::endl;
    delete[] infoLog;
  }
}
