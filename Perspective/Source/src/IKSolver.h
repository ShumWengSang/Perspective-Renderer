#pragma once
#include "MyMath.h"
struct IKBone
{
	MyMath::VQS worldTransformation;
	IKBone* parent = nullptr;
};

struct EndAffector
{
	MyMath::VQS worldTransformation;
	IKBone* parent;
};

class IKSolver
{
	public:

};