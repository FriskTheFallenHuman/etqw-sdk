// Copyright (C) 2007 Id Software, Inc.
//


#ifndef __PTRARRAY_H__
#define __PTRARRAY_H__

/*
===============================================================================

	Pointer Array template
	Does not allocate memory until the first array element is added.

===============================================================================
*/


template< class type, int gran >
ID_INLINE int idPtrArrayCompare( const type *a, const type *b ) {
	return *a - *b;
}

template< class type, int gran=16 >
class idPtrArray {
public:

	typedef int				cmp_t( const type *, const type * );

							idPtrArray( void );
							idPtrArray( const idPtrArray<type,gran> &other );
							~idPtrArray( void );

	void					Clear( void );											// clear the array
	int						Num( void ) const;										// returns number of elements in array

	size_t					Allocated( void ) const;								// returns total size of allocated memory
	size_t					Size( void ) const;										// returns total size of allocated memory including size of idPtrArray
	size_t					MemoryUsed( void ) const;								// returns size of the used elements in the array

	idPtrArray<type,gran> &	operator=( const idPtrArray<type,gran> &other );
	const type &			operator[]( int index ) const;
	type &					operator[]( int index );

	void					Condense( void );										// resizes array to exactly the number of elements it contains
	void					Resize( int newsize );									// resizes array to the given number of elements
	void					SetNum( int newnum, bool resize = true );				// set number of elements in array and resize to exactly this number if necessary
	void					AssureSize( int newSize);								// assure array has given number of elements, but leave them uninitialized
	void					AssureSize( int newSize, const type &initValue );		// assure array has given number of elements and initialize any new elements

	type *					Ptr( void );											// returns a pointer to the array
	const type *			Ptr( void ) const;										// returns a pointer to the array
	type &					Alloc( void );											// returns reference to a new data element at the end of the array
	int						Append( const type & obj );								// append element
	int						Append( const idPtrArray<type,gran> &other );			// append array
	int						AddUnique( const type & obj );							// add unique element
	int						Insert( const type & obj, int index );					// insert the element at the given index
	int						FindIndex( const type & obj ) const;					// find the index for the given element
	type *					Find( type const & obj ) const;							// find pointer to the given element
	bool					RemoveIndex( int index );								// remove the element at the given index
	bool					RemoveIndexFast( int index );							// remove the element at the given index and put the last element into its spot
	bool					Remove( const type & obj );								// remove the element
	bool					RemoveFast( const type & obj );							// remove the element, move the last element into its spot
	void					Sort( cmp_t *compare = idPtrArrayCompare<type,gran> );	// sort the array
	void					Swap( idPtrArray<type,gran> &other );					// swap the contents of the lists

private:
	int						num;
	int						size;
	type *					array;
};

/*
================
idPtrArray::idPtrArray
================
*/
template< class type, int gran >
ID_INLINE idPtrArray<type,gran>::idPtrArray( void ) {
	num = size = 0;
	array = NULL;
}

/*
================
idPtrArray::idPtrArray
================
*/
template< class type, int gran >
ID_INLINE idPtrArray<type,gran>::idPtrArray( const idPtrArray<type,gran> &other ) {
	num = size = 0;
	array = NULL;
	*this = other;
}

/*
================
idPtrArray::~idPtrArray
================
*/
template< class type, int gran >
ID_INLINE idPtrArray<type,gran>::~idPtrArray( void ) {
	Clear();
}

/*
================
idPtrArray::Clear

Frees up the memory allocated by the array.  Assumes that type automatically handles freeing up memory.
================
*/
template< class type, int gran >
ID_INLINE void idPtrArray<type,gran>::Clear( void ) {
	if ( array ) {
		delete[] array;
	}

	array	= NULL;
	num		= 0;
	size	= 0;
}

/*
================
idPtrArray::Allocated

return total memory allocated for the array in bytes, but doesn't take into account additional memory allocated by type
================
*/
template< class type, int gran >
ID_INLINE size_t idPtrArray<type,gran>::Allocated( void ) const {
	return size * sizeof( type );
}

/*
================
idPtrArray::Size

return total size of array in bytes, but doesn't take into account additional memory allocated by type
================
*/
template< class type, int gran >
ID_INLINE size_t idPtrArray<type,gran>::Size( void ) const {
	return sizeof( idPtrArray<type,gran> ) + Allocated();
}

/*
================
idPtrArray::MemoryUsed
================
*/
template< class type, int gran >
ID_INLINE size_t idPtrArray<type,gran>::MemoryUsed( void ) const {
	return num * sizeof( *array );
}

/*
================
idPtrArray::Num

Returns the number of elements currently contained in the array.
Note that this is NOT an indication of the memory allocated.
================
*/
template< class type, int gran >
ID_INLINE int idPtrArray<type,gran>::Num( void ) const {
	return num;
}

/*
================
idPtrArray::SetNum

Resize to the exact size specified irregardless of gran
================
*/
template< class type, int gran >
ID_INLINE void idPtrArray<type,gran>::SetNum( int newnum, bool resize ) {
	assert( newnum >= 0 );
	if ( resize || newnum > size ) {
		Resize( newnum );
	}
	num = newnum;
}

/*
================
idPtrArray::Condense

Resizes the array to exactly the number of elements it contains or frees up memory if empty.
================
*/
template< class type, int gran >
ID_INLINE void idPtrArray<type,gran>::Condense( void ) {
	if ( array ) {
		if ( num ) {
			Resize( num );
		} else {
			Clear();
		}
	}
}

/*
================
idPtrArray::Resize

Allocates memory for the amount of elements requested while keeping the contents intact.
Contents are copied using their = operator so that data is correnctly instantiated.
================
*/
template< class type, int gran >
ID_INLINE void idPtrArray<type,gran>::Resize( int newsize ) {
	type	*temp;
	int		i;

	assert( newsize >= 0 );

	// free up the array if no data is being reserved
	if ( newsize <= 0 ) {
		Clear();
		return;
	}

	if ( newsize == size ) {
		// not changing the size, so just exit
		return;
	}

	temp	= array;
	size	= newsize;
	if ( size < num ) {
		num = size;
	}

	// copy the old array into our new one
	array = new type[ size ];
	for( i = 0; i < num; i++ ) {
		array[ i ] = temp[ i ];
	}

	// delete the old array if it exists
	if ( temp ) {
		delete[] temp;
	}
}

/*
================
idPtrArray::AssureSize

Makes sure the array has at least the given number of elements.
================
*/
template< class type, int gran >
ID_INLINE void idPtrArray<type,gran>::AssureSize( int newSize ) {
	if ( newSize > size ) {
		newSize += gran - 1;
		newSize -= newSize % gran;
		Resize( newSize );
	}
}

/*
================
idPtrArray::AssureSize

Makes sure the array has at least the given number of elements and initialize any elements not yet initialized.
================
*/
template< class type, int gran >
ID_INLINE void idPtrArray<type,gran>::AssureSize( int newSize, const type &initValue ) {
	int oldSize;

	if ( newSize > size ) {

		newSize += gran - 1;
		newSize -= newSize % gran;
		oldSize = size;
		Resize( newSize );

		for ( int i = num; i < newSize; i++ ) {
			array[i] = initValue;
		}
	}
}

/*
================
idPtrArray::operator=

Copies the contents and size attributes of another array.
================
*/
template< class type, int gran >
ID_INLINE idPtrArray<type,gran> &idPtrArray<type,gran>::operator=( const idPtrArray<type,gran> &other ) {
	int	i;

	Clear();

	num			= other.num;
	size		= other.size;

	if ( size ) {
		array = new type[ size ];
		for( i = 0; i < num; i++ ) {
			array[ i ] = other.array[ i ];
		}
	}

	return *this;
}

/*
================
idPtrArray::operator[] const

Access operator.  Index must be within range or an assert will be issued in debug builds.
Release builds do no range checking.
================
*/
template< class type, int gran >
ID_INLINE const type &idPtrArray<type,gran>::operator[]( int index ) const {
	assert( index >= 0 );
	assert( index < num );

	return array[ index ];
}

/*
================
idPtrArray::operator[]

Access operator.  Index must be within range or an assert will be issued in debug builds.
Release builds do no range checking.
================
*/
template< class type, int gran >
ID_INLINE type &idPtrArray<type,gran>::operator[]( int index ) {
	assert( index >= 0 );
	assert( index < num );

	return array[ index ];
}

/*
================
idPtrArray::Ptr

Returns a pointer to the begining of the array.  Useful for iterating through the array in loops.

Note: may return NULL if the array is empty.

FIXME: Create an iterator template for this kind of thing.
================
*/
template< class type, int gran >
ID_INLINE type *idPtrArray<type,gran>::Ptr( void ) {
	return array;
}

/*
================
idPtrArray::Ptr

Returns a pointer to the begining of the array.  Useful for iterating through the array in loops.

Note: may return NULL if the array is empty.

FIXME: Create an iterator template for this kind of thing.
================
*/
template< class type, int gran >
const ID_INLINE type *idPtrArray<type,gran>::Ptr( void ) const {
	return array;
}

/*
================
idPtrArray::Alloc

Returns a reference to a new data element at the end of the array.
================
*/
template< class type, int gran >
ID_INLINE type &idPtrArray<type,gran>::Alloc( void ) {
	if ( !array ) {
		Resize( gran );
	}

	if ( num == size ) {
		Resize( size + gran );
	}

	return array[ num++ ];
}

/*
================
idPtrArray::Append

Increases the size of the array by one element and copies the supplied data into it.

Returns the index of the new element.
================
*/
template< class type, int gran >
ID_INLINE int idPtrArray<type,gran>::Append( const type & obj ) {
	if ( !array ) {
		Resize( gran );
	}

	if ( num == size ) {
		int newsize;

		newsize = size + gran;
		Resize( newsize - newsize % gran );
	}

	array[ num ] = obj;
	num++;

	return num - 1;
}


/*
================
idPtrArray::Insert

Increases the size of the array by at leat one element if necessary
and inserts the supplied data into it.

Returns the index of the new element.
================
*/
template< class type, int gran >
ID_INLINE int idPtrArray<type,gran>::Insert( const type & obj, int index ) {
	if ( !array ) {
		Resize( gran );
	}

	if ( num == size ) {
		int newsize;

		newsize = size + gran;
		Resize( newsize - newsize % gran );
	}

	if ( index < 0 ) {
		index = 0;
	}
	else if ( index > num ) {
		index = num;
	}
	for ( int i = num; i > index; --i ) {
		array[i] = array[i-1];
	}
	num++;
	array[index] = obj;
	return index;
}

/*
================
idPtrArray::Append

adds the other array to this one

Returns the size of the new combined array
================
*/
template< class type, int gran >
ID_INLINE int idPtrArray<type,gran>::Append( const idPtrArray<type,gran> &other ) {
	if ( !array ) {
		Resize( gran );
	}

	int n = other.Num();
	for ( int i = 0; i < n; i++ ) {
		Append( other[i] );
	}

	return Num();
}

/*
================
idPtrArray::AddUnique

Adds the data to the array if it doesn't already exist.  Returns the index of the data in the array.
================
*/
template< class type, int gran >
ID_INLINE int idPtrArray<type,gran>::AddUnique( const type & obj ) {
	int index;

	index = FindIndex( obj );
	if ( index < 0 ) {
		index = Append( obj );
	}

	return index;
}

/*
================
idPtrArray::FindIndex

Searches for the specified data in the array and returns it's index.  Returns -1 if the data is not found.
================
*/
template< class type, int gran >
ID_INLINE int idPtrArray<type,gran>::FindIndex( const type & obj ) const {
	int i;

	for( i = 0; i < num; i++ ) {
		if ( array[ i ] == obj ) {
			return i;
		}
	}

	// Not found
	return -1;
}

/*
================
idPtrArray::Find

Searches for the specified data in the array and returns it's address. Returns NULL if the data is not found.
================
*/
template< class type, int gran >
ID_INLINE type *idPtrArray<type,gran>::Find( const type & obj ) const {
	int i;

	i = FindIndex( obj );
	if ( i >= 0 ) {
		return &array[ i ];
	}

	return NULL;
}

/*
================
idPtrArray::RemoveIndex

Removes the element at the specified index and moves all data following the element down to fill in the gap.
The number of elements in the array is reduced by one.  Returns false if the index is outside the bounds of the array.
Note that the element is not destroyed, so any memory used by it may not be freed until the destruction of the array.
================
*/
template< class type, int gran >
ID_INLINE bool idPtrArray<type,gran>::RemoveIndex( int index ) {
	int i;

	assert( array != NULL );
	assert( index >= 0 );
	assert( index < num );

	if ( ( index < 0 ) || ( index >= num ) ) {
		return false;
	}

	num--;
	for( i = index; i < num; i++ ) {
		array[ i ] = array[ i + 1 ];
	}

	return true;
}

/*
===============
idPtrArray::RemoveIndexFast

Removes the element at the specified index and moves the last element into
it's spot, rather than moving the whole array down by one.  Of course, this
doesn't maintain the order of elements!
The number of elements in the array is reduced by one.  Returns false if the
index is outside the bounds of the array. Note that the element is not destroyed,
so any memory used by it may not be freed until the destruction of the array.
===============
*/
template< class type, int gran >
ID_INLINE bool idPtrArray<type,gran>::RemoveIndexFast( int index ) {
	assert( array != NULL );
	assert( index >= 0 );
	assert( index < num );

	if ( ( index < 0 ) || ( index >= num ) ) {
		return false;
	}

	num--;
	array[ index ] = array[ num ];

	return true;
}

/*
================
idPtrArray::Remove

Removes the element if it is found within the array and moves all data following the element down to fill in the gap.
The number of elements in the array is reduced by one.  Returns false if the data is not found in the array.  Note that
the element is not destroyed, so any memory used by it may not be freed until the destruction of the array.
================
*/
template< class type, int gran >
ID_INLINE bool idPtrArray<type,gran>::Remove( const type & obj ) {
	int index;

	index = FindIndex( obj );
	if ( index >= 0 ) {
		return RemoveIndex( index );
	}

	return false;
}

/*
================
idPtrArray::RemoveFast

Removes the element if it is found within the array and moves the last element into the gap.
The number of elements in the array is reduced by one.  Returns false if the data is not found in the array.  Note that
the element is not destroyed, so any memory used by it may not be freed until the destruction of the array.
================
*/
template< class type, int gran >
ID_INLINE bool idPtrArray<type,gran>::RemoveFast( const type & obj ) {
	int index;

	index = FindIndex( obj );
	if ( index >= 0 ) {
		return RemoveIndexFast( index );
	}

	return false;
}

/*
================
idPtrArray::Sort

Performs a qsort on the array using the supplied comparison function.  Note that the data is merely moved around the
array, so any pointers to data within the array may no longer be valid.
================
*/
template< class type, int gran >
ID_INLINE void idPtrArray<type,gran>::Sort( cmp_t *compare ) {
	if ( !array ) {
		return;
	}
	typedef int cmp_c(const void *, const void *);
	qsort( ( void * )array, ( size_t )num, sizeof( type ), (cmp_c *)compare );
}

/*
================
idPtrArray::Swap

Swaps the contents of two lists
================
*/
template< class type, int gran >
ID_INLINE void idPtrArray<type,gran>::Swap( idPtrArray<type,gran> &other ) {
	idSwap( num, other.num );
	idSwap( size, other.size );
	idSwap( array, other.array );
}

#endif /* !__PTRARRAY_H__ */
