// Copyright (C) 2007 Id Software, Inc.
//
#ifndef __SCRIPT_COMPILER_H__
#define __SCRIPT_COMPILER_H__

#include "Script_Program.h"

const char * const RESULT_STRING = "<RESULT>";

class idVarDef;
class idTypeDef;

typedef struct opcode_s {
	char		*name;
	char		*opname;
	int			priority;
	bool		rightAssociative;
	idVarDef	*type_a;
	idVarDef	*type_b;
	idVarDef	*type_c;
	mutable int	emitCount;
} opcode_t;

typedef const opcode_t* constOpCodePtr_t;
typedef idVarDef*		varPtr_t;

enum {
	OP_RETURN,

	OP_UINC_F,
	OP_UINCP_F,
	OP_UDEC_F,
	OP_UDECP_F,

	OP_COMP_F,

	OP_MUL_F,
	OP_MUL_V,
	OP_MUL_FV,
	OP_MUL_VF,

	OP_DIV_F,

	OP_MOD_F,

	OP_ADD_F,
	OP_ADD_V,
	OP_ADD_S,
	OP_ADD_FS,
	OP_ADD_SF,
	OP_ADD_VS,
	OP_ADD_SV,

	OP_SUB_F,
	OP_SUB_V,

	OP_EQ_B,
	OP_EQ_F,
	OP_EQ_V,
	OP_EQ_S,
	OP_EQ_W,
	OP_EQ_O,

	OP_NE_B,
	OP_NE_F,
	OP_NE_V,
	OP_NE_S,
	OP_NE_W,
	OP_NE_O,

	OP_LE,
	OP_GE,
	OP_LT,
	OP_GT,

	OP_INDIRECT_F,
	OP_INDIRECT_V,
	OP_INDIRECT_S,
	OP_INDIRECT_W,
	OP_INDIRECT_BOOL,
	OP_INDIRECT_OBJ,

	OP_ADDRESS,

	OP_EVENTCALL,
	OP_OBJECTCALL,
	OP_SYSCALL,

	OP_STORE_F,
	OP_STORE_V,
	OP_STORE_S,
	OP_STORE_W,
	OP_STORE_BOOL,
	OP_STORE_OBJ,

	OP_STORE_FTOS,
	OP_STORE_BTOS,
	OP_STORE_VTOS,
	OP_STORE_FTOBOOL,
	OP_STORE_BOOLTOF,

	OP_STOREP_F,
	OP_STOREP_V,
	OP_STOREP_S,
	OP_STOREP_W,
	OP_STOREP_FLD,
	OP_STOREP_BOOL,
	OP_STOREP_OBJ,

	OP_STOREP_FTOS,
	OP_STOREP_BTOS,
	OP_STOREP_VTOS,
	OP_STOREP_FTOBOOL,
	OP_STOREP_BOOLTOF,

	OP_UMUL_F,
	OP_UMUL_V,
	OP_UDIV_F,
	OP_UDIV_V,
	OP_UMOD_F,
	OP_UADD_F,
	OP_UADD_V,
	OP_USUB_F,
	OP_USUB_V,
	OP_UAND_F,
	OP_UOR_F,

	OP_NOT_BOOL,
	OP_NOT_F,
	OP_NOT_V,
	OP_NOT_S,
	OP_NOT_OBJ,

	OP_NEG_F,
	OP_NEG_V,

	OP_INT_F,
	OP_IF,
	OP_IFNOT,

	OP_CALL,

	OP_THREAD,
	OP_GUITHREAD,

	OP_OBJTHREAD,
	OP_GUIOBJTHREAD,

	OP_PUSH_F,
	OP_PUSH_V,
	OP_PUSH_S,
	OP_PUSH_W,
	OP_PUSH_OBJ,
	OP_PUSH_FTOS,
	OP_PUSH_FTOW,
	OP_PUSH_BTOF,
	OP_PUSH_FTOB,
	OP_PUSH_VTOS,
	OP_PUSH_BTOS,

	OP_GOTO,

	OP_AND,
	OP_AND_BOOLF,
	OP_AND_FBOOL,
	OP_AND_BOOLBOOL,
	OP_OR,
	OP_OR_BOOLF,
	OP_OR_FBOOL,
	OP_OR_BOOLBOOL,

	OP_BITAND,
	OP_BITOR,

	OP_BREAK,			// placeholder op.  not used in final code
	OP_CONTINUE,		// placeholder op.  not used in final code

	OP_VIRTUALEVENTCALL,

	OP_ALLOC_TYPE,
	OP_FREE_TYPE,

	NUM_OPCODES
};

class idCompiler {
private:
	static bool		punctuationValid[ 256 ];
	static char		*punctuation[];

	idParser		parser;
	idParser		*parserPtr;
	idToken			token;

	idTypeDef		*immediateType;
	eval_t			immediate;

	bool			eof;
	bool			callthread;
	bool			callguithread;
	bool			lastStatementWasReturn;
	int				braceDepth;
	int				loopDepth;
	int				currentLineNumber;
	int				currentFileNumber;
	int				errorCount;

	idProgram*		program;

	idVarDef		*scope;				// the function being parsed, or NULL
	const idVarDef	*basetype;			// for accessing fields

private:
	float			Divide( float numerator, float denominator );
	void			Error( const char *error, ... ) const;
	void			Warning( const char *message, ... ) const;
	idVarDef*		OptimizeOpcode( constOpCodePtr_t& op, varPtr_t& var_a, varPtr_t& var_b );
	idVarDef*		EmitOpcode( const opcode_t *op, idVarDef *var_a, idVarDef *var_b );
	idVarDef*		EmitOpcode( int op, idVarDef *var_a, idVarDef *var_b );
	bool			EmitPush( idVarDef *expression, const idTypeDef *funcArg );
	void			NextToken( void );
	void			ExpectToken( const char *string );
	bool			CheckToken( const char *string );
	void			ParseName( idStr &name );
	void			SkipOutOfFunction( void );
	void			SkipToSemicolon( void );
	idTypeDef		*CheckType( void );
	idTypeDef		*ParseType( void );
	idVarDef		*FindImmediate( const idTypeDef *type, const eval_t *eval, const char *string ) const;
	idVarDef		*GetImmediate( idTypeDef *type, const eval_t *eval, const char *string );
	idVarDef		*VirtualFunctionConstant( idVarDef *func );
	idVarDef*		GlobalVirtualFunctionConstant( idVarDef *func );
	idVarDef		*SizeConstant( int size );
	idVarDef		*JumpConstant( int value );
	idVarDef		*JumpDef( int jumpfrom, int jumpto );
	idVarDef		*JumpTo( int jumpto );
	idVarDef		*JumpFrom( int jumpfrom );
	idVarDef		*ParseImmediate( void );
	idVarDef		*EmitFunctionParms( int op, idVarDef *func, int startarg, int startsize, idVarDef *object );
	idVarDef		*ParseFunctionCall( idVarDef *func );
	idVarDef		*ParseObjectCall( idVarDef *object, idVarDef *func );
	idVarDef		*ParseEventCall( idVarDef *object, idVarDef *func );
	idVarDef		*ParseVirtualEventCall( idVarDef *object, idVarDef *func );
	idVarDef		*ParseSysObjectCall( idVarDef *func );
	idVarDef		*LookupDef( const char *name, const idVarDef *baseobj );
	idVarDef		*ParseValue( void );
	idVarDef		*GetTerm( void );
	bool			TypeMatches( etype_t type1, etype_t type2 ) const;
	idVarDef		*GetExpression( int priority );
	idTypeDef		*GetTypeForEventArg( char argType );
	void			PatchLoop( int start, int continuePos );
	void			ParseReturnStatement( void );
	void			ParseWhileStatement( void );
	void			ParseForStatement( void );
	void			ParseDoWhileStatement( void );
	void			ParseIfStatement( void );
	void			ParseStatement( void );
	void			ParseObjectDef( const char *objname );
	idTypeDef		*ParseFunction( idTypeDef *returnType, const char *name );
	void			ParseFunctionDef( idTypeDef *returnType, const char *name );
	void			ParseVariableDef( idTypeDef *type, const char *name );
	void			ParseScriptEventDef( idTypeDef *returnType, const char *name );
	void			ParseEventDef( idTypeDef *type, const char *name );
	void			ParseDefs( void );
	void			ParseNamespace( idVarDef *newScope );

public :
	static opcode_t	opcodes[];

					idCompiler( idProgram* _program );
	void			CompileFile( const char *text, const char *filename );
};

#endif /* !__SCRIPT_COMPILER_H__ */
