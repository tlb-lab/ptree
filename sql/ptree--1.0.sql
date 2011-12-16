CREATE FUNCTION ptree_in(cstring)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE FUNCTION ptree_out(ptree)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE TYPE ptree (
	INTERNALLENGTH = -1,
	INPUT = ptree_in,
	OUTPUT = ptree_out,
	STORAGE = extended
);


--Compare function for ptree
CREATE FUNCTION ptree_cmp(ptree,ptree)
RETURNS int4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION ptree_lt(ptree,ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION ptree_le(ptree,ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION ptree_eq(ptree,ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION ptree_ge(ptree,ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION ptree_gt(ptree,ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION ptree_ne(ptree,ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;


CREATE OPERATOR < (
        LEFTARG = ptree,
	RIGHTARG = ptree,
	PROCEDURE = ptree_lt,
        COMMUTATOR = '>',
	NEGATOR = '>=',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR <= (
        LEFTARG = ptree,
	RIGHTARG = ptree,
	PROCEDURE = ptree_le,
        COMMUTATOR = '>=',
	NEGATOR = '>',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR >= (
        LEFTARG = ptree,
	RIGHTARG = ptree,
	PROCEDURE = ptree_ge,
        COMMUTATOR = '<=',
	NEGATOR = '<',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR > (
        LEFTARG = ptree,
	RIGHTARG = ptree,
	PROCEDURE = ptree_gt,
        COMMUTATOR = '<',
	NEGATOR = '<=',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR = (
        LEFTARG = ptree,
	RIGHTARG = ptree,
	PROCEDURE = ptree_eq,
        COMMUTATOR = '=',
	NEGATOR = '<>',
        RESTRICT = eqsel,
	JOIN = eqjoinsel,
        SORT1 = '<',
	SORT2 = '<'
);

CREATE OPERATOR <> (
        LEFTARG = ptree,
	RIGHTARG = ptree,
	PROCEDURE = ptree_ne,
        COMMUTATOR = '<>',
	NEGATOR = '=',
        RESTRICT = neqsel,
	JOIN = neqjoinsel
);

--util functions

CREATE FUNCTION subptree(ptree,int4,int4)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION subpath(ptree,int4,int4)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION subpath(ptree,int4)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION index(ptree,ptree)
RETURNS int4
AS 'MODULE_PATHNAME', 'ptree_index'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION index(ptree,ptree,int4)
RETURNS int4
AS 'MODULE_PATHNAME', 'ptree_index'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION nlevel(ptree)
RETURNS int4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION ptree2text(ptree)
RETURNS text
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION text2ptree(text)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION lca(_ptree)
RETURNS ptree
AS 'MODULE_PATHNAME','_lca'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION lca(ptree,ptree)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION lca(ptree,ptree,ptree)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION lca(ptree,ptree,ptree,ptree)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION lca(ptree,ptree,ptree,ptree,ptree)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION lca(ptree,ptree,ptree,ptree,ptree,ptree)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION lca(ptree,ptree,ptree,ptree,ptree,ptree,ptree)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION lca(ptree,ptree,ptree,ptree,ptree,ptree,ptree,ptree)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION ptree_isparent(ptree,ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION ptree_risparent(ptree,ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION ptree_addptree(ptree,ptree)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION ptree_addtext(ptree,text)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION ptree_textadd(text,ptree)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION ptreeparentsel(internal, oid, internal, integer)
RETURNS float8
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR @> (
        LEFTARG = ptree,
	RIGHTARG = ptree,
	PROCEDURE = ptree_isparent,
        COMMUTATOR = '<@',
        RESTRICT = ptreeparentsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ^@> (
        LEFTARG = ptree,
	RIGHTARG = ptree,
	PROCEDURE = ptree_isparent,
        COMMUTATOR = '^<@',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR <@ (
        LEFTARG = ptree,
	RIGHTARG = ptree,
	PROCEDURE = ptree_risparent,
        COMMUTATOR = '@>',
        RESTRICT = ptreeparentsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ^<@ (
        LEFTARG = ptree,
	RIGHTARG = ptree,
	PROCEDURE = ptree_risparent,
        COMMUTATOR = '^@>',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR || (
        LEFTARG = ptree,
	RIGHTARG = ptree,
	PROCEDURE = ptree_addptree
);

CREATE OPERATOR || (
        LEFTARG = ptree,
	RIGHTARG = text,
	PROCEDURE = ptree_addtext
);

CREATE OPERATOR || (
        LEFTARG = text,
	RIGHTARG = ptree,
	PROCEDURE = ptree_textadd
);


-- B-tree support

CREATE OPERATOR CLASS ptree_ops
    DEFAULT FOR TYPE ptree USING btree AS
        OPERATOR        1       < ,
        OPERATOR        2       <= ,
        OPERATOR        3       = ,
        OPERATOR        4       >= ,
        OPERATOR        5       > ,
        FUNCTION        1       ptree_cmp(ptree, ptree);


--pquery type
CREATE FUNCTION pquery_in(cstring)
RETURNS pquery
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE FUNCTION pquery_out(pquery)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE TYPE pquery (
	INTERNALLENGTH = -1,
	INPUT = pquery_in,
	OUTPUT = pquery_out,
	STORAGE = extended
);

CREATE FUNCTION ltq_regex(ptree,pquery)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION ltq_rregex(pquery,ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR ~ (
        LEFTARG = ptree,
	RIGHTARG = pquery,
	PROCEDURE = ltq_regex,
	COMMUTATOR = '~',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ~ (
        LEFTARG = pquery,
	RIGHTARG = ptree,
	PROCEDURE = ltq_rregex,
	COMMUTATOR = '~',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

--not-indexed
CREATE OPERATOR ^~ (
        LEFTARG = ptree,
	RIGHTARG = pquery,
	PROCEDURE = ltq_regex,
	COMMUTATOR = '^~',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ^~ (
        LEFTARG = pquery,
	RIGHTARG = ptree,
	PROCEDURE = ltq_rregex,
	COMMUTATOR = '^~',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE FUNCTION lt_q_regex(ptree,_pquery)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION lt_q_rregex(_pquery,ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR ? (
        LEFTARG = ptree,
	RIGHTARG = _pquery,
	PROCEDURE = lt_q_regex,
	COMMUTATOR = '?',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ? (
        LEFTARG = _pquery,
	RIGHTARG = ptree,
	PROCEDURE = lt_q_rregex,
	COMMUTATOR = '?',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

--not-indexed
CREATE OPERATOR ^? (
        LEFTARG = ptree,
	RIGHTARG = _pquery,
	PROCEDURE = lt_q_regex,
	COMMUTATOR = '^?',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ^? (
        LEFTARG = _pquery,
	RIGHTARG = ptree,
	PROCEDURE = lt_q_rregex,
	COMMUTATOR = '^?',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE FUNCTION ptxtq_in(cstring)
RETURNS ptxtquery
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE FUNCTION ptxtq_out(ptxtquery)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE TYPE ptxtquery (
	INTERNALLENGTH = -1,
	INPUT = ptxtq_in,
	OUTPUT = ptxtq_out,
	STORAGE = extended
);

-- operations WITH ptxtquery

CREATE FUNCTION ptxtq_exec(ptree, ptxtquery)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION ptxtq_rexec(ptxtquery, ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR @ (
        LEFTARG = ptree,
	RIGHTARG = ptxtquery,
	PROCEDURE = ptxtq_exec,
	COMMUTATOR = '@',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR @ (
        LEFTARG = ptxtquery,
	RIGHTARG = ptree,
	PROCEDURE = ptxtq_rexec,
	COMMUTATOR = '@',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

--not-indexed
CREATE OPERATOR ^@ (
        LEFTARG = ptree,
	RIGHTARG = ptxtquery,
	PROCEDURE = ptxtq_exec,
	COMMUTATOR = '^@',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ^@ (
        LEFTARG = ptxtquery,
	RIGHTARG = ptree,
	PROCEDURE = ptxtq_rexec,
	COMMUTATOR = '^@',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

--GiST support for ptree
CREATE FUNCTION ptree_gist_in(cstring)
RETURNS ptree_gist
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE FUNCTION ptree_gist_out(ptree_gist)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE TYPE ptree_gist (
	internallength = -1,
	input = ptree_gist_in,
	output = ptree_gist_out,
	storage = plain
);


CREATE FUNCTION ptree_consistent(internal,internal,int2,oid,internal)
RETURNS bool as 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION ptree_compress(internal)
RETURNS internal as 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION ptree_decompress(internal)
RETURNS internal as 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION ptree_penalty(internal,internal,internal)
RETURNS internal as 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION ptree_picksplit(internal, internal)
RETURNS internal as 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION ptree_union(internal, internal)
RETURNS int4 as 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION ptree_same(internal, internal, internal)
RETURNS internal as 'MODULE_PATHNAME' LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR CLASS gist_ptree_ops
    DEFAULT FOR TYPE ptree USING gist AS
	OPERATOR	1	< ,
	OPERATOR	2	<= ,
	OPERATOR	3	= ,
	OPERATOR	4	>= ,
	OPERATOR	5	> ,
	OPERATOR	10	@> ,
	OPERATOR	11	<@ ,
	OPERATOR	12	~ (ptree, pquery) ,
	OPERATOR	13	~ (pquery, ptree) ,
	OPERATOR	14	@ (ptree, ptxtquery) ,
	OPERATOR	15	@ (ptxtquery, ptree) ,
	OPERATOR	16	? (ptree, _pquery) ,
	OPERATOR	17	? (_pquery, ptree) ,
	FUNCTION	1	ptree_consistent (internal, internal, int2, oid, internal),
	FUNCTION	2	ptree_union (internal, internal),
	FUNCTION	3	ptree_compress (internal),
	FUNCTION	4	ptree_decompress (internal),
	FUNCTION	5	ptree_penalty (internal, internal, internal),
	FUNCTION	6	ptree_picksplit (internal, internal),
	FUNCTION	7	ptree_same (internal, internal, internal),
	STORAGE		ptree_gist;


-- arrays of ptree

CREATE FUNCTION _ptree_isparent(_ptree,ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION _ptree_r_isparent(ptree,_ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION _ptree_risparent(_ptree,ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION _ptree_r_risparent(ptree,_ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION _ltq_regex(_ptree,pquery)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION _ltq_rregex(pquery,_ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION _lt_q_regex(_ptree,_pquery)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION _lt_q_rregex(_pquery,_ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION _ptxtq_exec(_ptree, ptxtquery)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION _ptxtq_rexec(ptxtquery, _ptree)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR @> (
        LEFTARG = _ptree,
	RIGHTARG = ptree,
	PROCEDURE = _ptree_isparent,
        COMMUTATOR = '<@',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR <@ (
        LEFTARG = ptree,
	RIGHTARG = _ptree,
	PROCEDURE = _ptree_r_isparent,
        COMMUTATOR = '@>',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR <@ (
        LEFTARG = _ptree,
	RIGHTARG = ptree,
	PROCEDURE = _ptree_risparent,
        COMMUTATOR = '@>',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR @> (
        LEFTARG = ptree,
	RIGHTARG = _ptree,
	PROCEDURE = _ptree_r_risparent,
        COMMUTATOR = '<@',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ~ (
        LEFTARG = _ptree,
	RIGHTARG = pquery,
	PROCEDURE = _ltq_regex,
        COMMUTATOR = '~',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ~ (
        LEFTARG = pquery,
	RIGHTARG = _ptree,
	PROCEDURE = _ltq_rregex,
        COMMUTATOR = '~',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ? (
        LEFTARG = _ptree,
	RIGHTARG = _pquery,
	PROCEDURE = _lt_q_regex,
        COMMUTATOR = '?',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ? (
        LEFTARG = _pquery,
	RIGHTARG = _ptree,
	PROCEDURE = _lt_q_rregex,
        COMMUTATOR = '?',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR @ (
        LEFTARG = _ptree,
	RIGHTARG = ptxtquery,
	PROCEDURE = _ptxtq_exec,
        COMMUTATOR = '@',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR @ (
        LEFTARG = ptxtquery,
	RIGHTARG = _ptree,
	PROCEDURE = _ptxtq_rexec,
        COMMUTATOR = '@',
        RESTRICT = contsel,
	JOIN = contjoinsel
);


--not indexed
CREATE OPERATOR ^@> (
        LEFTARG = _ptree,
	RIGHTARG = ptree,
	PROCEDURE = _ptree_isparent,
        COMMUTATOR = '^<@',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ^<@ (
        LEFTARG = ptree,
	RIGHTARG = _ptree,
	PROCEDURE = _ptree_r_isparent,
        COMMUTATOR = '^@>',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ^<@ (
        LEFTARG = _ptree,
	RIGHTARG = ptree,
	PROCEDURE = _ptree_risparent,
        COMMUTATOR = '^@>',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ^@> (
        LEFTARG = ptree,
	RIGHTARG = _ptree,
	PROCEDURE = _ptree_r_risparent,
        COMMUTATOR = '^<@',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ^~ (
        LEFTARG = _ptree,
	RIGHTARG = pquery,
	PROCEDURE = _ltq_regex,
        COMMUTATOR = '^~',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ^~ (
        LEFTARG = pquery,
	RIGHTARG = _ptree,
	PROCEDURE = _ltq_rregex,
        COMMUTATOR = '^~',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ^? (
        LEFTARG = _ptree,
	RIGHTARG = _pquery,
	PROCEDURE = _lt_q_regex,
        COMMUTATOR = '^?',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ^? (
        LEFTARG = _pquery,
	RIGHTARG = _ptree,
	PROCEDURE = _lt_q_rregex,
        COMMUTATOR = '^?',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ^@ (
        LEFTARG = _ptree,
	RIGHTARG = ptxtquery,
	PROCEDURE = _ptxtq_exec,
        COMMUTATOR = '^@',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR ^@ (
        LEFTARG = ptxtquery,
	RIGHTARG = _ptree,
	PROCEDURE = _ptxtq_rexec,
        COMMUTATOR = '^@',
        RESTRICT = contsel,
	JOIN = contjoinsel
);

--extractors
CREATE FUNCTION _ptree_extract_isparent(_ptree,ptree)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR ?@> (
        LEFTARG = _ptree,
	RIGHTARG = ptree,
	PROCEDURE = _ptree_extract_isparent
);

CREATE FUNCTION _ptree_extract_risparent(_ptree,ptree)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR ?<@ (
        LEFTARG = _ptree,
	RIGHTARG = ptree,
	PROCEDURE = _ptree_extract_risparent
);

CREATE FUNCTION _ltq_extract_regex(_ptree,pquery)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR ?~ (
        LEFTARG = _ptree,
	RIGHTARG = pquery,
	PROCEDURE = _ltq_extract_regex
);

CREATE FUNCTION _ptxtq_extract_exec(_ptree,ptxtquery)
RETURNS ptree
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR ?@ (
        LEFTARG = _ptree,
	RIGHTARG = ptxtquery,
	PROCEDURE = _ptxtq_extract_exec
);

--GiST support for ptree[]
CREATE FUNCTION _ptree_consistent(internal,internal,int2,oid,internal)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION _ptree_compress(internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION _ptree_penalty(internal,internal,internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION _ptree_picksplit(internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION _ptree_union(internal, internal)
RETURNS int4
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION _ptree_same(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR CLASS gist__ptree_ops
    DEFAULT FOR TYPE _ptree USING gist AS
	OPERATOR	10	<@ (_ptree, ptree),
	OPERATOR	11	@> (ptree, _ptree),
	OPERATOR	12	~ (_ptree, pquery),
	OPERATOR	13	~ (pquery, _ptree),
	OPERATOR	14	@ (_ptree, ptxtquery),
	OPERATOR	15	@ (ptxtquery, _ptree),
	OPERATOR	16	? (_ptree, _pquery),
	OPERATOR	17	? (_pquery, _ptree),
	FUNCTION	1	_ptree_consistent (internal, internal, int2, oid, internal),
	FUNCTION	2	_ptree_union (internal, internal),
	FUNCTION	3	_ptree_compress (internal),
	FUNCTION	4	ptree_decompress (internal),
	FUNCTION	5	_ptree_penalty (internal, internal, internal),
	FUNCTION	6	_ptree_picksplit (internal, internal),
	FUNCTION	7	_ptree_same (internal, internal, internal),
	STORAGE		ptree_gist;
