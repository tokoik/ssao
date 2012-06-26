/*
** 三角形分割した Alias OBJ 形式ファイルの読み込み
*/
#ifndef OBJ_H
#define OBJ_H

class Obj {
  int nv, nf;                   // 頂点の数，面の数
  float (*vert)[3];             // 頂点の位置
  float (*norm)[3];             // 頂点の法線ベクトル
  float (*fnorm)[3];            // 面の法線ベクトル
  int (*face)[3];               // 面データ（頂点のインデックス）
  void init();                  // 初期化
  void copy(const Obj &);       // メモリのコピー
  void free();                  // メモリの解放

public:
  Obj();
  Obj(const char *name);
  Obj(const Obj &o);
  ~Obj();
  
  Obj &operator=(const Obj &o);
  
  bool load(const char *name);  // OBJ ファイルの読み込み
  void draw();                  // 図形の描画
};

#endif
