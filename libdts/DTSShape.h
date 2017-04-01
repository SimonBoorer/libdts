#ifndef DTS_SHAPE_H_
#define DTS_SHAPE_H_

#include "DTSMesh.h"
#include "DTSIntegerSet.h"
#include "DTSShapeAlloc.h"

namespace DTS
{

class TSMaterialList;

// TSShape stores generic data for a 3space model.
class TSShape
{
public:
	enum
	{
		kUniformScale = 0x0001,
		kAlignedScale = 0x0002,
		kArbitraryScale = 0x0004,
		kBlend = 0x0008,
		kCyclic = 0x0010,
		kMakePath = 0x0020,
		kHasTranslucency = 0x0040
	};

	// Nodes hold the transforms in the shape's tree.  They are the bones of the skeleton.
	struct Node
	{
		int32_t name_index;
		int32_t parent_index;

		// computed at runtime
		int32_t first_object;
		int32_t first_child;
		int32_t next_sibling;
	};

	// Objects hold renderable items (in particular meshes).
	struct Object
	{
		int32_t name_index;
		int32_t num_meshes;
		int32_t start_mesh_index; // Index into meshes array.
		int32_t node_index;

		// computed at load
		int32_t next_sibling;
		int32_t first_decal; // DEPRECATED
	};

	// A Sequence holds all the information necessary to perform a particular animation (sequence).
	class Sequence
	{
	public:
		// IO
		bool LoadFromStream(IStream& is, bool read_name_index = true);
		bool WriteToStream(OStream& os, bool write_name_index = true);

		int32_t name_index_;
		int32_t num_keyframes_;
		float duration_;
		int32_t base_rotation_;
		int32_t base_translation_;
		int32_t base_scale_;
		int32_t base_object_state_;
		int32_t base_decal_state_; // DEPRECATED
		int32_t first_ground_frame_;
		int32_t num_ground_frames_;
		int32_t first_trigger_;
		int32_t num_triggers_;
		float tool_begin_;

		// Bitsets
		TSIntegerSet rotation_matters_;	// Set of nodes
		TSIntegerSet translation_matters_;	// Set of nodes
		TSIntegerSet scale_matters_;		// Set of nodes
		TSIntegerSet vis_matters_;			// Set of objects
		TSIntegerSet frame_matters_;		// Set of objects
		TSIntegerSet mat_frame_matters_;	// Set of objects
		
		int32_t priority_;
		uint32_t flags_;
	};

	// Describes state of an individual object.  Includes everything in an object that can be
	// controlled by animation.
	struct ObjectState
	{
		float vis;
		int32_t frame_index;
		int32_t mat_frame_index;
	};

	struct Trigger
	{
		enum TriggerStates
		{
			kStateOn = 0x80000000,
			kInvertOnReverse = 0x40000000,
			kStateMask = 0x3FFFFFFF
		};

		uint32_t state; // One of TriggerStates
		float pos;
	};

	// Details are used for render detail selection.
	struct Detail
	{
		int32_t name_index;
		int32_t sub_shape_num;
		int32_t object_detail_num;
		float size;
		float average_error;
		float max_error;
		int32_t poly_count;
	};

	// Version Info
	// Most recent version...the one we write
	static const int32_t kVersion;
	// Version currently being read, only valid during read
	static int32_t current_read_version_;
	static const int32_t kMostRecentExporterVersion;

	// constructor/destructor
	TSShape();
	TSShape(const std::string& filename);
	~TSShape();

	// Lookup Methods

	// Returns index into the name vector that equals the passed name.
	int32_t FindName(const std::string& name) const;

	int32_t FindNode(int32_t name_index) const;
	int32_t FindNode(const std::string& name) const { return FindNode(FindName(name)); }

	int32_t FindObject(int32_t name_index) const;
	int32_t FindObject(const std::string& name) const { return FindObject(FindName(name)); }

	int32_t GetSubShapeForNode(int32_t node_index);
	int32_t GetSubShapeForObject(int32_t obj_index);
	void GetSubShapeDetails(int32_t sub_shape_index, std::vector<int32_t>& valid_details);

	void GetNodeWorldTransform(int32_t node_index, MatrixF* mat) const;

	// Methods for saving/loading shapes to/from streams
	bool LoadFromFile(const std::string& filename);
	bool LoadFromStream(std::istream& is);

	bool WriteToFile(const std::string& filename);
	bool WriteToStream(std::ostream& os);

	// Persist Helper Functions
	void FixEndian(int32_t* buff32, int16_t* buff16, int8_t*, int32_t count32, int32_t count16, int32_t);

	//  Memory Buffer Transfer Methods
	void AssembleShape();
	void DisassembleShape();

	// Shape Editing
	int32_t AddName(const std::string& name);
	void UpdateSmallestVisibleDL();
	int32_t AddDetail(const std::string& dname, int32_t size, int32_t sub_shape_num);

	bool SetNodeTransform(const std::string& name, const Point3F& pos, const QuatF& rot);
	bool AddNode(const std::string& name, const std::string& parent_name, const Point3F& pos, const QuatF& rot);

	int32_t AddObject(const std::string& obj_name, int32_t sub_shape_index);
	void AddMeshToObject(int32_t obj_index, int32_t mesh_index, TSMesh* mesh);
	bool SetObjectNode(const std::string& obj_name, const std::string& node_name);

	TSMesh* CopyMesh(const TSMesh* src_mesh) const;
	bool AddMesh(TSMesh* mesh, const std::string& mesh_name);

	static TSShapeAlloc alloc_;

	// TSShape Vector Data
	std::vector<Node> nodes_;
	std::vector<Object> objects_;
	std::vector<ObjectState> object_states_;
	std::vector<int32_t> sub_shape_first_node_;
	std::vector<int32_t> sub_shape_first_object_;
	std::vector<int32_t> sub_shape_num_nodes_;
	std::vector<int32_t> sub_shape_num_objects_;
	std::vector<Detail>  details_;
	std::vector<Quat16> default_rotations_;
	std::vector<Point3F> default_translations_;

	std::vector<TSMesh*> meshes_;

	std::vector<Sequence> sequences_;
	std::vector<Quat16> node_rotations_;
	std::vector<Point3F> node_translations_;
	std::vector<float> node_uniform_scales_;
	std::vector<Point3F> node_aligned_scales_;
	std::vector<Quat16> node_arbitrary_scale_rots_;
	std::vector<Point3F> node_arbitrary_scale_factors_;
	std::vector<Quat16> ground_rotations_;
	std::vector<Point3F> ground_translations_;
	std::vector<Trigger> triggers_;
	std::vector<std::string> names_;

	TSMaterialList* material_list_;

	// Bounding
	float radius_;
	float tube_radius_;
	Point3F center_;
	Box3F bounds_;

	// various...
	uint32_t exporter_version_;
	float smallest_visible_size_; // Computed at load time from details vector.
	int32_t smallest_visible_dl_;
	int32_t read_version_; // File version that this shape was read from.

	int8_t* shape_data_;
	uint32_t shape_data_size_;
};

} // namespace DTS

#endif // DTS_SHAPE_H_
