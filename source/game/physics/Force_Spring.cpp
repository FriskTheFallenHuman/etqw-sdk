// Copyright (C) 2007 Id Software, Inc.
//

#include "Game_Precompiled.h"
#pragma hdrstop

#if defined( _DEBUG ) && !defined( ID_REDIRECT_NEWDELETE )
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Force_Spring.h"
#include "Physics.h"

CLASS_DECLARATION( idForce, idForce_Spring )
END_CLASS

/*
================
idForce_Spring::idForce_Spring
================
*/
idForce_Spring::idForce_Spring( void ) {
	Kstretch		= 100.0f;
	Kcompress		= 100.0f;
	damping			= 0.0f;
	restLength		= 0.0f;
	physics1		= NULL;
	id1				= 0;
	p1				= vec3_zero;
	physics2		= NULL;
	id2				= 0;
	p2				= vec3_zero;
}

/*
================
idForce_Spring::~idForce_Spring
================
*/
idForce_Spring::~idForce_Spring( void ) {
}

/*
================
idForce_Spring::InitSpring
================
*/
void idForce_Spring::InitSpring( float Kstretch, float Kcompress, float damping, float restLength ) {
	this->Kstretch = Kstretch;
	this->Kcompress = Kcompress;
	this->damping = damping;
	this->restLength = restLength;
}

/*
================
idForce_Spring::SetPosition
================
*/
void idForce_Spring::SetPosition( idPhysics *physics1, int id1, const idVec3 &p1, idPhysics *physics2, int id2, const idVec3 &p2 ) {
	this->physics1 = physics1;
	this->id1 = id1;
	this->p1 = p1;
	this->physics2 = physics2;
	this->id2 = id2;
	this->p2 = p2;
}

/*
================
idForce_Spring::Evaluate
================
*/
void idForce_Spring::Evaluate( int time ) {
	float length;
	idMat3 axis;
	idVec3 pos1, pos2, velocity1, velocity2, force, dampingForce;
	impactInfo_t info;

	pos1 = p1;
	pos2 = p2;
	velocity1 = velocity2 = vec3_origin;

	if ( physics1 ) {
		axis = physics1->GetAxis( id1 );
		pos1 = physics1->GetOrigin( id1 );
		pos1 += p1 * axis;
		if ( damping > 0.0f ) {
			physics1->GetImpactInfo( id1, pos1, &info );
			velocity1 = info.velocity;
		}
	}

	if ( physics2 ) {
		axis = physics2->GetAxis( id2 );
		pos2 = physics2->GetOrigin( id2 );
		pos2 += p2 * axis;
		if ( damping > 0.0f ) {
			physics2->GetImpactInfo( id2, pos2, &info );
			velocity2 = info.velocity;
		}
	}

	force = pos2 - pos1;
	dampingForce = ( damping * ( ((velocity2 - velocity1) * force) / (force * force) ) ) * force;
	length = force.Normalize();

	// if the spring is stretched
	if ( length > restLength ) {
		if ( Kstretch > 0.0f ) {
			force = ( Square( length - restLength ) * Kstretch ) * force - dampingForce;
			if ( physics1 ) {
				physics1->AddForce( id1, pos1, force );
			}
			if ( physics2 ) {
				physics2->AddForce( id2, pos2, -force );
			}
		}
	}
	else {
		if ( Kcompress > 0.0f ) {
			force = ( Square( length - restLength ) * Kcompress ) * force - dampingForce;
			if ( physics1 ) {
				physics1->AddForce( id1, pos1, -force );
			}
			if ( physics2 ) {
				physics2->AddForce( id2, pos2, force );
			}
		}
	}
}

/*
================
idForce_Spring::RemovePhysics
================
*/
void idForce_Spring::RemovePhysics( const idPhysics *phys ) {
	if ( physics1 == phys ) {
		physics1 = NULL;
	}
	if ( physics2 == phys ) {
		physics2 = NULL;
	}
}
