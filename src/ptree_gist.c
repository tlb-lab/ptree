/*
 * GiST support for ptree
 * Teodor Sigaev <teodor@stack.net>
 * contrib/ptree/ptree_gist.c
 */
#include "postgres.h"

#include "access/gist.h"
#include "access/nbtree.h"
#include "access/skey.h"
#include "utils/array.h"
#include "crc32.h"
#include "ptree.h"

#define NEXTVAL(x) ( (pquery*)( (char*)(x) + INTALIGN( VARSIZE(x) ) ) )

PG_FUNCTION_INFO_V1(ptree_gist_in);
Datum		ptree_gist_in(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(ptree_gist_out);
Datum		ptree_gist_out(PG_FUNCTION_ARGS);

Datum
ptree_gist_in(PG_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("ptree_gist_in() not implemented")));
	PG_RETURN_DATUM(0);
}

Datum
ptree_gist_out(PG_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("ptree_gist_out() not implemented")));
	PG_RETURN_DATUM(0);
}

PG_FUNCTION_INFO_V1(ptree_compress);
Datum		ptree_compress(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(ptree_decompress);
Datum		ptree_decompress(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(ptree_same);
Datum		ptree_same(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(ptree_union);
Datum		ptree_union(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(ptree_penalty);
Datum		ptree_penalty(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(ptree_picksplit);
Datum		ptree_picksplit(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(ptree_consistent);
Datum		ptree_consistent(PG_FUNCTION_ARGS);

#define ISEQ(a,b)	( (a)->numlevel == (b)->numlevel && ptree_compare(a,b)==0 )
#define GETENTRY(vec,pos) ((ptree_gist *) DatumGetPointer((vec)->vector[(pos)].key))

Datum
ptree_compress(PG_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) PG_GETARG_POINTER(0);
	GISTENTRY  *retval = entry;

	if (entry->leafkey)
	{							/* ptree */
		ptree_gist *key;
		ptree	   *val = (ptree *) DatumGetPointer(PG_DETOAST_DATUM(entry->key));
		int4		len = LTG_HDRSIZE + VARSIZE(val);

		key = (ptree_gist *) palloc(len);
		SET_VARSIZE(key, len);
		key->flag = LTG_ONENODE;
		memcpy((void *) LTG_NODE(key), (void *) val, VARSIZE(val));

		retval = (GISTENTRY *) palloc(sizeof(GISTENTRY));
		gistentryinit(*retval, PointerGetDatum(key),
					  entry->rel, entry->page,
					  entry->offset, FALSE);
	}
	PG_RETURN_POINTER(retval);
}

Datum
ptree_decompress(PG_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) PG_GETARG_POINTER(0);
	ptree_gist *key = (ptree_gist *) DatumGetPointer(PG_DETOAST_DATUM(entry->key));

	if (PointerGetDatum(key) != entry->key)
	{
		GISTENTRY  *retval = (GISTENTRY *) palloc(sizeof(GISTENTRY));

		gistentryinit(*retval, PointerGetDatum(key),
					  entry->rel, entry->page,
					  entry->offset, FALSE);
		PG_RETURN_POINTER(retval);
	}
	PG_RETURN_POINTER(entry);
}

Datum
ptree_same(PG_FUNCTION_ARGS)
{
	ptree_gist *a = (ptree_gist *) PG_GETARG_POINTER(0);
	ptree_gist *b = (ptree_gist *) PG_GETARG_POINTER(1);
	bool	   *result = (bool *) PG_GETARG_POINTER(2);

	*result = false;
	if (LTG_ISONENODE(a) != LTG_ISONENODE(b))
		PG_RETURN_POINTER(result);

	if (LTG_ISONENODE(a))
		*result = (ISEQ(LTG_NODE(a), LTG_NODE(b))) ? true : false;
	else
	{
		int4		i;
		BITVECP		sa = LTG_SIGN(a),
					sb = LTG_SIGN(b);

		if (LTG_ISALLTRUE(a) != LTG_ISALLTRUE(b))
			PG_RETURN_POINTER(result);

		if (!ISEQ(LTG_LNODE(a), LTG_LNODE(b)))
			PG_RETURN_POINTER(result);
		if (!ISEQ(LTG_RNODE(a), LTG_RNODE(b)))
			PG_RETURN_POINTER(result);

		*result = true;
		if (!LTG_ISALLTRUE(a))
		{
			LOOPBYTE
			{
				if (sa[i] != sb[i])
				{
					*result = false;
					break;
				}
			}
		}
	}

	PG_RETURN_POINTER(result);
}

static void
hashing(BITVECP sign, ptree *t)
{
	int			tlen = t->numlevel;
	ptree_level *cur = PTREE_FIRST(t);
	int			hash;

	while (tlen > 0)
	{
		hash = ptree_crc32_sz(cur->name, cur->len);
		HASH(sign, hash);
		cur = LEVEL_NEXT(cur);
		tlen--;
	}
}

Datum
ptree_union(PG_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) PG_GETARG_POINTER(0);
	int		   *size = (int *) PG_GETARG_POINTER(1);
	BITVEC		base;
	int4		i,
				j;
	ptree_gist *result,
			   *cur;
	ptree	   *left = NULL,
			   *right = NULL,
			   *curtree;
	bool		isalltrue = false;
	bool		isleqr;

	MemSet((void *) base, 0, sizeof(BITVEC));
	for (j = 0; j < entryvec->n; j++)
	{
		cur = GETENTRY(entryvec, j);
		if (LTG_ISONENODE(cur))
		{
			curtree = LTG_NODE(cur);
			hashing(base, curtree);
			if (!left || ptree_compare(left, curtree) > 0)
				left = curtree;
			if (!right || ptree_compare(right, curtree) < 0)
				right = curtree;
		}
		else
		{
			if (isalltrue || LTG_ISALLTRUE(cur))
				isalltrue = true;
			else
			{
				BITVECP		sc = LTG_SIGN(cur);

				LOOPBYTE
					((unsigned char *) base)[i] |= sc[i];
			}

			curtree = LTG_LNODE(cur);
			if (!left || ptree_compare(left, curtree) > 0)
				left = curtree;
			curtree = LTG_RNODE(cur);
			if (!right || ptree_compare(right, curtree) < 0)
				right = curtree;
		}
	}

	if (isalltrue == false)
	{
		isalltrue = true;
		LOOPBYTE
		{
			if (((unsigned char *) base)[i] != 0xff)
			{
				isalltrue = false;
				break;
			}
		}
	}

	isleqr = (left == right || ISEQ(left, right)) ? true : false;
	*size = LTG_HDRSIZE + ((isalltrue) ? 0 : SIGLEN) + VARSIZE(left) + ((isleqr) ? 0 : VARSIZE(right));

	result = (ptree_gist *) palloc(*size);
	SET_VARSIZE(result, *size);
	result->flag = 0;

	if (isalltrue)
		result->flag |= LTG_ALLTRUE;
	else
		memcpy((void *) LTG_SIGN(result), base, SIGLEN);

	memcpy((void *) LTG_LNODE(result), (void *) left, VARSIZE(left));
	if (isleqr)
		result->flag |= LTG_NORIGHT;
	else
		memcpy((void *) LTG_RNODE(result), (void *) right, VARSIZE(right));

	PG_RETURN_POINTER(result);
}

Datum
ptree_penalty(PG_FUNCTION_ARGS)
{
	ptree_gist *origval = (ptree_gist *) DatumGetPointer(((GISTENTRY *) PG_GETARG_POINTER(0))->key);
	ptree_gist *newval = (ptree_gist *) DatumGetPointer(((GISTENTRY *) PG_GETARG_POINTER(1))->key);
	float	   *penalty = (float *) PG_GETARG_POINTER(2);
	int4		cmpr,
				cmpl;

	cmpl = ptree_compare(LTG_GETLNODE(origval), LTG_GETLNODE(newval));
	cmpr = ptree_compare(LTG_GETRNODE(newval), LTG_GETRNODE(origval));

	*penalty = Max(cmpl, 0) + Max(cmpr, 0);

	PG_RETURN_POINTER(penalty);
}

/* used for sorting */
typedef struct rix
{
	int			index;
	ptree	   *r;
} RIX;

static int
treekey_cmp(const void *a, const void *b)
{
	return ptree_compare(
						 ((RIX *) a)->r,
						 ((RIX *) b)->r
		);
}


Datum
ptree_picksplit(PG_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) PG_GETARG_POINTER(0);
	GIST_SPLITVEC *v = (GIST_SPLITVEC *) PG_GETARG_POINTER(1);
	OffsetNumber j;
	int4		i;
	RIX		   *array;
	OffsetNumber maxoff;
	int			nbytes;
	int			size;
	ptree	   *lu_l,
			   *lu_r,
			   *ru_l,
			   *ru_r;
	ptree_gist *lu,
			   *ru;
	BITVEC		ls,
				rs;
	bool		lisat = false,
				risat = false,
				isleqr;

	memset((void *) ls, 0, sizeof(BITVEC));
	memset((void *) rs, 0, sizeof(BITVEC));
	maxoff = entryvec->n - 1;
	nbytes = (maxoff + 2) * sizeof(OffsetNumber);
	v->spl_left = (OffsetNumber *) palloc(nbytes);
	v->spl_right = (OffsetNumber *) palloc(nbytes);
	v->spl_nleft = 0;
	v->spl_nright = 0;
	array = (RIX *) palloc(sizeof(RIX) * (maxoff + 1));

	/* copy the data into RIXes, and sort the RIXes */
	for (j = FirstOffsetNumber; j <= maxoff; j = OffsetNumberNext(j))
	{
		array[j].index = j;
		lu = GETENTRY(entryvec, j);		/* use as tmp val */
		array[j].r = LTG_GETLNODE(lu);
	}

	qsort((void *) &array[FirstOffsetNumber], maxoff - FirstOffsetNumber + 1,
		  sizeof(RIX), treekey_cmp);

	lu_l = lu_r = ru_l = ru_r = NULL;
	for (j = FirstOffsetNumber; j <= maxoff; j = OffsetNumberNext(j))
	{
		lu = GETENTRY(entryvec, array[j].index);		/* use as tmp val */
		if (j <= (maxoff - FirstOffsetNumber + 1) / 2)
		{
			v->spl_left[v->spl_nleft] = array[j].index;
			v->spl_nleft++;
			if (lu_r == NULL || ptree_compare(LTG_GETRNODE(lu), lu_r) > 0)
				lu_r = LTG_GETRNODE(lu);
			if (LTG_ISONENODE(lu))
				hashing(ls, LTG_NODE(lu));
			else
			{
				if (lisat || LTG_ISALLTRUE(lu))
					lisat = true;
				else
				{
					BITVECP		sc = LTG_SIGN(lu);

					LOOPBYTE
						((unsigned char *) ls)[i] |= sc[i];
				}
			}
		}
		else
		{
			v->spl_right[v->spl_nright] = array[j].index;
			v->spl_nright++;
			if (ru_r == NULL || ptree_compare(LTG_GETRNODE(lu), ru_r) > 0)
				ru_r = LTG_GETRNODE(lu);
			if (LTG_ISONENODE(lu))
				hashing(rs, LTG_NODE(lu));
			else
			{
				if (risat || LTG_ISALLTRUE(lu))
					risat = true;
				else
				{
					BITVECP		sc = LTG_SIGN(lu);

					LOOPBYTE
						((unsigned char *) rs)[i] |= sc[i];
				}
			}
		}
	}

	if (lisat == false)
	{
		lisat = true;
		LOOPBYTE
		{
			if (((unsigned char *) ls)[i] != 0xff)
			{
				lisat = false;
				break;
			}
		}
	}

	if (risat == false)
	{
		risat = true;
		LOOPBYTE
		{
			if (((unsigned char *) rs)[i] != 0xff)
			{
				risat = false;
				break;
			}
		}
	}

	lu_l = LTG_GETLNODE(GETENTRY(entryvec, array[FirstOffsetNumber].index));
	isleqr = (lu_l == lu_r || ISEQ(lu_l, lu_r)) ? true : false;
	size = LTG_HDRSIZE + ((lisat) ? 0 : SIGLEN) + VARSIZE(lu_l) + ((isleqr) ? 0 : VARSIZE(lu_r));
	lu = (ptree_gist *) palloc(size);
	SET_VARSIZE(lu, size);
	lu->flag = 0;
	if (lisat)
		lu->flag |= LTG_ALLTRUE;
	else
		memcpy((void *) LTG_SIGN(lu), ls, SIGLEN);
	memcpy((void *) LTG_LNODE(lu), (void *) lu_l, VARSIZE(lu_l));
	if (isleqr)
		lu->flag |= LTG_NORIGHT;
	else
		memcpy((void *) LTG_RNODE(lu), (void *) lu_r, VARSIZE(lu_r));


	ru_l = LTG_GETLNODE(GETENTRY(entryvec, array[1 + ((maxoff - FirstOffsetNumber + 1) / 2)].index));
	isleqr = (ru_l == ru_r || ISEQ(ru_l, ru_r)) ? true : false;
	size = LTG_HDRSIZE + ((risat) ? 0 : SIGLEN) + VARSIZE(ru_l) + ((isleqr) ? 0 : VARSIZE(ru_r));
	ru = (ptree_gist *) palloc(size);
	SET_VARSIZE(ru, size);
	ru->flag = 0;
	if (risat)
		ru->flag |= LTG_ALLTRUE;
	else
		memcpy((void *) LTG_SIGN(ru), rs, SIGLEN);
	memcpy((void *) LTG_LNODE(ru), (void *) ru_l, VARSIZE(ru_l));
	if (isleqr)
		ru->flag |= LTG_NORIGHT;
	else
		memcpy((void *) LTG_RNODE(ru), (void *) ru_r, VARSIZE(ru_r));

	v->spl_ldatum = PointerGetDatum(lu);
	v->spl_rdatum = PointerGetDatum(ru);

	PG_RETURN_POINTER(v);
}

static bool
gist_isparent(ptree_gist *key, ptree *query)
{
	int4		numlevel = query->numlevel;
	int			i;

	for (i = query->numlevel; i >= 0; i--)
	{
		query->numlevel = i;
		if (ptree_compare(query, LTG_GETLNODE(key)) >= 0 && ptree_compare(query, LTG_GETRNODE(key)) <= 0)
		{
			query->numlevel = numlevel;
			return true;
		}
	}

	query->numlevel = numlevel;
	return false;
}

static ptree *
copy_ptree(ptree *src)
{
	ptree	   *dst = (ptree *) palloc(VARSIZE(src));

	memcpy(dst, src, VARSIZE(src));
	return dst;
}

static bool
gist_ischild(ptree_gist *key, ptree *query)
{
	ptree	   *left = copy_ptree(LTG_GETLNODE(key));
	ptree	   *right = copy_ptree(LTG_GETRNODE(key));
	bool		res = true;

	if (left->numlevel > query->numlevel)
		left->numlevel = query->numlevel;

	if (ptree_compare(query, left) < 0)
		res = false;

	if (right->numlevel > query->numlevel)
		right->numlevel = query->numlevel;

	if (res && ptree_compare(query, right) > 0)
		res = false;

	pfree(left);
	pfree(right);

	return res;
}

static bool
gist_qe(ptree_gist *key, pquery *query)
{
	pquery_level *curq = LQUERY_FIRST(query);
	BITVECP		sign = LTG_SIGN(key);
	int			qlen = query->numlevel;

	if (LTG_ISALLTRUE(key))
		return true;

	while (qlen > 0)
	{
		if (curq->numvar && LQL_CANLOOKSIGN(curq))
		{
			bool		isexist = false;
			int			vlen = curq->numvar;
			pquery_variant *curv = LQL_FIRST(curq);

			while (vlen > 0)
			{
				if (GETBIT(sign, HASHVAL(curv->val)))
				{
					isexist = true;
					break;
				}
				curv = LVAR_NEXT(curv);
				vlen--;
			}
			if (!isexist)
				return false;
		}

		curq = LQL_NEXT(curq);
		qlen--;
	}

	return true;
}

static int
gist_tqcmp(ptree *t, pquery *q)
{
	ptree_level *al = PTREE_FIRST(t);
	pquery_level *ql = LQUERY_FIRST(q);
	pquery_variant *bl;
	int			an = t->numlevel;
	int			bn = q->firstgood;
	int			res = 0;

	while (an > 0 && bn > 0)
	{
		bl = LQL_FIRST(ql);
		if ((res = memcmp(al->name, bl->name, Min(al->len, bl->len))) == 0)
		{
			if (al->len != bl->len)
				return al->len - bl->len;
		}
		else
			return res;
		an--;
		bn--;
		al = LEVEL_NEXT(al);
		ql = LQL_NEXT(ql);
	}

	return Min(t->numlevel, q->firstgood) - q->firstgood;
}

static bool
gist_between(ptree_gist *key, pquery *query)
{
	if (query->firstgood == 0)
		return true;

	if (gist_tqcmp(LTG_GETLNODE(key), query) > 0)
		return false;

	if (gist_tqcmp(LTG_GETRNODE(key), query) < 0)
		return false;

	return true;
}

static bool
checkcondition_bit(void *checkval, ITEM *val)
{
	return (FLG_CANLOOKSIGN(val->flag)) ? GETBIT(checkval, HASHVAL(val->val)) : true;
}

static bool
gist_qtxt(ptree_gist *key, ptxtquery *query)
{
	if (LTG_ISALLTRUE(key))
		return true;

	return ptree_execute(
						 GETQUERY(query),
						 (void *) LTG_SIGN(key), false,
						 checkcondition_bit
		);
}

static bool
arrq_cons(ptree_gist *key, ArrayType *_query)
{
	pquery	   *query = (pquery *) ARR_DATA_PTR(_query);
	int			num = ArrayGetNItems(ARR_NDIM(_query), ARR_DIMS(_query));

	if (ARR_NDIM(_query) > 1)
		ereport(ERROR,
				(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
				 errmsg("array must be one-dimensional")));
	if (array_contains_nulls(_query))
		ereport(ERROR,
				(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
				 errmsg("array must not contain nulls")));

	while (num > 0)
	{
		if (gist_qe(key, query) && gist_between(key, query))
			return true;
		num--;
		query = NEXTVAL(query);
	}
	return false;
}

Datum
ptree_consistent(PG_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) PG_GETARG_POINTER(0);
	StrategyNumber strategy = (StrategyNumber) PG_GETARG_UINT16(2);

	/* Oid		subtype = PG_GETARG_OID(3); */
	bool	   *recheck = (bool *) PG_GETARG_POINTER(4);
	ptree_gist *key = (ptree_gist *) DatumGetPointer(entry->key);
	void	   *query = NULL;
	bool		res = false;

	/* All cases served by this function are exact */
	*recheck = false;

	switch (strategy)
	{
		case BTLessStrategyNumber:
			query = PG_GETARG_PTREE(1);
			res = (GIST_LEAF(entry)) ?
				(ptree_compare((ptree *) query, LTG_NODE(key)) > 0)
				:
				(ptree_compare((ptree *) query, LTG_GETLNODE(key)) >= 0);
			break;
		case BTLessEqualStrategyNumber:
			query = PG_GETARG_PTREE(1);
			res = (ptree_compare((ptree *) query, LTG_GETLNODE(key)) >= 0);
			break;
		case BTEqualStrategyNumber:
			query = PG_GETARG_PTREE(1);
			if (GIST_LEAF(entry))
				res = (ptree_compare((ptree *) query, LTG_NODE(key)) == 0);
			else
				res = (
					   ptree_compare((ptree *) query, LTG_GETLNODE(key)) >= 0
					   &&
					   ptree_compare((ptree *) query, LTG_GETRNODE(key)) <= 0
					);
			break;
		case BTGreaterEqualStrategyNumber:
			query = PG_GETARG_PTREE(1);
			res = (ptree_compare((ptree *) query, LTG_GETRNODE(key)) <= 0);
			break;
		case BTGreaterStrategyNumber:
			query = PG_GETARG_PTREE(1);
			res = (GIST_LEAF(entry)) ?
				(ptree_compare((ptree *) query, LTG_GETRNODE(key)) < 0)
				:
				(ptree_compare((ptree *) query, LTG_GETRNODE(key)) <= 0);
			break;
		case 10:
			query = PG_GETARG_PTREE_COPY(1);
			res = (GIST_LEAF(entry)) ?
				inner_isparent((ptree *) query, LTG_NODE(key))
				:
				gist_isparent(key, (ptree *) query);
			break;
		case 11:
			query = PG_GETARG_PTREE(1);
			res = (GIST_LEAF(entry)) ?
				inner_isparent(LTG_NODE(key), (ptree *) query)
				:
				gist_ischild(key, (ptree *) query);
			break;
		case 12:
		case 13:
			query = PG_GETARG_LQUERY(1);
			if (GIST_LEAF(entry))
				res = DatumGetBool(DirectFunctionCall2(ltq_regex,
											  PointerGetDatum(LTG_NODE(key)),
											PointerGetDatum((pquery *) query)
													   ));
			else
				res = (gist_qe(key, (pquery *) query) && gist_between(key, (pquery *) query));
			break;
		case 14:
		case 15:
			query = PG_GETARG_LQUERY(1);
			if (GIST_LEAF(entry))
				res = DatumGetBool(DirectFunctionCall2(ptxtq_exec,
											  PointerGetDatum(LTG_NODE(key)),
											PointerGetDatum((pquery *) query)
													   ));
			else
				res = gist_qtxt(key, (ptxtquery *) query);
			break;
		case 16:
		case 17:
			query = DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(1)));
			if (GIST_LEAF(entry))
				res = DatumGetBool(DirectFunctionCall2(lt_q_regex,
											  PointerGetDatum(LTG_NODE(key)),
										 PointerGetDatum((ArrayType *) query)
													   ));
			else
				res = arrq_cons(key, (ArrayType *) query);
			break;
		default:
			/* internal error */
			elog(ERROR, "unrecognized StrategyNumber: %d", strategy);
	}

	PG_FREE_IF_COPY(query, 1);
	PG_RETURN_BOOL(res);
}
