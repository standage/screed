import _seqdb2
print dir(_seqdb2)
print dir(_seqdb2.dbread)

x = _seqdb2.dbread('../tests/test.fa.seqdb2')
print x.getSize()
