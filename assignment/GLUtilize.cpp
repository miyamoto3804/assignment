#pragma once

#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <opencv2/opencv.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLUtilize.h"



/////////////////////////// 関数 ////////////////////////////////////

// シェーダオブジェクトのコンパイル結果を表示する
// shader: シェーダオブジェクト名
// str: コンパイルエラーが発生した場所を示す文字列
GLboolean printShaderInfoLog(GLuint shader, const char *str)
{
	// コンパイル結果を取得する
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) std::cerr << "Compile Error in " << str << std::endl;
	// シェーダのコンパイル時のログの長さを取得する
	GLsizei bufSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);
	if (bufSize > 1)
	{
		// シェーダのコンパイル時のログの内容を取得する
		std::vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
		std::cerr << &infoLog[0] << std::endl;
	}
	return static_cast<GLboolean>(status);
}

// プログラムオブジェクトのリンク結果を表示する
// program: プログラムオブジェクト名
GLboolean printProgramInfoLog(GLuint program)
{
	// リンク結果を取得する
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) std::cerr << "Link Error." << std::endl;
	// シェーダのリンク時のログの長さを取得する
	GLsizei bufSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);
	if (bufSize > 1)
	{
		// シェーダのリンク時のログの内容を取得する
		std::vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
		std::cerr << &infoLog[0] << std::endl;
	}
	return static_cast<GLboolean>(status);
}


// プログラムオブジェクトを作成する
// vsrc: バーテックスシェーダのソースプログラムの文字列
// fsrc: フラグメントシェーダのソースプログラムの文字列
GLuint createProgram(const char *vsrc, const char *fsrc)
{
	// 空のプログラムオブジェクトを作成する
	const GLuint program(glCreateProgram());
	if (vsrc != NULL)
	{
		// バーテックスシェーダのシェーダオブジェクトを作成する
		const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
		glShaderSource(vobj, 1, &vsrc, NULL);
		glCompileShader(vobj);
		// バーテックスシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
		if (printShaderInfoLog(vobj, "vertex shader"))
			glAttachShader(program, vobj);
		glDeleteShader(vobj);
	}
	if (fsrc != NULL)
	{
		// フラグメントシェーダのシェーダオブジェクトを作成する
		const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
		glShaderSource(fobj, 1, &fsrc, NULL);
		glCompileShader(fobj);
		// フラグメントシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
		if (printShaderInfoLog(fobj, "fragment shader"))
			glAttachShader(program, fobj);
		glDeleteShader(fobj);
	}
	// プログラムオブジェクトをリンクする
	glBindAttribLocation(program, 0, "position"); //in
	glBindAttribLocation(program, 1, "color");
	glBindFragDataLocation(program, 0, "fragment"); //out
	glLinkProgram(program);
	// 作成したプログラムオブジェクトを返す
	if (printProgramInfoLog(program))
		return program;

	// プログラムオブジェクトが作成できなければ 0 を返す
	glDeleteProgram(program);
	return 0;
}


// シェーダのソースファイルを読み込んだメモリを返す
// name: シェーダのソースファイル名
// buffer: 読み込んだソースファイルのテキスト
bool readShaderSource(const char *name, std::vector<GLchar> &buffer)
{
	// ファイル名が NULL だった
	if (name == NULL) return false;
	// ソースファイルを開く
	std::ifstream file(name, std::ios::binary);
	if (file.fail())
	{
		// 開けなかった
		std::cerr << "Error: Can't open source file: " << name << std::endl;
		return false;
	}
	// ファイルの末尾に移動し現在位置（＝ファイルサイズ）を得る
	file.seekg(0L, std::ios::end);
	GLsizei length = static_cast<GLsizei>(file.tellg());
	// ファイルサイズのメモリを確保
	buffer.resize(length + 1);
	// ファイルを先頭から読み込む
	file.seekg(0L, std::ios::beg);
	file.read(buffer.data(), length);
	buffer[length] = '\0';
	if (file.fail())
	{
		// うまく読み込めなかった
		std::cerr << "Error: Could not read souce file: " << name << std::endl;
		file.close();
		return false;
	}
	// 読み込み成功
	file.close();
	return true;
}

// シェーダのソースファイルを読み込んでプログラムオブジェクトを作成する
// vert: バーテックスシェーダのソースファイル名
// frag: フラグメントシェーダのソースファイル名
GLuint loadProgram(const char *vert, const char *frag)
{
	// シェーダのソースファイルを読み込む
	std::vector<GLchar> vsrc;
	const bool vstat(readShaderSource(vert, vsrc));
	std::vector<GLchar> fsrc;
	const bool fstat(readShaderSource(frag, fsrc));
	// プログラムオブジェクトを作成する
	return vstat && fstat ? createProgram(vsrc.data(), fsrc.data()) : 0;
}

//  OpenCVカメラパラメータからOpenGL(GLM)プロジェクション行列を得る関数
void cameraFrustumRH(cv::Mat camMat, cv::Size camSz, glm::mat4 &projMat, double znear, double zfar)
{
	// Load camera parameters
	double fx = camMat.at<double>(0, 0);
	double fy = camMat.at<double>(1, 1);
	double cx = camMat.at<double>(0, 2);
	double cy = camMat.at<double>(1, 2);
	double w = camSz.width, h = camSz.height;

	// 参考:https://strawlab.org/2011/11/05/augmented-reality-with-OpenGL
	// With window_coords=="y_down", we have:
	// [2 * K00 / width,   -2 * K01 / width,   (width - 2 * K02 + 2 * x0) / width,     0]
	// [0,                 2 * K11 / height,   (-height + 2 * K12 + 2 * y0) / height,  0]
	// [0,                 0,                  (-zfar - znear) / (zfar - znear),       -2 * zfar*znear / (zfar - znear)]
	// [0,                 0,                  -1,                                     0]


	glm::mat4 projection(
		2.0 * fx / w, 0, 0, 0,
		0, 2.0 * fy / h, 0, 0,
		1.0 - 2.0 * cx / w, -1.0 + 2.0 * cy / h, -(zfar + znear) / (zfar - znear), -1.0,
		0, 0, -2.0 * zfar * znear / (zfar - znear), 0);
	projMat = projection;
}


void saveImage(const unsigned int imageWidth, const unsigned int imageHeight)
{
	const unsigned int channelNum = 3; // RGB卅日3, RGBA卅日4
	void* dataBuffer = NULL;
	dataBuffer = (GLubyte*)malloc(imageWidth * imageHeight * channelNum);

	// 掂心月OpneGL及田永白央毛硌隅 GL_FRONT:白伕件玄田永白央﹛GL_BACK:田永弁田永白央
	glReadBuffer(GL_BACK);

	// OpenGL匹賒醱卞鏡賒今木化中月囀毛田永白央卞跡慮
	glReadPixels(
		0,                 //掂心月薆郖及酘狟趶及x釱
		0,                 //掂心月薆郖及酘狟趶及y釱 //0 or getCurrentWidth() - 1
		imageWidth,             //掂心月薆郖及盟
		imageHeight,            //掂心月薆郖及詢今
		GL_BGR, //it means GL_BGR,           //腕仄凶中伎及倛宒
		GL_UNSIGNED_BYTE,  //掂心勻凶犯奈正毛悵湔允月饜蹈及倰
		dataBuffer      //申永玄穴永皿及疋弁本伙犯奈正ㄗ蕣卞反田奶玄饜蹈ㄘ尺及禾奶件正
	);

	GLubyte* p = static_cast<GLubyte*>(dataBuffer);
	std::string fname = "outputImage.jpg";
	//IplImage* outImage = cvCreateImage(cvSize(imageWidth, imageHeight), IPL_DEPTH_8U, 3);

	cv::Mat outImage(imageHeight, imageWidth, CV_8UC3);

	for (unsigned int j = 0; j < imageHeight; ++j)
	{
		for (unsigned int i = 0; i < imageWidth; ++i)
		{
			outImage.at<cv::Vec3b>((imageHeight - j - 1), i)[0] = *p;
			outImage.at<cv::Vec3b>((imageHeight - j - 1), i)[1] = *(p + 1);
			outImage.at<cv::Vec3b>((imageHeight - j - 1), i)[2] = *(p + 2);
			//outImage->imageData[(imageHeight - j - 1) * outImage->widthStep + i * 3 + 1] = *(p + 1);
			//outImage->imageData[(imageHeight - j - 1) * outImage->widthStep + i * 3 + 2] = *(p + 2);
			p += 3;
		}
	}

	cv::imwrite(fname.c_str(), outImage);
}