#pragma once
#include "Transform.h"
#include <vector>
#include <functional>

class PhysicalObject
{
	//PhysicalMaterial material
	std::vector<std::function<void()>> callbacks;
public:
	PhysicalObject();

	Transform transform;

	template< class F, class G >
	void RegisterCollisionCallback(F&& callback, G *obj)
	{
		callbacks.push_back(std::bind(std::forward<F>(callback), obj, this));
	}
	~PhysicalObject();
};

