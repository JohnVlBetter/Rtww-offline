#pragma once
#include "Core.hpp"
class Transform {
public:
	Transform() {}
	/*Transform(Float d00, Float d01, Float d02, Float d03, Float d10, Float d11, Float d12, Float d13,
		Float d20, Float d21, Float d22, Float d23, Float d30, Float d31, Float d32, Float d33) {
		object2World = Matrix4x4(d00, d01, d02, d03, d10, d11, d12, d13, d20, d21, d22, d23, d30, d31, d32, d33);
		world2Object = object2World.Inverse();
	}
	Transform(const Matrix4x4& o2w) : object2World(o2w), world2Object(o2w.Inverse()) {}
	Transform(const Matrix4x4& o2w, const Matrix4x4& w2o) : object2World(o2w), world2Object(w2o) {}*/
	Transform(const Point3f& position, const Vector3f& scale, const Vector3f& rotation) :scale(scale), position(position), rotation(rotation) {
		object2World = Translate(Convert(position)) * CreateQuaternionByVec3(rotation).ToMatrix4x4() * Scale(scale);
		world2Object = object2World.Inverse();
	}

	const Matrix4x4&  GetObject2WorldMatrix() const { return object2World; }
	const Matrix4x4&  GetWorld2ObjectMatrix() const { return world2Object; }
	const Point3f&    GetPosition()			  const { return position;	   }
	const Vector3f&   GetScale()			  const { return scale;		   }
	const Vector3f& GetRotation()			  const { return rotation;	   }

	/*Transform Inverse() const { return Transform(world2Object, object2World); }
	Transform Transpose() const { return Transform(object2World.Transpose(), world2Object.Transpose()); }*/

	bool operator==(const Transform& t) const { return object2World == t.object2World && world2Object == t.world2Object; }
	bool operator!=(const Transform& t) const { return object2World != t.object2World || world2Object != t.world2Object; }

private:
	Matrix4x4 object2World, world2Object;
	Vector3f scale;
	Point3f position;
	Vector3f rotation;
	friend class Quaternion;
};