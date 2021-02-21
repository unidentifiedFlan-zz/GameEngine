#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

class Transform
{
public:
	Transform(glm::vec3 translation = glm::vec3(0.0f), 
		      glm::vec3 scale = glm::vec3(1.0f), 
		      float rotationAngle = 0.0f,
		      glm::vec3 rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f))
	:
	translation(translation),
	scale(scale)
	{
		rotate(rotationAxis, rotationAngle);
	}

	Transform operator *(const Transform &otherTransform) const
	{
		glm::mat4 newTransformMat = transform() *otherTransform.transform();
		Transform newTransform;
		glm::vec3 scale;
		glm::quat orientation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;

		glm::decompose(newTransformMat, scale, orientation, translation, skew, perspective);
		orientation = glm::conjugate(orientation);
		if (orientation.x != 0 || orientation.y != 0 || orientation.z != 0) {
			newTransform.rotate(orientation);
		}
		newTransform.setScale( scale );
		newTransform.translate( translation );
		return newTransform;
	}

	glm::mat4 transform() const
	{
		glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);
		glm::mat4 transMat = glm::translate(glm::mat4(1.0f), translation);
		return transMat*rotation*scaleMat;
	}

	void translate(glm::vec3 vec)
	{
		translation = vec;
	}

	void rotate(glm::quat rotationQuaternion)
	{
		rotation = glm::toMat4(rotationQuaternion);
	}

	void rotate(glm::vec3 axis, float radians)
	{
		glm::mat4 r = glm::mat4(1.0f);
		rotation = glm::rotate(r, radians, axis);
	}

	void rotate(glm::mat4 rotation)
	{
		this->rotation = rotation;
	}

	void setScale(glm::vec3 vec)
	{
		scale = vec;
	}

	glm::vec3 translation;
	glm::mat4 rotation;
	glm::vec3 scale;
};