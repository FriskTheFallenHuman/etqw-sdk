// Copyright (C) 2007 Id Software, Inc.
//

#include "precompiled.h"
#pragma hdrstop


idSphere sphere_zero( vec3_zero, 0.0f );


/*
================
idSphere::PlaneDistance
================
*/
float idSphere::PlaneDistance( const idPlane &plane ) const {
	float d;

	d = plane.Distance( origin );
	if ( d > radius ) {
		return d - radius;
	}
	if ( d < -radius ) {
		return d + radius;
	}
	return 0.0f;
}

/*
================
idSphere::PlaneSide
================
*/
int idSphere::PlaneSide( const idPlane &plane, const float epsilon ) const {
	float d;

	d = plane.Distance( origin );
	if ( d > radius + epsilon ) {
		return PLANESIDE_FRONT;
	}
	if ( d < -radius - epsilon ) {
		return PLANESIDE_BACK;
	}
	return PLANESIDE_CROSS;
}

/*
============
idSphere::LineIntersection

  Returns true if the line intersects the sphere between the start and end point.
============
*/
bool idSphere::LineIntersection( const idVec3 &start, const idVec3 &end ) const {
	idVec3 r, s, e;
	float a;

	s = start - origin;
	e = end - origin;
	r = e - s;
	a = -s * r;
	if ( a <= 0 ) {
		return ( s * s < radius * radius );
	}
	else if ( a >= r * r ) {
		return ( e * e < radius * radius );
	}
	else {
		r = s + ( a / ( r * r ) ) * r;
		return ( r * r < radius * radius );
	}
}

/*
============
idSphere::RayIntersection

  Returns true if the ray intersects the sphere.
  The ray can intersect the sphere in both directions from the start point.
  If start is inside the sphere then scale1 < 0 and scale2 > 0.
============
*/
bool idSphere::RayIntersection( const idVec3 &start, const idVec3 &dir, float &scale1, float &scale2 ) const {
	double a, b, c, d, sqrtd;
	idVec3 p;

	p = start - origin;
	a = dir * dir;
	b = dir * p;
	c = p * p - radius * radius;
	d = b * b - c * a;

	if ( d < 0.0f ) {
		return false;
	}

	sqrtd = idMath::Sqrt( d );
	a = 1.0f / a;

	scale1 = ( -b + sqrtd ) * a;
	scale2 = ( -b - sqrtd ) * a;

	return true;
}

/*
============
idSphere::FromPoints

  Tight sphere for a point set.
============
*/
void idSphere::FromPoints( const idVec3 *points, const int numPoints ) {
	int i;
	float radiusSqr, dist;
	idVec3 mins, maxs;

	SIMDProcessor->MinMax( mins, maxs, points, numPoints );

	origin = ( mins + maxs ) * 0.5f;

	radiusSqr = 0.0f;
	for ( i = 0; i < numPoints; i++ ) {
		dist = ( points[i] - origin ).LengthSqr();
		if ( dist > radiusSqr ) {
			radiusSqr = dist;
		}
	}
	radius = idMath::Sqrt( radiusSqr );
}

/*
============
idSphere::IntersectsBounds
============
*/
bool idSphere::IntersectsBounds( const idBounds& bounds ) const {
	const idVec3& mins = bounds.GetMins();
	const idVec3& maxs = bounds.GetMaxs();

	float dmin = 0;
	int i;
	for( i = 0; i < 3; i++ ) {
		if( origin[ i ] < mins[ i ] ) {
			dmin += Square( origin[ i ] - mins[ i ] );
		} else if( origin[ i ] > maxs[ i ] ) {
			dmin += Square( origin[ i ] - maxs[ i ] );
		}
	}
	return dmin <= Square( radius );
}
