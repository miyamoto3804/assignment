#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


// ウィンドウ関連の処理
class Window
{
	// ウィンドウのハンドル
	GLFWwindow *const window;

	// 縦横比
	GLfloat aspect;

	// ウィンドウのサイズ
	GLfloat size[2];
	// ワールド座標系に対するデバイス座標系の拡大率
	GLfloat scale;

	int keyStatus;

public:
	// コンストラクタ(：の後ろはメンバイニシャライザってやつ)
	Window(int width = 1024, int height = 768, const char *title = "Hello!")
		: window(glfwCreateWindow(width, height, title, NULL, NULL))
		, scale(100.f), keyStatus(GLFW_RELEASE)
	{
		if (window == NULL)
		{
			// ウィンドウが作成できなかった
			std::cerr << "Can't create GLFW window." << std::endl;
			exit(1);
		}
		// 現在のウィンドウを処理対象にする
		glfwMakeContextCurrent(window);
		// GLEW を初期化する
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			// GLEW の初期化に失敗した
			std::cerr << "Can't initialize GLEW" << std::endl;
			exit(1);
		}
		// 垂直同期のタイミングを待つ
		glfwSwapInterval(1);

		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);

		// このインスタンスの this ポインタを記録しておく
		glfwSetWindowUserPointer(window, this);

		// ウィンドウのサイズ変更時に呼び出す処理の登録
		glfwSetWindowSizeCallback(window, resize);

		// キーボード操作時に呼び出す関数設定
		glfwSetKeyCallback(window, keyboard);

		// 開いたウィンドウの初期設定
		resize(window, width, height);

		
	}

	// デストラクタ
	virtual ~Window()
	{
		glfwDestroyWindow(window);
	}
	
	// 描画ループの継続判定
	explicit operator bool()
	{
		// イベントを取り出す(おしてはなしたときのみ作動)
		//glfwWaitEvents();

		// イベントを取り出す(おしてるあいだ作動)
		glfwPollEvents();

		// ウィンドウを閉じる必要がなければ true を返す
		return !glfwWindowShouldClose(window)&& !glfwGetKey(window, GLFW_KEY_ESCAPE) ;
	}
	// ダブルバッファリング
	void swapBuffers() const
	{
		// カラーバッファを入れ替える
		glfwSwapBuffers(window);
	}

	// ウィンドウのサイズ変更時の処理
	static void resize(GLFWwindow *const window, int width, int height)
	{
		// フレームバッファのサイズを調べる
		int fbWidth, fbHeight;
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		// フレームバッファ全体をビューポートに設定する
		glViewport(0, 0, fbWidth, fbHeight);

		// このインスタンスの this ポインタを得る
		Window *const
			instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));
		if (instance != NULL)
		{
			// 開いたウィンドウのサイズを保存する
			instance->size[0] = static_cast<GLfloat>(width);
			instance->size[1] = static_cast<GLfloat>(height);
		}
	}

	// キーボードが押されたときのためのコールバック関数
	//(引数はglfwSetKeyCallbackでこうなるよう決まってる)
	static void keyboard(GLFWwindow *window, int key, int scancode, int action, int mods) {
		
		//このインスタンスのthisポインタを得る(静的関数だからこうやってポインタをとんなきゃダメ)
		Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

		if (instance != NULL) {
			// キーの状態を保存
			instance->keyStatus = action;
		}
	}

	// ウィンドウのサイズを取り出す
	const GLfloat *getSize() const { return size; }

	// 縦横比を取り出す
	GLfloat getScale() const { return scale; }
};
