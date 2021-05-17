#pragma once

#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <opencv2/opencv.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>



/////////////////////////// 関数 ////////////////////////////////////

// シェーダオブジェクトのコンパイル結果を表示する
// shader: シェーダオブジェクト名
// str: コンパイルエラーが発生した場所を示す文字列
GLboolean printShaderInfoLog(GLuint shader, const char *str);

// プログラムオブジェクトのリンク結果を表示する
// program: プログラムオブジェクト名
GLboolean printProgramInfoLog(GLuint program);

// プログラムオブジェクトを作成する
// vsrc: バーテックスシェーダのソースプログラムの文字列
// fsrc: フラグメントシェーダのソースプログラムの文字列
GLuint createProgram(const char *vsrc, const char *fsrc);


// シェーダのソースファイルを読み込んだメモリを返す
// name: シェーダのソースファイル名
// buffer: 読み込んだソースファイルのテキスト
bool readShaderSource(const char *name, std::vector<GLchar> &buffer);

// シェーダのソースファイルを読み込んでプログラムオブジェクトを作成する
// vert: バーテックスシェーダのソースファイル名
// frag: フラグメントシェーダのソースファイル名
GLuint loadProgram(const char *vert, const char *frag);

//  OpenCVカメラパラメータからOpenGL(GLM)プロジェクション行列を得る関数
void cameraFrustumRH(cv::Mat camMat, cv::Size camSz, glm::mat4 &projMat, double znear, double zfar);

void saveImage(const unsigned int imageWidth, const unsigned int imageHeight);