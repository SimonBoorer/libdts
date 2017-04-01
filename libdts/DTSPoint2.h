#ifndef DTS_POINT2_H_
#define DTS_POINT2_H_

namespace DTS
{

// 2D floating-point point.
class Point2F
{
public:
	void Set(float x, float y);

	float x;
	float y;
};

inline void Point2F::Set(float _x, float _y)
{
	x = _x;
	y = _y;
}

} // namespace DTS

#endif // DTS_POINT2_H_
