#version 120
//
// pass1.vert
//
#define AMBIENT_ONLY 0

varying vec4 diffspec;                                                // 拡散反射光 D + 鏡面反射光 S
varying vec4 ambient;                                                 // 環境光の反射光 A
varying vec3 position;                                                // 物体表面上の位置 P
varying vec3 normal;                                                  // その点の法線ベクトル N

void main(void)
{
  position = vec3(gl_ModelViewMatrix * gl_Vertex);
  normal = normalize(gl_NormalMatrix * gl_Normal);
  
  vec3 light = normalize(gl_LightSource[0].position.xyz - position);  // その点から光源に向かう光線ベクトル L
  vec3 view = -normalize(position);                                   // その点から視点に向かう視線ベクトル V
  vec3 halfway = normalize(light + view);                             // 中間ベクトル H
  
  float nl = dot(normal, light);
  float nh = pow(dot(normal, halfway), gl_FrontMaterial.shininess);

  // 拡散反射光＋鏡面反射光を diffspec に格納する
  diffspec = gl_FrontMaterial.diffuse * vec4(vec3(nl), 1.0) * gl_LightSource[0].diffuse + gl_FrontMaterial.specular * vec4(vec3(nh), 1.0) * gl_LightSource[0].specular;

  // 環境光を ambient に格納する
  ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
  
  gl_Position = ftransform();                                         // 頂点位置の座標変換
}
