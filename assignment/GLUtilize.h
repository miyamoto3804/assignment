#pragma once

#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <opencv2/opencv.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>



/////////////////////////// �֐� ////////////////////////////////////

// �V�F�[�_�I�u�W�F�N�g�̃R���p�C�����ʂ�\������
// shader: �V�F�[�_�I�u�W�F�N�g��
// str: �R���p�C���G���[�����������ꏊ������������
GLboolean printShaderInfoLog(GLuint shader, const char *str);

// �v���O�����I�u�W�F�N�g�̃����N���ʂ�\������
// program: �v���O�����I�u�W�F�N�g��
GLboolean printProgramInfoLog(GLuint program);

// �v���O�����I�u�W�F�N�g���쐬����
// vsrc: �o�[�e�b�N�X�V�F�[�_�̃\�[�X�v���O�����̕�����
// fsrc: �t���O�����g�V�F�[�_�̃\�[�X�v���O�����̕�����
GLuint createProgram(const char *vsrc, const char *fsrc);


// �V�F�[�_�̃\�[�X�t�@�C����ǂݍ��񂾃�������Ԃ�
// name: �V�F�[�_�̃\�[�X�t�@�C����
// buffer: �ǂݍ��񂾃\�[�X�t�@�C���̃e�L�X�g
bool readShaderSource(const char *name, std::vector<GLchar> &buffer);

// �V�F�[�_�̃\�[�X�t�@�C����ǂݍ���Ńv���O�����I�u�W�F�N�g���쐬����
// vert: �o�[�e�b�N�X�V�F�[�_�̃\�[�X�t�@�C����
// frag: �t���O�����g�V�F�[�_�̃\�[�X�t�@�C����
GLuint loadProgram(const char *vert, const char *frag);

//  OpenCV�J�����p�����[�^����OpenGL(GLM)�v���W�F�N�V�����s��𓾂�֐�
void cameraFrustumRH(cv::Mat camMat, cv::Size camSz, glm::mat4 &projMat, double znear, double zfar);

void saveImage(const unsigned int imageWidth, const unsigned int imageHeight);