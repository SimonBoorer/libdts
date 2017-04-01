#include "DTSMesh.h"

#include "DTSDecal.h"
#include "DTSSortedMesh.h"
#include "DTSShape.h"

namespace DTS
{

#define tsalloc TSShape::alloc_

const Point3F TSMesh::kU8ToNormalTable[]{
	Point3F(0.565061f, -0.270644f, -0.779396f),
	Point3F(-0.309804f, -0.731114f, 0.607860f),
	Point3F(-0.867412f, 0.472957f, 0.154619f),
	Point3F(-0.757488f, 0.498188f, -0.421925f),
	Point3F(0.306834f, -0.915340f, 0.260778f),
	Point3F(0.098754f, 0.639153f, -0.762713f),
	Point3F(0.713706f, -0.558862f, -0.422252f),
	Point3F(-0.890431f, -0.407603f, -0.202466f),
	Point3F(0.848050f, -0.487612f, -0.207475f),
	Point3F(-0.232226f, 0.776855f, 0.585293f),
	Point3F(-0.940195f, 0.304490f, -0.152706f),
	Point3F(0.602019f, -0.491878f, -0.628991f),
	Point3F(-0.096835f, -0.494354f, -0.863850f),
	Point3F(0.026630f, -0.323659f, -0.945799f),
	Point3F(0.019208f, 0.909386f, 0.415510f),
	Point3F(0.854440f, 0.491730f, 0.167731f),
	Point3F(-0.418835f, 0.866521f, -0.271512f),
	Point3F(0.465024f, 0.409667f, 0.784809f),
	Point3F(-0.674391f, -0.691087f, -0.259992f),
	Point3F(0.303858f, -0.869270f, -0.389922f),
	Point3F(0.991333f, 0.090061f, -0.095640f),
	Point3F(-0.275924f, -0.369550f, 0.887298f),
	Point3F(0.426545f, -0.465962f, 0.775202f),
	Point3F(-0.482741f, -0.873278f, -0.065920f),
	Point3F(0.063616f, 0.932012f, -0.356800f),
	Point3F(0.624786f, -0.061315f, 0.778385f),
	Point3F(-0.530300f, 0.416850f, 0.738253f),
	Point3F(0.312144f, -0.757028f, -0.573999f),
	Point3F(0.399288f, -0.587091f, -0.704197f),
	Point3F(-0.132698f, 0.482877f, 0.865576f),
	Point3F(0.950966f, 0.306530f, 0.041268f),
	Point3F(-0.015923f, -0.144300f, 0.989406f),
	Point3F(-0.407522f, -0.854193f, 0.322925f),
	Point3F(-0.932398f, 0.220464f, 0.286408f),
	Point3F(0.477509f, 0.876580f, 0.059936f),
	Point3F(0.337133f, 0.932606f, -0.128796f),
	Point3F(-0.638117f, 0.199338f, 0.743687f),
	Point3F(-0.677454f, 0.445349f, 0.585423f),
	Point3F(-0.446715f, 0.889059f, -0.100099f),
	Point3F(-0.410024f, 0.909168f, 0.072759f),
	Point3F(0.708462f, 0.702103f, -0.071641f),
	Point3F(-0.048801f, -0.903683f, -0.425411f),
	Point3F(-0.513681f, -0.646901f, 0.563606f),
	Point3F(-0.080022f, 0.000676f, -0.996793f),
	Point3F(0.066966f, -0.991150f, -0.114615f),
	Point3F(-0.245220f, 0.639318f, -0.728793f),
	Point3F(0.250978f, 0.855979f, 0.452006f),
	Point3F(-0.123547f, 0.982443f, -0.139791f),
	Point3F(-0.794825f, 0.030254f, -0.606084f),
	Point3F(-0.772905f, 0.547941f, 0.319967f),
	Point3F(0.916347f, 0.369614f, -0.153928f),
	Point3F(-0.388203f, 0.105395f, 0.915527f),
	Point3F(-0.700468f, -0.709334f, 0.078677f),
	Point3F(-0.816193f, 0.390455f, 0.425880f),
	Point3F(-0.043007f, 0.769222f, -0.637533f),
	Point3F(0.911444f, 0.113150f, 0.395560f),
	Point3F(0.845801f, 0.156091f, -0.510153f),
	Point3F(0.829801f, -0.029340f, 0.557287f),
	Point3F(0.259529f, 0.416263f, 0.871418f),
	Point3F(0.231128f, -0.845982f, 0.480515f),
	Point3F(-0.626203f, -0.646168f, 0.436277f),
	Point3F(-0.197047f, -0.065791f, 0.978184f),
	Point3F(-0.255692f, -0.637488f, -0.726794f),
	Point3F(0.530662f, -0.844385f, -0.073567f),
	Point3F(-0.779887f, 0.617067f, -0.104899f),
	Point3F(0.739908f, 0.113984f, 0.662982f),
	Point3F(-0.218801f, 0.930194f, -0.294729f),
	Point3F(-0.374231f, 0.818666f, 0.435589f),
	Point3F(-0.720250f, -0.028285f, 0.693137f),
	Point3F(0.075389f, 0.415049f, 0.906670f),
	Point3F(-0.539724f, -0.106620f, 0.835063f),
	Point3F(-0.452612f, -0.754669f, -0.474991f),
	Point3F(0.682822f, 0.581234f, -0.442629f),
	Point3F(0.002435f, -0.618462f, -0.785811f),
	Point3F(-0.397631f, 0.110766f, -0.910835f),
	Point3F(0.133935f, -0.985438f, 0.104754f),
	Point3F(0.759098f, -0.608004f, 0.232595f),
	Point3F(-0.825239f, -0.256087f, 0.503388f),
	Point3F(0.101693f, -0.565568f, 0.818408f),
	Point3F(0.386377f, 0.793546f, -0.470104f),
	Point3F(-0.520516f, -0.840690f, 0.149346f),
	Point3F(-0.784549f, -0.479672f, 0.392935f),
	Point3F(-0.325322f, -0.927581f, -0.183735f),
	Point3F(-0.069294f, -0.428541f, 0.900861f),
	Point3F(0.993354f, -0.115023f, -0.004288f),
	Point3F(-0.123896f, -0.700568f, 0.702747f),
	Point3F(-0.438031f, -0.120880f, -0.890795f),
	Point3F(0.063314f, 0.813233f, 0.578484f),
	Point3F(0.322045f, 0.889086f, -0.325289f),
	Point3F(-0.133521f, 0.875063f, -0.465228f),
	Point3F(0.637155f, 0.564814f, 0.524422f),
	Point3F(0.260092f, -0.669353f, 0.695930f),
	Point3F(0.953195f, 0.040485f, -0.299634f),
	Point3F(-0.840665f, -0.076509f, 0.536124f),
	Point3F(-0.971350f, 0.202093f, 0.125047f),
	Point3F(-0.804307f, -0.396312f, -0.442749f),
	Point3F(-0.936746f, 0.069572f, 0.343027f),
	Point3F(0.426545f, -0.465962f, 0.775202f),
	Point3F(0.794542f, -0.227450f, 0.563000f),
	Point3F(-0.892172f, 0.091169f, -0.442399f),
	Point3F(-0.312654f, 0.541264f, 0.780564f),
	Point3F(0.590603f, -0.735618f, -0.331743f),
	Point3F(-0.098040f, -0.986713f, 0.129558f),
	Point3F(0.569646f, 0.283078f, -0.771603f),
	Point3F(0.431051f, -0.407385f, -0.805129f),
	Point3F(-0.162087f, -0.938749f, -0.304104f),
	Point3F(0.241533f, -0.359509f, 0.901341f),
	Point3F(-0.576191f, 0.614939f, 0.538380f),
	Point3F(-0.025110f, 0.085740f, 0.996001f),
	Point3F(-0.352693f, -0.198168f, 0.914515f),
	Point3F(-0.604577f, 0.700711f, 0.378802f),
	Point3F(0.465024f, 0.409667f, 0.784809f),
	Point3F(-0.254684f, -0.030474f, -0.966544f),
	Point3F(-0.604789f, 0.791809f, 0.085259f),
	Point3F(-0.705147f, -0.399298f, 0.585943f),
	Point3F(0.185691f, 0.017236f, -0.982457f),
	Point3F(0.044588f, 0.973094f, 0.226052f),
	Point3F(-0.405463f, 0.642367f, 0.650357f),
	Point3F(-0.563959f, 0.599136f, -0.568319f),
	Point3F(0.367162f, -0.072253f, -0.927347f),
	Point3F(0.960429f, -0.213570f, -0.178783f),
	Point3F(-0.192629f, 0.906005f, 0.376893f),
	Point3F(-0.199718f, -0.359865f, -0.911378f),
	Point3F(0.485072f, 0.121233f, -0.866030f),
	Point3F(0.467163f, -0.874294f, 0.131792f),
	Point3F(-0.638953f, -0.716603f, 0.279677f),
	Point3F(-0.622710f, 0.047813f, -0.780990f),
	Point3F(0.828724f, -0.054433f, -0.557004f),
	Point3F(0.130241f, 0.991080f, 0.028245f),
	Point3F(0.310995f, -0.950076f, -0.025242f),
	Point3F(0.818118f, 0.275336f, 0.504850f),
	Point3F(0.676328f, 0.387023f, 0.626733f),
	Point3F(-0.100433f, 0.495114f, -0.863004f),
	Point3F(-0.949609f, -0.240681f, -0.200786f),
	Point3F(-0.102610f, 0.261831f, -0.959644f),
	Point3F(-0.845732f, -0.493136f, 0.203850f),
	Point3F(0.672617f, -0.738838f, 0.041290f),
	Point3F(0.380465f, 0.875938f, 0.296613f),
	Point3F(-0.811223f, 0.262027f, -0.522742f),
	Point3F(-0.074423f, -0.775670f, -0.626736f),
	Point3F(-0.286499f, 0.755850f, -0.588735f),
	Point3F(0.291182f, -0.276189f, -0.915933f),
	Point3F(-0.638117f, 0.199338f, 0.743687f),
	Point3F(0.439922f, -0.864433f, -0.243359f),
	Point3F(0.177649f, 0.206919f, 0.962094f),
	Point3F(0.277107f, 0.948521f, 0.153361f),
	Point3F(0.507629f, 0.661918f, -0.551523f),
	Point3F(-0.503110f, -0.579308f, -0.641313f),
	Point3F(0.600522f, 0.736495f, -0.311364f),
	Point3F(-0.691096f, -0.715301f, -0.103592f),
	Point3F(-0.041083f, -0.858497f, 0.511171f),
	Point3F(0.207773f, -0.480062f, -0.852274f),
	Point3F(0.795719f, 0.464614f, 0.388543f),
	Point3F(-0.100433f, 0.495114f, -0.863004f),
	Point3F(0.703249f, 0.065157f, -0.707951f),
	Point3F(-0.324171f, -0.941112f, 0.096024f),
	Point3F(-0.134933f, -0.940212f, 0.312722f),
	Point3F(-0.438240f, 0.752088f, -0.492249f),
	Point3F(0.964762f, -0.198855f, 0.172311f),
	Point3F(-0.831799f, 0.196807f, 0.519015f),
	Point3F(-0.508008f, 0.819902f, 0.263986f),
	Point3F(0.471075f, -0.001146f, 0.882092f),
	Point3F(0.919512f, 0.246162f, -0.306435f),
	Point3F(-0.960050f, 0.279828f, -0.001187f),
	Point3F(0.110232f, -0.847535f, -0.519165f),
	Point3F(0.208229f, 0.697360f, 0.685806f),
	Point3F(-0.199680f, -0.560621f, 0.803637f),
	Point3F(0.170135f, -0.679985f, -0.713214f),
	Point3F(0.758371f, -0.494907f, 0.424195f),
	Point3F(0.077734f, -0.755978f, 0.649965f),
	Point3F(0.612831f, -0.672475f, 0.414987f),
	Point3F(0.142776f, 0.836698f, -0.528726f),
	Point3F(-0.765185f, 0.635778f, 0.101382f),
	Point3F(0.669873f, -0.419737f, 0.612447f),
	Point3F(0.593549f, 0.194879f, 0.780847f),
	Point3F(0.646930f, 0.752173f, 0.125368f),
	Point3F(0.837721f, 0.545266f, -0.030127f),
	Point3F(0.541505f, 0.768070f, 0.341820f),
	Point3F(0.760679f, -0.365715f, -0.536301f),
	Point3F(0.381516f, 0.640377f, 0.666605f),
	Point3F(0.565794f, -0.072415f, -0.821361f),
	Point3F(-0.466072f, -0.401588f, 0.788356f),
	Point3F(0.987146f, 0.096290f, 0.127560f),
	Point3F(0.509709f, -0.688886f, -0.515396f),
	Point3F(-0.135132f, -0.988046f, -0.074192f),
	Point3F(0.600499f, 0.476471f, -0.642166f),
	Point3F(-0.732326f, -0.275320f, -0.622815f),
	Point3F(-0.881141f, -0.470404f, 0.048078f),
	Point3F(0.051548f, 0.601042f, 0.797553f),
	Point3F(0.402027f, -0.763183f, 0.505891f),
	Point3F(0.404233f, -0.208288f, 0.890624f),
	Point3F(-0.311793f, 0.343843f, 0.885752f),
	Point3F(0.098132f, -0.937014f, 0.335223f),
	Point3F(0.537158f, 0.830585f, -0.146936f),
	Point3F(0.725277f, 0.298172f, -0.620538f),
	Point3F(-0.882025f, 0.342976f, -0.323110f),
	Point3F(-0.668829f, 0.424296f, -0.610443f),
	Point3F(-0.408835f, -0.476442f, -0.778368f),
	Point3F(0.809472f, 0.397249f, -0.432375f),
	Point3F(-0.909184f, -0.205938f, -0.361903f),
	Point3F(0.866930f, -0.347934f, -0.356895f),
	Point3F(0.911660f, -0.141281f, -0.385897f),
	Point3F(-0.431404f, -0.844074f, -0.318480f),
	Point3F(-0.950593f, -0.073496f, 0.301614f),
	Point3F(-0.719716f, 0.626915f, -0.298305f),
	Point3F(-0.779887f, 0.617067f, -0.104899f),
	Point3F(-0.475899f, -0.542630f, 0.692151f),
	Point3F(0.081952f, -0.157248f, -0.984153f),
	Point3F(0.923990f, -0.381662f, -0.024025f),
	Point3F(-0.957998f, 0.120979f, -0.260008f),
	Point3F(0.306601f, 0.227975f, -0.924134f),
	Point3F(-0.141244f, 0.989182f, 0.039601f),
	Point3F(0.077097f, 0.186288f, -0.979466f),
	Point3F(-0.630407f, -0.259801f, 0.731499f),
	Point3F(0.718150f, 0.637408f, 0.279233f),
	Point3F(0.340946f, 0.110494f, 0.933567f),
	Point3F(-0.396671f, 0.503020f, -0.767869f),
	Point3F(0.636943f, -0.245005f, 0.730942f),
	Point3F(-0.849605f, -0.518660f, -0.095724f),
	Point3F(-0.388203f, 0.105395f, 0.915527f),
	Point3F(-0.280671f, -0.776541f, -0.564099f),
	Point3F(-0.601680f, 0.215451f, -0.769131f),
	Point3F(-0.660112f, -0.632371f, -0.405412f),
	Point3F(0.921096f, 0.284072f, 0.266242f),
	Point3F(0.074850f, -0.300846f, 0.950731f),
	Point3F(0.943952f, -0.067062f, 0.323198f),
	Point3F(-0.917838f, -0.254589f, 0.304561f),
	Point3F(0.889843f, -0.409008f, 0.202219f),
	Point3F(-0.565849f, 0.753721f, -0.334246f),
	Point3F(0.791460f, 0.555918f, -0.254060f),
	Point3F(0.261936f, 0.703590f, -0.660568f),
	Point3F(-0.234406f, 0.952084f, 0.196444f),
	Point3F(0.111205f, 0.979492f, -0.168014f),
	Point3F(-0.869844f, -0.109095f, -0.481113f),
	Point3F(-0.337728f, -0.269701f, -0.901777f),
	Point3F(0.366793f, 0.408875f, -0.835634f),
	Point3F(-0.098749f, 0.261316f, 0.960189f),
	Point3F(-0.272379f, -0.847100f, 0.456324f),
	Point3F(-0.319506f, 0.287444f, -0.902935f),
	Point3F(0.873383f, -0.294109f, 0.388203f),
	Point3F(-0.088950f, 0.710450f, 0.698104f),
	Point3F(0.551238f, -0.786552f, 0.278340f),
	Point3F(0.724436f, -0.663575f, -0.186712f),
	Point3F(0.529741f, -0.606539f, 0.592861f),
	Point3F(-0.949743f, -0.282514f, 0.134809f),
	Point3F(0.155047f, 0.419442f, -0.894443f),
	Point3F(-0.562653f, -0.329139f, -0.758346f),
	Point3F(0.816407f, -0.576953f, 0.024576f),
	Point3F(0.178550f, -0.950242f, -0.255266f),
	Point3F(0.479571f, 0.706691f, 0.520192f),
	Point3F(0.391687f, 0.559884f, -0.730145f),
	Point3F(0.724872f, -0.205570f, -0.657496f),
	Point3F(-0.663196f, -0.517587f, -0.540624f),
	Point3F(-0.660054f, -0.122486f, -0.741165f),
	Point3F(-0.531989f, 0.374711f, -0.759328f),
	Point3F(0.194979f, -0.059120f, 0.979024f)
};

// structures used to share data between detail levels...
// used (and valid) during load only
std::vector<Point3F*>	TSMesh::verts_list_;
std::vector<Point3F*>	TSMesh::norms_list_;
std::vector<uint8_t*>	TSMesh::encoded_norms_list_;
std::vector<Point2F*>	TSMesh::tverts_list_;

std::vector<bool>		TSMesh::data_copied_;

std::vector<MatrixF*>	TSSkinMesh::init_transform_list_;
std::vector<int32_t*>	TSSkinMesh::vertex_index_list_;
std::vector<int32_t*>	TSSkinMesh::bone_index_list_;
std::vector<float*>		TSSkinMesh::weight_list_;
std::vector<int32_t*>	TSSkinMesh::node_index_list_;

TSMesh::TSMesh() :
	mesh_type_(kStandardMeshType)
{
	parent_mesh_ = -1;
}

void TSMesh::ComputeBounds()
{
	MatrixF mat(true);
	ComputeBounds(mat, bounds_, -1, &center_, &radius_);
}

void TSMesh::ComputeBounds(const MatrixF& transform, Box3F& bounds, int32_t frame, Point3F* center, float* radius)
{
	const Point3F* base_vert = Vector::Address(verts_);
	int32_t stride = sizeof(Point3F);
	int32_t num_verts = 0;

	if (frame < 0)
		num_verts = verts_.size();
	else
	{
		base_vert += frame * verts_per_frame_;
		num_verts = verts_per_frame_;
	}

	ComputeBounds(base_vert, num_verts, stride, transform, bounds, center, radius);
}

void TSMesh::ComputeBounds(const Point3F* v, int32_t num_verts, int32_t stride, const MatrixF& transform, Box3F& bounds, Point3F* center, float* radius)
{
	const uint8_t* vb = reinterpret_cast<const uint8_t*>(v);

	if (!num_verts)
	{
		bounds.min_extents = Point3F::kZero;
		bounds.max_extents = Point3F::kZero;
		if (center)
			*center = Point3F::kZero;
		if (radius)
			*radius = 0;
		return;
	}

	int32_t i;
	Point3F p;
	transform.MulP(*v, &bounds.min_extents);
	bounds.max_extents = bounds.min_extents;
	for (i = 0; i < num_verts; i++)
	{
		const Point3F &cur_vert = *reinterpret_cast<const Point3F *>(vb + i * stride);
		transform.MulP(cur_vert, &p);
		bounds.max_extents.SetMax(p);
		bounds.min_extents.SetMin(p);
	}
	Point3F c;
	if (!center)
		center = &c;
	center->x = 0.5f * (bounds.min_extents.x + bounds.max_extents.x);
	center->y = 0.5f * (bounds.min_extents.y + bounds.max_extents.y);
	center->z = 0.5f * (bounds.min_extents.z + bounds.max_extents.z);
	if (radius)
	{
		*radius = 0.0f;
		for (i = 0; i < num_verts; i++)
		{
			const Point3F &curVert = *reinterpret_cast<const Point3F *>(vb + i * stride);
			transform.MulP(curVert, &p);
			p -= *center;
			*radius = std::max(*radius, Math::Dot(p, p));
		}
		*radius = sqrt(*radius);
	}
}

uint8_t TSMesh::EncodeNormal(const Point3F& normal)
{
	uint8_t best_index = 0;
	float best_dot = -10E30f;
	for (uint32_t i = 0; i < 256; i++)
	{
		float dot = Math::Dot(normal, kU8ToNormalTable[i]);
		if (dot > best_dot)
		{
			best_index = i;
			best_dot = dot;
		}
	}
	return best_index;
}

TSMesh* TSMesh::AssembleMesh(uint32_t mesh_type, bool skip)
{
	static TSMesh temp_standard_mesh;
	static TSSkinMesh temp_skin_mesh;
	static TSDecalMesh temp_decal_mesh;
	static TSSortedMesh temp_sorted_mesh;

	bool just_size = skip | !tsalloc.IMemBuffer32::AllocShape(0); // if this returns NULL, we're just sizing memory block

	// a little funny business because we pretend decals are derived from meshes
	int32_t* ret = nullptr;
	TSMesh* mesh = nullptr;
	TSDecalMesh* decal = nullptr;

	if (just_size)
	{
		switch (mesh_type)
		{
			case kStandardMeshType:
			{
				ret = reinterpret_cast<int32_t*>(&temp_standard_mesh);
				mesh = &temp_standard_mesh;
				tsalloc.IMemBuffer32::AllocShape(sizeof(TSMesh) >> 2);
				break;
			}
			case kSkinMeshType:
			{
				ret = reinterpret_cast<int32_t*>(&temp_skin_mesh);
				mesh = &temp_skin_mesh;
				tsalloc.IMemBuffer32::AllocShape(sizeof(TSSkinMesh) >> 2);
				break;
			}
			case kDecalMeshType:
			{
				ret = reinterpret_cast<int32_t*>(&temp_decal_mesh);
				decal = &temp_decal_mesh;
				tsalloc.IMemBuffer32::AllocShape(sizeof(TSDecalMesh) >> 2);
				break;
			}
			case kSortedMeshType:
			{
				ret = reinterpret_cast<int32_t*>(&temp_sorted_mesh);
				mesh = &temp_sorted_mesh;
				tsalloc.IMemBuffer32::AllocShape(sizeof(TSSortedMesh) >> 2);
				break;
			}
		}
	}
	else
	{
		switch (mesh_type)
		{
			case kStandardMeshType:
			{
				ret = tsalloc.IMemBuffer32::AllocShape(sizeof(TSMesh) >> 2);
				new (ret) TSMesh;
				mesh = reinterpret_cast<TSMesh*>(ret);
				break;
			}
			case kSkinMeshType:
			{
				ret = tsalloc.IMemBuffer32::AllocShape(sizeof(TSSkinMesh) >> 2);
				new (ret) TSSkinMesh;
				mesh = reinterpret_cast<TSSkinMesh*>(ret);
				break;
			}
			case kDecalMeshType:
			{
				ret = tsalloc.IMemBuffer32::AllocShape(sizeof(TSDecalMesh) >> 2);
				new (ret) TSDecalMesh;
				decal = reinterpret_cast<TSDecalMesh*>(ret);
				break;
			}
			case kSortedMeshType:
			{
				ret = tsalloc.IMemBuffer32::AllocShape(sizeof(TSSortedMesh) >> 2);
				new (ret) TSSortedMesh;
				mesh = reinterpret_cast<TSSortedMesh*>(ret);
				break;
			}
		}
	}

	tsalloc.SetSkipMode(skip);

	if (mesh)
		mesh->Assemble(skip);

	if (decal)
		decal->Assemble(skip);

	tsalloc.SetSkipMode(false);

	return reinterpret_cast<TSMesh*>(ret);
}

void TSMesh::Assemble(bool skip)
{
	tsalloc.CheckGuard();

	num_frames_ = tsalloc.IMemBuffer32::Get();
	num_mat_frames_ = tsalloc.IMemBuffer32::Get();
	parent_mesh_ = tsalloc.IMemBuffer32::Get();
	tsalloc.IMemBuffer32::Get(reinterpret_cast<int32_t*>(&bounds_), 6);
	tsalloc.IMemBuffer32::Get(reinterpret_cast<int32_t*>(&center_), 3);
	radius_ = static_cast<float>(tsalloc.IMemBuffer32::Get());

	int32_t num_verts = tsalloc.IMemBuffer32::Get();
	int32_t* ptr32 = GetSharedData<int32_t>(parent_mesh_, 3 * num_verts, reinterpret_cast<int32_t**>(Vector::Address(verts_list_)), skip);
	Vector::Set(verts_, ptr32, num_verts);

	int32_t num_tverts = tsalloc.IMemBuffer32::Get();
	ptr32 = GetSharedData<int32_t>(parent_mesh_, 2 * num_tverts, reinterpret_cast<int32_t**>(Vector::Address(tverts_list_)), skip);
	Vector::Set(tverts_, ptr32, num_tverts);

	int8_t* ptr8;
	if (TSShape::current_read_version_ > 21)
	{
		ptr32 = GetSharedData<int32_t>(parent_mesh_, 3 * num_verts, reinterpret_cast<int32_t**>(Vector::Address(norms_list_)), skip);
		Vector::Set(norms_, ptr32, num_verts);

		ptr8 = GetSharedData<int8_t>(parent_mesh_, num_verts, reinterpret_cast<int8_t**>(Vector::Address(encoded_norms_list_)), skip);
		Vector::Set(encoded_norms_, ptr8, num_verts);
	}
	else
	{
		// no encoded normals...
		ptr32 = tsalloc.IMemBuffer32::CopyToShape(3 * num_verts);
		Vector::Set(norms_, ptr32, num_verts);
		Vector::Set(encoded_norms_, nullptr, 0);
	}

	int32_t szPrim, szInd;
	TSDrawPrimitive* prim;
	int32_t *ind;

	// mesh primitives (start, num_elements) indices are stored as 16 bit values
	szPrim = tsalloc.IMemBuffer32::Get();
	int16_t* prim16 = tsalloc.IMemBuffer16::GetPointer(szPrim * 2);	// primitive: start, num_elements
	int32_t* prim32 = tsalloc.IMemBuffer32::GetPointer(szPrim);		// primitive: mat_index
	szInd = tsalloc.IMemBuffer32::Get();

	int16_t* ind16 = tsalloc.IMemBuffer16::GetPointer(szInd);

	// need to copy to temporary arrays
	prim = new TSDrawPrimitive[szPrim];
	for (int32_t i = 0; i < szPrim; i++)
	{
		prim[i].start = prim16[i * 2];
		prim[i].num_elements = prim16[i * 2 + 1];
		prim[i].mat_index = prim32[i];
	}

	ind = new int32_t[szInd];
	for (int32_t i = 0; i < szInd; i++)
		ind[i] = static_cast<int32_t>(ind16[i]);

	// store output
	Vector::Set(primitives_, prim, szPrim);
	Vector::Set(indices_, ind, szInd);

	// delete temporary arrays if necessary
	delete[] prim;
	delete[] ind;

	int32_t sz = tsalloc.IMemBuffer32::Get();
	tsalloc.IMemBuffer16::GetPointer(sz); // skip deprecated merge indices
	tsalloc.Align32();

	verts_per_frame_ = tsalloc.IMemBuffer32::Get();
	uint32_t flags = (uint32_t)tsalloc.IMemBuffer32::Get();

	SetFlags(flags);

	tsalloc.CheckGuard();
}

void TSMesh::Disassemble()
{
	tsalloc.SetGuard();

	tsalloc.OMemBuffer32::Set(num_frames_);
	tsalloc.OMemBuffer32::Set(num_mat_frames_);
	tsalloc.OMemBuffer32::Set(parent_mesh_);
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(&bounds_), 6);
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(&center_), 3);
	tsalloc.OMemBuffer32::Set(static_cast<int32_t>(radius_));

	// verts...
	tsalloc.OMemBuffer32::Set(verts_.size());
	if (parent_mesh_ < 0)
		tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(verts_)), 3 * verts_.size()); // if no parent mesh, then save off our verts

	// tverts...
	tsalloc.OMemBuffer32::Set(tverts_.size());
	if (parent_mesh_ < 0)
		tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(tverts_)), 2 * tverts_.size()); // if no parent mesh, then save off our verts

	// norms...
	if (parent_mesh_ < 0) // if no parent mesh, then save off our norms
		tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(norms_)), 3 * norms_.size());  // norms.size()==verts.size() or error...

	// encoded norms...
	if (parent_mesh_ < 0)
	{
		// if no parent mesh, compute encoded normals and copy over
		for (int32_t i = 0; i < norms_.size(); i++)
		{
			uint8_t norm_idx = encoded_norms_.size() ? encoded_norms_[i] : EncodeNormal(norms_[i]);
			tsalloc.OMemBuffer8::CopyToBuffer(reinterpret_cast<int8_t*>(&norm_idx), 1);
		}
	}

	// primitives
	tsalloc.OMemBuffer32::Set(primitives_.size());
	for (int32_t i = 0; i < primitives_.size(); i++)
	{
		int16_t start = static_cast<int16_t>(primitives_[i].start);
		int16_t num_elements = static_cast<int16_t>(primitives_[i].num_elements);

		tsalloc.OMemBuffer16::CopyToBuffer(&start, 1);
		tsalloc.OMemBuffer16::CopyToBuffer(&num_elements, 1);
		tsalloc.OMemBuffer32::CopyToBuffer(&(primitives_[i].mat_index), 1);
	}

	// indices
	tsalloc.OMemBuffer32::Set(indices_.size());
	std::vector<int16_t> s16_indices;
	s16_indices.reserve(indices_.size());
	for (int32_t i = 0; i < indices_.size(); i++)
		s16_indices.push_back(static_cast<int16_t>(indices_[i]));
	tsalloc.OMemBuffer16::CopyToBuffer(reinterpret_cast<int16_t*>(Vector::Address(s16_indices)), s16_indices.size());

	// merge indices...DEPRECATED
	tsalloc.OMemBuffer32::Set(0);

	// small stuff...
	tsalloc.OMemBuffer32::Set(verts_per_frame_);
	tsalloc.OMemBuffer32::Set(GetFlags());

	tsalloc.SetGuard();
}

void TSMesh::CopySourceVertexDataFrom(const TSMesh* src_mesh)
{
	verts_ = src_mesh->verts_;
	tverts_ = src_mesh->tverts_;
	norms_ = src_mesh->norms_;
}

void TSSkinMesh::CopySourceVertexDataFrom(const TSMesh* src_mesh)
{
	TSMesh::CopySourceVertexDataFrom(src_mesh);

	if (src_mesh->GetMeshType() == TSMesh::kSkinMeshType)
	{
		const TSSkinMesh* src_skin_mesh = static_cast<const TSSkinMesh*>(src_mesh);

		weight_ = src_skin_mesh->weight_;
		bone_index_ = src_skin_mesh->bone_index_;
		vertex_index_ = src_skin_mesh->vertex_index_;
	}
}

TSSkinMesh::TSSkinMesh()
{
	mesh_type_ = kSkinMeshType;
}

void TSSkinMesh::Assemble(bool skip)
{
	TSMesh::Assemble(skip);

	int32_t sz = tsalloc.IMemBuffer32::Get();
	int32_t num_verts = sz;
	int32_t* ptr32 = GetSharedData<int32_t>(parent_mesh_, 3 * num_verts, reinterpret_cast<int32_t**>(Vector::Address(verts_list_)), skip);
	Vector::Set(initial_verts_, ptr32, sz);

	int8_t* ptr8;
	if (TSShape::current_read_version_ > 21)
	{
		ptr32 = GetSharedData<int32_t>(parent_mesh_, 3 * num_verts, reinterpret_cast<int32_t**>(Vector::Address(norms_list_)), skip);
		Vector::Set(initial_norms_, ptr32, num_verts);

		ptr8 = GetSharedData<int8_t>(parent_mesh_, num_verts, reinterpret_cast<int8_t**>(Vector::Address(encoded_norms_list_)), skip);
		Vector::Set(encoded_norms_, ptr8, num_verts);
	}
	else
	{
		// no encoded normals...
		ptr32 = tsalloc.IMemBuffer32::CopyToShape(3 * num_verts);
		Vector::Set(initial_norms_, ptr32, num_verts);
		Vector::Set(encoded_norms_, nullptr, 0);
	}

	sz = tsalloc.IMemBuffer32::Get();
	ptr32 = GetSharedData<int32_t>(parent_mesh_, 16 * sz, reinterpret_cast<int32_t**>(Vector::Address(init_transform_list_)), skip);
	Vector::Set(initial_transforms_, ptr32, sz);

	sz = tsalloc.IMemBuffer32::Get();
	ptr32 = GetSharedData<int32_t>(parent_mesh_, sz, reinterpret_cast<int32_t**>(Vector::Address(vertex_index_list_)), skip);
	Vector::Set(vertex_index_, ptr32, sz);

	ptr32 = GetSharedData<int32_t>(parent_mesh_, sz, reinterpret_cast<int32_t**>(Vector::Address(bone_index_list_)), skip);
	Vector::Set(bone_index_, ptr32, sz);

	ptr32 = GetSharedData<int32_t>(parent_mesh_, sz, reinterpret_cast<int32_t**>(Vector::Address(weight_list_)), skip);
	Vector::Set(weight_, ptr32, sz);

	sz = tsalloc.IMemBuffer32::Get();
	ptr32 = GetSharedData<int32_t>(parent_mesh_, sz, reinterpret_cast<int32_t**>(Vector::Address(node_index_list_)), skip);
	Vector::Set(node_index_, ptr32, sz);

	tsalloc.CheckGuard();
}

void TSSkinMesh::Disassemble()
{
	TSMesh::Disassemble();

	tsalloc.OMemBuffer32::Set(initial_verts_.size());
	// if we have no parent mesh, then save off our verts & norms
	if (parent_mesh_ < 0)
	{
		tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(verts_)), 3 * verts_.size());

		// no longer do this here...let tsmesh handle this
		tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(norms_)), 3 * norms_.size());

		// if no parent mesh, compute encoded normals and copy over
		for (int32_t i = 0; i < norms_.size(); i++)
		{
			uint8_t norm_idx = encoded_norms_.size() ? encoded_norms_[i] : EncodeNormal(norms_[i]);
			tsalloc.OMemBuffer8::CopyToBuffer(reinterpret_cast<int8_t*>(&norm_idx), 1);
		}
	}

	tsalloc.OMemBuffer32::Set(initial_transforms_.size());
	if (parent_mesh_ < 0)
		tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(initial_transforms_)), initial_transforms_.size() * 16);


	tsalloc.OMemBuffer32::Set(vertex_index_.size());
	if (parent_mesh_ < 0)
	{
		tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(vertex_index_)), vertex_index_.size());

		tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(bone_index_)), bone_index_.size());

		tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(weight_)), weight_.size());
	}

	tsalloc.OMemBuffer32::Set(node_index_.size());
	if (parent_mesh_ < 0)
		tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(node_index_)), node_index_.size());

	tsalloc.SetGuard();
}

} // namespace DTS
