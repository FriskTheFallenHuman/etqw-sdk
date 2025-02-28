// Copyright (C) 2007 Id Software, Inc.
//

#ifndef __BV_BOUNDS_H__
#define __BV_BOUNDS_H__

/*
===============================================================================

	Axis Aligned Bounding Box

===============================================================================
*/

class idBounds {
public:
					idBounds( void );
					explicit idBounds( const idSphere& sphere );
					explicit idBounds( const idVec3 &mins, const idVec3 &maxs );
					explicit idBounds( const idVec3 &point );

	const idVec3 &	operator[]( const int index ) const;
	idVec3 &		operator[]( const int index );
	idBounds		operator+( const idVec3 &t ) const;				// returns translated bounds
	idBounds &		operator+=( const idVec3 &t );					// translate the bounds
	idBounds		operator*( const idMat3 &r ) const;				// returns rotated bounds
	idBounds &		operator*=( const idMat3 &r );					// rotate the bounds
	idBounds		operator+( const idBounds &a ) const;
	idBounds &		operator+=( const idBounds &a );
	idBounds		operator-( const idBounds &a ) const;
	idBounds &		operator-=( const idBounds &a );

	bool			Compare( const idBounds &a ) const;							// exact compare, no epsilon
	bool			Compare( const idBounds &a, const float epsilon ) const;	// compare with epsilon
	bool			operator==(	const idBounds &a ) const;						// exact compare, no epsilon
	bool			operator!=(	const idBounds &a ) const;						// exact compare, no epsilon

	void			Clear( void );									// inside out bounds
	void			Zero( void );									// single point at origin
	void			Zero( const idVec3 &center );					// single point at center

	idVec3			GetSize( void ) const;							// returns max - min
	idVec3			GetCenter( void ) const;						// returns center of bounds
	float			GetRadius( void ) const;						// returns the radius relative to the bounds origin
	float			GetRadius( const idVec3 &center ) const;		// returns the radius relative to the given center
	float			GetVolume( void ) const;						// returns the volume of the bounds
	float			GetSurfaceArea( void ) const;						// returns the volume of the bounds
	float			GetMaxDistFromOrigin( void ) const;				// returns the maximum distance from the origin
	bool			IsCleared( void ) const;						// returns true if bounds are inside out

	void			AddPoint( const idVec3 &v );					// add the point
	bool			AddPointExpanded( const idVec3 &v );			// add the point, returns true if the bounds expanded
	void			AddBounds( const idBounds &a );					// add the bounds, returns true if the bounds expanded
	bool			AddBoundsExpanded( const idBounds &a );			// add the bounds, returns true if the bounds expanded
	idBounds		Intersect( const idBounds &a ) const;			// return intersection of this bounds with the given bounds
	idBounds &		IntersectSelf( const idBounds &a );				// intersect this bounds with the given bounds
	idBounds		Expand( const float d ) const;					// return bounds expanded in all directions with the given value
	idBounds &		ExpandSelf( const float d );					// expand bounds in all directions with the given value

	idBounds &		DirectedExpandSelf( const idVec3 &d );			// expand bounds in direction of vec

// RAVEN BEGIN
// jscott: for BSE
	idBounds &		ExpandSelf( const idVec3 &d );					// expand bounds in all directions by the given vector
	idVec3			Size( void ) const;
	float			ShortestDistance( const idVec3 &point ) const;
	bool			Contains( const idBounds &a ) const;
// jscott: for material types
	int				GetLargestAxis( void ) const;
// RAVEN END

	idBounds		Translate( const idVec3 &translation ) const;	// return translated bounds
	idBounds &		TranslateSelf( const idVec3 &translation );		// translate this bounds
	idBounds		Rotate( const idMat3 &rotation ) const;			// return rotated bounds
	idBounds &		RotateSelf( const idMat3 &rotation );			// rotate this bounds

	float			PlaneDistance( const idPlane &plane ) const;
	int				PlaneSide( const idPlane &plane, const float epsilon = ON_EPSILON ) const;

	bool			ContainsPoint( const idVec3 &p ) const;			// includes touching
	bool			ContainsPoint( const idVec3 &p, const float epsilon ) const;		// includes touching
	bool			ContainsPoint2D( const idVec3 &p ) const;		// includes touching
	bool			ContainsBounds( const idBounds &a ) const;							// includes touching
	bool			ContainsBounds2D( const idBounds &a ) const;						// includes touching
	bool			ContainsBounds( const idBounds &a, const float epsilon ) const;		// includes touching
	bool			IntersectsBounds( const idBounds &a ) const;	// includes touching
	bool			IntersectsBounds( const idBounds &a, const float epsilon ) const;	// includes touching
	bool			IntersectsBounds2D( const idBounds &a ) const;	// includes touching

	bool			LineIntersection( const idVec3 &start, const idVec3 &end ) const;
					// intersection point is start + dir * scale
	bool			RayIntersection( const idVec3 &start, const idVec3 &dir, float &scale ) const;

					// most tight bounds for the given transformed bounds
	void			FromTransformedBounds( const idBounds &bounds, const idVec3 &origin, const idMat3 &axis );
	void			FromTransformedBounds( const idBounds &bounds, const float matrix[16] );
					// most tight bounds for a point set
	void			FromPoints( const idVec3 *points, const int numPoints );
					// most tight bounds for a translation
	void			FromPointTranslation( const idVec3 &point, const idVec3 &translation );
	void			FromBoundsTranslation( const idBounds &bounds, const idVec3 &origin, const idMat3 &axis, const idVec3 &translation );
					// most tight bounds for a rotation
	void			FromPointRotation( const idVec3 &point, const idRotation &rotation );
	void			FromBoundsRotation( const idBounds &bounds, const idVec3 &origin, const idMat3 &axis, const idRotation &rotation );

	int				GetDimension( void ) const;

	void			ToPoints( idVec3 points[8] ) const;
	idSphere		ToSphere( void ) const;
	void			ToPlanes( idPlane planes[6] ) const;

	const float *	ToFloatPtr( void ) const;
	float *			ToFloatPtr( void );
	const char *	ToString( int precision = 2 ) const;

	void			AxisProjection( const idVec3 &dir, float &min, float &max ) const;
	void			AxisProjection( const idVec3 &origin, const idMat3 &axis, const idVec3 &dir, float &min, float &max ) const;

	idBounds		ToMaya( void ) const;
	idBounds&		ToMayaSelf( void );
	idBounds		FromMaya( void ) const;
	idBounds&		FromMayaSelf( void );

	const idVec3&	GetMins( void ) const;
	idVec3&			GetMins( void );
	const idVec3&	GetMaxs( void ) const;
	idVec3&			GetMaxs( void );

private:
	idVec3			b[2];
};

extern idBounds	bounds_zero;

ID_INLINE idBounds::idBounds( void ) {
}

ID_INLINE idBounds::idBounds( const idVec3 &mins, const idVec3 &maxs ) {
	b[0] = mins;
	b[1] = maxs;
}

ID_INLINE idBounds::idBounds( const idSphere& sphere ) {
	const idVec3& org = sphere.GetOrigin();
	float radius = sphere.GetRadius();
	b[ 0 ].Set( org[ 0 ] - radius, org[ 1 ] - radius, org[ 2 ] - radius );
	b[ 1 ].Set( org[ 0 ] + radius, org[ 1 ] + radius, org[ 2 ] + radius );
}

ID_INLINE idBounds::idBounds( const idVec3 &point ) {
	b[0] = point;
	b[1] = point;
}

ID_INLINE const idVec3 &idBounds::operator[]( const int index ) const {
	return b[index];
}

ID_INLINE idVec3 &idBounds::operator[]( const int index ) {
	return b[index];
}

ID_INLINE idBounds idBounds::operator+( const idVec3 &t ) const {
	return idBounds( b[0] + t, b[1] + t );
}

ID_INLINE idBounds &idBounds::operator+=( const idVec3 &t ) {
	b[0] += t;
	b[1] += t;
	return *this;
}

ID_INLINE idBounds idBounds::operator*( const idMat3 &r ) const {
	idBounds bounds;
	bounds.FromTransformedBounds( *this, vec3_origin, r );
	return bounds;
}

ID_INLINE idBounds &idBounds::operator*=( const idMat3 &r ) {
	this->FromTransformedBounds( *this, vec3_origin, r );
	return *this;
}

ID_INLINE idBounds idBounds::operator+( const idBounds &a ) const {
	idBounds newBounds;
	newBounds = *this;
	newBounds.AddBounds( a );
	return newBounds;
}

ID_INLINE idBounds &idBounds::operator+=( const idBounds &a ) {
	idBounds::AddBounds( a );
	return *this;
}

ID_INLINE idBounds idBounds::operator-( const idBounds &a ) const {
/*	assert( b[1][0] - b[0][0] > a.b[1][0] - a.b[0][0] &&
				b[1][1] - b[0][1] > a.b[1][1] - a.b[0][1] &&
					b[1][2] - b[0][2] > a.b[1][2] - a.b[0][2] );*/
	return idBounds( idVec3( b[0][0] + a.b[1][0], b[0][1] + a.b[1][1], b[0][2] + a.b[1][2] ),
					idVec3( b[1][0] + a.b[0][0], b[1][1] + a.b[0][1], b[1][2] + a.b[0][2] ) );
}

ID_INLINE idBounds &idBounds::operator-=( const idBounds &a ) {
	assert( b[1][0] - b[0][0] > a.b[1][0] - a.b[0][0] &&
				b[1][1] - b[0][1] > a.b[1][1] - a.b[0][1] &&
					b[1][2] - b[0][2] > a.b[1][2] - a.b[0][2] );
	b[0] += a.b[1];
	b[1] += a.b[0];
	return *this;
}

ID_INLINE bool idBounds::Compare( const idBounds &a ) const {
	return ( b[0].Compare( a.b[0] ) && b[1].Compare( a.b[1] ) );
}

ID_INLINE bool idBounds::Compare( const idBounds &a, const float epsilon ) const {
	return ( b[0].Compare( a.b[0], epsilon ) && b[1].Compare( a.b[1], epsilon ) );
}

ID_INLINE bool idBounds::operator==( const idBounds &a ) const {
	return Compare( a );
}

ID_INLINE bool idBounds::operator!=( const idBounds &a ) const {
	return !Compare( a );
}

ID_INLINE void idBounds::Clear( void ) {
	b[0][0] = b[0][1] = b[0][2] = idMath::INFINITY;
	b[1][0] = b[1][1] = b[1][2] = -idMath::INFINITY;
}

ID_INLINE void idBounds::Zero( void ) {
	b[0][0] = b[0][1] = b[0][2] =
	b[1][0] = b[1][1] = b[1][2] = 0;
}

ID_INLINE void idBounds::Zero( const idVec3 &center ) {
	b[0] = center;
	b[1] = center;
}

ID_INLINE idVec3 idBounds::GetSize( void ) const {
	return idVec3( ( b[1][0] - b[0][0] ), ( b[1][1] - b[0][1] ), ( b[1][2] - b[0][2] ) );
}

ID_INLINE idVec3 idBounds::GetCenter( void ) const {
	return idVec3( ( b[1][0] + b[0][0] ) * 0.5f, ( b[1][1] + b[0][1] ) * 0.5f, ( b[1][2] + b[0][2] ) * 0.5f );
}

ID_INLINE float idBounds::GetVolume( void ) const {
	if ( b[0][0] >= b[1][0] || b[0][1] >= b[1][1] || b[0][2] >= b[1][2] ) {
		return 0.0f;
	}
	return ( ( b[1][0] - b[0][0] ) * ( b[1][1] - b[0][1] ) * ( b[1][2] - b[0][2] ) );
}

ID_INLINE float idBounds::GetSurfaceArea( void ) const {
	idVec3 s = Size();
	return 2.f * (s.x * s.y + s.x * s.z + s.y * s.z);
}

ID_INLINE float idBounds::GetMaxDistFromOrigin( void ) const {
	float r1, r2;
	r1 = b[0].LengthSqr();
	r2 = b[1].LengthSqr();
	if ( r2 > r1 ) {
		r1 = r2;
	}
	return idMath::Sqrt( r1 );
}

ID_INLINE bool idBounds::IsCleared( void ) const {
	return b[0][0] > b[1][0];
}

ID_INLINE void idBounds::AddPoint( const idVec3 &v ) {
#if defined( ID_WIN_X86_SSE )

	__asm {
		mov			esi, this
		mov			edi, v
		movss		xmm0, [esi+0]
		movss		xmm1, [edi+0]
		minss		xmm0, xmm1
		movss		[esi+0], xmm0
		movss		xmm2, [esi+4]
		movss		xmm3, [edi+4]
		minss		xmm2, xmm3
		movss		[esi+4], xmm2
		movss		xmm4, [esi+8]
		movss		xmm5, [edi+8]
		minss		xmm4, xmm5
		movss		[esi+8], xmm4
		maxss		xmm1, [esi+12]
		movss		[esi+12], xmm1
		maxss		xmm3, [esi+16]
		movss		[esi+16], xmm3
		maxss		xmm5, [esi+20]
		movss		[esi+20], xmm5
	}

#else

	if ( v[0] < b[0][0] ) {
		b[0][0] = v[0];
	}
	if ( v[1] < b[0][1] ) {
		b[0][1] = v[1];
	}
	if ( v[2] < b[0][2] ) {
		b[0][2] = v[2];
	}
	if ( v[0] > b[1][0] ) {
		b[1][0] = v[0];
	}
	if ( v[1] > b[1][1] ) {
		b[1][1] = v[1];
	}
	if ( v[2] > b[1][2] ) {
		b[1][2] = v[2];
	}

#endif
}

ID_INLINE bool idBounds::AddPointExpanded( const idVec3 &v ) {
	bool expanded = false;
	if ( v[0] < b[0][0]) {
		b[0][0] = v[0];
		expanded = true;
	}
	if ( v[0] > b[1][0]) {
		b[1][0] = v[0];
		expanded = true;
	}
	if ( v[1] < b[0][1] ) {
		b[0][1] = v[1];
		expanded = true;
	}
	if ( v[1] > b[1][1]) {
		b[1][1] = v[1];
		expanded = true;
	}
	if ( v[2] < b[0][2] ) {
		b[0][2] = v[2];
		expanded = true;
	}
	if ( v[2] > b[1][2]) {
		b[1][2] = v[2];
		expanded = true;
	}
	return expanded;
}

ID_INLINE void idBounds::AddBounds( const idBounds &a ) {
#if defined( ID_WIN_X86_SSE )

	__asm {
		mov			esi, this
		mov			edi, a
		movss		xmm0, [esi+0]
		minss		xmm0, [edi+0]
		movss		[esi+0], xmm0
		movss		xmm1, [esi+4]
		minss		xmm1, [edi+4]
		movss		[esi+4], xmm1
		movss		xmm2, [esi+8]
		minss		xmm2, [edi+8]
		movss		[esi+8], xmm2
		movss		xmm3, [esi+12]
		maxss		xmm3, [edi+12]
		movss		[esi+12], xmm3
		movss		xmm4, [esi+16]
		maxss		xmm4, [edi+16]
		movss		[esi+16], xmm4
		movss		xmm5, [esi+20]
		maxss		xmm5, [edi+20]
		movss		[esi+20], xmm5
	}

#else

	if ( a[0][0] < b[0][0] ) {
		b[0][0] = a[0][0];
	}
	if ( a[0][1] < b[0][1] ) {
		b[0][1] = a[0][1];
	}
	if ( a[0][2] < b[0][2] ) {
		b[0][2] = a[0][2];
	}
	if ( a[1][0] > b[1][0] ) {
		b[1][0] = a[1][0];
	}
	if ( a[1][1] > b[1][1] ) {
		b[1][1] = a[1][1];
	}
	if ( a[1][2] > b[1][2] ) {
		b[1][2] = a[1][2];
	}

#endif
}

ID_INLINE bool idBounds::AddBoundsExpanded( const idBounds &a ) {
	bool expanded = false;
	if ( a.b[0][0] < b[0][0] ) {
		b[0][0] = a.b[0][0];
		expanded = true;
	}
	if ( a.b[0][1] < b[0][1] ) {
		b[0][1] = a.b[0][1];
		expanded = true;
	}
	if ( a.b[0][2] < b[0][2] ) {
		b[0][2] = a.b[0][2];
		expanded = true;
	}
	if ( a.b[1][0] > b[1][0] ) {
		b[1][0] = a.b[1][0];
		expanded = true;
	}
	if ( a.b[1][1] > b[1][1] ) {
		b[1][1] = a.b[1][1];
		expanded = true;
	}
	if ( a.b[1][2] > b[1][2] ) {
		b[1][2] = a.b[1][2];
		expanded = true;
	}
	return expanded;
}

ID_INLINE idBounds idBounds::Intersect( const idBounds &a ) const {
	idBounds n;
	n.b[0][0] = ( a.b[0][0] > b[0][0] ) ? a.b[0][0] : b[0][0];
	n.b[0][1] = ( a.b[0][1] > b[0][1] ) ? a.b[0][1] : b[0][1];
	n.b[0][2] = ( a.b[0][2] > b[0][2] ) ? a.b[0][2] : b[0][2];
	n.b[1][0] = ( a.b[1][0] < b[1][0] ) ? a.b[1][0] : b[1][0];
	n.b[1][1] = ( a.b[1][1] < b[1][1] ) ? a.b[1][1] : b[1][1];
	n.b[1][2] = ( a.b[1][2] < b[1][2] ) ? a.b[1][2] : b[1][2];
	return n;
}

ID_INLINE idBounds &idBounds::IntersectSelf( const idBounds &a ) {
	if ( a.b[0][0] > b[0][0] ) {
		b[0][0] = a.b[0][0];
	}
	if ( a.b[0][1] > b[0][1] ) {
		b[0][1] = a.b[0][1];
	}
	if ( a.b[0][2] > b[0][2] ) {
		b[0][2] = a.b[0][2];
	}
	if ( a.b[1][0] < b[1][0] ) {
		b[1][0] = a.b[1][0];
	}
	if ( a.b[1][1] < b[1][1] ) {
		b[1][1] = a.b[1][1];
	}
	if ( a.b[1][2] < b[1][2] ) {
		b[1][2] = a.b[1][2];
	}
	return *this;
}

ID_INLINE idBounds idBounds::Expand( const float d ) const {
	return idBounds( idVec3( b[0][0] - d, b[0][1] - d, b[0][2] - d ),
						idVec3( b[1][0] + d, b[1][1] + d, b[1][2] + d ) );
}

ID_INLINE idBounds &idBounds::ExpandSelf( const float d ) {
	b[0][0] -= d;
	b[0][1] -= d;
	b[0][2] -= d;
	b[1][0] += d;
	b[1][1] += d;
	b[1][2] += d;
	return *this;
}

// RAVEN BEGIN
// jscott: for BSE
ID_INLINE idBounds &idBounds::ExpandSelf( const idVec3 &d )
{
	b[0][0] -= d[0];
	b[0][1] -= d[1];
	b[0][2] -= d[2];
	b[1][0] += d[0];
	b[1][1] += d[1];
	b[1][2] += d[2];
	return( *this );
}

ID_INLINE idBounds &idBounds::DirectedExpandSelf( const idVec3 &d )  {
	if ( d[0] < 0.f ) {
		b[0][0] += d[0];
	} else {
		b[1][0] += d[0];
	}
	if ( d[1] < 0.f ) {
		b[0][1] += d[1];
	} else {
		b[1][1] += d[1];
	}
	if ( d[2] < 0.f ) {
		b[0][2] += d[2];
	} else {
		b[1][2] += d[2];
	}

	return *this;
}

ID_INLINE idVec3 idBounds::Size( void ) const {
	return( b[1] - b[0] );
}

ID_INLINE bool idBounds::Contains( const idBounds &a ) const
{
	int		i;

	for( i = 0; i < 3; i++ ) {
		if( a[1][i] > b[1][i] ) {
			return( false );
		}
		if( a[0][i] < b[0][i] ) {
			return( false );
		}
	}

	return( true );
}

// jscott: for material types
ID_INLINE int idBounds::GetLargestAxis( void ) const
{
	idVec3 work = b[1] - b[0];
	int axis = 0;

	if( work[1] > work[0] )
	{
		axis = 1;
	}
	if( work[2] > work[axis] )
	{
		axis = 2;
	}
	return( axis );
}
// RAVEN END

ID_INLINE idBounds idBounds::Translate( const idVec3 &translation ) const {
	return idBounds( b[0] + translation, b[1] + translation );
}

ID_INLINE idBounds &idBounds::TranslateSelf( const idVec3 &translation ) {
	b[0] += translation;
	b[1] += translation;
	return *this;
}

ID_INLINE idBounds idBounds::Rotate( const idMat3 &rotation ) const {
	idBounds bounds;
	bounds.FromTransformedBounds( *this, vec3_origin, rotation );
	return bounds;
}

ID_INLINE idBounds &idBounds::RotateSelf( const idMat3 &rotation ) {
	FromTransformedBounds( *this, vec3_origin, rotation );
	return *this;
}

ID_INLINE bool idBounds::ContainsPoint( const idVec3 &p ) const {
#if defined( ID_WIN_X86_SSE )

	bool r = false;

	__asm {
		mov			esi, p
		mov			edi, this
		movss		xmm0, [esi+0]
		movhps		xmm0, [esi+4]
		movaps		xmm2, xmm0
		movss		xmm1, [edi+0]
		movhps		xmm1, [edi+4]
		cmpnltps	xmm0, xmm1
		movss		xmm3, [edi+12]
		movhps		xmm3, [edi+16]
		cmpleps		xmm2, xmm3
		andps		xmm0, xmm2
		movmskps	eax, xmm0
		xor			eax, 0x0F
		setz		r
	}
	return r;

#else

	if ( p[0] < b[0][0] || p[1] < b[0][1] || p[2] < b[0][2] ||
			p[0] > b[1][0] || p[1] > b[1][1] || p[2] > b[1][2] ) {
		return false;
	}
	return true;

#endif
}

ID_INLINE bool idBounds::ContainsPoint( const idVec3 &p, const float epsilon ) const {
#if defined( ID_WIN_X86_SSE )

	bool r = false;

	__asm {
		mov			esi, p
		mov			edi, this
		movss		xmm7, epsilon
		shufps		xmm7, xmm7, 0
		movss		xmm0, [esi+0]
		movhps		xmm0, [esi+4]
		movaps		xmm2, xmm0
		movss		xmm1, [edi+0]
		movhps		xmm1, [edi+4]
		subps		xmm1, xmm7
		cmpnltps	xmm0, xmm1
		movss		xmm3, [edi+12]
		movhps		xmm3, [edi+16]
		addps		xmm3, xmm7
		cmpleps		xmm2, xmm3
		andps		xmm0, xmm2
		movmskps	eax, xmm0
		xor			eax, 0x0F
		setz		r
	}
	return r;

#else

	if ( p[0] < b[0][0] - epsilon || p[1] < b[0][1] - epsilon || p[2] < b[0][2] - epsilon ||
			p[0] > b[1][0] + epsilon || p[1] > b[1][1] + epsilon || p[2] > b[1][2] + epsilon ) {
		return false;
	}
	return true;

#endif
}

ID_INLINE bool idBounds::ContainsPoint2D( const idVec3 &p ) const {
	if ( p[0] < b[0][0] || p[1] < b[0][1] || p[0] > b[1][0] || p[1] > b[1][1] ) {
		return false;
	}
	return true;
}

ID_INLINE bool idBounds::ContainsBounds( const idBounds &a ) const {
	if ( a[0][0] < b[0][0] || a[0][1] < b[0][1] || a[0][2] < b[0][2] ||
			a[1][0] > b[1][0] || a[1][1] > b[1][1] || a[1][2] > b[1][2] ) {
		return false;
	}
	return true;
}

ID_INLINE bool idBounds::ContainsBounds( const idBounds &a, const float epsilon ) const {
	if ( a[0][0] < b[0][0] - epsilon || a[0][1] < b[0][1] - epsilon || a[0][2] < b[0][2] - epsilon ||
			a[1][0] > b[1][0] + epsilon || a[1][1] > b[1][1] + epsilon || a[1][2] > b[1][2] + epsilon ) {
		return false;
	}
	return true;
}

ID_INLINE bool idBounds::ContainsBounds2D( const idBounds &a ) const {
	if ( a[0][0] < b[0][0] || a[0][1] < b[0][1] || a[1][0] > b[1][0] || a[1][1] > b[1][1] ) {
		return false;
	}
	return true;
}

ID_INLINE bool idBounds::IntersectsBounds( const idBounds &a ) const {
#if defined( ID_WIN_X86_SSE )

	bool r = false;

	__asm {
		mov			esi, this
		mov			edi, a
		movss		xmm0, [esi+12]
		movhps		xmm0, [esi+16]
		movss		xmm1, [edi+0]
		movhps		xmm1, [edi+4]
		cmpnltps	xmm0, xmm1
		movss		xmm2, [esi+0]
		movhps		xmm2, [esi+4]
		movss		xmm3, [edi+12]
		movhps		xmm3, [edi+16]
		cmpleps		xmm2, xmm3
		andps		xmm0, xmm2
		movmskps	eax, xmm0
		xor			eax, 0x0F
		setz		r
	}
	return r;

#else

	if ( a.b[1][0] < b[0][0] || a.b[1][1] < b[0][1] || a.b[1][2] < b[0][2]
		|| a.b[0][0] > b[1][0] || a.b[0][1] > b[1][1] || a.b[0][2] > b[1][2] ) {
		return false;
	}
	return true;

#endif
}

ID_INLINE bool idBounds::IntersectsBounds( const idBounds &a, const float epsilon ) const {
#if defined( ID_WIN_X86_SSE )

	bool r = false;

	__asm {
		mov			esi, this
		mov			edi, a
		movss		xmm7, epsilon
		shufps		xmm7, xmm7, 0
		movss		xmm0, [esi+12]
		movhps		xmm0, [esi+16]
		movss		xmm1, [edi+0]
		movhps		xmm1, [edi+4]
		subps		xmm1, xmm7
		cmpnltps	xmm0, xmm1
		movss		xmm2, [esi+0]
		movhps		xmm2, [esi+4]
		movss		xmm3, [edi+12]
		movhps		xmm3, [edi+16]
		addps		xmm3, xmm7
		cmpleps		xmm2, xmm3
		andps		xmm0, xmm2
		movmskps	eax, xmm0
		xor			eax, 0x0F
		setz		r
	}
	return r;

#else

	if ( a.b[1][0] < b[0][0] - epsilon || a.b[1][1] < b[0][1] - epsilon || a.b[1][2] < b[0][2] - epsilon
		|| a.b[0][0] > b[1][0] + epsilon || a.b[0][1] > b[1][1] + epsilon || a.b[0][2] > b[1][2] + epsilon ) {
		return false;
	}
	return true;

#endif
}

ID_INLINE bool idBounds::IntersectsBounds2D( const idBounds &a ) const {
	if ( a.b[1][0] < b[0][0] || a.b[1][1] < b[0][1] || a.b[0][0] > b[1][0] || a.b[0][1] > b[1][1] ) {
		return false;
	}
	return true;
}

ID_INLINE int idBounds::GetDimension( void ) const {
	return 6;
}

ID_INLINE idSphere idBounds::ToSphere( void ) const {
	idSphere sphere;
	sphere.SetOrigin( ( b[0] + b[1] ) * 0.5f );
	sphere.SetRadius( ( b[1] - sphere.GetOrigin() ).Length() );
	return sphere;
}

ID_INLINE const float *idBounds::ToFloatPtr( void ) const {
	return &b[0].x;
}

ID_INLINE float *idBounds::ToFloatPtr( void ) {
	return &b[0].x;
}

ID_INLINE void idBounds::AxisProjection( const idVec3 &dir, float &min, float &max ) const {
	float d1, d2;
	idVec3 center, extents;

	center = ( b[0] + b[1] ) * 0.5f;
	extents = b[1] - center;

	d1 = dir * center;
	d2 = idMath::Fabs( extents[0] * dir[0] ) +
			idMath::Fabs( extents[1] * dir[1] ) +
				idMath::Fabs( extents[2] * dir[2] );

	min = d1 - d2;
	max = d1 + d2;
}

ID_INLINE void idBounds::AxisProjection( const idVec3 &origin, const idMat3 &axis, const idVec3 &dir, float &min, float &max ) const {
	float d1, d2;
	idVec3 center, extents;

	center = ( b[0] + b[1] ) * 0.5f;
	extents = b[1] - center;
	center = origin + center * axis;

	d1 = dir * center;
	d2 = idMath::Fabs( extents[0] * ( dir * axis[0] ) ) +
			idMath::Fabs( extents[1] * ( dir * axis[1] ) ) +
				idMath::Fabs( extents[2] * ( dir * axis[2] ) );

	min = d1 - d2;
	max = d1 + d2;
}

ID_INLINE const idVec3& idBounds::GetMins( void ) const {
	return b[ 0 ];
}

ID_INLINE idVec3& idBounds::GetMins( void ) {
	return b[ 0 ];
}

ID_INLINE const idVec3& idBounds::GetMaxs( void ) const {
	return b[ 1 ];
}

ID_INLINE idVec3& idBounds::GetMaxs( void ) {
	return b[ 1 ];
}

ID_INLINE idBounds idBounds::ToMaya( void ) const {
	idBounds boundsMaya;

	boundsMaya = *this;
	boundsMaya.ToMayaSelf();
	return boundsMaya;
}

ID_INLINE idBounds& idBounds::ToMayaSelf( void ) {
	b[0].ToMayaSelf();
	b[1].ToMayaSelf();
	Swap( b[0].z, b[1].z );

	return (*this);
}

ID_INLINE idBounds idBounds::FromMaya( void ) const {
	idBounds boundsId;

	boundsId = *this;
	boundsId.FromMayaSelf();
	return boundsId;
}

ID_INLINE idBounds& idBounds::FromMayaSelf( void ) {
	Swap( b[0].z, b[1].z );
	b[0].FromMayaSelf();
	b[1].FromMayaSelf();

	return (*this);
}

#endif /* !__BV_BOUNDS_H__ */
