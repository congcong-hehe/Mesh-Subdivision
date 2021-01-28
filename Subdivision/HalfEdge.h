#pragma once
#include <unordered_map>

struct HalfEdge
{
	unsigned int begin_id;		// 半边起始顶点序号
	unsigned int end_id;		// 半边指向的顶点序号
	HalfEdge* next;		// 在多边形中逆时针指向的下一条半边
	HalfEdge* oppo;		// 一条边中的对应半边
	int fid;	// 半边所属于的面的序号, -1表示在边界上

	HalfEdge() : begin_id(0), end_id(0), next(nullptr), oppo(nullptr), fid(-1) {}
};

struct EdgeKey
{
	unsigned int begin_id;	// 边的起点序号
	unsigned int end_id;		// 边的终点

	EdgeKey(unsigned int _begin_id = 0, unsigned int _end_id = 0) : begin_id(_begin_id), end_id(_end_id) {}

	bool operator == (const EdgeKey& key) const 
	{
		return begin_id == key.begin_id && end_id == key.end_id;
	}
};

struct EdgeKeyHashFuc
{
	std::size_t operator()(const EdgeKey& key) const
	{
		return std::hash<unsigned int>()(key.begin_id) + std::hash<unsigned int>()(key.end_id);
	}
};