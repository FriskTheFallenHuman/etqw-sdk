// Copyright (C) 2007 Id Software, Inc.
//

#include "precompiled.h"
#pragma hdrstop

idBox box_zero( vec3_zero, vec3_zero, mat3_identity );

/*
            4---{4}---5
 +         /|        /|
 Z      {7} {8}   {5} |
 -     /    |    /    {9}
      7--{6}----6     |
      |     |   |     |
    {11}    0---|-{0}-1
      |    /    |    /       -
      | {3}  {10} {1}       Y
      |/        |/         +
      3---{2}---2

	    - X +

  plane bits:
  0 = min x
  1 = max x
  2 = min y
  3 = max y
  4 = min z
  5 = max z

*/

static int boxVertPlanes[8] = {
	( (1<<0) | (1<<2) | (1<<4) ),
	( (1<<1) | (1<<2) | (1<<4) ),
	( (1<<1) | (1<<3) | (1<<4) ),
	( (1<<0) | (1<<3) | (1<<4) ),
	( (1<<0) | (1<<2) | (1<<5) ),
	( (1<<1) | (1<<2) | (1<<5) ),
	( (1<<1) | (1<<3) | (1<<5) ),
	( (1<<0) | (1<<3) | (1<<5) )
};

static int boxVertEdges[8][3] = {
	// bottom
	{ 3, 0, 8 },
	{ 0, 1, 9 },
	{ 1, 2, 10 },
	{ 2, 3, 11 },
	// top
	{ 7, 4, 8 },
	{ 4, 5, 9 },
	{ 5, 6, 10 },
	{ 6, 7, 11 }
};

static int boxEdgePlanes[12][2] = {
	// bottom
	{ 4, 2 },
	{ 4, 1 },
	{ 4, 3 },
	{ 4, 0 },
	// top
	{ 5, 2 },
	{ 5, 1 },
	{ 5, 3 },
	{ 5, 0 },
	// sides
	{ 0, 2 },
	{ 2, 1 },
	{ 1, 3 },
	{ 3, 0 }
};

static int boxEdgeVerts[12][2] = {
	// bottom
	{ 0, 1 },
	{ 1, 2 },
	{ 2, 3 },
	{ 3, 0 },
	// top
	{ 4, 5 },
	{ 5, 6 },
	{ 6, 7 },
	{ 7, 4 },
	// sides
	{ 0, 4 },
	{ 1, 5 },
	{ 2, 6 },
	{ 3, 7 }
};

#define POS_EDGE( e )	( e + 1 )
#define NEG_EDGE( e )	-( e + 1 )

static int boxPlaneEdges[6][4] = {
	{ NEG_EDGE( 3 ), POS_EDGE( 11 ), POS_EDGE(  7 ), NEG_EDGE(  8 ) },	// min x
	{ POS_EDGE( 9 ), POS_EDGE(  5 ), NEG_EDGE( 10 ), NEG_EDGE(  1 ) },	// max x
	{ NEG_EDGE( 0 ), POS_EDGE(  8 ), POS_EDGE(  4 ), NEG_EDGE(  9 ) },	// min y
	{ NEG_EDGE( 2 ), POS_EDGE( 10 ), POS_EDGE(  6 ), NEG_EDGE( 11 ) },	// max y
	{ POS_EDGE( 0 ), POS_EDGE(  1 ), POS_EDGE(  2 ), POS_EDGE(  3 ) },	// min z
	{ NEG_EDGE( 7 ), NEG_EDGE(  6 ), NEG_EDGE(  5 ), NEG_EDGE(  4 ) }	// max z
};

static int boxPlaneBitsSilVerts[64][7] = {
	{ 0, 0, 0, 0, 0, 0, 0 }, // 000000 = 0
	{ 4, 7, 4, 0, 3, 0, 0 }, // 000001 = 1
	{ 4, 5, 6, 2, 1, 0, 0 }, // 000010 = 2
	{ 0, 0, 0, 0, 0, 0, 0 }, // 000011 = 3
	{ 4, 4, 5, 1, 0, 0, 0 }, // 000100 = 4
	{ 6, 3, 7, 4, 5, 1, 0 }, // 000101 = 5
	{ 6, 4, 5, 6, 2, 1, 0 }, // 000110 = 6
	{ 0, 0, 0, 0, 0, 0, 0 }, // 000111 = 7
	{ 4, 6, 7, 3, 2, 0, 0 }, // 001000 = 8
	{ 6, 6, 7, 4, 0, 3, 2 }, // 001001 = 9
	{ 6, 5, 6, 7, 3, 2, 1 }, // 001010 = 10
	{ 0, 0, 0, 0, 0, 0, 0 }, // 001011 = 11
	{ 0, 0, 0, 0, 0, 0, 0 }, // 001100 = 12
	{ 0, 0, 0, 0, 0, 0, 0 }, // 001101 = 13
	{ 0, 0, 0, 0, 0, 0, 0 }, // 001110 = 14
	{ 0, 0, 0, 0, 0, 0, 0 }, // 001111 = 15
	{ 4, 0, 1, 2, 3, 0, 0 }, // 010000 = 16
	{ 6, 0, 1, 2, 3, 7, 4 }, // 010001 = 17
	{ 6, 3, 2, 6, 5, 1, 0 }, // 010010 = 18
	{ 0, 0, 0, 0, 0, 0, 0 }, // 010011 = 19
	{ 6, 1, 2, 3, 0, 4, 5 }, // 010100 = 20
	{ 6, 1, 2, 3, 7, 4, 5 }, // 010101 = 21
	{ 6, 2, 3, 0, 4, 5, 6 }, // 010110 = 22
	{ 0, 0, 0, 0, 0, 0, 0 }, // 010111 = 23
	{ 6, 0, 1, 2, 6, 7, 3 }, // 011000 = 24
	{ 6, 0, 1, 2, 6, 7, 4 }, // 011001 = 25
	{ 6, 0, 1, 5, 6, 7, 3 }, // 011010 = 26
	{ 0, 0, 0, 0, 0, 0, 0 }, // 011011 = 27
	{ 0, 0, 0, 0, 0, 0, 0 }, // 011100 = 28
	{ 0, 0, 0, 0, 0, 0, 0 }, // 011101 = 29
	{ 0, 0, 0, 0, 0, 0, 0 }, // 011110 = 30
	{ 0, 0, 0, 0, 0, 0, 0 }, // 011111 = 31
	{ 4, 7, 6, 5, 4, 0, 0 }, // 100000 = 32
	{ 6, 7, 6, 5, 4, 0, 3 }, // 100001 = 33
	{ 6, 5, 4, 7, 6, 2, 1 }, // 100010 = 34
	{ 0, 0, 0, 0, 0, 0, 0 }, // 100011 = 35
	{ 6, 4, 7, 6, 5, 1, 0 }, // 100100 = 36
	{ 6, 3, 7, 6, 5, 1, 0 }, // 100101 = 37
	{ 6, 4, 7, 6, 2, 1, 0 }, // 100110 = 38
	{ 0, 0, 0, 0, 0, 0, 0 }, // 100111 = 39
	{ 6, 6, 5, 4, 7, 3, 2 }, // 101000 = 40
	{ 6, 6, 5, 4, 0, 3, 2 }, // 101001 = 41
	{ 6, 5, 4, 7, 3, 2, 1 }, // 101010 = 42
	{ 0, 0, 0, 0, 0, 0, 0 }, // 101011 = 43
	{ 0, 0, 0, 0, 0, 0, 0 }, // 101100 = 44
	{ 0, 0, 0, 0, 0, 0, 0 }, // 101101 = 45
	{ 0, 0, 0, 0, 0, 0, 0 }, // 101110 = 46
	{ 0, 0, 0, 0, 0, 0, 0 }, // 101111 = 47
	{ 0, 0, 0, 0, 0, 0, 0 }, // 110000 = 48
	{ 0, 0, 0, 0, 0, 0, 0 }, // 110001 = 49
	{ 0, 0, 0, 0, 0, 0, 0 }, // 110010 = 50
	{ 0, 0, 0, 0, 0, 0, 0 }, // 110011 = 51
	{ 0, 0, 0, 0, 0, 0, 0 }, // 110100 = 52
	{ 0, 0, 0, 0, 0, 0, 0 }, // 110101 = 53
	{ 0, 0, 0, 0, 0, 0, 0 }, // 110110 = 54
	{ 0, 0, 0, 0, 0, 0, 0 }, // 110111 = 55
	{ 0, 0, 0, 0, 0, 0, 0 }, // 111000 = 56
	{ 0, 0, 0, 0, 0, 0, 0 }, // 111001 = 57
	{ 0, 0, 0, 0, 0, 0, 0 }, // 111010 = 58
	{ 0, 0, 0, 0, 0, 0, 0 }, // 111011 = 59
	{ 0, 0, 0, 0, 0, 0, 0 }, // 111100 = 60
	{ 0, 0, 0, 0, 0, 0, 0 }, // 111101 = 61
	{ 0, 0, 0, 0, 0, 0, 0 }, // 111110 = 62
	{ 0, 0, 0, 0, 0, 0, 0 }, // 111111 = 63
};


/*
============
idBox::AddPoint
============
*/
bool idBox::AddPoint( const idVec3 &v ) {
	idMat3 axis2;
	idBounds bounds1, bounds2;

	if ( extents[0] < 0.0f ) {
		extents.Zero();
		center = v;
		axis.Identity();
		return true;
	}

	bounds1[0][0] = bounds1[1][0] = center * axis[0];
	bounds1[0][1] = bounds1[1][1] = center * axis[1];
	bounds1[0][2] = bounds1[1][2] = center * axis[2];
	bounds1[0] -= extents;
	bounds1[1] += extents;
	if ( !bounds1.AddPointExpanded( idVec3( v * axis[0], v * axis[1], v * axis[2] ) ) ) {
		// point is contained in the box
		return false;
	}

	axis2[0] = v - center;
	axis2[0].Normalize();
	axis2[1] = axis[ Min3Index( axis2[0] * axis[0], axis2[0] * axis[1], axis2[0] * axis[2] ) ];
	axis2[1] = axis2[1] - ( axis2[1] * axis2[0] ) * axis2[0];
	axis2[1].Normalize();
	axis2[2].Cross( axis2[0], axis2[1] );

	AxisProjection( axis2, bounds2 );
	bounds2.AddPoint( idVec3( v * axis2[0], v * axis2[1], v * axis2[2] ) );

	// create new box based on the smallest bounds
	if ( bounds1.GetVolume() < bounds2.GetVolume() ) {
		center = ( bounds1[0] + bounds1[1] ) * 0.5f;
		extents = bounds1[1] - center;
		center *= axis;
	}
	else {
		center = ( bounds2[0] + bounds2[1] ) * 0.5f;
		extents = bounds2[1] - center;
		center *= axis2;
		axis = axis2;
	}
	return true;
}

/*
============
idBox::AddBox
============
*/
bool idBox::AddBox( const idBox &a ) {
	int i;
	int besti = 0;
	float v, bestv;
	idVec3 dir;
	idMat3 ax[4];
	idBounds bounds[4], b;

	if ( a.extents[0] < 0.0f ) {
		return false;
	}

	if ( extents[0] < 0.0f ) {
		center = a.center;
		extents = a.extents;
		axis = a.axis;
		return true;
	}

	// test axis of this box
	ax[0] = axis;
	bounds[0][0][0] = bounds[0][1][0] = center * ax[0][0];
	bounds[0][0][1] = bounds[0][1][1] = center * ax[0][1];
	bounds[0][0][2] = bounds[0][1][2] = center * ax[0][2];
	bounds[0][0] -= extents;
	bounds[0][1] += extents;
	a.AxisProjection( ax[0], b );
	if ( !bounds[0].AddBoundsExpanded( b ) ) {
		// the other box is contained in this box
		return false;
	}

	// test axis of other box
	ax[1] = a.axis;
	bounds[1][0][0] = bounds[1][1][0] = a.center * ax[1][0];
	bounds[1][0][1] = bounds[1][1][1] = a.center * ax[1][1];
	bounds[1][0][2] = bounds[1][1][2] = a.center * ax[1][2];
	bounds[1][0] -= a.extents;
	bounds[1][1] += a.extents;
	AxisProjection( ax[1], b );
	if ( !bounds[1].AddBoundsExpanded( b ) ) {
		// this box is contained in the other box
		center = a.center;
		extents = a.extents;
		axis = a.axis;
		return true;
	}

	// test axes aligned with the vector between the box centers and one of the box axis
	dir = a.center - center;
	dir.Normalize();
	for ( i = 2; i < 4; i++ ) {
		ax[i][0] = dir;
		ax[i][1] = ax[i-2][ Min3Index( dir * ax[i-2][0], dir * ax[i-2][1], dir * ax[i-2][2] ) ];
		ax[i][1] = ax[i][1] - ( ax[i][1] * dir ) * dir;
		ax[i][1].Normalize();
		ax[i][2].Cross( dir, ax[i][1] );

		AxisProjection( ax[i], bounds[i] );
		a.AxisProjection( ax[i], b );
		bounds[i].AddBounds( b );
	}

	// get the bounds with the smallest volume
	bestv = idMath::INFINITY;
	besti = 0;
	for ( i = 0; i < 4; i++ ) {
		v = bounds[i].GetVolume();
		if ( v < bestv ) {
			bestv = v;
			besti = i;
		}
	}

	// create a box from the smallest bounds axis pair
	center = ( bounds[besti][0] + bounds[besti][1] ) * 0.5f;
	extents = bounds[besti][1] - center;
	center *= ax[besti];
	axis = ax[besti];

	return false;
}

/*
================
idBox::PlaneDistance
================
*/
float idBox::PlaneDistance( const idPlane &plane ) const {
	float d1, d2;

	d1 = plane.Distance( center );
	d2 = idMath::Fabs( extents[0] * ( plane.Normal() * axis[0] ) ) +
			idMath::Fabs( extents[1] * ( plane.Normal() * axis[1] ) ) +
				idMath::Fabs( extents[2] * ( plane.Normal() * axis[2] ) );

	if ( d1 - d2 > 0.0f ) {
		return d1 - d2;
	}
	if ( d1 + d2 < 0.0f ) {
		return d1 + d2;
	}
	return 0.0f;
}

/*
================
idBox::PlaneSide
================
*/
int idBox::PlaneSide( const idPlane &plane, const float epsilon ) const {
	float d1, d2;

	d1 = plane.Distance( center );
	d2 = idMath::Fabs( extents[0] * ( plane.Normal() * axis[0] ) ) +
			idMath::Fabs( extents[1] * ( plane.Normal() * axis[1] ) ) +
				idMath::Fabs( extents[2] * ( plane.Normal() * axis[2] ) );

	if ( d1 - d2 > epsilon ) {
		return PLANESIDE_FRONT;
	}
	if ( d1 + d2 < -epsilon ) {
		return PLANESIDE_BACK;
	}
	return PLANESIDE_CROSS;
}

/*
============
idBox::IntersectsBox
============
*/
bool idBox::IntersectsBox( const idBox &a ) const {
    idVec3 dir;			// vector between centers
    float c[3][3];		// matrix c = axis.Transpose() * a.axis
    float ac[3][3];		// absolute values of c
    float axisdir[3];	// axis[i] * dir
    float d, e0, e1;	// distance between centers and projected extents

	dir = a.center - center;

    // axis C0 + t * A0
    c[0][0] = axis[0] * a.axis[0];
    c[0][1] = axis[0] * a.axis[1];
    c[0][2] = axis[0] * a.axis[2];
    axisdir[0] = axis[0] * dir;
    ac[0][0] = idMath::Fabs( c[0][0] );
    ac[0][1] = idMath::Fabs( c[0][1] );
    ac[0][2] = idMath::Fabs( c[0][2] );

    d = idMath::Fabs( axisdir[0] );
	e0 = extents[0];
    e1 = a.extents[0] * ac[0][0] + a.extents[1] * ac[0][1] + a.extents[2] * ac[0][2];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A1
    c[1][0] = axis[1] * a.axis[0];
    c[1][1] = axis[1] * a.axis[1];
    c[1][2] = axis[1] * a.axis[2];
    axisdir[1] = axis[1] * dir;
    ac[1][0] = idMath::Fabs( c[1][0] );
    ac[1][1] = idMath::Fabs( c[1][1] );
    ac[1][2] = idMath::Fabs( c[1][2] );

    d = idMath::Fabs( axisdir[1] );
	e0 = extents[1];
    e1 = a.extents[0] * ac[1][0] + a.extents[1] * ac[1][1] + a.extents[2] * ac[1][2];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A2
    c[2][0] = axis[2] * a.axis[0];
    c[2][1] = axis[2] * a.axis[1];
    c[2][2] = axis[2] * a.axis[2];
    axisdir[2] = axis[2] * dir;
    ac[2][0] = idMath::Fabs( c[2][0] );
    ac[2][1] = idMath::Fabs( c[2][1] );
    ac[2][2] = idMath::Fabs( c[2][2] );

    d = idMath::Fabs( axisdir[2] );
	e0 = extents[2];
    e1 = a.extents[0] * ac[2][0] + a.extents[1] * ac[2][1] + a.extents[2] * ac[2][2];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * B0
    d = idMath::Fabs( a.axis[0] * dir );
    e0 = extents[0] * ac[0][0] + extents[1] * ac[1][0] + extents[2] * ac[2][0];
	e1 = a.extents[0];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * B1
    d = idMath::Fabs( a.axis[1] * dir );
    e0 = extents[0] * ac[0][1] + extents[1] * ac[1][1] + extents[2] * ac[2][1];
	e1 = a.extents[1];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * B2
    d = idMath::Fabs( a.axis[2] * dir );
    e0 = extents[0] * ac[0][2] + extents[1] * ac[1][2] + extents[2] * ac[2][2];
	e1 = a.extents[2];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A0xB0
    d = idMath::Fabs( axisdir[2] * c[1][0] - axisdir[1] * c[2][0] );
    e0 = extents[1] * ac[2][0] + extents[2] * ac[1][0];
    e1 = a.extents[1] * ac[0][2] + a.extents[2] * ac[0][1];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A0xB1
    d = idMath::Fabs( axisdir[2] * c[1][1] - axisdir[1] * c[2][1] );
    e0 = extents[1] * ac[2][1] + extents[2] * ac[1][1];
    e1 = a.extents[0] * ac[0][2] + a.extents[2] * ac[0][0];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A0xB2
    d = idMath::Fabs( axisdir[2] * c[1][2] - axisdir[1] * c[2][2] );
    e0 = extents[1] * ac[2][2] + extents[2] * ac[1][2];
    e1 = a.extents[0] * ac[0][1] + a.extents[1] * ac[0][0];
    if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A1xB0
    d = idMath::Fabs( axisdir[0] * c[2][0] - axisdir[2] * c[0][0] );
    e0 = extents[0] * ac[2][0] + extents[2] * ac[0][0];
    e1 = a.extents[1] * ac[1][2] + a.extents[2] * ac[1][1];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A1xB1
    d = idMath::Fabs( axisdir[0] * c[2][1] - axisdir[2] * c[0][1] );
    e0 = extents[0] * ac[2][1] + extents[2] * ac[0][1];
    e1 = a.extents[0] * ac[1][2] + a.extents[2] * ac[1][0];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A1xB2
    d = idMath::Fabs( axisdir[0] * c[2][2] - axisdir[2] * c[0][2] );
    e0 = extents[0] * ac[2][2] + extents[2] * ac[0][2];
    e1 = a.extents[0] * ac[1][1] + a.extents[1] * ac[1][0];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A2xB0
    d = idMath::Fabs( axisdir[1] * c[0][0] - axisdir[0] * c[1][0] );
    e0 = extents[0] * ac[1][0] + extents[1] * ac[0][0];
    e1 = a.extents[1] * ac[2][2] + a.extents[2] * ac[2][1];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A2xB1
    d = idMath::Fabs( axisdir[1] * c[0][1] - axisdir[0] * c[1][1] );
    e0 = extents[0] * ac[1][1] + extents[1] * ac[0][1];
    e1 = a.extents[0] * ac[2][2] + a.extents[2] * ac[2][0];
	if ( d > e0 + e1 ) {
        return false;
	}

    // axis C0 + t * A2xB2
    d = idMath::Fabs( axisdir[1] * c[0][2] - axisdir[0] * c[1][2] );
    e0 = extents[0] * ac[1][2] + extents[1] * ac[0][2];
    e1 = a.extents[0] * ac[2][1] + a.extents[1] * ac[2][0];
	if ( d > e0 + e1 ) {
        return false;
	}
    return true;
}

/*
============
idBox::LineIntersection

  Returns true if the line intersects the box between the start and end point.
============
*/
bool idBox::LineIntersection( const idVec3 &start, const idVec3 &end ) const {
    float ld[3];
    idVec3 lineDir = 0.5f * ( end - start );
    idVec3 lineCenter = start + lineDir;
    idVec3 dir = lineCenter - center;

    ld[0] = idMath::Fabs( lineDir * axis[0] );
	if ( idMath::Fabs( dir * axis[0] ) > extents[0] + ld[0] ) {
        return false;
	}

    ld[1] = idMath::Fabs( lineDir * axis[1] );
	if ( idMath::Fabs( dir * axis[1] ) > extents[1] + ld[1] ) {
        return false;
	}

    ld[2] = idMath::Fabs( lineDir * axis[2] );
	if ( idMath::Fabs( dir * axis[2] ) > extents[2] + ld[2] ) {
        return false;
	}

    idVec3 cross = lineDir.Cross( dir );

	if ( idMath::Fabs( cross * axis[0] ) > extents[1] * ld[2] + extents[2] * ld[1] ) {
        return false;
	}

	if ( idMath::Fabs( cross * axis[1] ) > extents[0] * ld[2] + extents[2] * ld[0] ) {
        return false;
	}

	if ( idMath::Fabs( cross * axis[2] ) > extents[0] * ld[1] + extents[1] * ld[0] ) {
        return false;
	}

    return true;
}

/*
============
BoxPlaneClip
============
*/
static bool BoxPlaneClip( const float denom, const float numer, float &scale0, float &scale1 ) {
	if ( denom > 0.0f ) {
		if ( numer > denom * scale1 ) {
			return false;
		}
		if ( numer > denom * scale0 ) {
			scale0 = numer / denom;
		}
		return true;
	}
	else if ( denom < 0.0f ) {
		if ( numer > denom * scale0 ) {
			return false;
		}
		if ( numer > denom * scale1 ) {
			scale1 = numer / denom;
		}
		return true;
	}
	else {
		return ( numer <= 0.0f );
	}
}

/*
============
idBox::RayIntersection

  Returns true if the ray intersects the box.
  The ray can intersect the box in both directions from the start point.
  If start is inside the box then scale1 < 0 and scale2 > 0.
============
*/
bool idBox::RayIntersection( const idVec3 &start, const idVec3 &dir, float &scale1, float &scale2 ) const {
	idVec3 localStart, localDir;

	localStart = ( start - center ) * axis.Transpose();
	localDir = dir * axis.Transpose();

	scale1 = -idMath::INFINITY;
	scale2 = idMath::INFINITY;
    return	BoxPlaneClip(  localDir.x, -localStart.x - extents[0], scale1, scale2 ) &&
			BoxPlaneClip( -localDir.x,  localStart.x - extents[0], scale1, scale2 ) &&
			BoxPlaneClip(  localDir.y, -localStart.y - extents[1], scale1, scale2 ) &&
			BoxPlaneClip( -localDir.y,  localStart.y - extents[1], scale1, scale2 ) &&
			BoxPlaneClip(  localDir.z, -localStart.z - extents[2], scale1, scale2 ) &&
			BoxPlaneClip( -localDir.z,  localStart.z - extents[2], scale1, scale2 );
}

/*
============
idBox::FromPoints

  Tight box for a collection of points.
============
*/
void idBox::FromPoints( const idVec3 *points, const int numPoints ) {
	int i;
	float invNumPoints, sumXX, sumXY, sumXZ, sumYY, sumYZ, sumZZ;
	idVec3 dir;
	idBounds bounds;
	idMatX eigenVectors;
	idVecX eigenValues;

	// compute mean of points
	center = points[0];
	for ( i = 1; i < numPoints; i++ ) {
		center += points[i];
	}
	invNumPoints = 1.0f / numPoints;
	center *= invNumPoints;

	// compute covariances of points
	sumXX = 0.0f; sumXY = 0.0f; sumXZ = 0.0f;
	sumYY = 0.0f; sumYZ = 0.0f; sumZZ = 0.0f;
	for ( i = 0; i < numPoints; i++ ) {
		dir = points[i] - center;
		sumXX += dir.x * dir.x;
		sumXY += dir.x * dir.y;
		sumXZ += dir.x * dir.z;
		sumYY += dir.y * dir.y;
		sumYZ += dir.y * dir.z;
		sumZZ += dir.z * dir.z;
	}
	sumXX *= invNumPoints;
	sumXY *= invNumPoints;
	sumXZ *= invNumPoints;
	sumYY *= invNumPoints;
	sumYZ *= invNumPoints;
	sumZZ *= invNumPoints;

	// compute eigenvectors for covariance matrix
	eigenValues.SetData( 3, VECX_ALLOCA( 3 ) );
	eigenVectors.SetData( 3, 3, MATX_ALLOCA( 3 * 3 ) );

	eigenVectors[0][0] = sumXX;
	eigenVectors[0][1] = sumXY;
	eigenVectors[0][2] = sumXZ;
	eigenVectors[1][0] = sumXY;
	eigenVectors[1][1] = sumYY;
	eigenVectors[1][2] = sumYZ;
	eigenVectors[2][0] = sumXZ;
	eigenVectors[2][1] = sumYZ;
	eigenVectors[2][2] = sumZZ;
	eigenVectors.Eigen_SolveSymmetric( eigenValues );
	eigenVectors.Eigen_SortIncreasing( eigenValues );

	axis[1][0] = eigenVectors[0][1];
	axis[1][1] = eigenVectors[1][1];
	axis[1][2] = eigenVectors[2][1];

	axis[2][0] = eigenVectors[0][2];
	axis[2][1] = eigenVectors[1][2];
	axis[2][2] = eigenVectors[2][2];

	axis[0].Cross( axis[1], axis[2] );

	// if two eigenValues are the same then we need to search rotations to get the smallest volume box
	const float epsilon = 0.1f;
	int rotationAxis = -1;
	if ( idMath::Fabs( eigenValues[0] - eigenValues[1] ) < epsilon ) {
		rotationAxis = 2;
	} else if ( idMath::Fabs( eigenValues[0] - eigenValues[2] ) < epsilon ) {
		rotationAxis = 1;
	} else if ( idMath::Fabs( eigenValues[1] - eigenValues[2] ) < epsilon ) {
		rotationAxis = 0;
	}
	if ( rotationAxis >= 0 ) {
		idRotation rot;
		rot.SetVec( axis[rotationAxis] );
		bounds.Clear();
		for ( i = 0; i < numPoints; i++ ) {
			bounds.AddPoint( idVec3( points[i] * axis[0], points[i] * axis[1], points[i] * axis[2] ) );
		}
		float bestVolume = bounds.GetVolume();
		float bestAngle = 0.0f;
		float angleStep = 30.0f;
		for ( int iteration = 0; iteration < 8; iteration++ ) {
			float startAngle = bestAngle;
			for ( int a = -1; a <= 1; a += 2 ) {
				rot.SetAngle( startAngle + ( a * angleStep ) );
				idMat3 a2 = axis * rot.ToMat3();
				bounds.Clear();
				for ( i = 0; i < numPoints; i++ ) {
					bounds.AddPoint( idVec3( points[i] * a2[0], points[i] * a2[1], points[i] * a2[2] ) );
				}
				float volume = bounds.GetVolume();
				if ( volume < bestVolume ) {
					bestVolume = volume;
					bestAngle = rot.GetAngle();
				}
			}
			angleStep *= 0.5f;
		}
		rot.SetAngle( bestAngle );
		axis *= rot.ToMat3();
	}

	// refine by calculating the bounds of the points projected onto the axis and adjusting the center and extents
	bounds.Clear();
	for ( i = 0; i < numPoints; i++ ) {
		bounds.AddPoint( idVec3( points[i] * axis[0], points[i] * axis[1], points[i] * axis[2] ) );
	}

	center = ( bounds[0] + bounds[1] ) * 0.5f;
	extents = bounds[1] - center;
	center *= axis;
}

/*
============
idBox::FromPointTranslation

  Most tight box for the translational movement of the given point.
============
*/
void idBox::FromPointTranslation( const idVec3 &point, const idVec3 &translation ) {
	// FIXME: implement
}

/*
============
idBox::FromBoxTranslation

  Most tight box for the translational movement of the given box.
============
*/
void idBox::FromBoxTranslation( const idBox &box, const idVec3 &translation ) {
	// FIXME: implement
}

/*
============
idBox::FromPointRotation

  Most tight bounds for the rotational movement of the given point.
============
*/
void idBox::FromPointRotation( const idVec3 &point, const idRotation &rotation ) {
	// FIXME: implement
}

/*
============
idBox::FromBoxRotation

  Most tight box for the rotational movement of the given box.
============
*/
void idBox::FromBoxRotation( const idBox &box, const idRotation &rotation ) {
	// FIXME: implement
}

/*
============
idBox::ToPoints
============
*/
void idBox::ToPoints( idVec3 points[8] ) const {
	idMat3 ax;
	idVec3 temp[4];

	ax[0] = extents[0] * axis[0];
	ax[1] = extents[1] * axis[1];
	ax[2] = extents[2] * axis[2];
	temp[0] = center - ax[0];
	temp[1] = center + ax[0];
	temp[2] = ax[1] - ax[2];
	temp[3] = ax[1] + ax[2];
	points[0] = temp[0] - temp[3];
	points[1] = temp[1] - temp[3];
	points[2] = temp[1] + temp[2];
	points[3] = temp[0] + temp[2];
	points[4] = temp[0] - temp[2];
	points[5] = temp[1] - temp[2];
	points[6] = temp[1] + temp[3];
	points[7] = temp[0] + temp[3];
}

/*
============
idBox::GetProjectionSilhouetteVerts
============
*/
int idBox::GetProjectionSilhouetteVerts( const idVec3 &projectionOrigin, idVec3 silVerts[6] ) const {
	float f;
	int i, planeBits, *index;
	idVec3 points[8], dir1, dir2;

	ToPoints( points );

	dir1 = points[0] - projectionOrigin;
	dir2 = points[6] - projectionOrigin;
	f = dir1 * axis[0];
	planeBits = FLOATSIGNBITNOTSET( f );
	f = dir2 * axis[0];
	planeBits |= FLOATSIGNBITSET( f ) << 1;
	f = dir1 * axis[1];
	planeBits |= FLOATSIGNBITNOTSET( f ) << 2;
	f = dir2 * axis[1];
	planeBits |= FLOATSIGNBITSET( f ) << 3;
	f = dir1 * axis[2];
	planeBits |= FLOATSIGNBITNOTSET( f ) << 4;
	f = dir2 * axis[2];
	planeBits |= FLOATSIGNBITSET( f ) << 5;

	index = boxPlaneBitsSilVerts[planeBits];
	for ( i = 0; i < index[0]; i++ ) {
		silVerts[i] = points[index[i+1]];
	}

	return index[0];
}

/*
============
idBox::GetParallelProjectionSilhouetteVerts
============
*/
int idBox::GetParallelProjectionSilhouetteVerts( const idVec3 &projectionDir, idVec3 silVerts[6] ) const {
	float f;
	int i, planeBits, *index;
	idVec3 points[8];

	ToPoints( points );

	planeBits = 0;
	f = projectionDir * axis[0];
	if ( FLOATNOTZERO( f ) ) {
		planeBits = 1 << FLOATSIGNBITSET( f );
	}
	f = projectionDir * axis[1];
	if ( FLOATNOTZERO( f ) ) {
		planeBits |= 4 << FLOATSIGNBITSET( f );
	}
	f = projectionDir * axis[2];
	if ( FLOATNOTZERO( f ) ) {
		planeBits |= 16 << FLOATSIGNBITSET( f );
	}

	index = boxPlaneBitsSilVerts[planeBits];
	for ( i = 0; i < index[0]; i++ ) {
		silVerts[i] = points[index[i+1]];
	}

	return index[0];
}
