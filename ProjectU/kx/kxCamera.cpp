#include "kxCamera.h"
#include <math.h>

using namespace KevinX;

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

	this->mcam = new kxMatrix4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	this->mper = new kxMatrix4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	this->mscr = new kxMatrix4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	this->fov = fov;

	this->viewplane_width = 2.0;
	this->viewplane_height = 2.0 / this->aspect_ratio;

	float  tan_fov_div2 = tanf(fov / 2);
	this->view_dist = (0.5)*(this->viewplane_width)*tan_fov_div2;
}
