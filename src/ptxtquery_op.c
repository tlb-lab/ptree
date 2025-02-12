/*
 * txtquery operations with ptree
 * Teodor Sigaev <teodor@stack.net>
 * contrib/ptree/ptxtquery_op.c
 */
#include "postgres.h"

#include <ctype.h>

#include "ptree.h"

PG_FUNCTION_INFO_V1(ptxtq_exec);
PG_FUNCTION_INFO_V1(ptxtq_rexec);

/*
 * check for boolean condition
 */
bool
ptree_execute(ITEM *curitem, void *checkval, bool calcnot, bool (*chkcond) (void *checkval, ITEM *val))
{
	if (curitem->type == VAL)
		return (*chkcond) (checkval, curitem);
	else if (curitem->val == (int4) '!')
	{
		return (calcnot) ?
			((ptree_execute(curitem + 1, checkval, calcnot, chkcond)) ? false : true)
			: true;
	}
	else if (curitem->val == (int4) '&')
	{
		if (ptree_execute(curitem + curitem->left, checkval, calcnot, chkcond))
			return ptree_execute(curitem + 1, checkval, calcnot, chkcond);
		else
			return false;
	}
	else
	{							/* |-operator */
		if (ptree_execute(curitem + curitem->left, checkval, calcnot, chkcond))
			return true;
		else
			return ptree_execute(curitem + 1, checkval, calcnot, chkcond);
	}
	return false;
}

typedef struct
{
	ptree	   *node;
	char	   *operand;
} CHKVAL;

static bool
checkcondition_str(void *checkval, ITEM *val)
{
	ptree_level *level = PTREE_FIRST(((CHKVAL *) checkval)->node);
	int			tlen = ((CHKVAL *) checkval)->node->numlevel;
	char	   *op = ((CHKVAL *) checkval)->operand + val->distance;
	int			(*cmpptr) (const char *, const char *, size_t);

	cmpptr = (val->flag & LVAR_INCASE) ? ptree_strncasecmp : strncmp;
	while (tlen > 0)
	{
		if (val->flag & LVAR_SUBLEXEME)
		{
			if (compare_subnode(level, op, val->length, cmpptr, (val->flag & LVAR_ANYEND)))
				return true;
		}
		else if (
				 (
				  val->length == level->len ||
				  (level->len > val->length && (val->flag & LVAR_ANYEND))
				  ) &&
				 (*cmpptr) (op, level->name, val->length) == 0)
			return true;

		tlen--;
		level = LEVEL_NEXT(level);
	}

	return false;
}

Datum
ptxtq_exec(PG_FUNCTION_ARGS)
{
	ptree	   *val = PG_GETARG_PTREE(0);
	ptxtquery  *query = PG_GETARG_LTXTQUERY(1);
	CHKVAL		chkval;
	bool		result;

	chkval.node = val;
	chkval.operand = GETOPERAND(query);

	result = ptree_execute(
						   GETQUERY(query),
						   &chkval,
						   true,
						   checkcondition_str
		);

	PG_FREE_IF_COPY(val, 0);
	PG_FREE_IF_COPY(query, 1);
	PG_RETURN_BOOL(result);
}

Datum
ptxtq_rexec(PG_FUNCTION_ARGS)
{
	PG_RETURN_DATUM(DirectFunctionCall2(ptxtq_exec,
										PG_GETARG_DATUM(1),
										PG_GETARG_DATUM(0)
										));
}
