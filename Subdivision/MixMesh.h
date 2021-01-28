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

// 存在三角形和四边形混合的网格
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

	std::vector<HalfEdge*> halfEdges_;	// 半边结构

	int num_vtx_{ 0 };	// 顶点的数量
	int num_tri_{ 0 };	// 三角形的数量
	int num_quad_{ 0 };	// 四边形的数量

	std::vector<Vec3f> vtxs_;	// 存储顶点
	std::vector<Tri> tris_;		// 存储三角形
	std::vector<Quad> quads_;	// 存储四边形

	QOpenGLBuffer vbo_, ebo_tri_, ebo_quad_;	// 顶点缓冲对象和索引缓冲对象
	QOpenGLShaderProgram shader_program_;	// 着色器

	REAL scale_factor_{ 1 };	// 缩放因子
	QVector3D translate_;	// 平移向量
};

