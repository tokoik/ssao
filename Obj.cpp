/*
** 三角形分割した Alias OBJ 形式ファイルの読み込み
*/
#include <iostream>
#include <fstream>
#include <cmath>

#if defined(WIN32)
#  pragma warning(disable:4996)
#  include <GL/glew.h>
#  include <GL/gl.h>
#elif defined(__APPLE__) || defined(MACOSX)
#  include <OpenGL/gl.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#endif

#include "Obj.h"

/*
** デフォルトコンストラクタ
*/
Obj::Obj()
{
  init();
}

/*
** コンストラクタ
*/
Obj::Obj(const char *name)
{
  init();
  load(name);
}

/*
** コピーコンストラクタ
*/
Obj::Obj(const Obj &o)
{
  copy(o);
}

/*
** デストラクタ
*/
Obj::~Obj()
{
  free();
}

/*
** 代入演算子
*/
Obj &Obj::operator=(const Obj &o)
{
  if (this != &o) copy(o);

  return *this;
}

/*
** オブジェクトの初期化
*/
void Obj::init()
{
  nv = nf = 0;
  vert = norm = fnorm = 0;
  face = 0;
}

/*
** オブジェクトのコピー
*/
void Obj::copy(const Obj &o)
{
  if (o.nv > 0 && o.nf > 0) {

    try {

      // 頂点データのコピー
      nv = o.nv;

      delete[] vert;
      vert = new float[nv][3];
      memcpy(vert, o.vert, sizeof (float) * 3 * nv);

      delete[] norm;
      norm = new float[nv][3];
      memcpy(norm, o.norm, sizeof (float) * 3 * nv);

      // 面データのコピー
      nf = o.nf;

      delete[] fnorm;
      fnorm = new float[nf][3];
      memcpy(fnorm, o.fnorm, sizeof (float) * 3 * nf);

      delete[] face;
      face = new int[nf][3];
      memcpy(face, o.face, sizeof (int) * 3 * nf);
    }
    catch (std::bad_alloc e) {
      free();
      init();
      throw e;
    }
  }
}

/*
** メモリの解放
*/
void Obj::free()
{
  delete[] vert;
  delete[] norm;
  delete[] fnorm;
  delete[] face;
}

/*
** ファイルの読み込み
*/
bool Obj::load(const char *name)
{
  // ファイルの読み込み
  std::ifstream file(name, std::ios::binary);
  if (file.fail()) {
    std::cerr << "Can't open file: " << name << std::endl;
    return false;
  }

  // データの数を調べる
  char buf[1024];
  int v, f;
  v = f = 0;
  while (file.getline(buf, sizeof buf)) {
    if (buf[0] == 'v' && buf[1] == ' ') {
      ++v;
    }
    else if (buf[0] == 'f' && buf[1] == ' ') {
      ++f;
    }
  }

  // 頂点の数と面の数
  nv = v;
  nf = f;

  // メモリの開放
  free();

  // メモリの確保
  try {
    vert = new float[nv][3];
    norm = new float[nv][3];
    fnorm = new float[nf][3];
    face = new int[nf][3];
  }
  catch (std::bad_alloc e) {
    free();
    init();
    throw e;
  }

  // ファイルの巻き戻し
  file.clear();
  file.seekg(0L, std::ios::beg);

  // データの読み込み
  v = f = 0;
  while (file.getline(buf, sizeof buf)) {
    if (buf[0] == 'v' && buf[1] == ' ') {
      sscanf(buf, "%*s %f %f %f", vert[v], vert[v] + 1, vert[v] + 2);
      ++v;
    }
    else if (buf[0] == 'f' && buf[1] == ' ') {
      if (sscanf(buf + 2, "%d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", face[f], face[f] + 1, face[f] + 2) != 3) {
        if (sscanf(buf + 2, "%d//%*d %d//%*d %d//%*d", face[f], face[f] + 1, face[f] + 2) != 3) {
          sscanf(buf + 2, "%d %d %d", face[f], face[f] + 1, face[f] + 2);
        }
      }
      --face[f][0];
      --face[f][1];
      --face[f][2];
      ++f;
    }
  }

  // 面法線ベクトルの算出
  for (int i = 0; i < f; ++i) {
    float dx1 = vert[face[i][1]][0] - vert[face[i][0]][0];
    float dy1 = vert[face[i][1]][1] - vert[face[i][0]][1];
    float dz1 = vert[face[i][1]][2] - vert[face[i][0]][2];
    float dx2 = vert[face[i][2]][0] - vert[face[i][0]][0];
    float dy2 = vert[face[i][2]][1] - vert[face[i][0]][1];
    float dz2 = vert[face[i][2]][2] - vert[face[i][0]][2];

    fnorm[i][0] = dy1 * dz2 - dz1 * dy2;
    fnorm[i][1] = dz1 * dx2 - dx1 * dz2;
    fnorm[i][2] = dx1 * dy2 - dy1 * dx2;
  }

  // 頂点の仮想法線ベクトルの算出
  for (int i = 0; i < v; ++i) {
    norm[i][0] = norm[i][1] = norm[i][2] = 0.0;
  }
  
  for (int i = 0; i < f; ++i) {
    norm[face[i][0]][0] += fnorm[i][0];
    norm[face[i][0]][1] += fnorm[i][1];
    norm[face[i][0]][2] += fnorm[i][2];

    norm[face[i][1]][0] += fnorm[i][0];
    norm[face[i][1]][1] += fnorm[i][1];
    norm[face[i][1]][2] += fnorm[i][2];

    norm[face[i][2]][0] += fnorm[i][0];
    norm[face[i][2]][1] += fnorm[i][1];
    norm[face[i][2]][2] += fnorm[i][2];
  }

  // 頂点の仮想法線ベクトルの正規化
  for (int i = 0; i < v; ++i) {
    float a = sqrt(norm[i][0] * norm[i][0]
                 + norm[i][1] * norm[i][1]
                 + norm[i][2] * norm[i][2]);

    if (a != 0.0) {
      norm[i][0] /= a;
      norm[i][1] /= a;
      norm[i][2] /= a;
    }
  }

  return true;
}

/*
** 図形の表示
*/
void Obj::draw(void)
{
  // 頂点データ，法線データ，テクスチャ座標の配列を有効にする
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  
  // 頂点データ，法線データ，テクスチャ座標の場所を指定する
  glNormalPointer(GL_FLOAT, 0, norm);
  glVertexPointer(3, GL_FLOAT, 0, vert);
  
  // 頂点のインデックスの場所を指定して図形を描画する
  glDrawElements(GL_TRIANGLES, nf * 3, GL_UNSIGNED_INT, face);

  // 頂点データ，法線データ，テクスチャ座標の配列を無効にする
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
}
