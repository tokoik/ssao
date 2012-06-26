#version 120
//
// pass2.frag
//
#define MAXSAMPLES 256                  // サンプル点の最大数
#define JITTERING 0                     // jittering の有無

const int SAMPLES = 256;                // サンプル点数
const float SAMPLING_RATIO = 2.0;       // サンプル点の数の比較係数

uniform sampler2D unit[7];              // テクスチャユニット
uniform vec4 point[MAXSAMPLES];         // サンプル点の位置
uniform vec4 reflection;                // 映り込みの反射率
uniform float refraction;               // 屈折率比
uniform float bgdistance;               // 背景との距離
varying vec2 texcoord;                  // テクスチャ座標

void main(void)
{
  // unit[0] から拡散反射光強度 diffuse + specular を取得
  vec4 diffspec = texture2D(unit[0], texcoord);
  
  // diffspec のアルファ値が 0 なら背景 (unit[6]) のみ
  if (diffspec.a == 0.0) {
    gl_FragColor = texture2D(unit[6], texcoord);
    return;
  }
  
  // unit[2] から処理対象の画素の視点座標系上の位置 position を取得
  vec4 p = texture2D(unit[2], texcoord);

  // 遮蔽されないポイントの数
  int count = 0;
  
#if JITTERING
  // ジッタリング
  int offset = int(mod(gl_FragCoord.y, 4.0)) * 64 + int(mod(gl_FragCoord.x, 4.0)) * 16;
#endif
  
  // 個々のサンプル点について
  for (int i = 0; i < SAMPLES; ++i) {

    // サンプル点の位置を p からの相対位置に平行移動した後，その点のクリッピング座標系上の位置 q を求める
#if JITTERING
    vec4 q = gl_ProjectionMatrix * (p + point[i + offset]);
#else
    vec4 q = gl_ProjectionMatrix * (p + point[i]);
#endif
    
    // テクスチャ座標に変換
    q = q * 0.5 / q.w + 0.5;
      
    // q の深度が unit[4] の値 (デプスバッファの値) より小さければ，遮蔽されないポイントとしてカウントする
    if (q.z < texture2D(unit[4], q.xy).z) ++count;
  }

  // 遮蔽されないポイントの数から環境遮蔽係数を求める
  float a = clamp(float(count) * SAMPLING_RATIO / float(SAMPLES), 0.0, 1.0);
  
  // 環境光の反射光 (unit[1]) に環境遮蔽係数を適用
  vec4 color = diffspec + texture2D(unit[1], texcoord) * a;
  
  // unit[3] から法線ベクトルを取り出す
  vec3 normal = texture2D(unit[3], texcoord).xyz;

  // 環境 (反射マッピング)
  color += texture2D(unit[5], normal.xy * 0.5 + 0.5) * reflection;
  
  // 背景 (屈折マッピング)
  vec4 bg = texture2D(unit[6], texcoord + refract(vec3(0.0, 0.0, 1.0), normal, refraction).xy * bgdistance);

  // 全景と背景を合成して出力
  gl_FragColor = mix(bg, color, color.a);
}
