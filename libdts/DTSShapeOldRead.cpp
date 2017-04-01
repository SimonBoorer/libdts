#include "DTSShape.h"

#include "DTSStream.h"

namespace DTS
{

bool TSShape::Sequence::LoadFromStream(IStream& is, bool read_name_index)
{
	if (read_name_index)
		is.Read(&name_index_);
	flags_ = 0;
	if (TSShape::current_read_version_ > 21)
		is.Read(&flags_);
	else
		flags_ = 0;

	is.Read(&num_keyframes_);
	is.Read(&duration_);

	if (TSShape::current_read_version_ < 22)
	{
		bool tmp = false;
		is.Read(&tmp);
		if (tmp)
			flags_ |= kBlend;
		is.Read(&tmp);
		if (tmp)
			flags_ |= kCyclic;
		is.Read(&tmp);
		if (tmp)
			flags_ |= kMakePath;
	}

	is.Read(&priority_);
	is.Read(&first_ground_frame_);
	is.Read(&num_ground_frames_);
	if (TSShape::current_read_version_ > 21)
	{
		is.Read(&base_rotation_);
		is.Read(&base_translation_);
		is.Read(&base_scale_);
		is.Read(&base_object_state_);
		is.Read(&base_decal_state_);
	}
	else
	{
		is.Read(&base_rotation_);
		base_translation_ = base_rotation_;
		is.Read(&base_object_state_);
		is.Read(&base_decal_state_);
	}

	is.Read(&first_trigger_);
	is.Read(&num_triggers_);
	is.Read(&tool_begin_);

	// now the membership sets:
	rotation_matters_.LoadFromStream(is);
	if (TSShape::current_read_version_ < 22)
		translation_matters_ = rotation_matters_;
	else
	{
		translation_matters_.LoadFromStream(is);
		scale_matters_.LoadFromStream(is);
	}

	TSIntegerSet dummy;
	dummy.LoadFromStream(is); // DEPRECIATED: Decals
	dummy.LoadFromStream(is); // DEPRECIATED: Ifl materials

	vis_matters_.LoadFromStream(is);
	frame_matters_.LoadFromStream(is);
	mat_frame_matters_.LoadFromStream(is);

	return true;
}

bool TSShape::Sequence::WriteToStream(OStream& os, bool write_name_index)
{
	if (write_name_index)
		os.Write(name_index_);
	os.Write(flags_);
	os.Write(num_keyframes_);
	os.Write(duration_);
	os.Write(priority_);
	os.Write(first_ground_frame_);
	os.Write(num_ground_frames_);
	os.Write(base_rotation_);
	os.Write(base_translation_);
	os.Write(base_scale_);
	os.Write(base_object_state_);
	os.Write(base_decal_state_);
	os.Write(first_trigger_);
	os.Write(num_triggers_);
	os.Write(tool_begin_);

	// now the membership sets:
	rotation_matters_.WriteToStream(os);
	translation_matters_.WriteToStream(os);
	scale_matters_.WriteToStream(os);

	TSIntegerSet dummy;
	dummy.WriteToStream(os); // DEPRECIATED: Decals
	dummy.WriteToStream(os); // DEPRECIATED: Ifl materials

	vis_matters_.WriteToStream(os);
	frame_matters_.WriteToStream(os);
	mat_frame_matters_.WriteToStream(os);

	return true;
}

} // namespace DTS
