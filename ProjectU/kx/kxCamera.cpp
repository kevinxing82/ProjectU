#include "kxCamera.h"
#include <math.h>

using namespace KevinX;

kxCamera::kxCamera()
{
}

kxCamera::kxCamera(int attr, kxVector4* pos, kxVector4* dir, kxVector4* target, float near_clip_z, float far_clip_z, float fov, float viewport_width, float viewport_height)
{
	this->attr = attr;
	this->pos = kxVector4(*pos);
	this->dir = kxVector4(*dir);
	this->target = kxVector4(*target);

	this->u =  kxVector4(1, 0, 0, 0);
	this->v =  kxVector4(0, 1, 0, 0);
	this->n =  kxVector4(0, 0, 1, 0);

	if (target != NULL)
	{
		this->target = kxVector4(*target);
	}
	else
	{
		this->target = kxVector4(0, 0, 0, 0);
	}

	this->near_clip_z = near_clip_z;
	this->far_clip_z = far_clip_z;

	this->viewport_width = viewport_width;
	this->viewport_height = viewport_height;

	this->viewport_center_x = (viewport_width - 1) / 2;
	this->viewport_center_y = (viewport_height - 1) / 2;

	this->aspect_ratio = (float)viewplane_width / (float)viewplane_height;

	this->mcam = kxMatrix4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	this->mper = kxMatrix4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	this->mscr =  kxMatrix4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	this->fov = fov;

	this->viewplane_width = 2.0;
	this->viewplane_height = 2.0 / this->aspect_ratio;

	float  tan_fov_div2 = tanf(DEG_TO_RAD(fov / 2));
	this->view_dist = (0.5)*(this->viewplane_width)*tan_fov_div2;
}

int kxCamera::buildEulerMatrix(int camRotSeq)
{
	kxMatrix4 mt_inv,
		mx_inv,
		my_inv,
		mz_inv,
		mrot,
		mtmp;
	mt_inv = kxMatrix4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-pos.x, -pos.y, -pos.z, 1);

	float thetaX = dir.x;
	float thetaY = dir.y;
	float thetaZ = dir.z;

	float cosTheta = cosf(DEG_TO_RAD(thetaX));
	float sinTheta =- sinf(DEG_TO_RAD(thetaX));

	mx_inv = kxMatrix4(1, 0, 0, 0,
		0, cosTheta, sinTheta, 0,
		0, -sinTheta, cosTheta, 0,
		0, 0, 0, 1);

	cosTheta = cosf(DEG_TO_RAD(thetaY));
	sinTheta = -sinf(DEG_TO_RAD(thetaY));
	
	my_inv = kxMatrix4(
		cosTheta, 0, -sinTheta, 0,
		0, 1, 0, 0,
		sinTheta, 0, cosTheta, 0,
		0, 0, 0, 1);

	cosTheta = cosf(DEG_TO_RAD(thetaZ));
	sinTheta = -sinf(DEG_TO_RAD(thetaZ));

	mz_inv = kxMatrix4(
		cosTheta, sinTheta, 0, 0,
		-sinTheta, cosTheta, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	switch(camRotSeq)
	{
	case  CAM_ROT_SEQ_XYZ:
	{
		mrot = mx_inv*my_inv*mz_inv;
	}
	break;
	case CAM_ROT_SEQ_YXZ:
	{
		mrot = my_inv*mx_inv*mz_inv;
	}
	break;
	case CAM_ROT_SEQ_XZY:
	{
		mrot = mx_inv*mz_inv*my_inv;
	}
	break;
	case CAM_ROT_SEQ_YZX:
	{
		mrot = my_inv*mz_inv*mx_inv;
	}
	break;
	case CAM_ROT_SEQ_ZYX:
	{
		mrot = mz_inv*my_inv*mx_inv;
	}
	break;
	case CAM_ROT_SEQ_ZXY:
	{
		mrot = mz_inv*mx_inv*my_inv;
	}
	break;
	default:break;
	}
	mcam = mt_inv*mrot;
	return 0;
}
