#ifndef _VECTOR3D_H_
#define _VECTOR3D_H_

#include <math.h>
#include <exception>
using namespace std;

struct Vector3D {
public:
	double x, y, z;

	Vector3D() : x(0), y(0), z(0) {};

	Vector3D(const double& _x, const double& _y, const double& _z) { x = _x; y = _y; z = _z; }

	Vector3D(const double* pV) { x = pV[0]; y = pV[1]; z = pV[2]; }
	Vector3D(const double& val) { x = y = z = val; }
	Vector3D(const Vector3D& v) { x = v.x; y = v.y; z = v.z; }

	Vector3D& operator = (const double& val) { x = y = z = val; return *this; }
	Vector3D& operator += (const Vector3D& v) { x += v.x; y += v.y; z += v.z; return *this; }
	Vector3D& operator -= (const Vector3D& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	Vector3D& operator *= (const Vector3D& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
	Vector3D& operator /= (const Vector3D& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }

	Vector3D& operator *= (const double& v) { x *= v; y *= v; z *= v; return *this; }
	Vector3D& operator /= (const double& v) { x /= v; y /= v; z /= v; return *this; }

	Vector3D operator + () const { return *this; }
	Vector3D operator - () const { return Vector3D(-x, -y, -z); }

	Vector3D operator + (const Vector3D& v) const { return Vector3D(x + v.x, y + v.y, z + v.z); }
	Vector3D operator - (const Vector3D& v) const { return Vector3D(x - v.x, y - v.y, z - v.z); }
	Vector3D operator * (const Vector3D& v) const { return Vector3D(x * v.x, y * v.y, z * v.z); }
	Vector3D operator / (const Vector3D& v) const { return Vector3D(x / v.x, y / v.y, z / v.z); }
	Vector3D operator * (const double& v) const { return Vector3D(x * v, y * v, z * v); }
	Vector3D operator / (const double& v) const { return Vector3D(x / v, y / v, z / v); }
	//friend Vector3D<double> operator * ( const double& val, const Vector3D<double>& vec ) { return Vector3D( vec.x * val, vec.y * val, vec.z * val ); }
	friend Vector3D;

	double Length() { return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)); }
	double DotProduct(const Vector3D& v) const { return v.x * x + v.y * y + v.z * z; }
	//double LengthSqr()	{	return DotProduct( *this );	};

	Vector3D& Normalize() { return (*this) /= Length(); };

	/*
	Vector3D Reflect( const Vector3D& n ) const { return Vector3D( (*this) - 2 * DotProduct( n ) * n ); }
	Vector3D Refract( const Vector3D& n, const double& ri ) const {
		double cos = DotProduct( n ) * ri;
		return Interpolate( (*this),  n, cos ).Normalize();
	}
*/
/*
//double operator () ( const int i ) const { return val[ Check( i ) ]; }
//double& operator () ( const int i )	{	return val[ Check( i ) ]; }

private:
	inline int Check(const int index) const	{
		// Check bounds in debug build
		ASSERT(index >= 0 && index < 3);
		return (index);
	}*/
};

#endif //