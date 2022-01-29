#pragma once
#include "Core.hpp"
namespace rtww {
	class Transform {
	public:
		Transform() {}
		Transform(Float d00, Float d01, Float d02, Float d03, Float d10, Float d11, Float d12, Float d13,
			Float d20, Float d21, Float d22, Float d23, Float d30, Float d31, Float d32, Float d33) {
			mat = Matrix4x4(d00, d01, d02, d03, d10, d11, d12, d13, d20, d21, d22, d23, d30, d31, d32, d33);
			matInv = mat.Inverse();
		}
		Transform(const Matrix4x4& mat) :mat(mat), matInv(mat.Inverse()) {}
		Transform(const Matrix4x4& mat, const Matrix4x4& matInv) :mat(mat), matInv(matInv) {}

		const Matrix4x4& GetMatrix() const { return mat; }
		const Matrix4x4& GetInverseMatrix() const { return matInv; }

		Transform Inverse() const { return Transform(matInv, mat); }
		Transform Transpose() const { return Transform(mat.Transpose(), matInv.Transpose()); }

		bool operator==(const Transform& t) const { return mat == t.mat && matInv == t.matInv; }
		bool operator!=(const Transform& t) const { return mat != t.mat || matInv != t.matInv; }

		bool IsIdentity() const { return mat.IsIdentity(); }
		bool HasScale() const {
#define NOT_ONE(x) ((x) < .999f || (x) > 1.001f)
			return (NOT_ONE(mat.data[0][0]) || NOT_ONE(mat.data[1][1]) || NOT_ONE(mat.data[2][2]));
#undef NOT_ONE
		}
		template <typename T>
		inline Point3<T> operator()(const Point3<T> &p) const;
		template <typename T>
		inline Vector3<T> operator()(const Vector3<T> &v) const;
		inline Ray operator()(const Ray &r) const;

	private:
		Matrix4x4 mat, matInv;
		friend class hsm::Quaternion;
	};

	template <typename T>
	inline Point3<T> Transform::operator()(const Point3<T> &p) const {
		T x = p.x, y = p.y, z = p.z;
		T px = mat.data[0][0] * x + mat.data[0][1] * y + mat.data[0][2] * z + mat.data[0][3];
		T py = mat.data[1][0] * x + mat.data[1][1] * y + mat.data[1][2] * z + mat.data[1][3];
		T pz = mat.data[2][0] * x + mat.data[2][1] * y + mat.data[2][2] * z + mat.data[2][3];
		T pw = mat.data[3][0] * x + mat.data[3][1] * y + mat.data[3][2] * z + mat.data[3][3];
		assert(pw != 0);
		if (pw == 1) return Point3<T>(px, py, pz);
		else return Point3<T>(px, py, pz) / pw;
	}

	template <typename T>
	inline Vector3<T> Transform::operator()(const Vector3<T> &v) const {
		T x = v.x, y = v.y, z = v.z;
		return Vector3<T>(mat.data[0][0] * x + mat.data[0][1] * y + mat.data[0][2] * z,
			mat.data[1][0] * x + mat.data[1][1] * y + mat.data[1][2] * z,
			mat.data[2][0] * x + mat.data[2][1] * y + mat.data[2][2] * z);
	}

	inline Ray Transform::operator()(const Ray &r) const {
		Point3f o = (*this)(r.origin);
		Vector3f d = (*this)(r.direction);
		return Ray(o, d, r.time);
	}

	Transform Translate(const Vector3f &delta) {
		Matrix4x4 mat(1, 0, 0, delta.x, 0, 1, 0, delta.y, 0, 0, 1, delta.z, 0, 0, 0, 1);
		Matrix4x4 matInv(1, 0, 0, -delta.x, 0, 1, 0, -delta.y, 0, 0, 1, -delta.z, 0, 0, 0, 1);
		return Transform(mat, matInv);
	}

	Transform Scale(Float x, Float y, Float z) {
		assert(x != 0);
		assert(y != 0);
		assert(z != 0);
		Matrix4x4 mat(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);
		Matrix4x4 matInv(1 / x, 0, 0, 0, 0, 1 / y, 0, 0, 0, 0, 1 / z, 0, 0, 0, 0, 1);
		return Transform(mat, matInv);
	}

	Transform RotateX(Float theta) {
		auto mat = hsm::RotateX(theta);
		return Transform(mat, mat.Transpose());
	}

	Transform RotateY(Float theta) {
		auto mat = hsm::RotateY(theta);
		return Transform(mat, mat.Transpose());
	}

	Transform RotateZ(Float theta) {
		auto mat = hsm::RotateZ(theta);
		return Transform(mat, mat.Transpose());
	}

	Transform Rotate(const Vector3f &axis, Float theta) {
		auto mat = hsm::Rotate(axis, theta);
		return Transform(mat, mat.Transpose());
	}

	Transform LookAt(const Point3f& pos, const Point3f& target, const Vector3f& viewUp) {
		auto viewMat = hsm::GetViewMatrix(pos, target, viewUp);
		return Transform(viewMat, viewMat.Inverse());
	}

	Transform Perspective(Float aspect, Float fov, Float n, Float f) {
		auto perspectiveMat = hsm::GetPerspectiveMatrix(aspect, fov, n, f);
		return Transform(perspectiveMat, perspectiveMat.Inverse());
	}
}