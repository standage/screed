import sys
import randsel
from pygr.seqdb import SequenceFileDB
db = SequenceFileDB('mda.fa')

from cPickle import load
(randsel._length, randsel._keys) = load(open('keys-ri-mda.pickle'))

randsel.init(db)

s = set()
for i in range(0, 200000):
    v = randsel.retrieve_random(db)
    s.add(v.seq)
