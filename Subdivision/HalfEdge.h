#pragma once
#include <unordered_map>

struct HalfEdge
{
	unsigned int begin_id;		// �����ʼ�������
	unsigned int end_id;		// ���ָ��Ķ������
	HalfEdge* next;		// �ڶ��������ʱ��ָ�����һ�����
	HalfEdge* oppo;		// һ�����еĶ�Ӧ���
	int fid;	// ��������ڵ�������, -1��ʾ�ڱ߽���

	HalfEdge() : begin_id(0), end_id(0), next(nullptr), oppo(nullptr), fid(-1) {}
};

struct EdgeKey
{
	unsigned int begin_id;	// �ߵ�������
	unsigned int end_id;		// �ߵ��յ�

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