#version 120
//
// pass1.frag
//
varying vec4 diffspec;                                                // 拡散反射光 D + 鏡面反射光 S
varying vec4 ambient;                                                 // 環境光の反射光 A
varying vec3 position;                                                // 物体表面上の位置 P
varying vec3 normal;                                                  // その点の法線ベクトル N

void main(void)
{
  gl_FragData[0] = diffspec;
  gl_FragData[1] = ambient;
  gl_FragData[2] = vec4(position, 1.0);
  gl_FragData[3] = vec4(normal, 0.0);
}
