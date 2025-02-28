// Copyright (C) 2007 Id Software, Inc.
//

#ifndef __MATH_PLANE_H__
#define __MATH_PLANE_H__

/*
===============================================================================

	3D plane with equation: a * x + b * y + c * z + d = 0

===============================================================================
*/


class idVec3;
class idMat3;

#define	ON_EPSILON					0.1f
#define DEGENERATE_DIST_EPSILON		1e-4f

#define	SIDE_FRONT					0
#define	SIDE_BACK					1
#define	SIDE_ON						2
#define	SIDE_CROSS					3

// plane sides
#define PLANESIDE_FRONT				0
#define PLANESIDE_BACK				1
#define PLANESIDE_ON				2
#define PLANESIDE_CROSS				3

// plane types
#define PLANETYPE_X					0
#define PLANETYPE_Y					1
#define PLANETYPE_Z					2
#define PLANETYPE_NEGX				3
#define PLANETYPE_NEGY				4
#define PLANETYPE_NEGZ				5
#define PLANETYPE_TRUEAXIAL			6	// all types < 6 are true axial planes
#define PLANETYPE_ZEROX				6
#define PLANETYPE_ZEROY				7
#define PLANETYPE_ZEROZ				8
#define PLANETYPE_NONAXIAL			9

class idPlane {
public:
					idPlane( void );
					idPlane( float a, float b, float c, float d );
					idPlane( const idVec3 &normal, const float dist );

	float			operator[]( int index ) const;
	float &			operator[]( int index );
	idPlane			operator-() const;						// flips plane
	idPlane &		operator=( const idVec3 &v );			// sets normal and sets idPlane::d to zero
	idPlane			operator+( const idPlane &p ) const;	// add plane equations
	idPlane			operator-( const idPlane &p ) const;	// subtract plane equations
	idPlane &		operator*=( const idMat3 &m );			// Normal() *= m

	bool			Compare( const idPlane &p ) const;						// exact compare, no epsilon
	bool			Compare( const idPlane &p, const float epsilon ) const;	// compare with epsilon
	bool			Compare( const idPlane &p, const float normalEps, const float distEps ) const;	// compare with epsilon
	bool			operator==(	const idPlane &p ) const;					// exact compare, no epsilon
	bool			operator!=(	const idPlane &p ) const;					// exact compare, no epsilon

	void			Zero( void );							// zero plane
	void			SetNormal( const idVec3 &normal );		// sets the normal
	const idVec3 &	Normal( void ) const;					// reference to const normal
	idVec3 &		Normal( void );							// reference to normal
	float			Normalize( bool fixDegenerate = true );	// only normalizes the plane normal, does not adjust d
	bool			FixDegenerateNormal( void );			// fix degenerate normal
	bool			FixDegeneracies( float distEpsilon );	// fix degenerate normal and dist
	float			Dist( void ) const;						// returns: -d
	void			SetDist( const float dist );			// sets: d = -dist
	int				Type( void ) const;						// returns plane type
	void			Set( float x, float y, float z, float dist );

	bool			FromPoints( const idVec3 &p1, const idVec3 &p2, const idVec3 &p3, bool fixDegenerate = true );

	bool			FromPointsHighPrecision( const idVec3 &p0, const idVec3 &p1, const idVec3 &p2, bool fixDegenerate );

	bool			FromVecs( const idVec3 &dir1, const idVec3 &dir2, const idVec3 &p, bool fixDegenerate = true );
	void			FitThroughPoint( const idVec3 &p );	// assumes normal is valid
	bool			HeightFit( const idVec3 *points, const int numPoints );
	idPlane			Translate( const idVec3 &translation ) const;
	idPlane &		TranslateSelf( const idVec3 &translation );
	idPlane			Rotate( const idVec3 &origin, const idMat3 &axis ) const;
	idPlane &		RotateSelf( const idVec3 &origin, const idMat3 &axis );

	float			Distance( const idVec3 &v ) const;
	int				Side( const idVec3 &v, const float epsilon = 0.0f ) const;

	bool			LineIntersection( const idVec3 &start, const idVec3 &end ) const;
	bool			LineIntersection( const idVec3 &start, const idVec3 &end, float &fraction ) const;
					// intersection point is start + dir * scale
	bool			RayIntersection( const idVec3 &start, const idVec3 &dir, float &scale ) const;
	bool			PlaneIntersection( const idPlane &plane, idVec3 &start, idVec3 &dir ) const;

	int				GetDimension( void ) const;

	const idVec4 &	ToVec4( void ) const;
	idVec4 &		ToVec4( void );
	const float *	ToFloatPtr( void ) const;
	float *			ToFloatPtr( void );
	const char *	ToString( int precision = 2 ) const;

private:
	float			a;
	float			b;
	float			c;
	float			d;
};

extern idPlane plane_origin;
#define plane_zero plane_origin

ID_INLINE idPlane::idPlane( void ) {
}

ID_INLINE idPlane::idPlane( float a, float b, float c, float d ) {
	this->a = a;
	this->b = b;
	this->c = c;
	this->d = d;
}

ID_INLINE idPlane::idPlane( const idVec3 &normal, const float dist ) {
	this->a = normal.x;
	this->b = normal.y;
	this->c = normal.z;
	this->d = -dist;
}

ID_INLINE float idPlane::operator[]( int index ) const {
	return ( &a )[ index ];
}

ID_INLINE float& idPlane::operator[]( int index ) {
	return ( &a )[ index ];
}

ID_INLINE idPlane idPlane::operator-() const {
	return idPlane( -a, -b, -c, -d );
}

ID_INLINE idPlane &idPlane::operator=( const idVec3 &v ) {
	a = v.x;
	b = v.y;
	c = v.z;
	d = 0;
	return *this;
}

ID_INLINE idPlane idPlane::operator+( const idPlane &p ) const {
	return idPlane( a + p.a, b + p.b, c + p.c, d + p.d );
}

ID_INLINE idPlane idPlane::operator-( const idPlane &p ) const {
	return idPlane( a - p.a, b - p.b, c - p.c, d - p.d );
}

ID_INLINE idPlane &idPlane::operator*=( const idMat3 &m ) {
	Normal() *= m;
	return *this;
}

ID_INLINE bool idPlane::Compare( const idPlane &p ) const {
	return ( a == p.a && b == p.b && c == p.c && d == p.d );
}

ID_INLINE bool idPlane::Compare( const idPlane &p, const float epsilon ) const {
	if ( idMath::Fabs( a - p.a ) > epsilon ) {
		return false;
	}

	if ( idMath::Fabs( b - p.b ) > epsilon ) {
		return false;
	}

	if ( idMath::Fabs( c - p.c ) > epsilon ) {
		return false;
	}

	if ( idMath::Fabs( d - p.d ) > epsilon ) {
		return false;
	}

	return true;
}

ID_INLINE bool idPlane::Compare( const idPlane &p, const float normalEps, const float distEps ) const {
	if ( idMath::Fabs( d - p.d ) > distEps ) {
		return false;
	}
	if ( !Normal().Compare( p.Normal(), normalEps ) ) {
		return false;
	}
	return true;
}

ID_INLINE bool idPlane::operator==( const idPlane &p ) const {
	return Compare( p );
}

ID_INLINE bool idPlane::operator!=( const idPlane &p ) const {
	return !Compare( p );
}

ID_INLINE void idPlane::Zero( void ) {
	a = b = c = d = 0.0f;
}

ID_INLINE void idPlane::SetNormal( const idVec3 &normal ) {
	a = normal.x;
	b = normal.y;
	c = normal.z;
}

ID_INLINE const idVec3 &idPlane::Normal( void ) const {
	return *reinterpret_cast<const idVec3 *>(&a);
}

ID_INLINE idVec3 &idPlane::Normal( void ) {
	return *reinterpret_cast<idVec3 *>(&a);
}

ID_INLINE float idPlane::Normalize( bool fixDegenerate ) {
	float length = reinterpret_cast<idVec3 *>(&a)->Normalize();

	if ( fixDegenerate ) {
		FixDegenerateNormal();
	}
	return length;
}

ID_INLINE bool idPlane::FixDegenerateNormal( void ) {
	return Normal().FixDegenerateNormal();
}

ID_INLINE bool idPlane::FixDegeneracies( float distEpsilon ) {
	bool fixedNormal = FixDegenerateNormal();
	// only fix dist if the normal was degenerate
	if ( fixedNormal ) {
		if ( idMath::Fabs( d - idMath::Rint( d ) ) < distEpsilon ) {
			d = idMath::Rint( d );
		}
	}
	return fixedNormal;
}

ID_INLINE float idPlane::Dist( void ) const {
	return -d;
}

ID_INLINE void idPlane::SetDist( const float dist ) {
	d = -dist;
}

ID_INLINE void idPlane::Set( float a, float b, float c, float d ) {
	this->a = a;
	this->b = b;
	this->c = c;
	this->d = d;
}

ID_INLINE bool idPlane::FromPoints( const idVec3 &p1, const idVec3 &p2, const idVec3 &p3, bool fixDegenerate ) {
	Normal() = (p1 - p2).Cross( p3 - p2 );
	if ( Normalize( fixDegenerate ) == 0.0f ) {
		return false;
	}
	d = -( Normal() * p2 );
	return true;
}


ID_INLINE bool idPlane::FromPointsHighPrecision( const idVec3 &p0, const idVec3 &p1, const idVec3 &p2, bool fixDegenerate ) {
	// Take the cross product of the edge directions of the two shortest edges for maximum precision.
	// The shortest two edges of a triangle are also the two edges that are most orthogonal to each other.
#if 0
	float l0 = ( p2 - p1 ).LengthSqr();
	float l1 = ( p0 - p2 ).LengthSqr();
	float l2 = ( p1 - p0 ).LengthSqr();

	if ( l0 > l1 && l0 > l2 ) {
		idVec3 v1 = p1 - p0;
		idVec3 v2 = p2 - p0;
		Normal() = v1.Cross( v2 );
	} else if ( l1 > l0 && l1 > l2 ) {
		idVec3 v1 = p2 - p1;
		idVec3 v2 = p0 - p1;
		Normal() = v1.Cross( v2 );
	} else {
		idVec3 v1 = p0 - p2;
		idVec3 v2 = p1 - p2;
		Normal() = v1.Cross( v2 );
	}
	bool r = Normalize( fixDegenerate ) != 0.0f;
	FitThroughPoint( p0 );
	return r;
#else
	const idVec3 *p[3] = { &p0, &p1, &p2 };
	float l0 = ( p2 - p1 ).LengthSqr();
	float l1 = ( p0 - p2 ).LengthSqr();
	float l2 = ( p1 - p0 ).LengthSqr();
	int index = Max3Index( l0, l1, l2 );
	idVec3 v1 = *p[(index+1)%3] - *p[index];
	idVec3 v2 = *p[(index+2)%3] - *p[index];
	Normal() = v1.Cross( v2 );
	bool r = Normalize( fixDegenerate ) != 0.0f;
	FitThroughPoint( p0 );
	return r;
#endif
}

ID_INLINE bool idPlane::FromVecs( const idVec3 &dir1, const idVec3 &dir2, const idVec3 &p, bool fixDegenerate ) {
	Normal() = dir1.Cross( dir2 );
	if ( Normalize( fixDegenerate ) == 0.0f ) {
		return false;
	}
	d = -( Normal() * p );
	return true;
}

ID_INLINE void idPlane::FitThroughPoint( const idVec3 &p ) {
	d = -( Normal() * p );
}

ID_INLINE idPlane idPlane::Translate( const idVec3 &translation ) const {
	return idPlane( a, b, c, d - translation * Normal() );
}

ID_INLINE idPlane &idPlane::TranslateSelf( const idVec3 &translation ) {
	d -= translation * Normal();
	return *this;
}

ID_INLINE idPlane idPlane::Rotate( const idVec3 &origin, const idMat3 &axis ) const {
	idPlane p;
	p.Normal() = Normal() * axis;
	p.d = d + origin * Normal() - origin * p.Normal();
	return p;
}

ID_INLINE idPlane &idPlane::RotateSelf( const idVec3 &origin, const idMat3 &axis ) {
	d += origin * Normal();
	Normal() *= axis;
	d -= origin * Normal();
	return *this;
}

ID_INLINE float idPlane::Distance( const idVec3 &v ) const {
	return a * v.x + b * v.y + c * v.z + d;
}

ID_INLINE int idPlane::Side( const idVec3 &v, const float epsilon ) const {
	float dist = Distance( v );
	if ( dist > epsilon ) {
		return PLANESIDE_FRONT;
	}
	else if ( dist < -epsilon ) {
		return PLANESIDE_BACK;
	}
	else {
		return PLANESIDE_ON;
	}
}

ID_INLINE bool idPlane::LineIntersection( const idVec3 &start, const idVec3 &end ) const {
	float d1, d2, fraction;

	d1 = Normal() * start + d;
	d2 = Normal() * end + d;
	if ( d1 == d2 ) {
		return false;
	}
	if ( d1 > 0.0f && d2 > 0.0f ) {
		return false;
	}
	if ( d1 < 0.0f && d2 < 0.0f ) {
		return false;
	}
	fraction = ( d1 / ( d1 - d2 ) );
	return ( fraction >= 0.0f && fraction <= 1.0f );
}

ID_INLINE bool idPlane::LineIntersection( const idVec3 &start, const idVec3 &end, float &fraction ) const {
	float d1, d2;

	d1 = Normal() * start + d;
	d2 = Normal() * end + d;
	if ( d1 == d2 ) {
		return false;
	}
	if ( d1 > 0.0f && d2 > 0.0f ) {
		return false;
	}
	if ( d1 < 0.0f && d2 < 0.0f ) {
		return false;
	}
	fraction = ( d1 / ( d1 - d2 ) );
	return ( fraction >= 0.0f && fraction <= 1.0f );
}

ID_INLINE bool idPlane::RayIntersection( const idVec3 &start, const idVec3 &dir, float &scale ) const {
	float d1, d2;

	d1 = Normal() * start + d;
	d2 = Normal() * dir;
	if ( d2 == 0.0f ) {
		return false;
	}
	scale = -( d1 / d2 );
	return true;
}

ID_INLINE int idPlane::GetDimension( void ) const {
	return 4;
}

ID_INLINE const idVec4 &idPlane::ToVec4( void ) const {
	return *reinterpret_cast<const idVec4 *>(&a);
}

ID_INLINE idVec4 &idPlane::ToVec4( void ) {
	return *reinterpret_cast<idVec4 *>(&a);
}

ID_INLINE const float *idPlane::ToFloatPtr( void ) const {
	return reinterpret_cast<const float *>(&a);
}

ID_INLINE float *idPlane::ToFloatPtr( void ) {
	return reinterpret_cast<float *>(&a);
}

#endif /* !__MATH_PLANE_H__ */
