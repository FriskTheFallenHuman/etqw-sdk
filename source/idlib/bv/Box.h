// Copyright (C) 2007 Id Software, Inc.
//

#ifndef __BV_BOX_H__
#define __BV_BOX_H__

/*
===============================================================================

	Oriented Bounding Box

===============================================================================
*/

class idBox {
public:
					idBox( void );
					explicit idBox( const idVec3 &center, const idVec3 &extents, const idMat3 &axis );
					explicit idBox( const idVec3 &point );
					explicit idBox( const idBounds &bounds );
					explicit idBox( const idBounds &bounds, const idVec3 &origin, const idMat3 &axis );
					explicit idBox( const idBounds &bounds, const float modelMatrix[16] );

	idBox			operator+( const idVec3 &t ) const;				// returns translated box
	idBox &			operator+=( const idVec3 &t );					// translate the box
	idBox			operator*( const idMat3 &r ) const;				// returns rotated box
	idBox &			operator*=( const idMat3 &r );					// rotate the box
	idBox			operator+( const idBox &a ) const;
	idBox &			operator+=( const idBox &a );
	idBox			operator-( const idBox &a ) const;
	idBox &			operator-=( const idBox &a );

	bool			Compare( const idBox &a ) const;						// exact compare, no epsilon
	bool			Compare( const idBox &a, const float epsilon ) const;	// compare with epsilon
	bool			operator==(	const idBox &a ) const;						// exact compare, no epsilon
	bool			operator!=(	const idBox &a ) const;						// exact compare, no epsilon

	void			Clear( void );									// inside out box
	void			Zero( void );									// single point at origin

	const idVec3 &	GetCenter( void ) const;						// returns center of the box
	const idVec3 &	GetExtents( void ) const;						// returns extents of the box
	const idMat3 &	GetAxis( void ) const;							// returns the axis of the box
	float			GetVolume( void ) const;						// returns the volume of the box
	bool			IsCleared( void ) const;						// returns true if box are inside out

	bool			AddPoint( const idVec3 &v );					// add the point, returns true if the box expanded
	bool			AddBox( const idBox &a );						// add the box, returns true if the box expanded
	idBox			Expand( const float d ) const;					// return box expanded in all directions with the given value
	idBox &			ExpandSelf( const float d );					// expand box in all directions with the given value
	idBox &			ExpandSelf( const float dx, const float dy, const float dz );	// expand box in all directions with given values
	idBox			Translate( const idVec3 &translation ) const;	// return translated box
	idBox &			TranslateSelf( const idVec3 &translation );		// translate this box
	idBox			Rotate( const idMat3 &rotation ) const;			// return rotated box
	idBox &			RotateSelf( const idMat3 &rotation );			// rotate this box

	float			PlaneDistance( const idPlane &plane ) const;
	int				PlaneSide( const idPlane &plane, const float epsilon = ON_EPSILON ) const;

	bool			ContainsPoint( const idVec3 &p ) const;			// includes touching
	bool			IntersectsBox( const idBox &a ) const;			// includes touching
	bool			LineIntersection( const idVec3 &start, const idVec3 &end ) const;
					// intersection points are (start + dir * scale1) and (start + dir * scale2)
	bool			RayIntersection( const idVec3 &start, const idVec3 &dir, float &scale1, float &scale2 ) const;

					// tight box for a collection of points
	void			FromPoints( const idVec3 *points, const int numPoints );
					// most tight box for a translation
	void			FromPointTranslation( const idVec3 &point, const idVec3 &translation );
	void			FromBoxTranslation( const idBox &box, const idVec3 &translation );
					// most tight box for a rotation
	void			FromPointRotation( const idVec3 &point, const idRotation &rotation );
	void			FromBoxRotation( const idBox &box, const idRotation &rotation );

	void			ToPoints( idVec3 points[8] ) const;
	idSphere		ToSphere( void ) const;

					// calculates the projection of this box onto the given axis
	void			AxisProjection( const idVec3 &dir, float &min, float &max ) const;
	void			AxisProjection( const idMat3 &ax, idBounds &bounds ) const;

					// calculates the silhouette of the box
	int				GetProjectionSilhouetteVerts( const idVec3 &projectionOrigin, idVec3 silVerts[6] ) const;
	int				GetParallelProjectionSilhouetteVerts( const idVec3 &projectionDir, idVec3 silVerts[6] ) const;

private:
	idVec3			center;
	idVec3			extents;
	idMat3			axis;
};

extern idBox	box_zero;

ID_INLINE idBox::idBox( void ) {
}

ID_INLINE idBox::idBox( const idVec3 &center, const idVec3 &extents, const idMat3 &axis ) {
	this->center = center;
	this->extents = extents;
	this->axis = axis;
}

ID_INLINE idBox::idBox( const idVec3 &point ) {
	this->center = point;
	this->extents.Zero();
	this->axis.Identity();
}

ID_INLINE idBox::idBox( const idBounds &bounds ) {
	this->center = ( bounds[0] + bounds[1] ) * 0.5f;
	this->extents = bounds[1] - this->center;
	this->axis.Identity();
}

ID_INLINE idBox::idBox( const idBounds &bounds, const idVec3 &origin, const idMat3 &axis ) {
	this->center = ( bounds[0] + bounds[1] ) * 0.5f;
	this->extents = bounds[1] - this->center;
	this->center = origin + this->center * axis;
	this->axis = axis;
}

ID_INLINE idBox::idBox( const idBounds &bounds, const float modelMatrix[16] ) {
	this->center = ( bounds[0] + bounds[1] ) * 0.5f;
	this->extents = bounds[1] - this->center;

	idVec3 origin;
	axis[0][0] = modelMatrix[0];
	axis[0][1] = modelMatrix[1];
	axis[0][2] = modelMatrix[2];

	axis[1][0] = modelMatrix[4];
	axis[1][1] = modelMatrix[5];
	axis[1][2] = modelMatrix[6];

	axis[2][0] = modelMatrix[8];
	axis[2][1] = modelMatrix[9];
	axis[2][2] = modelMatrix[10];

	origin[0]  = modelMatrix[12];
	origin[1]  = modelMatrix[13];
	origin[2]  = modelMatrix[14];
	this->center = origin + this->center * this->axis;
}

ID_INLINE idBox idBox::operator+( const idVec3 &t ) const {
	return idBox( center + t, extents, axis );
}

ID_INLINE idBox &idBox::operator+=( const idVec3 &t ) {
	center += t;
	return *this;
}

ID_INLINE idBox idBox::operator*( const idMat3 &r ) const {
	return idBox( center * r, extents, axis * r );
}

ID_INLINE idBox &idBox::operator*=( const idMat3 &r ) {
	center *= r;
	axis *= r;
	return *this;
}

ID_INLINE idBox idBox::operator+( const idBox &a ) const {
	idBox newBox;
	newBox = *this;
	newBox.AddBox( a );
	return newBox;
}

ID_INLINE idBox &idBox::operator+=( const idBox &a ) {
	idBox::AddBox( a );
	return *this;
}

ID_INLINE idBox idBox::operator-( const idBox &a ) const {
	return idBox( center, extents - a.extents, axis );
}

ID_INLINE idBox &idBox::operator-=( const idBox &a ) {
	extents -= a.extents;
	return *this;
}

ID_INLINE bool idBox::Compare( const idBox &a ) const {
	return ( center.Compare( a.center ) && extents.Compare( a.extents ) && axis.Compare( a.axis ) );
}

ID_INLINE bool idBox::Compare( const idBox &a, const float epsilon ) const {
	return ( center.Compare( a.center, epsilon ) && extents.Compare( a.extents, epsilon ) && axis.Compare( a.axis, epsilon ) );
}

ID_INLINE bool idBox::operator==( const idBox &a ) const {
	return Compare( a );
}

ID_INLINE bool idBox::operator!=( const idBox &a ) const {
	return !Compare( a );
}

ID_INLINE void idBox::Clear( void ) {
	center.Zero();
	extents[0] = extents[1] = extents[2] = -idMath::INFINITY;
	axis.Identity();
}

ID_INLINE void idBox::Zero( void ) {
	center.Zero();
	extents.Zero();
	axis.Identity();
}

ID_INLINE const idVec3 &idBox::GetCenter( void ) const {
	return center;
}

ID_INLINE const idVec3 &idBox::GetExtents( void ) const {
	return extents;
}

ID_INLINE const idMat3 &idBox::GetAxis( void ) const {
	return axis;
}

ID_INLINE float idBox::GetVolume( void ) const {
	return ( extents[0] * extents[1] * extents[2] * ( 2.0f * 2.0f * 2.0f ) );
}

ID_INLINE bool idBox::IsCleared( void ) const {
	return extents[0] < 0.0f;
}

ID_INLINE idBox idBox::Expand( const float d ) const {
	return idBox( center, extents + idVec3( d, d, d ), axis );
}

ID_INLINE idBox &idBox::ExpandSelf( const float d ) {
	extents[0] += d;
	extents[1] += d;
	extents[2] += d;
	return *this;
}

ID_INLINE idBox &idBox::ExpandSelf ( const float dx, const float dy, const float dz ) {
	extents[0] += dx;
	extents[1] += dy;
	extents[2] += dz;
	return *this;
}

ID_INLINE idBox idBox::Translate( const idVec3 &translation ) const {
	return idBox( center + translation, extents, axis );
}

ID_INLINE idBox &idBox::TranslateSelf( const idVec3 &translation ) {
	center += translation;
	return *this;
}

ID_INLINE idBox idBox::Rotate( const idMat3 &rotation ) const {
	return idBox( center * rotation, extents, axis * rotation );
}

ID_INLINE idBox &idBox::RotateSelf( const idMat3 &rotation ) {
	center *= rotation;
	axis *= rotation;
	return *this;
}

ID_INLINE bool idBox::ContainsPoint( const idVec3 &p ) const {
	idVec3 lp = p - center;
	if ( idMath::Fabs( lp * axis[0] ) > extents[0] ||
			idMath::Fabs( lp * axis[1] ) > extents[1] ||
				idMath::Fabs( lp * axis[2] ) > extents[2] ) {
		return false;
	}
	return true;
}

ID_INLINE idSphere idBox::ToSphere( void ) const {
	return idSphere( center, extents.Length() );
}

ID_INLINE void idBox::AxisProjection( const idVec3 &dir, float &min, float &max ) const {
	float d1 = dir * center;
	float d2 = idMath::Fabs( extents[0] * ( dir * axis[0] ) ) +
				idMath::Fabs( extents[1] * ( dir * axis[1] ) ) +
				idMath::Fabs( extents[2] * ( dir * axis[2] ) );
	min = d1 - d2;
	max = d1 + d2;
}

ID_INLINE void idBox::AxisProjection( const idMat3 &ax, idBounds &bounds ) const {
	for ( int i = 0; i < 3; i++ ) {
		float d1 = ax[i] * center;
		float d2 = idMath::Fabs( extents[0] * ( ax[i] * axis[0] ) ) +
					idMath::Fabs( extents[1] * ( ax[i] * axis[1] ) ) +
					idMath::Fabs( extents[2] * ( ax[i] * axis[2] ) );
		bounds[0][i] = d1 - d2;
		bounds[1][i] = d1 + d2;
	}
}

#endif /* !__BV_BOX_H__ */
