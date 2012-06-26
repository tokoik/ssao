/*
** 簡易トラックボール処理
*/
#ifndef TRACKBALL_H
#define TRACKBALL_H

class Trackball {
  int cx, cy;                   // ドラッグ開始位置
  double sx, sy;                // マウスの絶対位置→ウィンドウ内での相対位置の換算係数
  double cq[4];                 // 回転の初期値 (クォータニオン)
  double tq[4];                 // ドラッグ中の回転 (クォータニオン)
  double rt[16];                // 回転の変換行列
  bool drag;                    // ドラッグ中か否か
public:
  Trackball(void);
  ~Trackball(void) {};
  void region(int w, int h);    // トラックボール処理の範囲指定
  void start(int x, int y);     // トラックボール処理の開始
  void motion(int x, int y);    // 回転の変換行列の計算
  void stop(int x, int y);      // トラックボール処理の停止
  const double *rotation(void); // 回転の変換行列の取り出し
};

#endif
