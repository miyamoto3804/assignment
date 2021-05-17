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
#include <opencv2/highgui/highgui.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"
#include "Shape.h"
#include "Object.h"
#include "GLUtilize.h"


#define IMAGE_NUM  (20)         /* 画像数 */
#define PAT_ROW    (6)          /* パターンの行数 */
#define PAT_COL    (7)         /* パターンの列数 */
#define PAT_SIZE   (PAT_ROW*PAT_COL)
#define ALL_POINTS (IMAGE_NUM*PAT_SIZE)
#define CHESS_SIZE (20.0)       /* パターン1マスの1辺サイズ[mm] */

#define ENABLE_CALIBRATION (0)

// OpenCV-4.5.2

using namespace std;

/////////////////////////// 描画用データ//////////////////////////

// 原点に一頂点のある40.0mm四方の立方体
constexpr Object::Vertex g_vertex_buffer_data[] = {
		{0,0,0 ,0.583f,  0.771f,  0.014f},
		{0,0, 40,0.583f,  0.771f,  0.014f},
		{0, 40, 40,0.583f,  0.771f,  0.014f},
		{40, 40,0,0.822f,  0.569f,  0.201f },
		{0,0,0 ,0.822f,  0.569f,  0.201f},
		{0, 40,0,0.822f,  0.569f,  0.201f},
		{40,0, 40,0.597f,  0.770f,  0.761f},
		{0,0,0 , 0.597f,  0.770f,  0.761f},
		{40,0,0,0.597f,  0.770f,  0.761f},
		{ 40, 40,0,0.483f,  0.596f,  0.789f},
		{40,0,0 ,0.483f,  0.596f,  0.789f},
		{0,0,0,0.483f,  0.596f,  0.789f},
		{0,0,0,0.771f,  0.328f,  0.970f },
		{0, 40, 40,0.771f,  0.328f,  0.970f},
		{0, 40,0,0.771f,  0.328f,  0.970f},
		{ 40,0, 40,0.971f,  0.572f,  0.833f},
		{0,0, 40,0.971f,  0.572f,  0.833f},
		{0,0,0,0.971f,  0.572f,  0.833f},
		{0, 40, 40,0.945f,  0.719f,  0.592f},
		{0,0, 40,0.945f,  0.719f,  0.592f},
		{ 40,0, 40,0.945f,  0.719f,  0.592f},
		{ 40, 40, 40,0.167f,  0.620f,  0.077f},
		{ 40,0,0,0.167f,  0.620f,  0.077f},
		{40, 40,0,0.167f,  0.620f,  0.077f},
		{ 40,0,0,0.714f,  0.505f,  0.345f},
		{ 40, 40, 40,0.714f,  0.505f,  0.345f},
		{ 40,0, 40,0.714f,  0.505f,  0.345f},
		{ 40, 40, 40,0.302f,  0.455f,  0.848f},
		{ 40, 40,0,0.302f,  0.455f,  0.848f},
		{0, 40,0,0.302f,  0.455f,  0.848f},
		{ 40, 40, 40,0.053f,  0.959f,  0.120f},
		{0, 40,0,0.053f,  0.959f,  0.120f},
		{0, 40, 40,0.053f,  0.959f,  0.120f},
		{ 40, 40, 40,0.820f,  0.883f,  0.371f},
		{0, 40, 40,0.820f,  0.883f,  0.371f},
		{ 40,0, 40,0.820f,  0.883f,  0.371f}
};


////////////////////////// main ///////////////////////////////


int main() {


	///////////////// カメラで検出(openCV) //////////////////////////

	//Releaseにおいてうまく結果が保存できない問題発生中

	int i, j, k;
	int cornerCount, found;
	int p_count[IMAGE_NUM];
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

	cv::Mat rvec; // 各ビューの回転ベクトル
	cv::Mat tvec = cv::Mat::zeros(3, 1, CV_64FC1); // 各ビューの並進ベクトル
	cv::Mat rmat = cv::Mat::eye(3, 3, CV_64FC1);

	glm::mat4 projectionMatrix;

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

	
	// (3)チェスボード（キャリブレーションパターン）のコーナー検出

	cv::VideoCapture cap(0);//デバイスのオープン
	cap.read(frame);
	int camWidth = frame.cols;
	int camHeight = frame.rows;
	//cap.open(0);//こっちでも良い．

	if (!cap.isOpened())//カメラデバイスが正常にオープンしたか確認．
	{
		//読み込みに失敗したときの処理
		return -1;
	}

#if ENABLE_CALIBRATION
	// とる画像分用意
	vector<vector<cv::Point3f>> objPoints;
	for (i = 0; i < IMAGE_NUM; i++)
	{
		objPoints.push_back(object);
	}



	cv::FileStorage fs2("parameter.xml", cv::FileStorage::READ);
	if (!fs2.isOpened()) {
		cout << "File can not be opened." << endl;
		
		// 規定枚数とるまでループ
		while (foundNum < IMAGE_NUM) {
			cap.read(frame);
			cv::imshow("win", frame);//画像を表示．


			auto found = cv::findChessboardCorners(frame, pattern_size, corners);
			if (found)
			{
				cout << "detected corner:" << ++foundNum << endl;
				srcImages.push_back(frame);
				// (4)コーナー位置をサブピクセル精度に修正，描画
				cv::Mat src_gray = cv::Mat(srcImages[foundNum - 1].size(), CV_8UC1);
				cv::cvtColor(srcImages[foundNum - 1], src_gray, cv::COLOR_BGR2GRAY);
				cv::find4QuadCornerSubpix(src_gray, corners, cv::Size(3, 3));
				cv::drawChessboardCorners(srcImages[foundNum - 1], pattern_size, corners, found);
				imgPoints.push_back(corners);

			}



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
		}
		cv::destroyAllWindows();// ウィンドウを閉じる．


		// (5)内部パラメータ，歪み係数の推定

		cv::calibrateCamera(
			objPoints,
			imgPoints,
			srcImages[0].size(),
			camMat,
			distCoefs,
			rvecs,
			tvecs
		);

		//書き込みでXMLファイルを開く
		cv::FileStorage fs("parameter.xml", cv::FileStorage::WRITE);
		if (!fs.isOpened()) {
			cout << "File can not be opened." << endl;
			return -1;
		}

		string camMatName = "camMat";
		string distCoefsName = "distCoefs";

		fs << camMatName << camMat << distCoefsName << distCoefs;

		//書き出し
		fs.release();

	}
	else {
		fs2["camMat"] >> camMat;
		fs2["distCoefs"] >> distCoefs;

		//書き出し
		fs2.release();
	}
	
	

#endif

	//////////////姿勢を推定して描画////////////////////

	//背景テクスチャ貼り付け用の正方形	
	static const GLfloat position[][3] =
	{
	  { -1.0f, -1.0f,0.999999f},
	  {  1.0f, -1.0f,0.999999f},
	  {  1.0f,  1.0f,0.999999f},
	  { -1.0f,  1.0f,0.999999f}
	};

	//頂点数
	static const int vertices(sizeof position / sizeof position[0]);


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
	Window window(camWidth,camHeight);

	glClearDepth(1.0);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// テクスチャを準備する
	GLuint image;
	glGenTextures(1, &image);
	glBindTexture(GL_TEXTURE_RECTANGLE, image);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, camWidth, camHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	

	// プログラムオブジェクトを作成する
	const GLuint program(loadProgram("point.vert", "point.frag"));

	//　背景のプログラムオブジェクト
	const GLuint bgprogram(loadProgram("background.vert", "background.frag"));

	// uniform 変数の場所を取得する
	const GLint mvpLoc(glGetUniformLocation(program, "MVP"));
	const GLint imgLoc(glGetUniformLocation(bgprogram, "image"));

	// 図形データを作成する
	std::unique_ptr<const Shape> shape(new Shape(3, 36, g_vertex_buffer_data));

	// 背景色を指定する
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//背景画像の設定部
	// 頂点配列オブジェクト
	GLuint vao;

	// 頂点バッファオブジェクト
	GLuint vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof position, position, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Bind our texture in Texture Unit 0
	glGenTextures(1, &image);
	glBindTexture(GL_TEXTURE_RECTANGLE, image);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, camWidth, camHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glUniform1i(imgLoc, 0);

	
	//キャリブレーションパラメータから透視投影行列を計算
	cameraFrustumRH(camMat, cv::Size(camWidth, camHeight), projectionMatrix, 1.0f, 1000.0f);

	/////////////////////////////// 描画ループ /////////////////////////

	while (cap.read(frame) && window) {

		// ウィンドウを消去する(GL)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//　チェスボードのコーナー探索

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

			// シェーダプログラムの使用開始
			glUseProgram(program);

			// MVP(model view projection)行列の作成
			// モデル行列(ワールド座標系と同じなので単位行列)
			glm::mat4 modelMatrix = glm::mat4(1.0);

			//// ビュー行列(ここにsolvePnPで得た回転行列を用いる)…座標系がX軸に対して180°回転していることに注意！！！！！！
			glm::mat4 viewMatrix = glm::mat4((float)(rmat.at<double>(0, 0)), (float)(-1 * (rmat.at<double>(1, 0))), (float)(rmat.at<double>(2, 0)), 0.f,
				(float)(rmat.at<double>(0, 1)), (float)(-1 * (rmat.at<double>(1, 1))), (float)(-1 * (rmat.at<double>(2, 1))), 0.f,
				(float)(rmat.at<double>(0, 2)), (float)(-1 * (rmat.at<double>(1, 2))), (float)(-1 * (rmat.at<double>(2, 2))), 0.f,
				(float)tvec.at<double>(0, 0), (float)(-1 * tvec.at<double>(1, 0)), (float)(-1 * tvec.at<double>(2, 0)), 1.f);

			//glm::mat4 viewMatrix = glm::lookAt(glm::vec3(100, 100, 100), glm::vec3(-100, -100, -100), glm::vec3(0, 1.0, 0));

			glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

			//glm::mat4 mvpMatrix = glm::mat4(1.0);

			// uniform 変数に値を設定する
			glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvpMatrix[0][0]);

			// 図形を描画する
			shape->draw();
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


		// Bが押されたか確認
		// 背景の描画の有無が切り替わる
		if (window.getBFlag()) {


			cv::flip(frame, frame, 0);

			// 背景用シェーダプログラム
			glUseProgram(bgprogram);

			glBindVertexArray(vao);
			// 切り出した画像をテクスチャに転送する
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_RECTANGLE, image);
			glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);

			// 背景の描画
			glDrawArrays(GL_TRIANGLE_FAN, 0, vertices);

			// 頂点配列オブジェクトの指定解除
			glBindVertexArray(0);
		}

		

		// カラーバッファを入れ替える
		window.swapBuffers();

	}
	cv::destroyAllWindows();// ウィンドウを閉じる

	return 0;

}



