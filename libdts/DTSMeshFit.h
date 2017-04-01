#ifndef DTS_MESHFIT_H_
#define DTS_MESHFIT_H_

#define _USE_MATH_DEFINES
#include <math.h>

#include <array>

#include "NvFloatMath.h"
#include "NvConvexDecomposition.h"
#include "NvStanHull.h"

#include "DTSMath.h"
#include "DTSVector.h"
#include "DTSShape.h"

namespace DTS
{

struct PrimFit
{
	MatrixF box_transform_;
	Point3F box_sides_;

	Point3F sphere_center_;
	float sphere_radius_;

	MatrixF cap_transform_;
	float cap_radius_;
	float cap_height_;

public:
	PrimFit() :
		box_transform_(true), box_sides_(1, 1, 1),
		sphere_center_(0, 0, 0), sphere_radius_(1),
		cap_transform_(true), cap_radius_(1), cap_height_(1)
	{
	}

	inline float GetBoxVolume() const { return box_sides_.x * box_sides_.y * box_sides_.z; }
	inline float GetSphereVolume() const { return 4.0f / 3.0f * M_PI * pow(sphere_radius_ , 3.0f); }
	inline float GetCapsuleVolume() const { return 2 * M_PI * pow(cap_radius_, 2.0f) * (4.0f / 3.0f * cap_radius_ + cap_height_); }

	void FitBox(uint32_t vert_count, const float* verts)
	{
		CONVEX_DECOMPOSITION::fm_computeBestFitOBB(vert_count, verts, sizeof(float) * 3, box_sides_, box_transform_);
		box_transform_.Transpose();
	}

	void FitSphere(uint32_t vert_count, const float* verts)
	{
		sphere_radius_ = CONVEX_DECOMPOSITION::fm_computeBestFitSphere(vert_count, verts, sizeof(float) * 3, sphere_center_);
	}

	void FitCapsule(uint32_t vert_count, const float* verts)
	{
		CONVEX_DECOMPOSITION::fm_computeBestFitCapsule(vert_count, verts, sizeof(float) * 3, cap_radius_, cap_height_, cap_transform_);
		cap_transform_.Transpose();
	}
};

class MeshFit
{
public:
	static const std::array<Point3F, 6> kFacePlanes;
	static const std::array<Point3F, 4> kXEdgePlanes;
	static const std::array<Point3F, 4> kYEdgePlanes;
	static const std::array<Point3F, 4> kZEdgePlanes;
	static const std::array<Point3F, 8> kCornerPlanes;

	enum MeshType
	{
		kBox = 0,
		kSphere,
		kCapsule,
		kHull,
	};

	struct Mesh
	{
		MeshType type;
		MatrixF transform;
		TSMesh* tsmesh;
	};

	MeshFit(TSShape* shape) :
		shape_(shape), is_ready_(false) {}

	void SetReady() { is_ready_ = true; }
	bool IsReady() const { return is_ready_; }

	void InitSourceGeometry(const std::string& target);

	int32_t GetMeshCount() const { return meshes_.size(); }
	Mesh* GetMesh(int32_t index) { return &(meshes_[index]); }

	// Box
	void AddBox(const Point3F& sides, const MatrixF& mat);
	void FitOBB();

	// Sphere
	void AddSphere(float radius, const Point3F& center);
	void FitSphere();

	// Capsule
	void AddCapsule(float radius, float height, const MatrixF& mat);
	void FitCapsule();

	// k-DOP
	void Fit10_DOP_X();
	void Fit10_DOP_Y();
	void Fit10_DOP_Z();
	void Fit18_DOP();
	void Fit26_DOP();

private:
	void AddSourceMesh(const TSShape::Object& obj, const TSMesh* mesh);
	TSMesh* InitMeshFromFile(const std::string& filename) const;
	TSMesh* CreateTriMesh(float* verts, int32_t num_verts, uint32_t* indices, int32_t num_tris) const;
	float MaxDot(const VectorF& v) const;
	void FitK_DOP(const std::vector<Point3F>& planes);

	TSShape*				shape_;		// Source geometry shape
	std::vector<Point3F>	verts_;		// Source geometry verts (all meshes)
	std::vector<uint32_t>	indices_;   // Source geometry indices (triangle lists, all meshes)

	bool					is_ready_;	// Flag indicating whether we are ready to fit/create meshes

	std::vector<Mesh>		meshes_;	// Fitted meshes
};

} // namespace DTS

#endif // DTS_MESHFIT_H_
