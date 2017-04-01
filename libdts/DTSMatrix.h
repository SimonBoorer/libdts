#ifndef DTS_MATRIX_H_
#define DTS_MATRIX_H_

#include "DTSPoint3.h"

// 4x4 Matrix Class
namespace DTS
{

class MatrixF
{
public:
	// Create an uninitialized matrix.
	explicit MatrixF(bool identity = false);

	operator float*() { return (m_); }	// Allow people to get at m_.
	operator const float*() const { return (m_); }	// Allow people to get at m_.

	bool IsIdentity() const;			// Checks for identity matrix.

	MatrixF& Identity();				// Make this an identity matrix.

	// Invert m_.
	MatrixF& Inverse();

	// Swap rows and columns.
	MatrixF& Transpose();

	// Set the specified column from a Point3F.
	void SetColumn(int32_t col, const Point3F& cptr);

	// Get the position of the matrix.
	Point3F GetPosition() const;

	// Set the position of the matrix.
	void SetPosition(const Point3F& pos) { SetColumn(3, pos); }

	MatrixF& Mul(const MatrixF& a);						// M * a -> M
	MatrixF& Mul(const MatrixF& a, const MatrixF& b);	// a * b -> M

	void MulP(Point3F& p) const;						// M * p -> p (assume w = 1.0f)
	void MulP(const Point3F& p, Point3F* d) const;		// M * p -> d (assume w = 1.0f)
	void MulV(VectorF& p) const;						// M * v -> v (assume w = 0.0f)
	void MulV(const VectorF& p, Point3F* d) const;		// M * v -> d (assume w = 0.0f)

private:
	float Determinant();

	void MulP(const float* v, float* vresult) const;
	void MulV(const float* v, float* vresult) const;

	float m_[16]; // Note: Torque uses row-major matrices
};

inline MatrixF::MatrixF(bool identity)
{
	if (identity)
		Identity();
}

inline bool MatrixF::IsIdentity() const
{
	return
		m_[0] == 1.0f &&
		m_[1] == 0.0f &&
		m_[2] == 0.0f &&
		m_[3] == 0.0f &&
		m_[4] == 0.0f &&
		m_[5] == 1.0f &&
		m_[6] == 0.0f &&
		m_[7] == 0.0f &&
		m_[8] == 0.0f &&
		m_[9] == 0.0f &&
		m_[10] == 1.0f &&
		m_[11] == 0.0f &&
		m_[12] == 0.0f &&
		m_[13] == 0.0f &&
		m_[14] == 0.0f &&
		m_[15] == 1.0f;
}

inline MatrixF& MatrixF::Identity()
{
	m_[0] = 1.0f;
	m_[1] = 0.0f;
	m_[2] = 0.0f;
	m_[3] = 0.0f;
	m_[4] = 0.0f;
	m_[5] = 1.0f;
	m_[6] = 0.0f;
	m_[7] = 0.0f;
	m_[8] = 0.0f;
	m_[9] = 0.0f;
	m_[10] = 1.0f;
	m_[11] = 0.0f;
	m_[12] = 0.0f;
	m_[13] = 0.0f;
	m_[14] = 0.0f;
	m_[15] = 1.0f;
	return (*this);
}

inline void MatrixF::SetColumn(int32_t col, const Point3F& cptr)
{
	m_[col] = cptr.x;
	m_[col + 4] = cptr.y;
	m_[col + 8] = cptr.z;
}

inline Point3F MatrixF::GetPosition() const
{
	return Point3F(m_[3], m_[3 + 4], m_[3 + 8]);
}

inline MatrixF& MatrixF::Mul(const MatrixF& a)
{
	assert(&a != this);

	MatrixF temp_this(*this);
	Mul(temp_this, a);
	return (*this);
}

inline void MatrixF::MulP(Point3F& p) const
{
	Point3F d;
	MulP(p, &d);
	p = d;
}

inline void MatrixF::MulP(const Point3F& p, Point3F* d) const
{
	MulP(&p.x, &d->x);
}

inline void MatrixF::MulV(VectorF& v) const
{
	VectorF temp;
	MulV(v, &temp);
	v = temp;
}

inline void MatrixF::MulV(const VectorF& v, Point3F* d) const
{
	MulV(&v.x, &d->x);
}

} // namespace DTS

#endif // DTS_MATRIX_H_
