// Copyright (C) 2007 Id Software, Inc.
//

#include "precompiled.h"
#pragma hdrstop

#include <float.h>

/*
=================
rvAngles::NormalizeFull

returns angles normalized to the range [0 <= angle < TWO_PI]
=================
*/
rvAngles &rvAngles::NormalizeFull( void )
{
	// Get to -TWO_PI < a < TWO_PI
	pitch = fmodf( pitch, idMath::TWO_PI );
	yaw = fmodf( yaw, idMath::TWO_PI );
	roll = fmodf( roll, idMath::TWO_PI );

	// Fix any negatives
	if( pitch < 0.0f )
	{
		pitch += idMath::TWO_PI;
	}

	if( yaw < 0.0f )
	{
		yaw += idMath::TWO_PI;
	}

	if( roll < 0.0f )
	{
		roll += idMath::TWO_PI;
	}

	return( *this );
}

/*
=================
rvAngles::NormalizeHalf

returns angles normalized to the range [-PI < angle <= PI]
=================
*/
rvAngles &rvAngles::NormalizeHalf( void )
{
	int		i;

	// Get to -TWO_PI < a < TWO_PI
	pitch = fmodf( pitch, idMath::TWO_PI );
	yaw = fmodf( yaw, idMath::TWO_PI );
	roll = fmodf( roll, idMath::TWO_PI );

	for( i = 0; i < 3; i++ )
	{
		if( ( *this )[i] < -idMath::PI )
		{
			( *this )[i] += idMath::TWO_PI;
		}

		if( ( *this )[i] > idMath::PI )
		{
			( *this )[i] -= idMath::TWO_PI;
		}
	}

	return( *this );
}

/*
=================
rvAngles::ToVectors
=================
*/
void rvAngles::ToVectors( idVec3 *forward, idVec3 *right, idVec3 *up ) const
{
	float	sr, sp, sy, cr, cp, cy;

	idMath::SinCos( yaw, sy, cy );
	idMath::SinCos( pitch, sp, cp );
	idMath::SinCos( roll, sr, cr );

	if ( forward ) {
		forward->Set( cp * cy, cp * sy, -sp );
	}

	if ( right ) {
		right->Set( -sr * sp * cy + cr * sy, -sr * sp * sy + -cr * cy, -sr * cp );
	}

	if ( up ) {
		up->Set( cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp );
	}
}


// end
