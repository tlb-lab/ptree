/*
 * op function for ptree
 * Teodor Sigaev <teodor@stack.net>
 * contrib/ptree/ptree_op.c
 */
#include "postgres.h"

#include <ctype.h>

#include "catalog/pg_statistic.h"
#include "utils/builtins.h"
#include "utils/lsyscache.h"
#include "utils/selfuncs.h"
#include "utils/syscache.h"
#include "ptree.h"

PG_MODULE_MAGIC;

/* compare functions */
PG_FUNCTION_INFO_V1(ptree_cmp);
PG_FUNCTION_INFO_V1(ptree_lt);
PG_FUNCTION_INFO_V1(ptree_le);
PG_FUNCTION_INFO_V1(ptree_eq);
PG_FUNCTION_INFO_V1(ptree_ne);
PG_FUNCTION_INFO_V1(ptree_ge);
PG_FUNCTION_INFO_V1(ptree_gt);
PG_FUNCTION_INFO_V1(nlevel);
PG_FUNCTION_INFO_V1(ptree_isparent);
PG_FUNCTION_INFO_V1(ptree_risparent);
PG_FUNCTION_INFO_V1(subptree);
PG_FUNCTION_INFO_V1(subpath);
PG_FUNCTION_INFO_V1(ptree_index);
PG_FUNCTION_INFO_V1(ptree_addptree);
PG_FUNCTION_INFO_V1(ptree_addtext);
PG_FUNCTION_INFO_V1(ptree_textadd);
PG_FUNCTION_INFO_V1(lca);
PG_FUNCTION_INFO_V1(ptree2text);
PG_FUNCTION_INFO_V1(text2ptree);
PG_FUNCTION_INFO_V1(ptreeparentsel);

Datum		ptree_cmp(PG_FUNCTION_ARGS);
Datum		ptree_lt(PG_FUNCTION_ARGS);
Datum		ptree_le(PG_FUNCTION_ARGS);
Datum		ptree_eq(PG_FUNCTION_ARGS);
Datum		ptree_ne(PG_FUNCTION_ARGS);
Datum		ptree_ge(PG_FUNCTION_ARGS);
Datum		ptree_gt(PG_FUNCTION_ARGS);
Datum		nlevel(PG_FUNCTION_ARGS);
Datum		subptree(PG_FUNCTION_ARGS);
Datum		subpath(PG_FUNCTION_ARGS);
Datum		ptree_index(PG_FUNCTION_ARGS);
Datum		ptree_addptree(PG_FUNCTION_ARGS);
Datum		ptree_addtext(PG_FUNCTION_ARGS);
Datum		ptree_textadd(PG_FUNCTION_ARGS);
Datum		lca(PG_FUNCTION_ARGS);
Datum		ptree2text(PG_FUNCTION_ARGS);
Datum		text2ptree(PG_FUNCTION_ARGS);
Datum		ptreeparentsel(PG_FUNCTION_ARGS);

int
ptree_compare(const ptree *a, const ptree *b)
{
	ptree_level *al = PTREE_FIRST(a);
	ptree_level *bl = PTREE_FIRST(b);
	int			an = a->numlevel;
	int			bn = b->numlevel;
	int			res = 0;

	while (an > 0 && bn > 0)
	{
		if ((res = memcmp(al->name, bl->name, Min(al->len, bl->len))) == 0)
		{
			if (al->len != bl->len)
				return (al->len - bl->len) * 10 * (an + 1);
		}
		else
			return res * 10 * (an + 1);

		an--;
		bn--;
		al = LEVEL_NEXT(al);
		bl = LEVEL_NEXT(bl);
	}

	return (a->numlevel - b->numlevel) * 10 * (an + 1);
}

#define RUNCMP						\
ptree *a	= PG_GETARG_PTREE(0);			\
ptree *b	= PG_GETARG_PTREE(1);			\
int res = ptree_compare(a,b);				\
PG_FREE_IF_COPY(a,0);					\
PG_FREE_IF_COPY(b,1);					\

Datum
ptree_cmp(PG_FUNCTION_ARGS)
{
	RUNCMP
		PG_RETURN_INT32(res);
}

Datum
ptree_lt(PG_FUNCTION_ARGS)
{
	RUNCMP
		PG_RETURN_BOOL((res < 0) ? true : false);
}

Datum
ptree_le(PG_FUNCTION_ARGS)
{
	RUNCMP
		PG_RETURN_BOOL((res <= 0) ? true : false);
}

Datum
ptree_eq(PG_FUNCTION_ARGS)
{
	RUNCMP
		PG_RETURN_BOOL((res == 0) ? true : false);
}

Datum
ptree_ge(PG_FUNCTION_ARGS)
{
	RUNCMP
		PG_RETURN_BOOL((res >= 0) ? true : false);
}

Datum
ptree_gt(PG_FUNCTION_ARGS)
{
	RUNCMP
		PG_RETURN_BOOL((res > 0) ? true : false);
}

Datum
ptree_ne(PG_FUNCTION_ARGS)
{
	RUNCMP
		PG_RETURN_BOOL((res != 0) ? true : false);
}

Datum
nlevel(PG_FUNCTION_ARGS)
{
	ptree	   *a = PG_GETARG_PTREE(0);
	int			res = a->numlevel;

	PG_FREE_IF_COPY(a, 0);
	PG_RETURN_INT32(res);
}

bool
inner_isparent(const ptree *c, const ptree *p)
{
	ptree_level *cl = PTREE_FIRST(c);
	ptree_level *pl = PTREE_FIRST(p);
	int			pn = p->numlevel;

	if (pn > c->numlevel)
		return false;

	while (pn > 0)
	{
		if (cl->len != pl->len)
			return false;
		if (memcmp(cl->name, pl->name, cl->len))
			return false;

		pn--;
		cl = LEVEL_NEXT(cl);
		pl = LEVEL_NEXT(pl);
	}
	return true;
}

Datum
ptree_isparent(PG_FUNCTION_ARGS)
{
	ptree	   *c = PG_GETARG_PTREE(1);
	ptree	   *p = PG_GETARG_PTREE(0);
	bool		res = inner_isparent(c, p);

	PG_FREE_IF_COPY(c, 1);
	PG_FREE_IF_COPY(p, 0);
	PG_RETURN_BOOL(res);
}

Datum
ptree_risparent(PG_FUNCTION_ARGS)
{
	ptree	   *c = PG_GETARG_PTREE(0);
	ptree	   *p = PG_GETARG_PTREE(1);
	bool		res = inner_isparent(c, p);

	PG_FREE_IF_COPY(c, 0);
	PG_FREE_IF_COPY(p, 1);
	PG_RETURN_BOOL(res);
}


static ptree *
inner_subptree(ptree *t, int4 startpos, int4 endpos)
{
	char	   *start = NULL,
			   *end = NULL;
	ptree_level *ptr = PTREE_FIRST(t);
	ptree	   *res;
	int			i;

	if (startpos < 0 || endpos < 0 || startpos >= t->numlevel || startpos > endpos)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("invalid positions")));

	if (endpos > t->numlevel)
		endpos = t->numlevel;

	start = end = (char *) ptr;
	for (i = 0; i < endpos; i++)
	{
		if (i == startpos)
			start = (char *) ptr;
		if (i == endpos - 1)
		{
			end = (char *) LEVEL_NEXT(ptr);
			break;
		}
		ptr = LEVEL_NEXT(ptr);
	}

	res = (ptree *) palloc(PTREE_HDRSIZE + (end - start));
	SET_VARSIZE(res, PTREE_HDRSIZE + (end - start));
	res->numlevel = endpos - startpos;

	memcpy(PTREE_FIRST(res), start, end - start);

	return res;
}

Datum
subptree(PG_FUNCTION_ARGS)
{
	ptree	   *t = PG_GETARG_PTREE(0);
	ptree	   *res = inner_subptree(t, PG_GETARG_INT32(1), PG_GETARG_INT32(2));

	PG_FREE_IF_COPY(t, 0);
	PG_RETURN_POINTER(res);
}

Datum
subpath(PG_FUNCTION_ARGS)
{
	ptree	   *t = PG_GETARG_PTREE(0);
	int4		start = PG_GETARG_INT32(1);
	int4		len = (fcinfo->nargs == 3) ? PG_GETARG_INT32(2) : 0;
	int4		end;
	ptree	   *res;

	end = start + len;

	if (start < 0)
	{
		start = t->numlevel + start;
		end = start + len;
	}
	if (start < 0)
	{							/* start > t->numlevel */
		start = t->numlevel + start;
		end = start + len;
	}

	if (len < 0)
		end = t->numlevel + len;
	else if (len == 0)
		end = (fcinfo->nargs == 3) ? start : 0xffff;

	res = inner_subptree(t, start, end);

	PG_FREE_IF_COPY(t, 0);
	PG_RETURN_POINTER(res);
}

static ptree *
ptree_concat(ptree *a, ptree *b)
{
	ptree	   *r;

	r = (ptree *) palloc(VARSIZE(a) + VARSIZE(b) - PTREE_HDRSIZE);
	SET_VARSIZE(r, VARSIZE(a) + VARSIZE(b) - PTREE_HDRSIZE);
	r->numlevel = a->numlevel + b->numlevel;

	memcpy(PTREE_FIRST(r), PTREE_FIRST(a), VARSIZE(a) - PTREE_HDRSIZE);
	memcpy(((char *) PTREE_FIRST(r)) + VARSIZE(a) - PTREE_HDRSIZE,
		   PTREE_FIRST(b),
		   VARSIZE(b) - PTREE_HDRSIZE);
	return r;
}

Datum
ptree_addptree(PG_FUNCTION_ARGS)
{
	ptree	   *a = PG_GETARG_PTREE(0);
	ptree	   *b = PG_GETARG_PTREE(1);
	ptree	   *r;

	r = ptree_concat(a, b);
	PG_FREE_IF_COPY(a, 0);
	PG_FREE_IF_COPY(b, 1);
	PG_RETURN_POINTER(r);
}

Datum
ptree_addtext(PG_FUNCTION_ARGS)
{
	ptree	   *a = PG_GETARG_PTREE(0);
	text	   *b = PG_GETARG_TEXT_PP(1);
	char	   *s;
	ptree	   *r,
			   *tmp;

	s = text_to_cstring(b);

	tmp = (ptree *) DatumGetPointer(DirectFunctionCall1(ptree_in,
														PointerGetDatum(s)));

	pfree(s);

	r = ptree_concat(a, tmp);

	pfree(tmp);

	PG_FREE_IF_COPY(a, 0);
	PG_FREE_IF_COPY(b, 1);
	PG_RETURN_POINTER(r);
}

Datum
ptree_index(PG_FUNCTION_ARGS)
{
	ptree	   *a = PG_GETARG_PTREE(0);
	ptree	   *b = PG_GETARG_PTREE(1);
	int			start = (fcinfo->nargs == 3) ? PG_GETARG_INT32(2) : 0;
	int			i,
				j;
	ptree_level *startptr,
			   *aptr,
			   *bptr;
	bool		found = false;

	if (start < 0)
	{
		if (-start >= a->numlevel)
			start = 0;
		else
			start = (int) (a->numlevel) + start;
	}

	if (a->numlevel - start < b->numlevel || a->numlevel == 0 || b->numlevel == 0)
	{
		PG_FREE_IF_COPY(a, 0);
		PG_FREE_IF_COPY(b, 1);
		PG_RETURN_INT32(-1);
	}

	startptr = PTREE_FIRST(a);
	for (i = 0; i <= a->numlevel - b->numlevel; i++)
	{
		if (i >= start)
		{
			aptr = startptr;
			bptr = PTREE_FIRST(b);
			for (j = 0; j < b->numlevel; j++)
			{
				if (!(aptr->len == bptr->len && memcmp(aptr->name, bptr->name, aptr->len) == 0))
					break;
				aptr = LEVEL_NEXT(aptr);
				bptr = LEVEL_NEXT(bptr);
			}

			if (j == b->numlevel)
			{
				found = true;
				break;
			}
		}
		startptr = LEVEL_NEXT(startptr);
	}

	if (!found)
		i = -1;

	PG_FREE_IF_COPY(a, 0);
	PG_FREE_IF_COPY(b, 1);
	PG_RETURN_INT32(i);
}

Datum
ptree_textadd(PG_FUNCTION_ARGS)
{
	ptree	   *a = PG_GETARG_PTREE(1);
	text	   *b = PG_GETARG_TEXT_PP(0);
	char	   *s;
	ptree	   *r,
			   *tmp;

	s = text_to_cstring(b);

	tmp = (ptree *) DatumGetPointer(DirectFunctionCall1(ptree_in,
														PointerGetDatum(s)));

	pfree(s);

	r = ptree_concat(tmp, a);

	pfree(tmp);

	PG_FREE_IF_COPY(a, 1);
	PG_FREE_IF_COPY(b, 0);
	PG_RETURN_POINTER(r);
}

ptree *
lca_inner(ptree **a, int len)
{
	int			tmp,
				num = ((*a)->numlevel) ? (*a)->numlevel - 1 : 0;
	ptree	  **ptr = a + 1;
	int			i,
				reslen = PTREE_HDRSIZE;
	ptree_level *l1,
			   *l2;
	ptree	   *res;


	if ((*a)->numlevel == 0)
		return NULL;

	while (ptr - a < len)
	{
		if ((*ptr)->numlevel == 0)
			return NULL;
		else if ((*ptr)->numlevel == 1)
			num = 0;
		else
		{
			l1 = PTREE_FIRST(*a);
			l2 = PTREE_FIRST(*ptr);
			tmp = num;
			num = 0;
			for (i = 0; i < Min(tmp, (*ptr)->numlevel - 1); i++)
			{
				if (l1->len == l2->len && memcmp(l1->name, l2->name, l1->len) == 0)
					num = i + 1;
				else
					break;
				l1 = LEVEL_NEXT(l1);
				l2 = LEVEL_NEXT(l2);
			}
		}
		ptr++;
	}

	l1 = PTREE_FIRST(*a);
	for (i = 0; i < num; i++)
	{
		reslen += MAXALIGN(l1->len + LEVEL_HDRSIZE);
		l1 = LEVEL_NEXT(l1);
	}

	res = (ptree *) palloc(reslen);
	SET_VARSIZE(res, reslen);
	res->numlevel = num;

	l1 = PTREE_FIRST(*a);
	l2 = PTREE_FIRST(res);

	for (i = 0; i < num; i++)
	{
		memcpy(l2, l1, MAXALIGN(l1->len + LEVEL_HDRSIZE));
		l1 = LEVEL_NEXT(l1);
		l2 = LEVEL_NEXT(l2);
	}

	return res;
}

Datum
lca(PG_FUNCTION_ARGS)
{
	int			i;
	ptree	  **a,
			   *res;

	a = (ptree **) palloc(sizeof(ptree *) * fcinfo->nargs);
	for (i = 0; i < fcinfo->nargs; i++)
		a[i] = PG_GETARG_PTREE(i);
	res = lca_inner(a, (int) fcinfo->nargs);
	for (i = 0; i < fcinfo->nargs; i++)
		PG_FREE_IF_COPY(a[i], i);
	pfree(a);

	if (res)
		PG_RETURN_POINTER(res);
	else
		PG_RETURN_NULL();
}

Datum
text2ptree(PG_FUNCTION_ARGS)
{
	text	   *in = PG_GETARG_TEXT_PP(0);
	char	   *s;
	ptree	   *out;

	s = text_to_cstring(in);

	out = (ptree *) DatumGetPointer(DirectFunctionCall1(ptree_in,
														PointerGetDatum(s)));
	pfree(s);
	PG_FREE_IF_COPY(in, 0);
	PG_RETURN_POINTER(out);
}


Datum
ptree2text(PG_FUNCTION_ARGS)
{
	ptree	   *in = PG_GETARG_PTREE(0);
	char	   *ptr;
	int			i;
	ptree_level *curlevel;
	text	   *out;

	out = (text *) palloc(VARSIZE(in) + VARHDRSZ);
	ptr = VARDATA(out);
	curlevel = PTREE_FIRST(in);
	for (i = 0; i < in->numlevel; i++)
	{
		if (i != 0)
		{
			*ptr = '.';
			ptr++;
		}
		memcpy(ptr, curlevel->name, curlevel->len);
		ptr += curlevel->len;
		curlevel = LEVEL_NEXT(curlevel);
	}

	SET_VARSIZE(out, ptr - ((char *) out));
	PG_FREE_IF_COPY(in, 0);

	PG_RETURN_POINTER(out);
}


#define DEFAULT_PARENT_SEL 0.001

/*
 *	ptreeparentsel - Selectivity of parent relationship for ptree data types.
 */
Datum
ptreeparentsel(PG_FUNCTION_ARGS)
{
	PlannerInfo *root = (PlannerInfo *) PG_GETARG_POINTER(0);
	Oid			operator = PG_GETARG_OID(1);
	List	   *args = (List *) PG_GETARG_POINTER(2);
	int			varRelid = PG_GETARG_INT32(3);
	VariableStatData vardata;
	Node	   *other;
	bool		varonleft;
	double		selec;

	/*
	 * If expression is not variable <@ something or something <@ variable,
	 * then punt and return a default estimate.
	 */
	if (!get_restriction_variable(root, args, varRelid,
								  &vardata, &other, &varonleft))
		PG_RETURN_FLOAT8(DEFAULT_PARENT_SEL);

	/*
	 * If the something is a NULL constant, assume operator is strict and
	 * return zero, ie, operator will never return TRUE.
	 */
	if (IsA(other, Const) &&
		((Const *) other)->constisnull)
	{
		ReleaseVariableStats(vardata);
		PG_RETURN_FLOAT8(0.0);
	}

	if (IsA(other, Const))
	{
		/* Variable is being compared to a known non-null constant */
		Datum		constval = ((Const *) other)->constvalue;
		FmgrInfo	contproc;
		double		mcvsum;
		double		mcvsel;
		double		nullfrac;
		int			hist_size;

		fmgr_info(get_opcode(operator), &contproc);

		/*
		 * Is the constant "<@" to any of the column's most common values?
		 */
		mcvsel = mcv_selectivity(&vardata, &contproc, constval, varonleft,
								 &mcvsum);

		/*
		 * If the histogram is large enough, see what fraction of it the
		 * constant is "<@" to, and assume that's representative of the
		 * non-MCV population.	Otherwise use the default selectivity for the
		 * non-MCV population.
		 */
		selec = histogram_selectivity(&vardata, &contproc,
									  constval, varonleft,
									  10, 1, &hist_size);
		if (selec < 0)
		{
			/* Nope, fall back on default */
			selec = DEFAULT_PARENT_SEL;
		}
		else if (hist_size < 100)
		{
			/*
			 * For histogram sizes from 10 to 100, we combine the histogram
			 * and default selectivities, putting increasingly more trust in
			 * the histogram for larger sizes.
			 */
			double		hist_weight = hist_size / 100.0;

			selec = selec * hist_weight +
				DEFAULT_PARENT_SEL * (1.0 - hist_weight);
		}

		/* In any case, don't believe extremely small or large estimates. */
		if (selec < 0.0001)
			selec = 0.0001;
		else if (selec > 0.9999)
			selec = 0.9999;

		if (HeapTupleIsValid(vardata.statsTuple))
			nullfrac = ((Form_pg_statistic) GETSTRUCT(vardata.statsTuple))->stanullfrac;
		else
			nullfrac = 0.0;

		/*
		 * Now merge the results from the MCV and histogram calculations,
		 * realizing that the histogram covers only the non-null values that
		 * are not listed in MCV.
		 */
		selec *= 1.0 - nullfrac - mcvsum;
		selec += mcvsel;
	}
	else
		selec = DEFAULT_PARENT_SEL;

	ReleaseVariableStats(vardata);

	/* result should be in range, but make sure... */
	CLAMP_PROBABILITY(selec);

	PG_RETURN_FLOAT8((float8) selec);
}
