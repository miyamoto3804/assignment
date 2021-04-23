#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


// �E�B���h�E�֘A�̏���
class Window
{
	// �E�B���h�E�̃n���h��
	GLFWwindow *const window;

	// �c����
	GLfloat aspect;

	// �E�B���h�E�̃T�C�Y
	GLfloat size[2];
	// ���[���h���W�n�ɑ΂���f�o�C�X���W�n�̊g�嗦
	GLfloat scale;

	int keyStatus;

public:
	// �R���X�g���N�^(�F�̌��̓����o�C�j�V�����C�U���Ă��)
	Window(int width = 1024, int height = 768, const char *title = "Hello!")
		: window(glfwCreateWindow(width, height, title, NULL, NULL))
		, scale(100.f), keyStatus(GLFW_RELEASE)
	{
		if (window == NULL)
		{
			// �E�B���h�E���쐬�ł��Ȃ�����
			std::cerr << "Can't create GLFW window." << std::endl;
			exit(1);
		}
		// ���݂̃E�B���h�E�������Ώۂɂ���
		glfwMakeContextCurrent(window);
		// GLEW ������������
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			// GLEW �̏������Ɏ��s����
			std::cerr << "Can't initialize GLEW" << std::endl;
			exit(1);
		}
		// ���������̃^�C�~���O��҂�
		glfwSwapInterval(1);

		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);

		// ���̃C���X�^���X�� this �|�C���^���L�^���Ă���
		glfwSetWindowUserPointer(window, this);

		// �E�B���h�E�̃T�C�Y�ύX���ɌĂяo�������̓o�^
		glfwSetWindowSizeCallback(window, resize);

		// �L�[�{�[�h���쎞�ɌĂяo���֐��ݒ�
		glfwSetKeyCallback(window, keyboard);

		// �J�����E�B���h�E�̏����ݒ�
		resize(window, width, height);

		
	}

	// �f�X�g���N�^
	virtual ~Window()
	{
		glfwDestroyWindow(window);
	}
	
	// �`�惋�[�v�̌p������
	explicit operator bool()
	{
		// �C�x���g�����o��(�����Ă͂Ȃ����Ƃ��̂ݍ쓮)
		//glfwWaitEvents();

		// �C�x���g�����o��(�����Ă邠�����쓮)
		glfwPollEvents();

		// �E�B���h�E�����K�v���Ȃ���� true ��Ԃ�
		return !glfwWindowShouldClose(window)&& !glfwGetKey(window, GLFW_KEY_ESCAPE) ;
	}
	// �_�u���o�b�t�@�����O
	void swapBuffers() const
	{
		// �J���[�o�b�t�@�����ւ���
		glfwSwapBuffers(window);
	}

	// �E�B���h�E�̃T�C�Y�ύX���̏���
	static void resize(GLFWwindow *const window, int width, int height)
	{
		// �t���[���o�b�t�@�̃T�C�Y�𒲂ׂ�
		int fbWidth, fbHeight;
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		// �t���[���o�b�t�@�S�̂��r���[�|�[�g�ɐݒ肷��
		glViewport(0, 0, fbWidth, fbHeight);

		// ���̃C���X�^���X�� this �|�C���^�𓾂�
		Window *const
			instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));
		if (instance != NULL)
		{
			// �J�����E�B���h�E�̃T�C�Y��ۑ�����
			instance->size[0] = static_cast<GLfloat>(width);
			instance->size[1] = static_cast<GLfloat>(height);
		}
	}

	// �L�[�{�[�h�������ꂽ�Ƃ��̂��߂̃R�[���o�b�N�֐�
	//(������glfwSetKeyCallback�ł����Ȃ�悤���܂��Ă�)
	static void keyboard(GLFWwindow *window, int key, int scancode, int action, int mods) {
		
		//���̃C���X�^���X��this�|�C���^�𓾂�(�ÓI�֐������炱������ă|�C���^���Ƃ�Ȃ���_��)
		Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

		if (instance != NULL) {
			// �L�[�̏�Ԃ�ۑ�
			instance->keyStatus = action;
		}
	}

	// �E�B���h�E�̃T�C�Y�����o��
	const GLfloat *getSize() const { return size; }

	// �c��������o��
	GLfloat getScale() const { return scale; }
};
