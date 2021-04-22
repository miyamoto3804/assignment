#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"
#include "Shape.h"
#include "Object.h"


#define IMAGE_NUM  (20)         /* 画像数 */
#define PAT_ROW    (6)          /* パターンの行数 */
#define PAT_COL    (7)         /* パターンの列数 */
#define PAT_SIZE   (PAT_ROW*PAT_COL)
#define ALL_POINTS (IMAGE_NUM*PAT_SIZE)
#define CHESS_SIZE (20.0)       /* パターン1マスの1辺サイズ[mm] */

using namespace std;



/////////////////////////// 描画用データ//////////////////////////

const float c = 60.0f;

const float a = -1*c;
const float b = c;

// 矩形の頂点の位置
constexpr Object::Vertex rectangleVertex[] =
{
 { -0.5f, -0.5f },
 { 0.5f, -0.5f },
 { 0.5f, 0.5f },
 { -0.5f, 0.5f }
};

constexpr Object::Vertex g_vertex_buffer_data[] = {
		{a,a,a },
		{a,a, b},
		{a, b, b},
		{b, b,a},
		{a,a,a },
		{a, b,a},
		{b,a, b},
		{a,a,a },
		{b,a,a},
		{ b, b,a},
		{b,a,a },
		{a,a,a},
		{a,a,a },
		{a, b, b},
		{a, b,a},
		{ b,a, b},
		{a,a, b},
		{a,a,a},
		{a, b, b},
		{a,a, b},
		{ b,a, b},
		{ b, b, b},
		{ b,a,a},
		{b, b,a},
		{ b,a,a},
		{ b, b, b},
		{ b,a, b},
		{ b, b, b},
		{ b, b,a},
		{a, b,a},
		{ b, b, b},
		{a, b,a},
		{a, b, b},
		{ b, b, b},
		{a, b, b},
		{ b,a, b}
};

static GLfloat g_color_buffer_data[] = {
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,
		0.822f,  0.569f,  0.201f,
		0.435f,  0.602f,  0.223f,
		0.310f,  0.747f,  0.185f,
		0.597f,  0.770f,  0.761f,
		0.559f,  0.436f,  0.730f,
		0.359f,  0.583f,  0.152f,
		0.483f,  0.596f,  0.789f,
		0.559f,  0.861f,  0.639f,
		0.195f,  0.548f,  0.859f,
		0.014f,  0.184f,  0.576f,
		0.771f,  0.328f,  0.970f,
		0.406f,  0.615f,  0.116f,
		0.676f,  0.977f,  0.133f,
		0.971f,  0.572f,  0.833f,
		0.140f,  0.616f,  0.489f,
		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.055f,  0.953f,  0.042f,
		0.714f,  0.505f,  0.345f,
		0.783f,  0.290f,  0.734f,
		0.722f,  0.645f,  0.174f,
		0.302f,  0.455f,  0.848f,
		0.225f,  0.587f,  0.040f,
		0.517f,  0.713f,  0.338f,
		0.053f,  0.959f,  0.120f,
		0.393f,  0.621f,  0.362f,
		0.673f,  0.211f,  0.457f,
		0.820f,  0.883f,  0.371f,
		0.982f,  0.099f,  0.879f
};


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


// openCVの4×4のcv::Matからglmのglm::mat4への変換
void fromCV2GLM(const cv::Mat& cvmat, glm::mat4* glmmat) {
	if (cvmat.cols != 4 || cvmat.rows != 4 || cvmat.type() != CV_32FC1) {
		cout << "Matrix conversion error!" << endl;
		return;
	}
	memcpy(glm::value_ptr(*glmmat), cvmat.data, 16 * sizeof(float));
	*glmmat = glm::transpose(*glmmat);
}



// glmのglm::mat4からopenCVの4×4のcv::Matへの変換
void fromGLM2CV(const glm::mat4& glmmat, cv::Mat* cvmat) {
	if (cvmat->cols != 4 || cvmat->rows != 4) {
		(*cvmat) = cv::Mat(4, 4, CV_32F);
	}
	memcpy(cvmat->data, glm::value_ptr(glmmat), 16 * sizeof(float));
	*cvmat = cvmat->t();
}

////////////////////////// main ///////////////////////////////


int main() {


	///////////////// カメラで検出(openCV) //////////////////////////

	//Releaseにおいてうまく結果が保存できない問題発生中

	int i, j, k;
	int cornerCount, found;
	int p_count[IMAGE_NUM];
	// cv::Mat src_img[IMAGE_NUM];
	vector<cv::Mat> srcImages;
	cv::Size pattern_size = cv::Size2i(PAT_COL, PAT_ROW);
	vector<cv::Point2f> corners;
	vector<vector<cv::Point2f>> imgPoints;
	int foundNum = 0;
	cv::Mat frame; //取得したフレーム
	// カメラ内部パラメータ行列(うまく読み込めないのでべたうち)
	cv::Mat camMat = (cv::Mat_<double>(3, 3) << 7.8796863296351978e+02, 0., 3.0976603626934264e+02, 0.,
		7.8512474403498004e+02, 2.4196530938772287e+02, 0., 0., 1.);
	// 歪み係数(うまくよみこめないのでべたうち)
	cv::Mat distCoefs = (cv::Mat_<double>(1, 5) << -1.8052503745524187e-01, 1.9934271045586345e+00,
		-4.8211617866061474e-03, 5.3468712500454960e-03,
		-9.3317994274449774e+00);
	vector<cv::Mat> rvecs, tvecs; // 各ビューの回転ベクトルと並進ベクトル



	// (2)3次元空間座標の設定(チェスボードの間隔が既知なので)

	vector<cv::Point3f> object;
	for (j = 0; j < PAT_ROW; j++)
	{
		for (k = 0; k < PAT_COL; k++)
		{
			cv::Point3f p(
				j * CHESS_SIZE,
				k * CHESS_SIZE,
				0.0f);
			object.push_back(p);
		}
	}

	// とる画像分用意
	vector<vector<cv::Point3f>> objPoints;
	for (i = 0; i < IMAGE_NUM; i++)
	{
		objPoints.push_back(object);
	}


	// (3)チェスボード（キャリブレーションパターン）のコーナー検出

	cv::VideoCapture cap(0);//デバイスのオープン
	//cap.open(0);//こっちでも良い．

	if (!cap.isOpened())//カメラデバイスが正常にオープンしたか確認．
	{
		//読み込みに失敗したときの処理
		return -1;
	}

	//cv::FileStorage fs2("a.xml", cv::FileStorage::READ);
	//if (!fs2.isOpened()) {
	//	cout << "File can not be opened." << endl;
	//	
	//	// 規定枚数とるまでループ
	//	while (foundNum < IMAGE_NUM) {
	//		cap.read(frame);
	//		cv::imshow("win", frame);//画像を表示．


	//		auto found = cv::findChessboardCorners(frame, pattern_size, corners);
	//		if (found)
	//		{
	//			cout << "detected corner:" << ++foundNum << endl;
	//			srcImages.push_back(frame);
	//			// (4)コーナー位置をサブピクセル精度に修正，描画
	//			cv::Mat src_gray = cv::Mat(srcImages[foundNum - 1].size(), CV_8UC1);
	//			cv::cvtColor(srcImages[foundNum - 1], src_gray, cv::COLOR_BGR2GRAY);
	//			cv::find4QuadCornerSubpix(src_gray, corners, cv::Size(3, 3));
	//			cv::drawChessboardCorners(srcImages[foundNum - 1], pattern_size, corners, found);
	//			imgPoints.push_back(corners);

	//		}



	//		const int key = cv::waitKey(1);
	//		if (key == 'q'/*113*/)//qボタンが押されたとき
	//		{
	//			break;//whileループから抜ける．
	//		}
	//		else if (key == 's'/*115*/)//sが押されたとき
	//		{
	//			//フレーム画像を保存する．
	//			cv::imwrite("img.png", frame);
	//		}
	//	}
	//	cv::destroyAllWindows();// ウィンドウを閉じる．


	//	// (5)内部パラメータ，歪み係数の推定

	//	cv::calibrateCamera(
	//		objPoints,
	//		imgPoints,
	//		srcImages[0].size(),
	//		camMat,
	//		distCoefs,
	//		rvecs,
	//		tvecs
	//	);

	//	//書き込みでXMLファイルを開く
	//	cv::FileStorage fs("parameter.xml", cv::FileStorage::WRITE);
	//	if (!fs.isOpened()) {
	//		cout << "File can not be opened." << endl;
	//		return -1;
	//	}

	//	string camMatName = "camMat";
	//	string distCoefsName = "distCoefs";

	//	fs << camMatName << camMat << distCoefsName << distCoefs;

	//	//書き出し
	//	fs.release();

	//}
	//else {
	//	fs2["camMat"] >> camMat;
	//	fs2["distCoefs"] >> distCoefs;

	//	//書き出し
	//	fs2.release();
	//}
	//
	//



	//////////////姿勢を推定して描画////////////////////

	// GLFWを初期化する
	if (glfwInit() == GL_FALSE) {
		// 初期化に失敗
		std::cerr << "Can't initialize GLFW" << std::endl;
		return 1;
	}

	// プログラム終了時の処理を登録する
	atexit(glfwTerminate);

	// バージョン指定的なやつ・ここではopenGL3.3を選択
	//glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// ウィンドウを作成する
	Window window;

	// プログラムオブジェクトを作成する
	const GLuint program(loadProgram("point.vert", "point.frag"));

	// uniform 変数の場所を取得する
	const GLint mvpLoc(glGetUniformLocation(program, "MVP"));

	//// 図形データを作成する
	//std::unique_ptr<const Shape> shape(new Shape(2, 4, rectangleVertex));

	// 図形データを作成する
	std::unique_ptr<const Shape> shape(new Shape(3, 36, g_vertex_buffer_data));


	// 背景色を指定する
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);


	cv::Mat rvec; // 各ビューの回転ベクトル
	cv::Mat tvec = cv::Mat::zeros(3,1,CV_64FC1); // 各ビューの並進ベクトル
	cv::Mat rmat = cv::Mat::eye(3, 3, CV_64FC1);
	float FoV = 45.0f;
	double maxLocVal = 1.0;
	double moveScale = 10.0;
	//　チェスボードが映る大きさ的に近づけられる限界
	double shortestDist = 0;// -200くらい
	// 投影行列
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 1000.0f);
	/////////////////////////////// 描画ループ /////////////////////////

	while (cap.read(frame) && window) {
		//
		//取得したフレーム画像に対して，グレースケール変換や2値化などの処理を書き込む．
		//


		auto found = cv::findChessboardCorners(frame, pattern_size, corners);
		if (found)
		{
			// (4)コーナー位置をサブピクセル精度に修正，描画
			cv::Mat src_gray = cv::Mat(frame.size(), CV_8UC1);
			// グレースケール画像へ
			cv::cvtColor(frame, src_gray, cv::COLOR_BGR2GRAY);
			cv::find4QuadCornerSubpix(src_gray, corners, cv::Size(3, 3));
			cv::drawChessboardCorners(frame, pattern_size, corners, found);
			//PnP問題を解く(回転ベクトルと並進ベクトルを求める)
			cv::solvePnP(object, corners, camMat, distCoefs, rvec, tvec);
			// 回転ベクトル→回転行列
			cv::Rodrigues(rvec, rmat);


			cv::minMaxLoc(tvec, NULL, &maxLocVal);
			maxLocVal /= moveScale;
			maxLocVal = 1.0;
		}

		cv::imshow("win", frame);//画像を表示．

		const int key = cv::waitKey(1);
		if (key == 'q'/*113*/)//qボタンが押されたとき
		{
			break;//whileループから抜ける．
		}
		else if (key == 's'/*115*/)//sが押されたとき
		{
			//フレーム画像を保存する．
			cv::imwrite("img.png", frame);
		}



		// ウィンドウを消去する(GL)
		glClear(GL_COLOR_BUFFER_BIT);

		// シェーダプログラムの使用開始
		glUseProgram(program);



		// MVP(model view projection)行列の作成
		// モデル行列(ワールド座標系と同じなので単位行列)
		glm::mat4 modelMatrix = glm::mat4(1.0);

		//// ビュー行列(ここにsolvePnPで得た回転行列を用いる)…座標系が左手系と右手系なことに注意！！！！！！
		glm::mat4 viewMatrix = glm::mat4((float)(rmat.at<double>(0,0)), (float)(-1*(rmat.at<double>(1, 0))), (float)(rmat.at<double>(2, 0)), 0.f,
			(float)(rmat.at<double>(0, 1)), (float)(-1*(rmat.at<double>(1, 1))), (float)(-1*(rmat.at<double>(2, 1))), 0.f,
			(float)(rmat.at<double>(0, 2)), (float)(-1*(rmat.at<double>(1, 2))), (float)(-1*(rmat.at<double>(2, 2))), 0.f,
			(float)(tvec.at<double>(0, 0)/maxLocVal), (float)(-1*(tvec.at<double>(1, 0)/maxLocVal)), (float)(-1*((tvec.at<double>(2, 0) + shortestDist) /maxLocVal)), 1.f);

		/*glm::mat4 viewMatrix = glm::lookAt(glm::vec3(3, 3, 3), glm::vec3(-3, -3, -3), glm::vec3(0, 1.0, 0));*/		

		glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;


		// uniform 変数に値を設定する
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvpMatrix[0][0]);


		//
		// ここで描画処理を行う
		//
		// 図形を描画する
		shape->draw();


		// カラーバッファを入れ替える
		window.swapBuffers();

	}
	cv::destroyAllWindows();// ウィンドウを閉じる


	return 0;

}