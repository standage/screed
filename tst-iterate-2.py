import sys
from screed_pygr_api import ScreedSequenceDB as SequenceDB
db = SequenceDB(sys.argv[1])

s = set()
for k in db:
    v = db[k]
    s.add(v.seq)
