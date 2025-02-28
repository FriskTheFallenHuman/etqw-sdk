// Copyright (C) 2007 Id Software, Inc.
//

#include "precompiled.h"
#pragma hdrstop

#if defined( MACOS_X )
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#endif

/*
===============================================================================

	idLib

===============================================================================
*/

idSys *			idLib::sys			= NULL;
idCommon *		idLib::common		= NULL;
idCVarSystem *	idLib::cvarSystem	= NULL;
idFileSystem *	idLib::fileSystem	= NULL;
int				idLib::frameNumber	= 0;

/*
================
idLib::Init
================
*/
void idLib::Init( void ) {

	assert( sizeof( bool ) == 1 );

	// initialize little/big endian conversion
	Swap_Init();

	// initialize memory manager
	Mem_Init();

	// init string memory allocator
	idStr::InitMemory();
	idWStr::InitMemory();

	// initialize generic SIMD implementation
	idSIMD::Init();

	// initialize math
	idMath::Init();

	// test idMatX
	// jrad - this is horribly broken
	//idMatX::Test();

	// initialize the dictionary string pools
	idDict::Init();

	// initialize the global random generator
	idRandom::StaticRandom().SetSeed( sys->Milliseconds() );
}

/*
================
idLib::ShutDown
================
*/
void idLib::ShutDown( void ) {

	// shut down the string memory allocator
	idStr::ShutdownMemory();
	idWStr::ShutdownMemory();

	// shut down the SIMD engine
	idSIMD::Shutdown();

	// shut down the dynamic pools
	sdDynamicBlockManagerBase::ShutdownPools();

	// shut down property allocators
	sdProperties::sdPropertyHandler::Shutdown();

	// shut down the memory manager
	Mem_Shutdown();
}

/*
===============
idLib::Printf
===============
*/
void idLib::Printf( const char *fmt, ... ) {
	va_list		argptr;
	char		text[MAX_STRING_CHARS];

	va_start( argptr, fmt );
	idStr::vsnPrintf( text, sizeof( text ), fmt, argptr );
	va_end( argptr );

	common->Printf( "%s", text );
}

/*
===============
idLib::Error
===============
*/
void idLib::Error( const char *fmt, ... ) {
	va_list		argptr;
	char		text[MAX_STRING_CHARS];

	va_start( argptr, fmt );
	idStr::vsnPrintf( text, sizeof( text ), fmt, argptr );
	va_end( argptr );

	common->Error( "%s", text );
}

/*
===============
idLib::Warning
===============
*/
void idLib::Warning( const char *fmt, ... ) {
	va_list		argptr;
	char		text[MAX_STRING_CHARS];

	va_start( argptr, fmt );
	idStr::vsnPrintf( text, sizeof( text ), fmt, argptr );
	va_end( argptr );

	common->Warning( "%s", text );
}

/*
===============================================================================

	Colors

===============================================================================
*/

const idVec4	colorBlack	= idVec4( 0.00f, 0.00f, 0.00f, 1.00f );
const idVec4	colorWhite	= idVec4( 1.00f, 1.00f, 1.00f, 1.00f );
const idVec4	colorRed	= idVec4( 1.00f, 0.00f, 0.00f, 1.00f );
const idVec4	colorGreen	= idVec4( 0.00f, 1.00f, 0.00f, 1.00f );
const idVec4	colorBlue	= idVec4( 0.00f, 0.00f, 1.00f, 1.00f );
const idVec4	colorYellow	= idVec4( 1.00f, 1.00f, 0.00f, 1.00f );
const idVec4	colorMagenta= idVec4( 1.00f, 0.00f, 1.00f, 1.00f );
const idVec4	colorCyan	= idVec4( 0.00f, 1.00f, 1.00f, 1.00f );
const idVec4	colorOrange	= idVec4( 1.00f, 0.50f, 0.00f, 1.00f );
const idVec4	colorPurple	= idVec4( 0.60f, 0.00f, 0.60f, 1.00f );
const idVec4	colorPink	= idVec4( 0.73f, 0.40f, 0.48f, 1.00f );
const idVec4	colorBrown	= idVec4( 0.40f, 0.35f, 0.08f, 1.00f );
const idVec4	colorLtGrey	= idVec4( 0.75f, 0.75f, 0.75f, 1.00f );
const idVec4	colorMdGrey	= idVec4( 0.50f, 0.50f, 0.50f, 1.00f );
const idVec4	colorDkGrey	= idVec4( 0.25f, 0.25f, 0.25f, 1.00f );
const idVec4	colorLtBlue	= idVec4( 0.40f, 0.70f, 1.00f, 1.00f );
const idVec4	colorDkRed	= idVec4( 0.70f, 0.00f, 0.00f, 1.00f );


/*
===============================================================================

	Byte order functions

===============================================================================
*/

// can't just use function pointers, or dll linkage can mess up
static short	(*_BigShort)( short l );
static short	(*_LittleShort)( short l );
static int		(*_BigLong)( int l );
static int		(*_LittleLong)( int l );
static float	(*_BigFloat)( float l );
static float	(*_LittleFloat)( float l );
static double	(*_LittleDouble)( double l );
static void		(*_BigRevBytes)( void *bp, int elsize, int elcount );
static void		(*_LittleRevBytes)( void *bp, int elsize, int elcount );
static void     (*_LittleBitField)( void *bp, int elsize );
static void		(*_SixtetsForInt)( byte *out, int src );
static int		(*_IntForSixtets)( byte *in );

short	BigShort( short l ) { return _BigShort( l ); }
short	LittleShort( short l ) { return _LittleShort( l ); }
int		BigLong( int l ) { return _BigLong( l ); }
int		LittleLong( int l ) { return _LittleLong( l ); }
float	BigFloat( float l ) { return _BigFloat( l ); }
float	LittleFloat( float l ) { return _LittleFloat( l ); }
double	LittleDouble( double l ) { return _LittleDouble( l ); }
void	BigRevBytes( void *bp, int elsize, int elcount ) { _BigRevBytes( bp, elsize, elcount ); }
void	LittleRevBytes( void *bp, int elsize, int elcount ) { _LittleRevBytes( bp, elsize, elcount ); }
void	LittleBitField( void *bp, int elsize ) { _LittleBitField( bp, elsize ); }

void	SixtetsForInt( byte *out, int src ) { _SixtetsForInt( out, src ); }
int		IntForSixtets( byte *in ) { return _IntForSixtets( in ); }

/*
================
ShortSwap
================
*/
short ShortSwap( short l ) {
	byte    b1,b2;

	b1 = l&255;
	b2 = (l>>8)&255;

	return (b1<<8) + b2;
}

/*
================
ShortNoSwap
================
*/
short ShortNoSwap( short l ) {
	return l;
}

/*
================
LongSwap
================
*/
int LongSwap ( int l ) {
	byte    b1,b2,b3,b4;

	b1 = l&255;
	b2 = (l>>8)&255;
	b3 = (l>>16)&255;
	b4 = (l>>24)&255;

	return ((int)b1<<24) + ((int)b2<<16) + ((int)b3<<8) + b4;
}

/*
================
LongNoSwap
================
*/
int	LongNoSwap( int l ) {
	return l;
}

/*
================
FloatSwap
================
*/
float FloatSwap( float f ) {
	union {
		float	f;
		byte	b[4];
	} dat1, dat2;


	dat1.f = f;
	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];
	return dat2.f;
}

/*
================
FloatNoSwap
================
*/
float FloatNoSwap( float f ) {
	return f;
}

/*
================
DoubleSwap
================
*/
double DoubleSwap( double d ) {
	union {
		double	d;
		byte	b[8];
	} dat1, dat2;


	dat1.d = d;
	dat2.b[0] = dat1.b[7];
	dat2.b[1] = dat1.b[6];
	dat2.b[2] = dat1.b[5];
	dat2.b[3] = dat1.b[4];
	dat2.b[4] = dat1.b[3];
	dat2.b[5] = dat1.b[2];
	dat2.b[6] = dat1.b[1];
	dat2.b[7] = dat1.b[0];
	return dat2.d;
}

/*
================
DoubleNoSwap
================
*/
double DoubleNoSwap( double d ) {
	return d;
}

/*
=====================================================================
RevBytesSwap

Reverses byte order in place.

INPUTS
   bp       bytes to reverse
   elsize   size of the underlying data type
   elcount  number of elements to swap

RESULTS
   Reverses the byte order in each of elcount elements.
=====================================================================
*/
void RevBytesSwap( void *bp, int elsize, int elcount ) {
	register unsigned char *p, *q;

	p = ( unsigned char * ) bp;

	if ( elsize == 1 ) {
		return;
	}

	if ( elsize == 2 ) {
		q = p + 1;
		while( elcount-- ) {
			*p ^= *q;
			*q ^= *p;
			*p ^= *q;
			p += 2;
			q += 2;
		}
		return;
	}

	while( elcount-- ) {
		q = p + elsize - 1;
		while( p < q ) {
			*p ^= *q;
			*q ^= *p;
			*p ^= *q;
			++p;
			--q;
		}
		p += elsize >> 1;
	}
}

/*
=====================================================================
RevBitFieldSwap

Reverses byte order in place, then reverses bits in those bytes

INPUTS
bp       bitfield structure to reverse
elsize   size of the underlying data type

RESULTS
Reverses the bitfield of size elsize.
=====================================================================
*/
void RevBitFieldSwap( void *bp, int elsize) {
	int i;
	unsigned char *p, t, v;

	LittleRevBytes( bp, elsize, 1 );

	p = (unsigned char *) bp;
	while( elsize-- ) {
		v = *p;
		t = 0;
		for ( i = 7; i; i-- ) {
			t <<= 1;
			v >>= 1;
			t |= v & 1;
		}
		*p++ = t;
	}
}

/*
================
RevBytesNoSwap
================
*/
void RevBytesNoSwap( void *bp, int elsize, int elcount ) {
	return;
}

/*
================
RevBytesNoSwap
================
*/
void RevBitFieldNoSwap( void *bp, int elsize ) {
	return;
}

/*
================
SixtetsForIntLittle
================
*/
void SixtetsForIntLittle( byte *out, int src) {
	byte *b = (byte *)&src;
	out[0] = ( b[0] & 0xfc ) >> 2;
	out[1] = ( ( b[0] & 0x3 ) << 4 ) + ( ( b[1] & 0xf0 ) >> 4 );
	out[2] = ( ( b[1] & 0xf ) << 2 ) + ( ( b[2] & 0xc0 ) >> 6 );
	out[3] = b[2] & 0x3f;
}

/*
================
SixtetsForIntBig
TTimo: untested - that's the version from initial base64 encode
================
*/
void SixtetsForIntBig( byte *out, int src) {
	for( int i = 0 ; i < 4 ; i++ ) {
		out[i] = src & 0x3f;
		src >>= 6;
	}
}

/*
================
IntForSixtetsLittle
================
*/
int IntForSixtetsLittle( byte *in ) {
	int ret = 0;
	byte *b = (byte *)&ret;
	b[0] |= in[0] << 2;
	b[0] |= ( in[1] & 0x30 ) >> 4;
	b[1] |= ( in[1] & 0xf ) << 4;
	b[1] |= ( in[2] & 0x3c ) >> 2;
	b[2] |= ( in[2] & 0x3 ) << 6;
	b[2] |= in[3];
	return ret;
}

/*
================
IntForSixtetsBig
TTimo: untested - that's the version from initial base64 decode
================
*/
int IntForSixtetsBig( byte *in ) {
	int ret = 0;
	ret |= in[0];
	ret |= in[1] << 6;
	ret |= in[2] << 2*6;
	ret |= in[3] << 3*6;
	return ret;
}

/*
================
Swap_Init
================
*/
void Swap_Init( void ) {

	// set the byte swapping variables in a portable manner
	if ( Swap_IsBigEndian() ) {
		// big endian ex: ppc
		_BigShort = ShortNoSwap;
		_LittleShort = ShortSwap;
		_BigLong = LongNoSwap;
		_LittleLong = LongSwap;
		_BigFloat = FloatNoSwap;
		_LittleFloat = FloatSwap;
		_LittleDouble = DoubleSwap;
		_BigRevBytes = RevBytesNoSwap;
		_LittleRevBytes = RevBytesSwap;
		_LittleBitField = RevBitFieldSwap;
		_SixtetsForInt = SixtetsForIntBig;
		_IntForSixtets = IntForSixtetsBig;
	} else {
		// little endian ex: x86
		_BigShort = ShortSwap;
		_LittleShort = ShortNoSwap;
		_BigLong = LongSwap;
		_LittleLong = LongNoSwap;
		_BigFloat = FloatSwap;
		_LittleFloat = FloatNoSwap;
		_LittleDouble = DoubleNoSwap;
		_BigRevBytes = RevBytesSwap;
		_LittleRevBytes = RevBytesNoSwap;
		_LittleBitField = RevBitFieldNoSwap;
		_SixtetsForInt = SixtetsForIntLittle;
		_IntForSixtets = IntForSixtetsLittle;
	}
}

/*
===============================================================================

	Assertion

===============================================================================
*/

/*
JUMPIN' JEHOSEPHAT YEEHAW!
*/


void AssertFailed( const char *file, int line, const char *expression ) {
	idLib::sys->DebugPrintf( "\n\nASSERTION FAILED!\n%s(%d): '%s'\n", file, line, expression );

#if defined( _WIN32 )
	__debugbreak();
#elif defined( __linux__ )
	__asm__ __volatile__ ("int $0x03");
#elif defined( MACOS_X )
#if 0
	kill( getpid(), SIGINT );
#endif
#endif
}
