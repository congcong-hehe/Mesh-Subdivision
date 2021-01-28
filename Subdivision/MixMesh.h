#pragma once
#include <string>
#include <QDebug>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShader>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <limits.h>
#include <algorithm>
#include "Vec3f.h"
#include "Quad.h"
#include "Tri.h"
#include <vector>

const float PIa = cos(-1);

// ���������κ��ı��λ�ϵ�����
class MixMesh
{
public:
	MixMesh();
	~MixMesh();

	int loadFile(std::string file_name);
	void saveFile(std::string file_name);
	void setShader();
	void initBO();
	void drawMesh(QOpenGLFunctions_3_3_Core* ff, QMatrix4x4& projection, QMatrix4x4& model);
	void destroyGL();
	bool isEmpty();
	void buildHalfEdge();
	int getVerticesNum();
	int getFaceNum();
	void findNeighborVertex(int vid, std::vector<unsigned int>& vertexs);

	void clearHalfEdge();
	void buildTriHalfEdge();
	void buildQuadHalfEdge();

	void dooSabinSubdivision();
	void loopSubdivision();
	void catmullClarkSubdivision();

	std::vector<HalfEdge*> halfEdges_;	// ��߽ṹ

	int num_vtx_{ 0 };	// ���������
	int num_tri_{ 0 };	// �����ε�����
	int num_quad_{ 0 };	// �ı��ε�����

	std::vector<Vec3f> vtxs_;	// �洢����
	std::vector<Tri> tris_;		// �洢������
	std::vector<Quad> quads_;	// �洢�ı���

	QOpenGLBuffer vbo_, ebo_tri_, ebo_quad_;	// ���㻺�����������������
	QOpenGLShaderProgram shader_program_;	// ��ɫ��

	REAL scale_factor_{ 1 };	// ��������
	QVector3D translate_;	// ƽ������
};

