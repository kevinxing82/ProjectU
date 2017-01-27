#pragma once

#include "kxGlobal.h"
#include "kxVector.h"
#include "kxGeometry.h"
#include "kxMatrix.h"


KX_BEGIN
	class kxCamera
	{
	public:
		kxCamera();
		kxCamera(int attr, kxVector4* pos, kxVector4* dir, kxVector4* target, float near_clip_z, float far_clip_z, float fov, float viewport_width, float viewport_height);
		int buildEulerMatrix(int camRotSeq);
	public:
		int state;
		int attr;

		kxVector4 pos;

		kxVector4 dir;

		kxVector4 u;
		kxVector4 v;
		kxVector4 n;

		kxVector4 target;

		float view_dist;

		float fov;

		float near_clip_z;
		float far_clip_z;

		kxPlane* rt_clip_plane;
		kxPlane* lt_clip_plane;
		kxPlane* tp_clip_plane;
		kxPlane* bp_clip_plane;

		float viewplane_width;
		float viewplane_height;

		float viewport_width;
		float viewport_height;
		float viewport_center_x;
		float viewport_center_y;

		float aspect_ratio;

		kxMatrix4* mcam;
		kxMatrix4* mper;
		kxMatrix4* mscr;
	};
KX_END