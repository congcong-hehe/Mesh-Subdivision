#include "MixMesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <QOpenGLWidget>
#include "PaintStructure/PaintQuad.h"
#include "PaintStructure/PaintTri.h"
#include "PaintStructure/PaintVec3f.h"
#include <unordered_map>
#include <unordered_set>

MixMesh::MixMesh() : vbo_(QOpenGLBuffer::VertexBuffer),
ebo_tri_(QOpenGLBuffer::IndexBuffer), ebo_quad_(QOpenGLBuffer::IndexBuffer)
{

}

MixMesh::~MixMesh()
{

}

void MixMesh::setShader()
{
	//  顶点着色器
	bool success = shader_program_.addShaderFromSourceFile(QOpenGLShader::Vertex, "GLSL/vert.glsl");
	if (!success)
	{
		qDebug() << "shaderProgram addShaderFromSourceFile failed!" << shader_program_.log();
		return;
	}

	// 片段着色器
	success = shader_program_.addShaderFromSourceFile(QOpenGLShader::Fragment, "GLSL/frag.glsl");
	if (!success)
	{
		qDebug() << "shaderProgram addShaderFromSourceFile failed!" << shader_program_.log();
		return;
	}

	// 链接着色器
	success = shader_program_.link();
	if (!success)
	{
		qDebug() << "shaderProgram link failed!" << shader_program_.log();
		return;
	}
}

void MixMesh::initBO()
{
	std::vector<PaintTri> paint_tris(num_tri_);
	std::vector<PaintVec3f> paint_vtxs(num_vtx_);
	std::vector<PaintQuad> paint_quads(num_quad_);
	for (int i = 0; i < num_tri_; ++i)
	{
		unsigned int* v = tris_[i].v;
		paint_tris[i] = PaintTri(v[0], v[1], v[2]);
	}
	for (int i = 0; i < num_vtx_; ++i)
	{
		Vec3f& vec3f = vtxs_[i];
		paint_vtxs[i] = PaintVec3f(vec3f.x, vec3f.y, vec3f.z);
	}
	for (int i = 0; i < num_quad_; ++i)
	{
		unsigned int* v = quads_[i].v;
		paint_quads[i] = PaintQuad(v[0], v[1], v[2], v[3]);
	}

	// 创建并分配vbo对象
	vbo_.create();
	vbo_.bind();
	vbo_.allocate(&paint_vtxs[0], paint_vtxs.size() * sizeof(PaintVec3f));

	// 创建并分配ebo对象
	if (num_tri_ != 0)
	{
		ebo_tri_.create();
		ebo_tri_.bind();
		ebo_tri_.allocate(&paint_tris[0], paint_tris.size() * sizeof(PaintTri));
		ebo_tri_.release();
	}
	if (num_quad_ != 0)
	{
		ebo_quad_.create();
		ebo_quad_.bind();
		ebo_quad_.allocate(&paint_quads[0], paint_quads.size() * sizeof(PaintQuad));
		ebo_quad_.release();
	}
	

	int attr = -1;
	attr = shader_program_.attributeLocation("aPos");
	shader_program_.setAttributeBuffer(attr, GL_FLOAT, 0, 3, sizeof(PaintVec3f));	// 设置解析数据的方式
	shader_program_.enableAttributeArray(attr);

	vbo_.release();

	shader_program_.bind();
	// 视图view变换矩阵
	QMatrix4x4 view;
	view.lookAt(QVector3D(0.0f, 0.0f, 4.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
	shader_program_.setUniformValue("view", view);

}

void MixMesh::drawMesh(QOpenGLFunctions_3_3_Core* ff, QMatrix4x4& projection, QMatrix4x4& model)
{
	model.translate(translate_);		// 平移模型到原点
	model.scale(1.0f / scale_factor_ * 2.0f);
	shader_program_.bind();
	shader_program_.setUniformValue("projection", projection);
	shader_program_.setUniformValue("model", model);

	if (num_tri_ != 0)
	{
		ebo_tri_.bind();
		ff->glDrawElements(GL_TRIANGLES, tris_.size() * 3, GL_UNSIGNED_INT, 0);		// 绘制
		ebo_tri_.release();
	}
	if (num_quad_ != 0)
	{
		ebo_quad_.bind();
		ff->glDrawElements(GL_QUADS, quads_.size() * 4, GL_UNSIGNED_INT, 0);		// 绘制
		ebo_quad_.release();
	}

	shader_program_.release();
}

void MixMesh::destroyGL()
{
	vbo_.destroy();
	ebo_tri_.destroy();
	ebo_quad_.destroy();
}

bool MixMesh::isEmpty()
{
	return num_vtx_ == 0;
}

int MixMesh::loadFile(std::string file_name)
{
	std::ifstream in(file_name);

	if (!in)
	{
		std::cout << "The file can not be opened!" << std::endl;
		return -1;
	}

	REAL x_min = FLT_MAX, x_max = -FLT_MAX;
	REAL y_min = FLT_MAX, y_max = -FLT_MAX;
	REAL z_min = FLT_MAX, z_max = -FLT_MAX;

	std::string line;
	while (std::getline(in, line))
	{
		std::string tag;
		std::istringstream is(line);

		is >> tag;
		if (tag == "v")
		{
			Vec3f vec3f;
			is >> vec3f.x >> vec3f.y >> vec3f.z;

			// 计算包围盒
			x_min = fmin(x_min, vec3f.x);
			x_max = fmax(x_max, vec3f.x);
			y_min = fmin(y_min, vec3f.y);
			y_max = fmax(y_max, vec3f.y);
			z_min = fmin(z_min, vec3f.z);
			z_max = fmax(z_max, vec3f.z);

			vtxs_.push_back(vec3f);
		}
		else if (tag == "f")
		{
			unsigned v0, v1, v2, v3;
			is >> v0 >> v1 >> v2;
			if (is >> v3)
			{
				Quad quad;
				quad.v[0] = v0 - 1;
				quad.v[1] = v1 - 1;
				quad.v[2] = v2 - 1;
				quad.v[3] = v3 - 1;
				quads_.push_back(quad);
			}
			else
			{
				Tri tri;
				tri.v[0] = v0 - 1;
				tri.v[1] = v1 - 1;
				tri.v[2] = v2 - 1;
				tris_.push_back(tri);
			}
		}
	}
	in.close();

	num_vtx_ = vtxs_.size();
	num_tri_ = tris_.size();
	num_quad_ = quads_.size();

	// 计算模型的缩放比例，保证模型的大小在窗口中合适的显示
	scale_factor_ = fmax(fmax((x_max - x_min), (y_max - y_min)), (z_max - z_min));
	// 将模型的中点平移到原点位置的平移量
	translate_ = QVector3D(-(x_max + x_min) / 2.0f, -(y_max + y_min) / 2.0f, -(y_max + y_min) / 2.0f);

	if (num_quad_ == 0 && num_tri_ != 0)
	{
		return 0;	// 三角形网格
	}
	else if(num_quad_ != 0 && num_tri_ == 0)
	{
		return 1;	// 四边形网格
	}
	else
	{
		return 2;	// 混合网格
	}
}

void MixMesh::saveFile(std::string file_name)
{
	std::ofstream out(file_name);

	out << "# created by yqHe" << "\n";

	for (int i = 0; i < num_vtx_; ++i)
	{
		out << "v" << " " << vtxs_[i].x << " " << vtxs_[i].y << " " << vtxs_[i].z << std::endl;
	}

	for (int i = 0; i < num_quad_; ++i)
	{
		out << "f" << " " << quads_[i].v[0]+1 << " " << quads_[i].v[1]+1<< " " << quads_[i].v[2]+1 << " " << quads_[i].v[3]+1 << std::endl;
	}

	for (int i = 0; i < num_tri_; ++i)
	{
		out << "f" << " " << tris_[i].v[0]+1 << " " << tris_[i].v[1]+1 << " " << tris_[i].v[2]+1 << std::endl;
	}
	out.close();
}

void MixMesh::buildTriHalfEdge()
{
	// 构建半边结构
	std::unordered_map<EdgeKey, HalfEdge*, EdgeKeyHashFuc> edgekey_map;		// 对于非流形，只需要在创建半边的时候存储对边，即若存在，则对边也存在
	for (int i = 0; i < num_tri_; ++i)
	{
		Tri& tri = tris_[i];
		unsigned int* vid = tri.v;
		HalfEdge* tri_halfEdges[3];
		for (int j = 0; j < 3; ++j)
		{
			unsigned int vid1 = vid[j % 3], vid2 = vid[(j + 1) % 3];
			if (edgekey_map.find(EdgeKey(vid1, vid2)) == edgekey_map.end())	// 如果半边没有被创建
			{
				// 创建半边和其对边
				HalfEdge* curr_hedge = new HalfEdge();
				HalfEdge* curr_oppo = new HalfEdge();

				curr_hedge->end_id = vid2;
				curr_hedge->begin_id = vid1;
				curr_hedge->oppo = curr_oppo;

				curr_oppo->end_id = vid1;
				curr_oppo->begin_id = vid2;
				curr_oppo->oppo = curr_hedge;
				edgekey_map[EdgeKey(vid2, vid1)] = curr_oppo;
				edgekey_map[EdgeKey(vid1, vid2)] = curr_hedge;

				tri_halfEdges[j] = curr_hedge;

				halfEdges_.push_back(curr_hedge);
				halfEdges_.push_back(curr_oppo);
			}
			else	// 如果半边已经被创建
			{
				tri_halfEdges[j] = edgekey_map[EdgeKey(vid1, vid2)];
			}

			if (vtxs_[vid1].halfEdge == nullptr)
			{
				vtxs_[vid1].halfEdge = tri_halfEdges[j];	// vid1开头的半边
			}
		}

		tri_halfEdges[0]->next = tri_halfEdges[1];
		tri_halfEdges[1]->next = tri_halfEdges[2];
		tri_halfEdges[2]->next = tri_halfEdges[0];

		tri.halfedge = tri_halfEdges[0];	// 保存面中的任意一条半边
	}
}

void MixMesh::clearHalfEdge()
{
	// 释放半边结构的内存，并清空半边表
	for (int i = 0; i < halfEdges_.size(); ++i)
	{
		if (halfEdges_[i] != nullptr)
		{
			delete halfEdges_[i];
		}
	}
	halfEdges_.clear();
}

void MixMesh::buildQuadHalfEdge()
{
	// 构建半边结构
	std::unordered_map<EdgeKey, HalfEdge*, EdgeKeyHashFuc> edgekey_map;		// 对于非流形，只需要在创建半边的时候存储对边，即若存在，则对边也存在
	for (int i = 0; i < num_quad_; ++i)
	{
		Quad& quad = quads_[i];
		unsigned int* vid = quad.v;
		HalfEdge* quad_halfEdges[4];
		for (int j = 0; j < 4; ++j)
		{
			unsigned int vid1 = vid[j % 4], vid2 = vid[(j + 1) % 4];
			if (edgekey_map.find(EdgeKey(vid1, vid2)) == edgekey_map.end())	// 如果半边没有被创建
			{
				// 创建半边和其对边
				HalfEdge* curr_hedge = new HalfEdge();
				HalfEdge* curr_oppo = new HalfEdge();

				curr_hedge->end_id = vid2;
				curr_hedge->begin_id = vid1;
				curr_hedge->oppo = curr_oppo;

				curr_oppo->end_id = vid1;
				curr_oppo->begin_id = vid2;
				curr_oppo->oppo = curr_hedge;
				edgekey_map[EdgeKey(vid2, vid1)] = curr_oppo;
				edgekey_map[EdgeKey(vid1, vid2)] = curr_hedge;

				quad_halfEdges[j] = curr_hedge;

				halfEdges_.push_back(curr_hedge);
				halfEdges_.push_back(curr_oppo);
			}
			else	// 如果半边已经被创建
			{
				quad_halfEdges[j] = edgekey_map[EdgeKey(vid1, vid2)];
			}

			if (vtxs_[vid1].halfEdge == nullptr)
			{
				vtxs_[vid1].halfEdge = quad_halfEdges[j];	// vid1开头的半边
			}
		}

		for (int j = 0; j < 4; ++j)
		{
			quad_halfEdges[j]->fid = i;
		}

		quad_halfEdges[0]->next = quad_halfEdges[1];
		quad_halfEdges[1]->next = quad_halfEdges[2];
		quad_halfEdges[2]->next = quad_halfEdges[3];
		quad_halfEdges[3]->next = quad_halfEdges[0];

		quad.halfedge = quad_halfEdges[0];	// 保存面中的任意一条半边
	}
}

void MixMesh::buildHalfEdge()
{
	// 构建半边结构
	std::unordered_map<EdgeKey, HalfEdge*, EdgeKeyHashFuc> edgekey_map;		// 对于非流形，只需要在创建半边的时候存储对边，即若存在，则对边也存在
	for (int i = 0; i < num_quad_; ++i)
	{
		Quad& quad = quads_[i];
		unsigned int* vid = quad.v;
		HalfEdge* quad_halfEdges[4];

		for (int j = 0; j < 4; ++j)
		{
			unsigned int vid1 = vid[j % 4], vid2 = vid[(j + 1) % 4];
			if (edgekey_map.find(EdgeKey(vid1, vid2)) == edgekey_map.end())	// 如果半边没有被创建
			{
				// 创建半边和其对边
				HalfEdge* curr_hedge = new HalfEdge();
				HalfEdge* curr_oppo = new HalfEdge();

				curr_hedge->end_id = vid2;
				curr_hedge->begin_id = vid1;
				curr_hedge->oppo = curr_oppo;

				curr_oppo->end_id = vid1;
				curr_oppo->begin_id = vid2;
				curr_oppo->oppo = curr_hedge;
				edgekey_map[EdgeKey(vid2, vid1)] = curr_oppo;
				//edgekey_map[EdgeKey(vid1, vid2)] = curr_hedge;

				quad_halfEdges[j] = curr_hedge;

				halfEdges_.push_back(curr_hedge);
				halfEdges_.push_back(curr_oppo);
			}
			else	// 如果半边已经被创建
			{
				quad_halfEdges[j] = edgekey_map[EdgeKey(vid1, vid2)];
			}

			if (vtxs_[vid1].halfEdge == nullptr)
			{
				vtxs_[vid1].halfEdge = quad_halfEdges[j];	// vid1开头的半边
			}
		}

		for (int j = 0; j < 4; ++j)
		{
			quad_halfEdges[j]->fid = i;
		}

		quad_halfEdges[0]->next = quad_halfEdges[1];
		quad_halfEdges[1]->next = quad_halfEdges[2];
		quad_halfEdges[2]->next = quad_halfEdges[3];
		quad_halfEdges[3]->next = quad_halfEdges[0];

		quad.halfedge = quad_halfEdges[0];	// 保存面中的任意一条半边
	}
	for (int i = 0; i < num_tri_; ++i)
	{
		Tri& tri = tris_[i];
		unsigned int* vid = tri.v;
		HalfEdge* tri_halfEdges[3];
		for (int j = 0; j < 3; ++j)
		{
			unsigned int vid1 = vid[j % 3], vid2 = vid[(j + 1) % 3];
			if (edgekey_map.find(EdgeKey(vid1, vid2)) == edgekey_map.end())	// 如果半边没有被创建
			{
				// 创建半边和其对边
				HalfEdge* curr_hedge = new HalfEdge();
				HalfEdge* curr_oppo = new HalfEdge();

				curr_hedge->end_id = vid2;
				curr_hedge->begin_id = vid1;
				curr_hedge->oppo = curr_oppo;

				curr_oppo->end_id = vid1;
				curr_oppo->begin_id = vid2;
				curr_oppo->oppo = curr_hedge;
				edgekey_map[EdgeKey(vid2, vid1)] = curr_oppo;
				//edgekey_map[EdgeKey(vid1, vid2)] = curr_hedge;

				tri_halfEdges[j] = curr_hedge;

				halfEdges_.push_back(curr_hedge);
				halfEdges_.push_back(curr_oppo);
			}
			else	// 如果半边已经被创建
			{
				tri_halfEdges[j] = edgekey_map[EdgeKey(vid1, vid2)];
			}

			if (vtxs_[vid1].halfEdge == nullptr)
			{
				vtxs_[vid1].halfEdge = tri_halfEdges[j];	// vid1开头的半边
			}
		}

		tri_halfEdges[0]->next = tri_halfEdges[1];
		tri_halfEdges[1]->next = tri_halfEdges[2];
		tri_halfEdges[2]->next = tri_halfEdges[0];

		tri.halfedge = tri_halfEdges[0];	// 保存面中的任意一条半边
	}
}

int MixMesh::getVerticesNum()
{
	return num_vtx_;
}

int MixMesh::getFaceNum()
{
	return num_quad_ + num_tri_;
}

void MixMesh::dooSabinSubdivision()
{
	clearHalfEdge();
	buildHalfEdge();

	std::vector<Vec3f> new_vtxs;	// 新的顶点
	std::vector<Tri> new_tris;	// 新的三角形面
	std::vector<Quad> new_quads;	// 新的四边形面

	std::unordered_map<EdgeKey, unsigned int, EdgeKeyHashFuc> edge_map; // 每一条半边和新生成的点对应

	// 计算新点并且面内的点生成新的面
	for (int i = 0; i < num_quad_; ++i)
	{
		unsigned int* v = quads_[i].v;
		
		// 面的中心点
		Vec3f center = (vtxs_[v[0]] + vtxs_[v[1]] + vtxs_[v[2]] + vtxs_[v[3]]) / 4.0f;	// 面的中心点
		
		// 四条边的中心点
		Vec3f edge_mid[4];
		HalfEdge* ori_edge = quads_[i].halfedge;
		HalfEdge* curr_edge = ori_edge;
		int count = 0;
		do
		{
			edge_mid[count++] = (vtxs_[curr_edge->begin_id] + vtxs_[curr_edge->end_id]) / 2.0f;
			curr_edge = curr_edge->next;
		} while (curr_edge != ori_edge);

		// 计算新的点并把半边和新的点对应
		unsigned int new_vid[4];
		curr_edge = ori_edge;
		for (int j = 0; j < 4; ++j)
		{
			Vec3f new_vtx = (vtxs_[v[j]] + center + edge_mid[(j + 3) % 4] + edge_mid[j]) / 4.0f;
			edge_map[EdgeKey(curr_edge->begin_id, curr_edge->end_id)] = new_vtxs.size();
			new_vid[j] = new_vtxs.size();
			new_vtxs.push_back(new_vtx);
			curr_edge = curr_edge->next;
		}

		// 连接面内的点生成新的面
		new_quads.push_back(Quad(new_vid[0], new_vid[1], new_vid[2], new_vid[3]));
	}
	for (int i = 0; i < num_tri_; ++i)
	{
		unsigned int* v = tris_[i].v;

		Vec3f center = (vtxs_[v[0]] + vtxs_[v[1]] + vtxs_[v[2]]) / 3.0f;	// 面的中心点

		// 四条边的中心点
		Vec3f edge_mid[3];
		HalfEdge* ori_edge = tris_[i].halfedge;
		HalfEdge* curr_edge = ori_edge;
		int count = 0;
		do
		{
			edge_mid[count++] = (vtxs_[curr_edge->begin_id] + vtxs_[curr_edge->end_id]) / 2.0f;
			curr_edge = curr_edge->next;
		} while (curr_edge != ori_edge);

		// 计算新的点并把半边和新的点对应
		unsigned int new_vid[3];
		curr_edge = ori_edge;
		for (int j = 0; j < 3; ++j)
		{
			Vec3f new_vtx = (vtxs_[v[j]] + center + edge_mid[(j + 2) % 3] + edge_mid[j]) / 4.0f;
			edge_map[EdgeKey(curr_edge->begin_id, curr_edge->end_id)] = new_vtxs.size();
			new_vid[j] = new_vtxs.size();
			new_vtxs.push_back(new_vtx);
			curr_edge = curr_edge->next;
		}

		// 连接面内的点生成新的面
		new_tris.push_back(Tri(new_vid[0], new_vid[1], new_vid[2]));
	}

	// 点周围的点相连生成新面
	for (int i = 0; i < num_vtx_; ++i)
	{
		HalfEdge* ori_edge = vtxs_[i].halfEdge;
		HalfEdge* curr_edge = ori_edge;
		std::vector<unsigned int> near_vtxs;
		do
		{
			near_vtxs.push_back(edge_map[EdgeKey(curr_edge->begin_id, curr_edge->end_id)]);
			curr_edge = curr_edge->oppo->next;
		} while (curr_edge != ori_edge);

		if (near_vtxs.size() == 3)
		{
			new_tris.push_back(Tri(near_vtxs[2], near_vtxs[1], near_vtxs[0]));
		}
		else
		{
			new_quads.push_back((Quad(near_vtxs[3], near_vtxs[2], near_vtxs[1], near_vtxs[0])));
		}
	}

	// 边两边的点相连生成新面
	std::unordered_set<EdgeKey, EdgeKeyHashFuc> edge_set;	// 边的两条半边只访问一次
	for (int i = 0; i < halfEdges_.size(); ++i)
	{
		HalfEdge* curr_edge = halfEdges_[i];
		if (edge_set.find(EdgeKey(curr_edge->begin_id, curr_edge->end_id)) == edge_set.end())
		{
			edge_set.insert(EdgeKey(curr_edge->begin_id, curr_edge->end_id));
			edge_set.insert(EdgeKey(curr_edge->end_id, curr_edge->begin_id));

			unsigned int v0 = edge_map[EdgeKey(curr_edge->begin_id, curr_edge->end_id)];
			unsigned int v1 = edge_map[EdgeKey(curr_edge->next->begin_id, curr_edge->next->end_id)];
			curr_edge = curr_edge->oppo;
			unsigned int v2 = edge_map[EdgeKey(curr_edge->begin_id, curr_edge->end_id)];
			unsigned int v3 = edge_map[EdgeKey(curr_edge->next->begin_id, curr_edge->next->end_id)];
			new_quads.push_back(Quad(v3, v2, v1, v0));
		}
	}

	vtxs_ = new_vtxs;
	tris_ = new_tris;
	quads_ = new_quads;

	num_vtx_ = vtxs_.size();
	num_tri_ = tris_.size();
	num_quad_ = quads_.size();
}

void MixMesh::findNeighborVertex(int vid, std::vector<unsigned int>& vertexs)
{
	HalfEdge* oriHedge = vtxs_[vid].halfEdge;
	vertexs.push_back(oriHedge->end_id);

	for (HalfEdge* currHedge = oriHedge->oppo->next; currHedge != oriHedge; currHedge = currHedge->oppo->next)
	{
		vertexs.push_back(currHedge->end_id);
	}
}

void MixMesh::loopSubdivision()
{
	clearHalfEdge();
	buildTriHalfEdge();

	// 得到新的旧点的位置
	std::vector<Vec3f> old_vertex(num_vtx_);		// 存储更新后的旧点
	std::vector<unsigned int> neighbor_vertex;		// 存储邻接点的序号
	for (int i = 0; i < num_vtx_; ++i)
	{
		neighbor_vertex.clear();
		findNeighborVertex(i, neighbor_vertex);

		int n = neighbor_vertex.size();
		float beta = 1.0f / n * (0.625f - std::pow((0.375f + 0.25f * cos(2.0f * PIa / n)), 2));

		Vec3f vec3f;
		for (int j = 0; j < n; ++j)
		{
			vec3f += vtxs_[neighbor_vertex[j]] * beta;
		}
		old_vertex[i] = vtxs_[i] * (1 - n * beta) + vec3f;
	}

	// 创建新的点
	std::unordered_map<EdgeKey, unsigned int, EdgeKeyHashFuc> edge_map;		// 建立边和中点序号的映射
	for (int i = 0; i < num_tri_; ++i)
	{
		HalfEdge* curr_hedge = tris_[i].halfedge;
		for (int j = 0; j < 3; ++j)
		{
			if (edge_map.find(EdgeKey(curr_hedge->begin_id, curr_hedge->end_id)) == edge_map.end())
			{
				Vec3f vec3f;
				vec3f += vtxs_[curr_hedge->begin_id] * 0.375f + vtxs_[curr_hedge->end_id] * 0.375f;
				vec3f += vtxs_[curr_hedge->next->end_id] * 0.125f + vtxs_[curr_hedge->oppo->next->end_id] * 0.125f;
				edge_map[EdgeKey(curr_hedge->begin_id, curr_hedge->end_id)] = vtxs_.size();
				edge_map[EdgeKey(curr_hedge->end_id, curr_hedge->begin_id)] = vtxs_.size();
				vtxs_.push_back(vec3f);
			}

			curr_hedge = curr_hedge->next;
		}
	}

	// 更新拓扑
	std::vector<Tri> tris(num_tri_ * 4);	//新的三角形
	int count = 0;
	for (int i = 0; i < num_tri_; ++i)
	{
		HalfEdge* curr_hedge = tris_[i].halfedge;

		unsigned int vid0 = tris_[i].v[0];
		unsigned int vid1 = tris_[i].v[1];
		unsigned int vid2 = tris_[i].v[2];

		unsigned int vid3 = edge_map[EdgeKey(curr_hedge->begin_id, curr_hedge->end_id)];
		curr_hedge = curr_hedge->next;
		unsigned int vid4 = edge_map[EdgeKey(curr_hedge->begin_id, curr_hedge->end_id)];
		curr_hedge = curr_hedge->next;
		unsigned int vid5 = edge_map[EdgeKey(curr_hedge->begin_id, curr_hedge->end_id)];

		tris[count++] = Tri(vid0, vid3, vid5);
		tris[count++] = Tri(vid3, vid1, vid4);
		tris[count++] = Tri(vid3, vid4, vid5);
		tris[count++] = Tri(vid5, vid4, vid2);
	}
	tris_ = tris;

	// 更新旧点
	for (int i = 0; i < num_vtx_; ++i)
	{
		vtxs_[i] = old_vertex[i];
	}

	num_tri_ = tris_.size();
	num_vtx_ = vtxs_.size();
}

void MixMesh::catmullClarkSubdivision()
{
	clearHalfEdge();
	buildQuadHalfEdge();

	// 计算新的面点 f = (v1 + v2 + v3 + v4) / 4 **对于四边形网格
	for (int i = 0; i < num_quad_; ++i)
	{
		unsigned int* v = quads_[i].v;
		Vec3f vec3f;
		for (int j = 0; j < 4; ++j)
		{
			vec3f += vtxs_[v[j]];
		}
		vtxs_.push_back(vec3f / 4.0f);
	}



	// 计算新的边点 e = (v1 + v2 + f1 + f2) / 4 
	std::unordered_map<EdgeKey, unsigned int, EdgeKeyHashFuc> edge_map;		// 半边和边点序号映射，对于边的两条半边，计算边点只需要访问其中一个
	for (int i = 0; i < num_quad_; ++i)
	{
		HalfEdge* curr_hedge = quads_[i].halfedge;

		for (int j = 0; j < 4; ++j)
		{
			if (edge_map.find(EdgeKey(curr_hedge->begin_id, curr_hedge->end_id)) == edge_map.end())		// 如果边没有计算过
			{
				Vec3f vec3f;
				vec3f += vtxs_[curr_hedge->begin_id] + vtxs_[curr_hedge->end_id];
				vec3f += vtxs_[curr_hedge->fid + num_vtx_] + vtxs_[curr_hedge->oppo->fid + num_vtx_];
				edge_map[EdgeKey(curr_hedge->begin_id, curr_hedge->end_id)] = vtxs_.size();
				edge_map[EdgeKey(curr_hedge->end_id, curr_hedge->begin_id)] = vtxs_.size();
				vtxs_.push_back(vec3f / 4.0f);
			}
			curr_hedge = curr_hedge->next;
		}
	}

	// 计算更新后的原本的点的位置
	std::vector<Vec3f> new_vertex(num_vtx_);
	for (int i = 0; i < num_vtx_; ++i)
	{
		HalfEdge* ori_hedge = vtxs_[i].halfEdge;
		HalfEdge* curr_hedge = ori_hedge;

		Vec3f Q;	// 与点相邻的面的面点的平均值
		Vec3f R;	// 与点相邻的边点的平均值
		int n = 0;

		// 遍历从点出发的所有半边
		do
		{
			n++;
			Q += vtxs_[curr_hedge->fid + num_vtx_];
			R += vtxs_[edge_map[EdgeKey(curr_hedge->begin_id, curr_hedge->end_id)]];
			curr_hedge = curr_hedge->oppo->next;
		} while (curr_hedge != ori_hedge);
		Q = Q / n;
		R = R / n;
		new_vertex[i] = Q / n + R * 2.0f / n + vtxs_[i] * (float)(n - 3) / n;
	}

	// 更新拓扑
	std::vector<Quad> quads(num_quad_ * 4);
	int count = 0;
	for (int i = 0; i < num_quad_; ++i)
	{
		Quad& quad = quads_[i];

		unsigned int vid0 = quad.v[0];
		unsigned int vid1 = quad.v[1];
		unsigned int vid2 = quad.v[2];
		unsigned int vid3 = quad.v[3];

		HalfEdge* curr_hedge = quad.halfedge;
		unsigned int vid4 = edge_map[EdgeKey(curr_hedge->begin_id, curr_hedge->end_id)];
		curr_hedge = curr_hedge->next;
		unsigned int vid5 = edge_map[EdgeKey(curr_hedge->begin_id, curr_hedge->end_id)];
		curr_hedge = curr_hedge->next;
		unsigned int vid6 = edge_map[EdgeKey(curr_hedge->begin_id, curr_hedge->end_id)];
		curr_hedge = curr_hedge->next;
		unsigned int vid7 = edge_map[EdgeKey(curr_hedge->begin_id, curr_hedge->end_id)];

		unsigned int vid8 = i + num_vtx_;	// 面的数量加上初始网格点的数量就是面点的序号

		quads[count++] = Quad(vid0, vid4, vid8, vid7);
		quads[count++] = Quad(vid4, vid1, vid5, vid8);
		quads[count++] = Quad(vid8, vid5, vid2, vid6);
		quads[count++] = Quad(vid7, vid8, vid6, vid3);
	}
	quads_ = quads;

	// 将更新后的点更新到网格中
	for (int i = 0; i < num_vtx_; ++i)
	{
		vtxs_[i] = new_vertex[i];
	}

	num_quad_ = quads_.size();
	num_vtx_ = vtxs_.size();

}
