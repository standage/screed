import _seqdb2
print dir(_seqdb2)
print dir(_seqdb2.dbread)

x = _seqdb2.dbread('tests/test.fa_seqdb2')
print x.getSize()
