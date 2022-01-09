#pragma once
#include "Core.hpp"
#include "Shape.hpp"
#include "World.hpp"

class BVHNode : public Shape{
public:
	BVHNode() {}
	BVHNode(const std::vector<std::shared_ptr<Shape>>& srcObjects, size_t start, size_t end, Float time0, Float time1);
	BVHNode(const ShapesSet& set, Float time0, Float time1) :
		BVHNode(set.objects, 0, set.objects.size(), time0, time1) {}

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override;

public:
	std::shared_ptr<Shape> leftChild;
	std::shared_ptr<Shape> rightChild;
	AABB boundingBox;
};

inline bool BoxCompare(const std::shared_ptr<Shape> a, const std::shared_ptr<Shape> b, int axis) {
	AABB boxA;
	AABB boxB;

	if (!a->BoundingBox(0, 0, boxA) || !b->BoundingBox(0, 0, boxB))
		std::cerr << "No bounding box in bvh_node constructor.\n";

	return boxA.Min()[axis] < boxB.Min()[axis];
}

bool BoxXCompare(const std::shared_ptr<Shape> a, const std::shared_ptr<Shape> b) {
	return BoxCompare(a, b, 0);
}

bool BoxYCompare(const std::shared_ptr<Shape> a, const std::shared_ptr<Shape> b) {
	return BoxCompare(a, b, 1);
}

bool BoxZCompare(const std::shared_ptr<Shape> a, const std::shared_ptr<Shape> b) {
	return BoxCompare(a, b, 2);
}

BVHNode::BVHNode(const std::vector<std::shared_ptr<Shape>>& srcObjects, size_t start, size_t end, Float time0, Float time1) {
	auto objects = srcObjects;
	int axis = RandomInt(0, 2);
	auto comparator = (axis == 0) ? BoxXCompare : (axis == 1) ? BoxYCompare : BoxZCompare;

	size_t objectSpan = end - start;
	if (objectSpan == 1) {
		leftChild = rightChild = objects[start];
	}
	else if (objectSpan == 2) {
		if (comparator(objects[start], objects[start + 1])) {
			leftChild = objects[start];
			rightChild = objects[start + 1];
		}
		else {
			leftChild = objects[start + 1];
			rightChild = objects[start];
		}
	}
	else {
		std::sort(objects.begin() + start, objects.begin() + end, comparator);

		auto mid = start + objectSpan / 2;
		leftChild = std::make_shared<BVHNode>(objects, start, mid, time0, time1);
		rightChild = std::make_shared<BVHNode>(objects, mid, end, time0, time1);
	}

	AABB boxL, boxR;
	if (!leftChild->BoundingBox(time0, time1, boxL) || !rightChild->BoundingBox(time0, time1, boxR)) {
		std::cerr << "No bounding box in bvh_node constructor.\n";
	}

	boundingBox = SurroundingBox(boxL, boxR);
}

bool BVHNode::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	if (!boundingBox.Intersection(r, tMin, tMax)) return false;

	bool left = leftChild->Intersection(r, tMin, tMax, rec);
	bool right = rightChild->Intersection(r, tMin, left ? rec.time : tMax, rec);

	return left || right;
}

bool BVHNode::BoundingBox(Float time0, Float time1, AABB& outputBox) const {
	outputBox = boundingBox;
	return true;
}