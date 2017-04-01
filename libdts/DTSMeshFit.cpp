#include "DTSMeshFit.h"

#include "DTSShapeConstruct.h"

namespace DTS
{

const std::array<Point3F, 6> MeshFit::kFacePlanes {
	Point3F(-1.0f,  0.0f,  0.0f),
	Point3F(1.0f,  0.0f,  0.0f),
	Point3F(0.0f, -1.0f,  0.0f),
	Point3F(0.0f,  1.0f,  0.0f),
	Point3F(0.0f,  0.0f, -1.0f),
	Point3F(0.0f,  0.0f,  1.0f),
};

const std::array<Point3F, 4> MeshFit::kXEdgePlanes {
	Point3F(0.0f, -0.7071f, -0.7071f),
	Point3F(0.0f, -0.7071f,  0.7071f),
	Point3F(0.0f,  0.7071f, -0.7071f),
	Point3F(0.0f,  0.7071f,  0.7071f),
};

const std::array<Point3F, 4> MeshFit::kYEdgePlanes {
	Point3F(-0.7071f, 0.0f, -0.7071f),
	Point3F(-0.7071f, 0.0f,  0.7071f),
	Point3F(0.7071f, 0.0f, -0.7071f),
	Point3F(0.7071f, 0.0f,  0.7071f),
};

const std::array<Point3F, 4> MeshFit::kZEdgePlanes {
	Point3F(-0.7071f, -0.7071f, 0.0f),
	Point3F(-0.7071f,  0.7071f, 0.0f),
	Point3F(0.7071f, -0.7071f, 0.0f),
	Point3F(0.7071f,  0.7071f, 0.0f),
};

const std::array<Point3F, 8> MeshFit::kCornerPlanes {
	Point3F(-0.5774f, -0.5774f, -0.5774f),
	Point3F(-0.5774f, -0.5774f,  0.5774f),
	Point3F(-0.5774f,  0.5774f, -0.5774f),
	Point3F(-0.5774f,  0.5774f,  0.5774f),
	Point3F(0.5774f, -0.5774f, -0.5774f),
	Point3F(0.5774f, -0.5774f,  0.5774f),
	Point3F(0.5774f,  0.5774f, -0.5774f),
	Point3F(0.5774f,  0.5774f,  0.5774f),
};

void MeshFit::InitSourceGeometry(const std::string& target)
{
	meshes_.clear();
	verts_.clear();
	indices_.clear();

	if (target == "bounds")
	{
		// Add all geometry in the highest detail level
		int32_t dl = 0;
		int32_t ss = shape_->details_[dl].sub_shape_num;
		if (ss < 0)
			return;

		int32_t od = shape_->details_[dl].object_detail_num;
		int32_t start = shape_->sub_shape_first_object_[ss];
		int32_t end = start + shape_->sub_shape_num_objects_[ss];

		for (int32_t i = start; i < end; i++)
		{
			const TSShape::Object &obj = shape_->objects_[i];
			const TSMesh* mesh = (od < obj.num_meshes) ? shape_->meshes_[obj.start_mesh_index + od] : nullptr;
			if (mesh)
				AddSourceMesh(obj, mesh);
		}
	}
	else
	{
		// Add highest detail mesh from this object
		int32_t obj_index = shape_->FindObject(target);
		if (obj_index == -1)
			return;

		const TSShape::Object &obj = shape_->objects_[obj_index];
		for (int32_t i = 0; i < obj.num_meshes; i++)
		{
			const TSMesh* mesh = shape_->meshes_[obj.start_mesh_index + i];
			if (mesh)
			{
				AddSourceMesh(obj, mesh);
				break;
			}
		}
	}

	is_ready_ = (!verts_.empty() && !indices_.empty());
}

TSMesh* MeshFit::InitMeshFromFile(const std::string& filename) const
{
	// Open the source shape file and make a copy of the mesh
	TSShape shape(filename);
	if (shape.meshes_.empty())
	{
		return nullptr;
	}

	TSMesh* src_mesh = shape.meshes_[0];
	return shape.CopyMesh(src_mesh);
}

void MeshFit::AddSourceMesh(const TSShape::Object& obj, const TSMesh* mesh)
{
	// Add indices
	int32_t indices_base = indices_.size();
	for (int32_t i = 0; i < mesh->primitives_.size(); i++)
	{
		const TSDrawPrimitive& draw = mesh->primitives_[i];
		if ((draw.mat_index & TSDrawPrimitive::kTypeMask) == TSDrawPrimitive::kTriangles)
		{
			Vector::Merge(indices_, &mesh->indices_[draw.start], draw.num_elements);
		}
		else
		{
			uint32_t idx0 = mesh->indices_[draw.start + 0];
			uint32_t idx1;
			uint32_t idx2 = mesh->indices_[draw.start + 1];
			uint32_t* next_idx = &idx1;
			for (int32_t j = 2; j < draw.num_elements; j++)
			{
				*next_idx = idx2;
				next_idx = reinterpret_cast<std::size_t*>(reinterpret_cast<std::size_t>(next_idx) ^ reinterpret_cast<std::size_t>(&idx0) ^ reinterpret_cast<std::size_t>(&idx1));
				idx2 = mesh->indices_[draw.start + j];
				if (idx0 == idx1 || idx0 == idx2 || idx1 == idx2)
					continue;

				indices_.push_back(idx0);
				indices_.push_back(idx1);
				indices_.push_back(idx2);
			}
		}
	}

	// Offset indices for already added verts
	for (int32_t j = indices_base; j < indices_.size(); j++)
		indices_[j] += verts_.size();

	// Add verts
	int32_t count = mesh->verts_.size();

	MatrixF obj_mat;
	shape_->GetNodeWorldTransform(obj.node_index, &obj_mat);

	verts_.reserve(verts_.size() + count);
	for (int32_t j = 0; j < count; j++)
	{
		Point3F vert;
		obj_mat.MulP(mesh->verts_[j], &vert);
		verts_.push_back(vert);
	}
}

TSMesh* MeshFit::CreateTriMesh(float* verts, int32_t num_verts, uint32_t* indices, int32_t num_tris) const
{
	TSMesh* mesh = shape_->CopyMesh(NULL);
	mesh->num_frames_ = 1;
	mesh->num_mat_frames_ = 1;
	mesh->verts_per_frame_ = num_verts;
	mesh->SetFlags(0);

	mesh->indices_.reserve(num_tris * 3);
	for (int32_t i = 0; i < num_tris; i++)
	{
		mesh->indices_.push_back(indices[i * 3 + 0]);
		mesh->indices_.push_back(indices[i * 3 + 2]);
		mesh->indices_.push_back(indices[i * 3 + 1]);
	}

	Vector::Set(mesh->verts_, verts, num_verts);

	// Compute mesh normals
	mesh->norms_.resize(mesh->verts_.size());
	for (int32_t i_norm = 0; i_norm < mesh->norms_.size(); i_norm++)
		mesh->norms_[i_norm] = Point3F::kZero;

	// Sum triangle normals for each vertex
	for (int32_t i_ind = 0; i_ind < mesh->indices_.size(); i_ind += 3)
	{
		// Compute the normal for this triangle
		int32_t idx0 = mesh->indices_[i_ind + 0];
		int32_t idx1 = mesh->indices_[i_ind + 1];
		int32_t idx2 = mesh->indices_[i_ind + 2];

		const Point3F& v0 = mesh->verts_[idx0];
		const Point3F& v1 = mesh->verts_[idx1];
		const Point3F& v2 = mesh->verts_[idx2];

		Point3F n;
		Math::Cross(v2 - v0, v1 - v0, &n);
		n.Normalize(); // remove this to use 'weighted' normals (large triangles will have more effect)

		mesh->norms_[idx0] += n;
		mesh->norms_[idx1] += n;
		mesh->norms_[idx2] += n;
	}

	// Normalize the vertex normals (this takes care of averaging the triangle normals)
	for (int32_t i_norm = 0; i_norm < mesh->norms_.size(); i_norm++)
		mesh->norms_[i_norm].Normalize();

	// Set some dummy UVs
	mesh->tverts_.resize(num_verts);
	for (int32_t j = 0; j < mesh->tverts_.size(); j++)
		mesh->tverts_[j].Set(0, 0);

	// Add a single triangle-list primitive
	TSDrawPrimitive primitive;
	primitive.start = 0;
	primitive.num_elements = mesh->indices_.size();
	primitive.mat_index = TSDrawPrimitive::kTriangles |
		TSDrawPrimitive::kIndexed |
		TSDrawPrimitive::kNoMaterial;
	mesh->primitives_.push_back(primitive);

	Vector::Set(mesh->encoded_norms_, NULL, 0);

	return mesh;
}

// Best-fit oriented bounding box
void MeshFit::AddBox(const Point3F& sides, const MatrixF& mat)
{
	TSMesh* tsmesh = InitMeshFromFile(TSShapeConstructor::GetCubeShapePath());
	if (!tsmesh)
		return;

	for (int32_t i = 0; i < tsmesh->verts_.size(); i++)
	{
		Point3F v = tsmesh->verts_[i];
		v.Convolve(sides);
		tsmesh->verts_[i] = v;
	}

	tsmesh->ComputeBounds();

	Mesh mesh;
	mesh.type = MeshFit::kBox;
	mesh.transform = mat;
	mesh.tsmesh = tsmesh;
	meshes_.push_back(mesh);
}

void MeshFit::FitOBB()
{
	PrimFit prim_fitter;
	prim_fitter.FitBox(verts_.size(), reinterpret_cast<float*>(Vector::Address(verts_)));
	AddBox(prim_fitter.box_sides_, prim_fitter.box_transform_);
}

// Best-fit sphere
void MeshFit::AddSphere(float radius, const Point3F& center)
{
	TSMesh* tsmesh = InitMeshFromFile(TSShapeConstructor::GetSphereShapePath());
	if (!tsmesh)
		return;

	for (int32_t i = 0; i < tsmesh->verts_.size(); i++)
	{
		Point3F v = tsmesh->verts_[i];
		tsmesh->verts_[i] = v * radius;
	}
	tsmesh->ComputeBounds();

	Mesh mesh;
	mesh.type = MeshFit::kSphere;
	mesh.transform.Identity();
	mesh.transform.SetPosition(center);
	mesh.tsmesh = tsmesh;
	meshes_.push_back(mesh);
}

void MeshFit::FitSphere()
{
	PrimFit primFitter;
	primFitter.FitSphere(verts_.size(), reinterpret_cast<float*>(Vector::Address(verts_)));
	AddSphere(primFitter.sphere_radius_, primFitter.sphere_center_);
}

// Best-fit capsule
void MeshFit::AddCapsule(float radius, float height, const MatrixF& mat)
{
	TSMesh* tsmesh = InitMeshFromFile(TSShapeConstructor::GetCapsuleShapePath());
	if (!tsmesh)
		return;

	// Translate and scale the mesh verts
	height = std::max(0.0f, height);
	float offset = (height / (2 * radius)) - 0.5f;
	for (int32_t i = 0; i < tsmesh->verts_.size(); i++)
	{
		Point3F v = tsmesh->verts_[i];
		v.y += ((v.y > 0) ? offset : -offset);
		tsmesh->verts_[i] = v * radius;
	}
	tsmesh->ComputeBounds();

	Mesh mesh;
	mesh.type = MeshFit::kCapsule;
	mesh.transform = mat;
	mesh.tsmesh = tsmesh;
	meshes_.push_back(mesh);
}

void MeshFit::FitCapsule()
{
	PrimFit prim_fitter;
	prim_fitter.FitCapsule(verts_.size(), reinterpret_cast<float*>(Vector::Address(verts_)));
	AddCapsule(prim_fitter.cap_radius_, prim_fitter.cap_height_, prim_fitter.cap_transform_);
}

// Best-fit k-discrete-oriented-polytope (where k is the number of axis-aligned planes)

// All faces + 4 edges (aligned to X axis) of the unit cube
void MeshFit::Fit10_DOP_X()
{
	std::vector<Point3F> planes;
	planes.reserve(10);
	planes.insert(planes.end(), kFacePlanes.begin(), kFacePlanes.end());
	planes.insert(planes.end(), kXEdgePlanes.begin(), kXEdgePlanes.end());
	FitK_DOP(planes);
}

// All faces + 4 edges (aligned to Y axis) of the unit cube
void MeshFit::Fit10_DOP_Y()
{
	std::vector<Point3F> planes;
	planes.reserve(10);
	planes.insert(planes.end(), kFacePlanes.begin(), kFacePlanes.end());
	planes.insert(planes.end(), kYEdgePlanes.begin(), kYEdgePlanes.end());
	FitK_DOP(planes);
}

// All faces + 4 edges (aligned to Z axis) of the unit cube
void MeshFit::Fit10_DOP_Z()
{
	std::vector<Point3F> planes;
	planes.reserve(10);
	planes.insert(planes.end(), kFacePlanes.begin(), kFacePlanes.end());
	planes.insert(planes.end(), kZEdgePlanes.begin(), kZEdgePlanes.end());
	FitK_DOP(planes);
}

// All faces and edges of the unit cube
void MeshFit::Fit18_DOP()
{
	std::vector<Point3F> planes;
	planes.reserve(18);
	planes.insert(planes.end(), kFacePlanes.begin(), kFacePlanes.end());
	planes.insert(planes.end(), kXEdgePlanes.begin(), kXEdgePlanes.end());
	planes.insert(planes.end(), kYEdgePlanes.begin(), kYEdgePlanes.end());
	planes.insert(planes.end(), kZEdgePlanes.begin(), kZEdgePlanes.end());
	FitK_DOP(planes);
}

// All faces, edges and corners of the unit cube
void MeshFit::Fit26_DOP()
{
	std::vector<Point3F> planes;
	planes.reserve(26);
	planes.insert(planes.end(), kFacePlanes.begin(), kFacePlanes.end());
	planes.insert(planes.end(), kXEdgePlanes.begin(), kXEdgePlanes.end());
	planes.insert(planes.end(), kYEdgePlanes.begin(), kYEdgePlanes.end());
	planes.insert(planes.end(), kZEdgePlanes.begin(), kZEdgePlanes.end());
	planes.insert(planes.end(), kCornerPlanes.begin(), kCornerPlanes.end());
	FitK_DOP(planes);
}

float MeshFit::MaxDot(const VectorF& v) const
{
	float max_dot = -FLT_MAX;
	for (int32_t i = 0; i < verts_.size(); i++)
		max_dot = std::max(max_dot, Math::Dot(v, verts_[i]));
	return max_dot;
}

void MeshFit::FitK_DOP(const std::vector<Point3F>& planes)
{
	// Push the planes up against the mesh
	std::vector<float> plane_ds;
	for (int32_t i = 0; i < planes.size(); i++)
		plane_ds.push_back(MaxDot(planes[i]));

	// Collect the intersection points of any 3 planes that lie inside
	// the maximum distances found above
	std::vector <Point3F> points;
	for (int32_t i = 0; i < planes.size() - 2; i++)
	{
		for (int32_t j = i + 1; j < planes.size() - 1; j++)
		{
			for (int32_t k = j + 1; k < planes.size(); k++)
			{
				Point3F v23 = Math::Cross(planes[j], planes[k]);
				float denom = Math::Dot(planes[i], v23);
				if (denom == 0)
					continue;

				Point3F v31 = Math::Cross(planes[k], planes[i]);
				Point3F v12 = Math::Cross(planes[i], planes[j]);
				Point3F p = (plane_ds[i] * v23 + plane_ds[j] * v31 + plane_ds[k] * v12) / denom;

				// Ignore intersection points outside the volume
				// described by the planes
				bool add_point = true;
				for (int32_t n = 0; n < planes.size(); n++)
				{
					if ((Math::Dot(p, planes[n]) - plane_ds[n]) > 0.005f)
					{
						add_point = false;
						break;
					}
				}

				if (add_point)
					points.push_back(p);
			}
		}
	}

	// Create a convex hull from the point set
	CONVEX_DECOMPOSITION::HullDesc hd;
	hd.mVcount = points.size();
	hd.mVertices = reinterpret_cast<float*>(Vector::Address(points));
	hd.mVertexStride = sizeof(Point3F);
	hd.mMaxVertices = 64;
	hd.mSkinWidth = 0.0f;

	CONVEX_DECOMPOSITION::HullLibrary hl;
	CONVEX_DECOMPOSITION::HullResult result;
	hl.CreateConvexHull(hd, result);

	// Create TSMesh from convex hull
	Mesh mesh;
	mesh.type = MeshFit::kHull;
	mesh.transform.Identity();
	mesh.tsmesh = CreateTriMesh(result.mOutputVertices, result.mNumOutputVertices,
		result.mIndices, result.mNumFaces);
	mesh.tsmesh->ComputeBounds();
	meshes_.push_back(mesh);
}

} // namespace DTS
