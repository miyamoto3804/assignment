#pragma once

#include <GL/glew.h>
// �}�`�f�[�^
class Object
{
	// ���_�z��I�u�W�F�N�g��
	GLuint vao;
	// ���_�o�b�t�@�I�u�W�F�N�g��
	GLuint vbo;


	//�J���[�o�b�t�@�z��
	GLuint cba;
	//�J���[�o�b�t�@���Ԃ�������
	GLuint cbo;
	

public:
	// ���_����
	struct Vertex
	{
		// �ʒu
		GLfloat position[3];

		// ����
		GLfloat color[3];
	};

	// �R���X�g���N�^
	// size: ���_�̈ʒu�̎���
	// vertexcount: ���_�̐�
	// vertex: ���_�������i�[�����z��
	Object(GLint size, GLsizei vertexcount, const Vertex *vertex)
	{
		// ���_�z��I�u�W�F�N�g
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		// ���_�o�b�t�@�I�u�W�F�N�g
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER,
			vertexcount * sizeof(Vertex), vertex, GL_STATIC_DRAW);
		// ��������Ă��钸�_�o�b�t�@�I�u�W�F�N�g�� in �ϐ�����Q�Ƃł���悤�ɂ���
		glVertexAttribPointer(0, size, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<Vertex *>(0)->position);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			static_cast<Vertex *>(0)->color);
		glEnableVertexAttribArray(1);

		////�F�ݒ�
		//glGenBuffers(1, &cbo);
		//glBindBuffer(GL_ARRAY_BUFFER, cbo);
		////�o�b�t�@�ɐF����o�^
		//glBufferData(GL_ARRAY_BUFFER, vertexcount*sizeof(Color), color, GL_STATIC_DRAW);
		//// 2nd attribute buffer : colors
		////�o�b�t�@�ɐF�f�[�^��o�^
		//glEnableVertexAttribArray(1);
		//glBindBuffer(GL_ARRAY_BUFFER, cbo);
		//glVertexAttribPointer(
		//	1,                                // attribute. No particular reason for 1, but must match the layout in the shader.�����l
		//	3,                                // size
		//	GL_FLOAT,                         // type
		//	GL_FALSE,                         // normalized?
		//	0,                                // stride
		//	(void*)0                          // array buffer offset
		//);


	}

	// �f�X�g���N�^
	virtual ~Object()
	{
		// ���_�z��I�u�W�F�N�g���폜����
		glDeleteVertexArrays(1, &vao);
		// ���_�o�b�t�@�I�u�W�F�N�g���폜����
		glDeleteBuffers(1, &vbo);
	}

private:
	// �R�s�[�R���X�g���N�^�ɂ��R�s�[�֎~
	Object(const Object &o);
	// ����ɂ��R�s�[�֎~
	Object &operator=(const Object &o);

public:
	// ���_�z��I�u�W�F�N�g�̌���
	void bind() const
	{
		// �`�悷�钸�_�z��I�u�W�F�N�g���w�肷��
		glBindVertexArray(vao);
	}
};