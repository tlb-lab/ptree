/* contrib/ptree/ptree.h */

#ifndef __PTREE_H__
#define __PTREE_H__

#include "postgres.h"
#include "fmgr.h"
#include "tsearch/ts_locale.h"

// character that is used to separate labels
#define PTREE_SEPARATOR '/'

typedef struct
{
	uint16		len;
	char		name[1];
} ptree_level;

#define LEVEL_HDRSIZE	(offsetof(ptree_level,name))
#define LEVEL_NEXT(x)	( (ptree_level*)( ((char*)(x)) + MAXALIGN(((ptree_level*)(x))->len + LEVEL_HDRSIZE) ) )

typedef struct
{
	int32		vl_len_;		/* varlena header (do not touch directly!) */
	uint16		numlevel;
	char		data[1];
} ptree;

#define PTREE_HDRSIZE	MAXALIGN( offsetof(ptree, data) )
#define PTREE_FIRST(x)	( (ptree_level*)( ((char*)(x))+PTREE_HDRSIZE ) )


/* pquery */

typedef struct
{
	int4		val;
	uint16		len;
	uint8		flag;
	char		name[1];
} pquery_variant;

#define LVAR_HDRSIZE   MAXALIGN(offsetof(pquery_variant, name))
#define LVAR_NEXT(x)	( (pquery_variant*)( ((char*)(x)) + MAXALIGN(((pquery_variant*)(x))->len) + LVAR_HDRSIZE ) )

#define LVAR_ANYEND 0x01
#define LVAR_INCASE 0x02
#define LVAR_SUBLEXEME	0x04

typedef struct
{
	uint16		totallen;
	uint16		flag;
	uint16		numvar;
	uint16		low;
	uint16		high;
	char		variants[1];
} pquery_level;

#define LQL_HDRSIZE MAXALIGN( offsetof(pquery_level,variants) )
#define LQL_NEXT(x) ( (pquery_level*)( ((char*)(x)) + MAXALIGN(((pquery_level*)(x))->totallen) ) )
#define LQL_FIRST(x)	( (pquery_variant*)( ((char*)(x))+LQL_HDRSIZE ) )

#define LQL_NOT		0x10
#ifdef LOWER_NODE
#define FLG_CANLOOKSIGN(x) ( ( (x) & ( LQL_NOT | LVAR_ANYEND | LVAR_SUBLEXEME ) ) == 0 )
#else
#define FLG_CANLOOKSIGN(x) ( ( (x) & ( LQL_NOT | LVAR_ANYEND | LVAR_SUBLEXEME | LVAR_INCASE ) ) == 0 )
#endif
#define LQL_CANLOOKSIGN(x) FLG_CANLOOKSIGN( ((pquery_level*)(x))->flag )

typedef struct
{
	int32		vl_len_;		/* varlena header (do not touch directly!) */
	uint16		numlevel;
	uint16		firstgood;
	uint16		flag;
	char		data[1];
} pquery;

#define LQUERY_HDRSIZE  MAXALIGN( offsetof(pquery, data) )
#define LQUERY_FIRST(x) ( (pquery_level*)( ((char*)(x))+LQUERY_HDRSIZE ) )

#define LQUERY_HASNOT   0x01

// allow '-' and '`'
#define ISALNUM(x)      ( t_isalpha(x) || t_isdigit(x) || (pg_mblen(x) == 1 && t_iseq((x), '_')) || (pg_mblen(x) == 1 && t_iseq((x), '-')) || (pg_mblen(x) == 1 && t_iseq((x), '`')) )

/* full text query */

/*
 * item in polish notation with back link
 * to left operand
 */
typedef struct ITEM
{
	int2		type;
	int2		left;
	int4		val;
	uint8		flag;
	/* user-friendly value */
	uint8		length;
	uint16		distance;
} ITEM;

/*
 *Storage:
 *		(len)(size)(array of ITEM)(array of operand in user-friendly form)
 */
typedef struct
{
	int32		vl_len_;		/* varlena header (do not touch directly!) */
	int4		size;
	char		data[1];
} ptxtquery;

#define HDRSIZEQT		MAXALIGN(VARHDRSZ + sizeof(int4))
#define COMPUTESIZE(size,lenofoperand)	( HDRSIZEQT + (size) * sizeof(ITEM) + (lenofoperand) )
#define GETQUERY(x)  (ITEM*)( (char*)(x)+HDRSIZEQT )
#define GETOPERAND(x)	( (char*)GETQUERY(x) + ((ptxtquery*)x)->size * sizeof(ITEM) )

#define ISOPERATOR(x) ( (x)=='!' || (x)=='&' || (x)=='|' || (x)=='(' || (x)==')' )

#define END						0
#define ERR						1
#define VAL						2
#define OPR						3
#define OPEN					4
#define CLOSE					5
#define VALTRUE					6		/* for stop words */
#define VALFALSE				7


/* use in array iterator */
Datum		ptree_isparent(PG_FUNCTION_ARGS);
Datum		ptree_risparent(PG_FUNCTION_ARGS);
Datum		ltq_regex(PG_FUNCTION_ARGS);
Datum		ltq_rregex(PG_FUNCTION_ARGS);
Datum		lt_q_regex(PG_FUNCTION_ARGS);
Datum		lt_q_rregex(PG_FUNCTION_ARGS);
Datum		ltxtq_exec(PG_FUNCTION_ARGS);
Datum		ltxtq_rexec(PG_FUNCTION_ARGS);
Datum		_ltq_regex(PG_FUNCTION_ARGS);
Datum		_ltq_rregex(PG_FUNCTION_ARGS);
Datum		_lt_q_regex(PG_FUNCTION_ARGS);
Datum		_lt_q_rregex(PG_FUNCTION_ARGS);
Datum		_ltxtq_exec(PG_FUNCTION_ARGS);
Datum		_ltxtq_rexec(PG_FUNCTION_ARGS);
Datum		_ptree_isparent(PG_FUNCTION_ARGS);
Datum		_ptree_risparent(PG_FUNCTION_ARGS);

/* Concatenation functions */
Datum		ptree_addptree(PG_FUNCTION_ARGS);
Datum		ptree_addtext(PG_FUNCTION_ARGS);
Datum		ptree_textadd(PG_FUNCTION_ARGS);

/* Util function */
Datum		ptree_in(PG_FUNCTION_ARGS);

bool ptree_execute(ITEM *curitem, void *checkval,
			  bool calcnot, bool (*chkcond) (void *checkval, ITEM *val));

int			ptree_compare(const ptree *a, const ptree *b);
bool		inner_isparent(const ptree *c, const ptree *p);
bool compare_subnode(ptree_level *t, char *q, int len,
			int (*cmpptr) (const char *, const char *, size_t), bool anyend);
ptree	   *lca_inner(ptree **a, int len);
int			ptree_strncasecmp(const char *a, const char *b, size_t s);

#define PG_GETARG_PTREE(x)	((ptree*)DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(x))))
#define PG_GETARG_PTREE_COPY(x) ((ptree*)DatumGetPointer(PG_DETOAST_DATUM_COPY(PG_GETARG_DATUM(x))))
#define PG_GETARG_LQUERY(x) ((pquery*)DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(x))))
#define PG_GETARG_LQUERY_COPY(x) ((pquery*)DatumGetPointer(PG_DETOAST_DATUM_COPY(PG_GETARG_DATUM(x))))
#define PG_GETARG_LTXTQUERY(x) ((ptxtquery*)DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(x))))
#define PG_GETARG_LTXTQUERY_COPY(x) ((ptxtquery*)DatumGetPointer(PG_DETOAST_DATUM_COPY(PG_GETARG_DATUM(x))))

/* GiST support for ptree */

#define BITBYTE 8
#define SIGLENINT  2
#define SIGLEN	( sizeof(int4)*SIGLENINT )
#define SIGLENBIT (SIGLEN*BITBYTE)
typedef unsigned char BITVEC[SIGLEN];
typedef unsigned char *BITVECP;

#define LOOPBYTE \
			for(i=0;i<SIGLEN;i++)

#define GETBYTE(x,i) ( *( (BITVECP)(x) + (int)( (i) / BITBYTE ) ) )
#define GETBITBYTE(x,i) ( ((unsigned char)(x)) >> i & 0x01 )
#define CLRBIT(x,i)   GETBYTE(x,i) &= ~( 0x01 << ( (i) % BITBYTE ) )
#define SETBIT(x,i)   GETBYTE(x,i) |=  ( 0x01 << ( (i) % BITBYTE ) )
#define GETBIT(x,i) ( (GETBYTE(x,i) >> ( (i) % BITBYTE )) & 0x01 )

#define HASHVAL(val) (((unsigned int)(val)) % SIGLENBIT)
#define HASH(sign, val) SETBIT((sign), HASHVAL(val))

/*
 * type of index key for ptree. Tree are combined B-Tree and R-Tree
 * Storage:
 *	Leaf pages
 *		(len)(flag)(ptree)
 *	Non-Leaf
 *				 (len)(flag)(sign)(left_ptree)(right_ptree)
 *		ALLTRUE: (len)(flag)(left_ptree)(right_ptree)
 *
 */

typedef struct
{
	int32		vl_len_;		/* varlena header (do not touch directly!) */
	uint32		flag;
	char		data[1];
} ptree_gist;

#define LTG_ONENODE 0x01
#define LTG_ALLTRUE 0x02
#define LTG_NORIGHT 0x04

#define LTG_HDRSIZE MAXALIGN(VARHDRSZ + sizeof(uint32))
#define LTG_SIGN(x) ( (BITVECP)( ((char*)(x))+LTG_HDRSIZE ) )
#define LTG_NODE(x) ( (ptree*)( ((char*)(x))+LTG_HDRSIZE ) )
#define LTG_ISONENODE(x) ( ((ptree_gist*)(x))->flag & LTG_ONENODE )
#define LTG_ISALLTRUE(x) ( ((ptree_gist*)(x))->flag & LTG_ALLTRUE )
#define LTG_ISNORIGHT(x) ( ((ptree_gist*)(x))->flag & LTG_NORIGHT )
#define LTG_LNODE(x)	( (ptree*)( ( ((char*)(x))+LTG_HDRSIZE ) + ( LTG_ISALLTRUE(x) ? 0 : SIGLEN ) ) )
#define LTG_RENODE(x)	( (ptree*)( ((char*)LTG_LNODE(x)) + VARSIZE(LTG_LNODE(x))) )
#define LTG_RNODE(x)	( LTG_ISNORIGHT(x) ? LTG_LNODE(x) : LTG_RENODE(x) )

#define LTG_GETLNODE(x) ( LTG_ISONENODE(x) ? LTG_NODE(x) : LTG_LNODE(x) )
#define LTG_GETRNODE(x) ( LTG_ISONENODE(x) ? LTG_NODE(x) : LTG_RNODE(x) )


/* GiST support for ptree[] */

#define ASIGLENINT	(7)
#define ASIGLEN		(sizeof(int4)*ASIGLENINT)
#define ASIGLENBIT (ASIGLEN*BITBYTE)
typedef unsigned char ABITVEC[ASIGLEN];

#define ALOOPBYTE \
			for(i=0;i<ASIGLEN;i++)

#define AHASHVAL(val) (((unsigned int)(val)) % ASIGLENBIT)
#define AHASH(sign, val) SETBIT((sign), AHASHVAL(val))

/* type of key is the same to ptree_gist */

#endif
