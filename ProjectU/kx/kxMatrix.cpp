#include "kxMatrix.h"
#include <cmath>
#include <algorithm>

USING_KX

const float DEG2RAD = 3.141593f / 180.0f;
const float RAD2DEG = 180.0f / 3.141593f;
const float EPSILON = 0.00001f;

kxMatrix22 & kxMatrix22::transpose()
{
	std::swap(m[1], m[2]);
	return *this;
}

kxMatrix22 & kxMatrix22::invert()
{
	float determinant = getDeterminant();
	if (fabs(determinant) <= EPSILON)
	{
		return identity();
	}

	float tmp = m[0];   // copy the first element
	float invDeterminant = 1.0f / determinant;
	m[0] = invDeterminant * m[3];
	m[1] = -invDeterminant * m[1];
	m[2] = -invDeterminant * m[2];
	m[3] = invDeterminant * tmp;

	return *this;
}

float kxMatrix22::getDeterminant() const
{
	return m[0] * m[3] - m[1] * m[2];
}

///////////////////////////////////////////////////////////////////////////////
// retrieve rotation angle in degree from rotation matrix, R
// R = | c -s |
//     | s  c |
// angle = atan(s / c)
///////////////////////////////////////////////////////////////////////////////
float kxMatrix22::getAngle() const
{
	// angle between -pi ~ +pi (-180 ~ +180)
	return RAD2DEG * atan2f(m[2], m[0]);
}

kxMatrix22 kxMatrix22::operator*(const kxMatrix22 & rhs) const
{
	const float x1((m[0] + m[3]) * (rhs[0] +rhs[3]));
	const float x2((m[2] + m[3]) * rhs[0]);
	const float x3(m[0] * (rhs[1] - rhs[3]));
	const float x4(m[3] * (rhs[2] - rhs[0]));
	const float x5((m[0] + m[1]) * rhs[3]);
	const float x6((m[2] - m[0]) * (rhs[0] + rhs[1]));
	const float x7((m[1] - m[3]) * (rhs[2] + rhs[3]));

	return kxMatrix22(x1 + x4 - x5 + x7, x3 + x5,
		x2 + x4, x1 - x2 + x3 + x6);
}

float kxMatrix33::getDeterminant() const
{
	return m[0] * m[4] * m[8] + m[1] * m[5] * m[6] + m[2] * m[3] * m[7] - m[2] * m[4] * m[6] - m[1] * m[3] * m[8] - m[0] * m[5] * m[7];
}

///////////////////////////////////////////////////////////////////////////////
// retrieve angles in degree from rotation matrix, M = Rx*Ry*Rz
// Rx: rotation about X-axis, pitch
// Ry: rotation about Y-axis, yaw(heading)
// Rz: rotation about Z-axis, roll
//    Rx           Ry          Rz
// |1  0   0| | Cy  0 Sy| |Cz -Sz 0|   | CyCz        -CySz         Sy  |
// |0 Cx -Sx|*|  0  1  0|*|Sz  Cz 0| = | SxSyCz+CxSz -SxSySz+CxCz -SxCy|
// |0 Sx  Cx| |-Sy  0 Cy| | 0   0 1|   |-CxSyCz+SxSz  CxSySz+SxCz  CxCy|
//
// Pitch: atan(-m[7] / m[8]) = atan(SxCy/CxCy)
// Yaw  : asin(m[6]) = asin(Sy)
// Roll : atan(-m[3] / m[0]) = atan(SzCy/CzCy)
///////////////////////////////////////////////////////////////////////////////
kxVector3 kxMatrix33::getAngle() const
{
	float pitch, yaw, roll;         // 3 angles

									// find yaw (around y-axis) first
									// NOTE: asin() returns -90~+90, so correct the angle range -180~+180
									// using z value of forward vector
	yaw = RAD2DEG * asinf(m[2]);
	if (m[8] < 0)
	{
		if (yaw >= 0) yaw = 180.0f - yaw;
		else         yaw = -180.0f - yaw;
	}

	// find roll (around z-axis) and pitch (around x-axis)
	// if forward vector is (1,0,0) or (-1,0,0), then m[0]=m[4]=m[9]=m[10]=0
	if (m[0] > -EPSILON && m[0] < EPSILON)
	{
		roll = 0;  //@@ assume roll=0
		pitch = RAD2DEG * atan2f(m[3], m[4]);
	}
	else
	{
		roll = RAD2DEG * atan2f(-m[1], m[0]);
		pitch = RAD2DEG * atan2f(-m[5], m[8]);
	}

	return kxVector3(pitch, yaw, roll);
}

kxMatrix33 & kxMatrix33::transpose()
{
	std::swap(m[1], m[3]);
	std::swap(m[2], m[6]);
	std::swap(m[5], m[7]);

	return *this;
}

kxMatrix33 & kxMatrix33::invert()
{
	float determinant, invDeterminant;
	float tmp[9];

	tmp[0] = m[4] * m[8] - m[5] * m[7];
	tmp[1] = m[2] * m[7] - m[1] * m[8];
	tmp[2] = m[1] * m[5] - m[2] * m[4];
	tmp[3] = m[5] * m[6] - m[3] * m[8];
	tmp[4] = m[0] * m[8] - m[2] * m[6];
	tmp[5] = m[2] * m[3] - m[0] * m[5];
	tmp[6] = m[3] * m[7] - m[4] * m[6];
	tmp[7] = m[1] * m[6] - m[0] * m[7];
	tmp[8] = m[0] * m[4] - m[1] * m[3];

	// check determinant if it is 0
	determinant = m[0] * tmp[0] + m[3] * tmp[3] + m[6] * tmp[6];
	if (fabs(determinant) <= EPSILON)
	{
		return identity(); // cannot inverse, make it idenety matrix
	}

	// divide by the determinant
	invDeterminant = 1.0f / determinant;
	m[0] = invDeterminant * tmp[0];
	m[1] = invDeterminant * tmp[1];
	m[2] = invDeterminant * tmp[2];
	m[3] = invDeterminant * tmp[3];
	m[4] = invDeterminant * tmp[4];
	m[5] = invDeterminant * tmp[5];
	m[6] = invDeterminant * tmp[6];
	m[7] = invDeterminant * tmp[7];
	m[8] = invDeterminant * tmp[8];

	return *this;
}

kxMatrix44 & kxMatrix44::transpose()
{
	std::swap(m[1], m[4]);
	std::swap(m[2], m[8]);
	std::swap(m[3], m[12]);
	std::swap(m[6], m[9]);
	std::swap(m[7], m[13]);
	std::swap(m[11], m[14]);

	return *this;
}

float kxMatrix44::getDeterminant() const
{
	return m[0] * (m[4] * m[8] - m[5] * m[7]) -
		m[1] * (m[3] * m[8] - m[5] * m[6]) +
		m[2] * (m[3] * m[7] - m[4] * m[6]);
}

kxMatrix33 kxMatrix44::getRotationMatrix() const
{
	return kxMatrix33(m[0], m[1], m[2],
		m[4], m[5], m[6],
		m[8], m[9], m[10]);
}

///////////////////////////////////////////////////////////////////////////////
// retrieve angles in degree from rotation matrix, M = Rx*Ry*Rz
// Rx: rotation about X-axis, pitch
// Ry: rotation about Y-axis, yaw(heading)
// Rz: rotation about Z-axis, roll
//    Rx           Ry          Rz
// |1  0   0| | Cy  0 Sy| |Cz -Sz 0|   | CyCz        -CySz         Sy  |
// |0 Cx -Sx|*|  0  1  0|*|Sz  Cz 0| = | SxSyCz+CxSz -SxSySz+CxCz -SxCy|
// |0 Sx  Cx| |-Sy  0 Cy| | 0   0 1|   |-CxSyCz+SxSz  CxSySz+SxCz  CxCy|
//
// Pitch: atan(-m[9] / m[10]) = atan(SxCy/CxCy)
// Yaw  : asin(m[8]) = asin(Sy)
// Roll : atan(-m[4] / m[0]) = atan(SzCy/CzCy)
///////////////////////////////////////////////////////////////////////////////
kxVector3 kxMatrix44::getAngle() const
{
	float pitch, yaw, roll;         // 3 angles

									// find yaw (around y-axis) first
									// NOTE: asin() returns -90~+90, so correct the angle range -180~+180
									// using z value of forward vector
	yaw = RAD2DEG * asinf(m[2]);
	if (m[10] < 0)
	{
		if (yaw >= 0) yaw = 180.0f - yaw;
		else         yaw = -180.0f - yaw;
	}

	// find roll (around z-axis) and pitch (around x-axis)
	// if forward vector is (1,0,0) or (-1,0,0), then m[0]=m[4]=m[9]=m[10]=0
	if (m[0] > -EPSILON && m[0] < EPSILON)
	{
		roll = 0;  //@@ assume roll=0
		pitch = RAD2DEG * atan2f(m[4], m[5]);
	}
	else
	{
		roll = RAD2DEG * atan2f(-m[1], m[0]);
		pitch = RAD2DEG * atan2f(-m[6], m[10]);
	}

	return kxVector3(pitch, yaw, roll);
}

kxMatrix44 & kxMatrix44::invert()
{
	// If the 4th row is [0,0,0,1] then it is affine matrix and
	// it has no projective transformation.
	if (m[12] == 0 && m[13] == 0 && m[14] == 0 && m[15] == 1)
		this->invertAffine();
	else
	{
		this->invertGeneral();
		/*@@ invertProjective() is not optimized (slower than generic one)
		if(fabs(m[0]*m[5] - m[1]*m[4]) > EPSILON)
		this->invertProjective();   // inverse using matrix partition
		else
		this->invertGeneral();      // generalized inverse
		*/
	}

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// compute the inverse of 4x4 Euclidean transformation matrix
//
// Euclidean transformation is translation, rotation, and reflection.
// With Euclidean transform, only the position and orientation of the object
// will be changed. Euclidean transform does not change the shape of an object
// (no scaling). Length and angle are reserved.
//
// Use inverseAffine() if the matrix has scale and shear transformation.
//
// M = [ R | T ]
//     [ --+-- ]    (R denotes 3x3 rotation/reflection matrix)
//     [ 0 | 1 ]    (T denotes 1x3 translation matrix)
//
// y = M*x  ->  y = R*x + T  ->  x = R^-1*(y - T)  ->  x = R^T*y - R^T*T
// (R is orthogonal,  R^-1 = R^T)
//
//  [ R | T ]-1    [ R^T | -R^T * T ]    (R denotes 3x3 rotation matrix)
//  [ --+-- ]   =  [ ----+--------- ]    (T denotes 1x3 translation)
//  [ 0 | 1 ]      [  0  |     1    ]    (R^T denotes R-transpose)
///////////////////////////////////////////////////////////////////////////////
kxMatrix44 & kxMatrix44::invertEuclidean()
{
	// transpose 3x3 rotation matrix part
	// | R^T | 0 |
	// | ----+-- |
	// |  0  | 1 |
	float tmp;
	tmp = m[1];  m[1] = m[4];  m[4] = tmp;
	tmp = m[2];  m[2] = m[8];  m[8] = tmp;
	tmp = m[6];  m[6] = m[9];  m[9] = tmp;

	// compute translation part -R^T * T
	// | 0 | -R^T x |
	// | --+------- |
	// | 0 |   0    |
	float x = m[3];
	float y = m[7];
	float z = m[11];
	m[3] = -(m[0] * x + m[1] * y + m[2] * z);
	m[7] = -(m[4] * x + m[5] * y + m[6] * z);
	m[11] = -(m[8] * x + m[9] * y + m[10] * z);

	// last row should be unchanged (0,0,0,1)

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// compute the inverse of a 4x4 affine transformation matrix
//
// Affine transformations are generalizations of Euclidean transformations.
// Affine transformation includes translation, rotation, reflection, scaling,
// and shearing. Length and angle are NOT preserved.
// M = [ R | T ]
//     [ --+-- ]    (R denotes 3x3 rotation/scale/shear matrix)
//     [ 0 | 1 ]    (T denotes 1x3 translation matrix)
//
// y = M*x  ->  y = R*x + T  ->  x = R^-1*(y - T)  ->  x = R^-1*y - R^-1*T
//
//  [ R | T ]-1   [ R^-1 | -R^-1 * T ]
//  [ --+-- ]   = [ -----+---------- ]
//  [ 0 | 1 ]     [  0   +     1     ]
///////////////////////////////////////////////////////////////////////////////
kxMatrix44 & kxMatrix44::invertAffine()
{
	// R^-1
	kxMatrix33 r(m[0], m[1], m[2], m[4], m[5], m[6], m[8], m[9], m[10]);
	r.invert();
	m[0] = r[0];  m[1] = r[1];  m[2] = r[2];
	m[4] = r[3];  m[5] = r[4];  m[6] = r[5];
	m[8] = r[6];  m[9] = r[7];  m[10] = r[8];

	// -R^-1 * T
	float x = m[3];
	float y = m[7];
	float z = m[11];
	m[3] = -(r[0] * x + r[1] * y + r[2] * z);
	m[7] = -(r[3] * x + r[4] * y + r[5] * z);
	m[11] = -(r[6] * x + r[7] * y + r[8] * z);

	// last row should be unchanged (0,0,0,1)
	//m[3] = m[7] = m[11] = 0.0f;
	//m[15] = 1.0f;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// inverse matrix using matrix partitioning (blockwise inverse)
// It devides a 4x4 matrix into 4 of 2x2 matrices. It works in case of where
// det(A) != 0. If not, use the generic inverse method
// inverse formula.
// M = [ A | B ]    A, B, C, D are 2x2 matrix blocks
//     [ --+-- ]    det(M) = |A| * |D - ((C * A^-1) * B)|
//     [ C | D ]
//
// M^-1 = [ A' | B' ]   A' = A^-1 - (A^-1 * B) * C'
//        [ ---+--- ]   B' = (A^-1 * B) * -D'
//        [ C' | D' ]   C' = -D' * (C * A^-1)
//                      D' = (D - ((C * A^-1) * B))^-1
//
// NOTE: I wrap with () if it it used more than once.
//       The matrix is invertable even if det(A)=0, so must check det(A) before
//       calling this function, and use invertGeneric() instead.
///////////////////////////////////////////////////////////////////////////////
kxMatrix44 & kxMatrix44::invertProjective()
{
	// partition
	kxMatrix22 a(m[0], m[1], m[4], m[5]);
	kxMatrix22 b(m[8], m[9], m[12], m[13]);
	kxMatrix22 c(m[2], m[3], m[6], m[7]);
	kxMatrix22 d(m[10], m[11], m[14], m[15]);

	// pre-compute repeated parts
	a.invert();             // A^-1
	kxMatrix22 ab = a * b;     // A^-1 * B
	kxMatrix22 ca = c * a;     // C * A^-1
	kxMatrix22 cab = ca * b;   // C * A^-1 * B
	kxMatrix22 dcab = d - cab; // D - C * A^-1 * B

							  // check determinant if |D - C * A^-1 * B| = 0
							  //NOTE: this function assumes det(A) is already checked. if |A|=0 then,
							  //      cannot use this function.
	float determinant = dcab[0] * dcab[3] - dcab[1] * dcab[2];
	if (fabs(determinant) <= EPSILON)
	{
		return identity();
	}

	// compute D' and -D'
	kxMatrix22 d1 = dcab;      //  (D - C * A^-1 * B)
	d1.invert();            //  (D - C * A^-1 * B)^-1
	kxMatrix22 d2 = -d1;       // -(D - C * A^-1 * B)^-1

							  // compute C'
	kxMatrix22 c1 = d2 * ca;   // -D' * (C * A^-1)

							  // compute B'
	kxMatrix22 b1 = ab * d2;   // (A^-1 * B) * -D'

							  // compute A'
	kxMatrix22 a1 = a - (ab * c1); // A^-1 - (A^-1 * B) * C'

								  // assemble inverse matrix
	m[0] = a1[0];  m[1] = a1[1];        /*|*/ m[2] = b1[0];  m[3] = b1[1];
	m[4] = a1[2];  m[5] = a1[3];       /*|*/ m[6] = b1[2];  m[7] = b1[3];
	/*-----------------------------+-----------------------------*/
	m[8] = c1[0];  m[9] = c1[1];      /*|*/ m[10] = d1[0];  m[11] = d1[1];
	m[12] = c1[2];  m[13] = c1[3]; /*|*/ m[14] = d1[2];  m[15] = d1[3];

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// compute the inverse of a general 4x4 matrix using Cramer's Rule
// If cannot find inverse, return indentity matrix
// M^-1 = adj(M) / det(M)
///////////////////////////////////////////////////////////////////////////////
kxMatrix44 & kxMatrix44::invertGeneral()
{
	// get cofactors of minor matrices
	float cofactor0 = getCofactor(m[5], m[6], m[7], m[9], m[10], m[11], m[13], m[14], m[15]);
	float cofactor1 = getCofactor(m[1], m[2], m[3], m[9], m[10], m[11], m[13], m[14], m[15]);
	float cofactor2 = getCofactor(m[1], m[2], m[3], m[5], m[6], m[7], m[13], m[14], m[15]);
	float cofactor3 = getCofactor(m[1], m[2], m[3], m[5], m[6], m[7], m[9], m[10], m[11]);

	// get determinant
	float determinant = m[0] * cofactor0 - m[4] * cofactor1 + m[8] * cofactor2 - m[12] * cofactor3;
	if (fabs(determinant) <= EPSILON)
	{
		return identity();
	}

	// get rest of cofactors for adj(M)
	float cofactor4 = getCofactor(m[1], m[2], m[3], m[9], m[10], m[11], m[13], m[14], m[15]);
	float cofactor5 = getCofactor(m[0], m[2], m[3], m[8], m[10], m[11], m[12], m[14], m[15]);
	float cofactor6 = getCofactor(m[0], m[1], m[3], m[8], m[9], m[11], m[12], m[13], m[15]);
	float cofactor7 = getCofactor(m[0], m[1], m[2], m[8], m[9], m[10], m[12], m[13], m[14]);

	float cofactor8 = getCofactor(m[1], m[2], m[3], m[5], m[6], m[7], m[13], m[14], m[15]);
	float cofactor9 = getCofactor(m[0], m[2], m[3], m[4], m[6], m[7], m[12], m[14], m[15]);
	float cofactor10 = getCofactor(m[0], m[1], m[3], m[4], m[5], m[7], m[12], m[13], m[15]);
	float cofactor11 = getCofactor(m[0], m[1], m[2], m[4], m[5], m[6], m[12], m[13], m[14]);

	float cofactor12 = getCofactor(m[1], m[2], m[3], m[5], m[6], m[7], m[9], m[10], m[11]);
	float cofactor13 = getCofactor(m[0], m[2], m[3], m[4], m[6], m[7], m[8], m[10], m[11]);
	float cofactor14 = getCofactor(m[0], m[1], m[3], m[4], m[5], m[7], m[8], m[9], m[11]);
	float cofactor15 = getCofactor(m[0], m[1], m[2], m[4], m[5], m[6], m[8], m[9], m[10]);

	// build inverse matrix = adj(M) / det(M)
	// adjugate of M is the transpose of the cofactor matrix of M
	float invDeterminant = 1.0f / determinant;
	m[0] = invDeterminant * cofactor0;
	m[1] = -invDeterminant * cofactor4;
	m[2] = invDeterminant * cofactor8;
	m[3] = -invDeterminant * cofactor12;

	m[4] = -invDeterminant * cofactor1;
	m[5] = invDeterminant * cofactor5;
	m[6] = -invDeterminant * cofactor9;
	m[7] = invDeterminant * cofactor13;

	m[8] = invDeterminant * cofactor2;
	m[9] = -invDeterminant * cofactor6;
	m[10] = invDeterminant * cofactor10;
	m[11] = -invDeterminant * cofactor14;

	m[12] = -invDeterminant * cofactor3;
	m[13] = invDeterminant * cofactor7;
	m[14] = -invDeterminant * cofactor11;
	m[15] = invDeterminant * cofactor15;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// translate this matrix by (x, y, z)
///////////////////////////////////////////////////////////////////////////////
kxMatrix44 & kxMatrix44::translate(float x, float y, float z)
{
	m[0] += m[12] * x;   m[1] += m[13] * x;   m[2] += m[14] * x;   m[3] += m[15] * x;
	m[4] += m[12] * y;   m[5] += m[13] * y;   m[6] += m[14] * y;   m[7] += m[15] * y;
	m[8] += m[12] * z;   m[9] += m[13] * z;   m[10] += m[14] * z;   m[11] += m[15] * z;

	return *this;
}

kxMatrix44 & kxMatrix44::translate(const kxVector3 & v)
{
	return translate(v.x, v.y, v.z);
}

kxMatrix44 & kxMatrix44::rotate(float angle, const kxVector3 & axis)
{
	return rotate(angle, axis.x, axis.y, axis.z);
}

kxMatrix44 & kxMatrix44::rotate(float angle, float x, float y, float z)
{
	float c = cosf(angle * DEG2RAD);    // cosine
	float s = sinf(angle * DEG2RAD);    // sine
	float c1 = 1.0f - c;                // 1 - c
	float m0 = m[0], m4 = m[4], m8 = m[8], m12 = m[12],
		m1 = m[1], m5 = m[5], m9 = m[9], m13 = m[13],
		m2 = m[2], m6 = m[6], m10 = m[10], m14 = m[14];

	// build rotation matrix
	float r0 = x * x * c1 + c;
	float r1 = x * y * c1 + z * s;
	float r2 = x * z * c1 - y * s;
	float r4 = x * y * c1 - z * s;
	float r5 = y * y * c1 + c;
	float r6 = y * z * c1 + x * s;
	float r8 = x * z * c1 + y * s;
	float r9 = y * z * c1 - x * s;
	float r10 = z * z * c1 + c;

	// multiply rotation matrix
	m[0] = r0 * m0 + r4 * m1 + r8 * m2;
	m[1] = r1 * m0 + r5 * m1 + r9 * m2;
	m[2] = r2 * m0 + r6 * m1 + r10* m2;
	m[4] = r0 * m4 + r4 * m5 + r8 * m6;
	m[5] = r1 * m4 + r5 * m5 + r9 * m6;
	m[6] = r2 * m4 + r6 * m5 + r10* m6;
	m[8] = r0 * m8 + r4 * m9 + r8 * m10;
	m[9] = r1 * m8 + r5 * m9 + r9 * m10;
	m[10] = r2 * m8 + r6 * m9 + r10* m10;
	m[12] = r0 * m12 + r4 * m13 + r8 * m14;
	m[13] = r1 * m12 + r5 * m13 + r9 * m14;
	m[14] = r2 * m12 + r6 * m13 + r10* m14;

	return *this;
}

kxMatrix44 & kxMatrix44::rotateX(float angle)
{
	float c = cosf(angle * DEG2RAD);
	float s = sinf(angle * DEG2RAD);
	float m1 = m[1], m2 = m[2],
		m5 = m[5], m6 = m[6],
		m9 = m[9], m10 = m[10],
		m13 = m[13], m14 = m[14];

	m[1] = m1 * c + m2 *-s;
	m[2] = m1 * s + m2 * c;
	m[5] = m5 * c + m6 *-s;
	m[6] = m5 * s + m6 * c;
	m[9] = m9 * c + m10*-s;
	m[10] = m9 * s + m10* c;
	m[13] = m13* c + m14*-s;
	m[14] = m13* s + m14* c;

	return *this;
}

kxMatrix44 & kxMatrix44::rotateY(float angle)
{
	float c = cosf(angle * DEG2RAD);
	float s = sinf(angle * DEG2RAD);
	float m0 = m[0], m2 = m[2],
		m4 = m[4], m6 = m[6],
		m8 = m[8], m10 = m[10],
		m12 = m[12], m14 = m[14];

	m[0] = m0 * c + m2 * s;
	m[2] = m0 *-s + m2 * c;
	m[4] = m4 * c + m6 * s;
	m[6] = m4 *-s + m6 * c;
	m[8] = m8 * c + m10* s;
	m[10] = m8 *-s + m10* c;
	m[12] = m12* c + m14* s;
	m[14] = m12*-s + m14* c;

	return *this;
}

kxMatrix44 & kxMatrix44::rotateZ(float angle)
{
	float c = cosf(angle * DEG2RAD);
	float s = sinf(angle * DEG2RAD);
	float m0 = m[0], m1 = m[1],
		m4 = m[4], m5 = m[5],
		m8 = m[8], m9 = m[9],
		m12 = m[12], m13 = m[13];

	m[0] = m0 * c + m1 *-s;
	m[1] = m0 * s + m1 * c;
	m[4] = m4 * c + m5 *-s;
	m[5] = m4 * s + m5 * c;
	m[8] = m8 * c + m9 *-s;
	m[9] = m8 * s + m9 * c;
	m[12] = m12* c + m13*-s;
	m[13] = m12* s + m13* c;

	return *this;
}

kxMatrix44 & kxMatrix44::scale(float scale)
{
	return this->scale(scale, scale, scale);
}

kxMatrix44 & kxMatrix44::scale(float x, float y, float z)
{
	m[0] *= x;   m[4] *= x;   m[8] *= x;   m[12] *= x;
	m[1] *= y;   m[5] *= y;   m[9] *= y;   m[13] *= y;
	m[2] *= z;   m[6] *= z;   m[10] *= z;   m[14] *= z;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// rotate matrix to face along the target direction
// NOTE: This function will clear the previous rotation and scale info and
// rebuild the matrix with the target vector. But it will keep the previous
// translation values.
// NOTE: It is for rotating object to look at the target, NOT for camera
///////////////////////////////////////////////////////////////////////////////
kxMatrix44 & kxMatrix44::lookAt(float tx, float ty, float tz)
{
	return lookAt(kxVector3(tx, ty, tz));
}

kxMatrix44 & kxMatrix44::lookAt(float tx, float ty, float tz, float ux, float uy, float uz)
{
	return lookAt(kxVector3(tx, ty, tz), kxVector3(ux, uy, uz));
}

kxMatrix44 & kxMatrix44::lookAt(const kxVector3 & target)
{
	// compute forward vector and normalize
	kxVector3 position = kxVector3(m[12], m[13], m[14]);
	kxVector3 forward = target - position;
	forward.normalize();
	kxVector3 up;             // up vector of object
	kxVector3 left;           // left vector of object

							  // compute temporal up vector
							  // if forward vector is near Y-axis, use up vector (0,0,-1) or (0,0,1)
	if (fabs(forward.x) < EPSILON && fabs(forward.z) < EPSILON)
	{
		// forward vector is pointing +Y axis
		if (forward.y > 0)
			up.set(0, 0, -1);
		// forward vector is pointing -Y axis
		else
			up.set(0, 0, 1);
	}
	else
	{
		// assume up vector is +Y axis
		up.set(0, 1, 0);
	}

	// compute left vector
	left = up.cross(forward);
	left.normalize();

	// re-compute up vector
	up = forward.cross(left);
	//up.normalize();

	// NOTE: overwrite rotation and scale info of the current matrix
	this->setColumn(0, left);
	this->setColumn(1, up);
	this->setColumn(2, forward);

	return *this;
}

kxMatrix44 & kxMatrix44::lookAt(const kxVector3 & target, const kxVector3 & up)
{
	// compute forward vector and normalize
	kxVector3 position = kxVector3(m[12], m[13], m[14]);
	kxVector3 forward = target - position;
	forward.normalize();

	// compute left vector
	kxVector3 left = up.cross(forward);
	left.normalize();

	// compute orthonormal up vector
	//kxVector3 up = forward.cross(left);
	//up.normalize();

	// NOTE: overwrite rotation and scale info of the current matrix
	this->setColumn(0, left);
	this->setColumn(1, up);
	this->setColumn(2, forward);

	return *this;
}

kxMatrix44 kxMatrix44::operator*(const kxMatrix44 & rhs) const
{
	kxMatrix22 ma11 = kxMatrix22(m[0], m[1], m[4], m[5]);
	kxMatrix22 ma12 = kxMatrix22(m[2], m[3], m[6], m[7]);
	kxMatrix22 ma21 = kxMatrix22(m[8], m[9], m[12], m[13]);
	kxMatrix22 ma22 = kxMatrix22(m[10], m[11], m[14], m[15]);

	kxMatrix22 mb11 = kxMatrix22(rhs[0], rhs[1], rhs[4], rhs[5]);
	kxMatrix22 mb12 = kxMatrix22(rhs[2], rhs[3], rhs[6], rhs[7]);
	kxMatrix22 mb21 = kxMatrix22(rhs[8], rhs[9], rhs[12], rhs[13]);
	kxMatrix22 mb22 = kxMatrix22(rhs[10], rhs[11], rhs[14], rhs[15]);

	kxMatrix22 tmp1 = (ma11 + ma22) * (mb11 + mb22);
	kxMatrix22 tmp2 = (ma21 + ma22) * mb11;
	kxMatrix22 tmp3 = ma11 * (mb12 - mb22);
	kxMatrix22 tmp4 = ma22 * (mb21 - mb11);
	kxMatrix22 tmp5 = (ma11 + ma12) * mb22;
	kxMatrix22 tmp6 = (ma21 - ma11) * (mb11 + mb12);
	kxMatrix22 tmp7 = (ma12 - ma22) * (mb21 + mb22);

	kxMatrix22 r11 = tmp1 + tmp4 - tmp5 + tmp7;
	kxMatrix22 r12 = tmp3 + tmp5;
	kxMatrix22 r21 = tmp2 + tmp4;
	kxMatrix22 r22 = tmp1 + tmp3 - tmp2 + tmp6;
	return kxMatrix44(r11[0],r11[1],r12[0],r12[1],
		r11[2],r11[3],r12[2],r12[3],
		r21[0],r21[1],r22[0],r22[1],
		r21[2],r21[3],r22[2],r22[3]);
}

float kxMatrix44::getCofactor(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33)
{
	return m11*m22*m33 + m12*m23*m31 + m13*m21*m32 - m13*m22*m31 - m11*m23*m32 - m12*m21*m33;
}

