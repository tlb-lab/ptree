CREATE EXTENSION ptree;

SELECT ''::ptree;
SELECT '1'::ptree;
SELECT '1/2'::ptree;
SELECT '1/2/_3'::ptree;

SELECT ptree2text('1/2/3/34/sdf');
SELECT text2ptree('1/2/3/34/sdf');

SELECT subptree('Top/Child1/Child2',1,2);
SELECT subpath('Top/Child1/Child2',1,2);
SELECT subpath('Top/Child1/Child2',-1,1);
SELECT subpath('Top/Child1/Child2',0,-2);
SELECT subpath('Top/Child1/Child2',0,-1);
SELECT subpath('Top/Child1/Child2',0,0);
SELECT subpath('Top/Child1/Child2',1,0);
SELECT subpath('Top/Child1/Child2',0);
SELECT subpath('Top/Child1/Child2',1);


SELECT index('1/2/3/4/5/6','1/2');
SELECT index('a/1/2/3/4/5/6','1/2');
SELECT index('a/1/2/3/4/5/6','1/2/3');
SELECT index('a/1/2/3/4/5/6','1/2/3/j');
SELECT index('a/1/2/3/4/5/6','1/2/3/j/4/5/5/5/5/5/5');
SELECT index('a/1/2/3/4/5/6','1/2/3');
SELECT index('a/1/2/3/4/5/6','6');
SELECT index('a/1/2/3/4/5/6','6/1');
SELECT index('a/1/2/3/4/5/6','5/6');
SELECT index('0/1/2/3/5/4/5/6','5/6');
SELECT index('0/1/2/3/5/4/5/6/8/5/6/8','5/6',3);
SELECT index('0/1/2/3/5/4/5/6/8/5/6/8','5/6',6);
SELECT index('0/1/2/3/5/4/5/6/8/5/6/8','5/6',7);
SELECT index('0/1/2/3/5/4/5/6/8/5/6/8','5/6',-7);
SELECT index('0/1/2/3/5/4/5/6/8/5/6/8','5/6',-4);
SELECT index('0/1/2/3/5/4/5/6/8/5/6/8','5/6',-3);
SELECT index('0/1/2/3/5/4/5/6/8/5/6/8','5/6',-2);
SELECT index('0/1/2/3/5/4/5/6/8/5/6/8','5/6',-20000);


SELECT 'Top/Child1/Child2'::ptree || 'Child3'::text;
SELECT 'Top/Child1/Child2'::ptree || 'Child3'::ptree;
SELECT 'Top_0'::ptree || 'Top/Child1/Child2'::ptree;
SELECT 'Top/Child1/Child2'::ptree || ''::ptree;
SELECT ''::ptree || 'Top/Child1/Child2'::ptree;

SELECT lca('{la/2/3,1/2/3/4/5/6,""}') IS NULL;
SELECT lca('{la/2/3,1/2/3/4/5/6}') IS NULL;
SELECT lca('{1/la/2/3,1/2/3/4/5/6}');
SELECT lca('{1/2/3,1/2/3/4/5/6}');
SELECT lca('1/la/2/3','1/2/3/4/5/6');
SELECT lca('1/2/3','1/2/3/4/5/6');
SELECT lca('1/2/2/3','1/2/3/4/5/6');
SELECT lca('1/2/2/3','1/2/3/4/5/6','');
SELECT lca('1/2/2/3','1/2/3/4/5/6','2');
SELECT lca('1/2/2/3','1/2/3/4/5/6','1');


SELECT '1'::pquery;
SELECT '4|3|2'::pquery;
SELECT '1/2'::pquery;
SELECT '1/4|3|2'::pquery;
SELECT '1/0'::pquery;
SELECT '4|3|2/0'::pquery;
SELECT '1/2/0'::pquery;
SELECT '1/4|3|2/0'::pquery;
SELECT '1/*'::pquery;
SELECT '4|3|2/*'::pquery;
SELECT '1/2/*'::pquery;
SELECT '1/4|3|2/*'::pquery;
SELECT '*/1/*'::pquery;
SELECT '*/4|3|2/*'::pquery;
SELECT '*/1/2/*'::pquery;
SELECT '*/1/4|3|2/*'::pquery;
SELECT '1/*/4|3|2'::pquery;
SELECT '1/*/4|3|2/0'::pquery;
SELECT '1/*/4|3|2/*{1,4}'::pquery;
SELECT '1/*/4|3|2/*{,4}'::pquery;
SELECT '1/*/4|3|2/*{1,}'::pquery;
SELECT '1/*/4|3|2/*{1}'::pquery;
SELECT 'qwerty%@*/tu'::pquery;

SELECT nlevel('1/2/3/4');
SELECT '1/2'::ptree  < '2/2'::ptree;
SELECT '1/2'::ptree  <= '2/2'::ptree;
SELECT '2/2'::ptree  = '2/2'::ptree;
SELECT '3/2'::ptree  >= '2/2'::ptree;
SELECT '3/2'::ptree  > '2/2'::ptree;

SELECT '1/2/3'::ptree @> '1/2/3/4'::ptree;
SELECT '1/2/3/4'::ptree @> '1/2/3/4'::ptree;
SELECT '1/2/3/4/5'::ptree @> '1/2/3/4'::ptree;
SELECT '1/3/3'::ptree @> '1/2/3/4'::ptree;

SELECT 'a/b/c/d/e'::ptree ~ 'a/b/c/d/e';
SELECT 'a/b/c/d/e'::ptree ~ 'A/b/c/d/e';
SELECT 'a/b/c/d/e'::ptree ~ 'A@/b/c/d/e';
SELECT 'aa/b/c/d/e'::ptree ~ 'A@/b/c/d/e';
SELECT 'aa/b/c/d/e'::ptree ~ 'A*/b/c/d/e';
SELECT 'aa/b/c/d/e'::ptree ~ 'A*@/b/c/d/e';
SELECT 'aa/b/c/d/e'::ptree ~ 'A*@|g/b/c/d/e';
SELECT 'g/b/c/d/e'::ptree ~ 'A*@|g/b/c/d/e';
SELECT 'a/b/c/d/e'::ptree ~ 'a/b/c/d/e';
SELECT 'a/b/c/d/e'::ptree ~ 'a/*/e';
SELECT 'a/b/c/d/e'::ptree ~ 'a/*{3}/e';
SELECT 'a/b/c/d/e'::ptree ~ 'a/*{2}/e';
SELECT 'a/b/c/d/e'::ptree ~ 'a/*{4}/e';
SELECT 'a/b/c/d/e'::ptree ~ 'a/*{,4}/e';
SELECT 'a/b/c/d/e'::ptree ~ 'a/*{2,}/e';
SELECT 'a/b/c/d/e'::ptree ~ 'a/*{2,4}/e';
SELECT 'a/b/c/d/e'::ptree ~ 'a/*{2,3}/e';
SELECT 'a/b/c/d/e'::ptree ~ 'a/*{2,3}';
SELECT 'a/b/c/d/e'::ptree ~ 'a/*{2,4}';
SELECT 'a/b/c/d/e'::ptree ~ 'a/*{2,5}';
SELECT 'a/b/c/d/e'::ptree ~ '*{2,3}/e';
SELECT 'a/b/c/d/e'::ptree ~ '*{2,4}/e';
SELECT 'a/b/c/d/e'::ptree ~ '*{2,5}/e';
SELECT 'a/b/c/d/e'::ptree ~ '*/e';
SELECT 'a/b/c/d/e'::ptree ~ '*/e/*';
SELECT 'a/b/c/d/e'::ptree ~ '*/d/*';
SELECT 'a/b/c/d/e'::ptree ~ '*/a/*/d/*';
SELECT 'a/b/c/d/e'::ptree ~ '*/!d/*';
SELECT 'a/b/c/d/e'::ptree ~ '*/!d';
SELECT 'a/b/c/d/e'::ptree ~ '!d/*';
SELECT 'a/b/c/d/e'::ptree ~ '!a/*';
SELECT 'a/b/c/d/e'::ptree ~ '*/!e';
SELECT 'a/b/c/d/e'::ptree ~ '*/!e/*';
SELECT 'a/b/c/d/e'::ptree ~ 'a/*/!e';
SELECT 'a/b/c/d/e'::ptree ~ 'a/*/!d';
SELECT 'a/b/c/d/e'::ptree ~ 'a/*/!d/*';
SELECT 'a/b/c/d/e'::ptree ~ 'a/*/!f/*';
SELECT 'a/b/c/d/e'::ptree ~ '*/a/*/!f/*';
SELECT 'a/b/c/d/e'::ptree ~ '*/a/*/!d/*';
SELECT 'a/b/c/d/e'::ptree ~ '*/a/!d/*';
SELECT 'a/b/c/d/e'::ptree ~ '*/a/!d';
SELECT 'a/b/c/d/e'::ptree ~ 'a/!d/*';
SELECT 'a/b/c/d/e'::ptree ~ '*/a/*/!d/*';
SELECT 'a/b/c/d/e'::ptree ~ '*/!b/*';
SELECT 'a/b/c/d/e'::ptree ~ '*/!b/c/*';
SELECT 'a/b/c/d/e'::ptree ~ '*/!b/*/c/*';
SELECT 'a/b/c/d/e'::ptree ~ '!b/*/c/*';
SELECT 'a/b/c/d/e'::ptree ~ '!b/b/*';
SELECT 'a/b/c/d/e'::ptree ~ '!b/*/e';
SELECT 'a/b/c/d/e'::ptree ~ '!b/!c/*/e';
SELECT 'a/b/c/d/e'::ptree ~ '!b/*/!c/*/e';
SELECT 'a/b/c/d/e'::ptree ~ '*{2}/!b/*/!c/*/e';
SELECT 'a/b/c/d/e'::ptree ~ '*{1}/!b/*/!c/*/e';
SELECT 'a/b/c/d/e'::ptree ~ '*{1}/!b/*{1}/!c/*/e';
SELECT 'a/b/c/d/e'::ptree ~ 'a/!b/*{1}/!c/*/e';
SELECT 'a/b/c/d/e'::ptree ~ '!b/*{1}/!c/*/e';
SELECT 'a/b/c/d/e'::ptree ~ '*/!b/*{1}/!c/*/e';
SELECT 'a/b/c/d/e'::ptree ~ '*/!b/*/!c/*/e';
SELECT 'a/b/c/d/e'::ptree ~ '!b/!c/*';
SELECT 'a/b/c/d/e'::ptree ~ '!b/*/!c/*';
SELECT 'a/b/c/d/e'::ptree ~ '*{2}/!b/*/!c/*';
SELECT 'a/b/c/d/e'::ptree ~ '*{1}/!b/*/!c/*';
SELECT 'a/b/c/d/e'::ptree ~ '*{1}/!b/*{1}/!c/*';
SELECT 'a/b/c/d/e'::ptree ~ 'a/!b/*{1}/!c/*';
SELECT 'a/b/c/d/e'::ptree ~ '!b/*{1}/!c/*';
SELECT 'a/b/c/d/e'::ptree ~ '*/!b/*{1}/!c/*';
SELECT 'a/b/c/d/e'::ptree ~ '*/!b/*/!c/*';


SELECT 'QWER_TY'::ptree ~ 'q%@*';
SELECT 'QWER_TY'::ptree ~ 'Q_t%@*';
SELECT 'QWER_GY'::ptree ~ 'q_t%@*';

--ptxtquery
SELECT '!tree & aWdf@*'::ptxtquery;
SELECT 'tree & aw_qw%*'::ptxtquery;
SELECT 'ptree/awdfg'::ptree @ '!tree & aWdf@*'::ptxtquery;
SELECT 'tree/awdfg'::ptree @ '!tree & aWdf@*'::ptxtquery;
SELECT 'tree/awdfg'::ptree @ '!tree | aWdf@*'::ptxtquery;
SELECT 'tree/awdfg'::ptree @ 'tree | aWdf@*'::ptxtquery;
SELECT 'tree/awdfg'::ptree @ 'tree & aWdf@*'::ptxtquery;
SELECT 'tree/awdfg'::ptree @ 'tree & aWdf@'::ptxtquery;
SELECT 'tree/awdfg'::ptree @ 'tree & aWdf*'::ptxtquery;
SELECT 'tree/awdfg'::ptree @ 'tree & aWdf'::ptxtquery;
SELECT 'tree/awdfg'::ptree @ 'tree & awdf*'::ptxtquery;
SELECT 'tree/awdfg'::ptree @ 'tree & aWdfg@'::ptxtquery;
SELECT 'tree/awdfg_qwerty'::ptree @ 'tree & aw_qw%*'::ptxtquery;
SELECT 'tree/awdfg_qwerty'::ptree @ 'tree & aw_rw%*'::ptxtquery;

--arrays

SELECT '{1/2/3}'::ptree[] @> '1/2/3/4';
SELECT '{1/2/3/4}'::ptree[] @> '1/2/3/4';
SELECT '{1/2/3/4/5}'::ptree[] @> '1/2/3/4';
SELECT '{1/3/3}'::ptree[] @> '1/2/3/4';
SELECT '{5/67/8, 1/2/3}'::ptree[] @> '1/2/3/4';
SELECT '{5/67/8, 1/2/3/4}'::ptree[] @> '1/2/3/4';
SELECT '{5/67/8, 1/2/3/4/5}'::ptree[] @> '1/2/3/4';
SELECT '{5/67/8, 1/3/3}'::ptree[] @> '1/2/3/4';
SELECT '{1/2/3, 7/12/asd}'::ptree[] @> '1/2/3/4';
SELECT '{1/2/3/4, 7/12/asd}'::ptree[] @> '1/2/3/4';
SELECT '{1/2/3/4/5, 7/12/asd}'::ptree[] @> '1/2/3/4';
SELECT '{1/3/3, 7/12/asd}'::ptree[] @> '1/2/3/4';
SELECT '{ptree/asd, tree/awdfg}'::ptree[] @ 'tree & aWdfg@'::ptxtquery;
SELECT '{j/k/l/m, g/b/c/d/e}'::ptree[] ~ 'A*@|g/b/c/d/e';
SELECT 'a/b/c/d/e'::ptree ? '{A/b/c/d/e}';
SELECT 'a/b/c/d/e'::ptree ? '{a/b/c/d/e}';
SELECT 'a/b/c/d/e'::ptree ? '{A/b/c/d/e, a/*}';
SELECT '{a/b/c/d/e,B/df}'::ptree[] ? '{A/b/c/d/e}';
SELECT '{a/b/c/d/e,B/df}'::ptree[] ? '{A/b/c/d/e,*/df}';

--exractors
SELECT ('{3456,1/2/3/34}'::ptree[] ?@> '1/2/3/4') is null;
SELECT '{3456,1/2/3}'::ptree[] ?@> '1/2/3/4';
SELECT '{3456,1/2/3/4}'::ptree[] ?<@ '1/2/3';
SELECT ('{3456,1/2/3/4}'::ptree[] ?<@ '1/2/5') is null;
SELECT '{ptree/asd, tree/awdfg}'::ptree[] ?@ 'tree & aWdfg@'::ptxtquery;
SELECT '{j/k/l/m, g/b/c/d/e}'::ptree[] ?~ 'A*@|g/b/c/d/e';

CREATE TABLE ptreetest (t ptree);
\copy ptreetest FROM 'data/ptree/data'

SELECT * FROM ptreetest WHERE t <  '12/3' order by t asc;
SELECT * FROM ptreetest WHERE t <= '12/3' order by t asc;
SELECT * FROM ptreetest WHERE t =  '12/3' order by t asc;
SELECT * FROM ptreetest WHERE t >= '12/3' order by t asc;
SELECT * FROM ptreetest WHERE t >  '12/3' order by t asc;
SELECT * FROM ptreetest WHERE t @> '1/1/1' order by t asc;
SELECT * FROM ptreetest WHERE t <@ '1/1/1' order by t asc;
SELECT * FROM ptreetest WHERE t @ '23 & 1' order by t asc;
SELECT * FROM ptreetest WHERE t ~ '1/1/1/*' order by t asc;
SELECT * FROM ptreetest WHERE t ~ '*/1' order by t asc;
SELECT * FROM ptreetest WHERE t ~ '23/*{1}/1' order by t asc;
SELECT * FROM ptreetest WHERE t ~ '23/*/1' order by t asc;
SELECT * FROM ptreetest WHERE t ~ '23/*/2' order by t asc;
SELECT * FROM ptreetest WHERE t ? '{23/*/1,23/*/2}' order by t asc;

create unique index tstidx on ptreetest (t);
set enable_seqscan=off;

SELECT * FROM ptreetest WHERE t <  '12/3' order by t asc;
SELECT * FROM ptreetest WHERE t <= '12/3' order by t asc;
SELECT * FROM ptreetest WHERE t =  '12/3' order by t asc;
SELECT * FROM ptreetest WHERE t >= '12/3' order by t asc;
SELECT * FROM ptreetest WHERE t >  '12/3' order by t asc;

drop index tstidx;
create index tstidx on ptreetest using gist (t);
set enable_seqscan=off;

SELECT * FROM ptreetest WHERE t <  '12/3' order by t asc;
SELECT * FROM ptreetest WHERE t <= '12/3' order by t asc;
SELECT * FROM ptreetest WHERE t =  '12/3' order by t asc;
SELECT * FROM ptreetest WHERE t >= '12/3' order by t asc;
SELECT * FROM ptreetest WHERE t >  '12/3' order by t asc;
SELECT * FROM ptreetest WHERE t @> '1/1/1' order by t asc;
SELECT * FROM ptreetest WHERE t <@ '1/1/1' order by t asc;
SELECT * FROM ptreetest WHERE t @ '23 & 1' order by t asc;
SELECT * FROM ptreetest WHERE t ~ '1/1/1/*' order by t asc;
SELECT * FROM ptreetest WHERE t ~ '*/1' order by t asc;
SELECT * FROM ptreetest WHERE t ~ '23/*{1}/1' order by t asc;
SELECT * FROM ptreetest WHERE t ~ '23/*/1' order by t asc;
SELECT * FROM ptreetest WHERE t ~ '23/*/2' order by t asc;
SELECT * FROM ptreetest WHERE t ? '{23/*/1,23/*/2}' order by t asc;

create table _ptreetest (t ptree[]);
\copy _ptreetest FROM 'data/_ptree.data'

SELECT count(*) FROM _ptreetest WHERE t @> '1/1/1' ;
SELECT count(*) FROM _ptreetest WHERE t <@ '1/1/1' ;
SELECT count(*) FROM _ptreetest WHERE t @ '23 & 1' ;
SELECT count(*) FROM _ptreetest WHERE t ~ '1/1/1/*' ;
SELECT count(*) FROM _ptreetest WHERE t ~ '*/1' ;
SELECT count(*) FROM _ptreetest WHERE t ~ '23/*{1}/1' ;
SELECT count(*) FROM _ptreetest WHERE t ~ '23/*/1' ;
SELECT count(*) FROM _ptreetest WHERE t ~ '23/*/2' ;
SELECT count(*) FROM _ptreetest WHERE t ? '{23/*/1,23/*/2}' ;

create index _tstidx on _ptreetest using gist (t);
set enable_seqscan=off;

SELECT count(*) FROM _ptreetest WHERE t @> '1/1/1' ;
SELECT count(*) FROM _ptreetest WHERE t <@ '1/1/1' ;
SELECT count(*) FROM _ptreetest WHERE t @ '23 & 1' ;
SELECT count(*) FROM _ptreetest WHERE t ~ '1/1/1/*' ;
SELECT count(*) FROM _ptreetest WHERE t ~ '*/1' ;
SELECT count(*) FROM _ptreetest WHERE t ~ '23/*{1}/1' ;
SELECT count(*) FROM _ptreetest WHERE t ~ '23/*/1' ;
SELECT count(*) FROM _ptreetest WHERE t ~ '23/*/2' ;
SELECT count(*) FROM _ptreetest WHERE t ? '{23/*/1,23/*/2}' ;
