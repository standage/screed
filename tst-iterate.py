import sys
from pygr.seqdb import SequenceFileDB
db = SequenceFileDB(sys.argv[1])

s = set()
for k in db:
    v = db[k]
    s.add(v.seq)
