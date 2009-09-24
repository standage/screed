import sys
import randsel
from screed_pygr_api import ScreedSequenceDB as SequenceDB
db = SequenceDB('mda.fa')

from cPickle import load
(randsel._length, randsel._keys) = load(open('keys-ri2-mda.pickle'))

randsel.init(db)

s = set()
for i in range(0, 200000):
    v = randsel.retrieve_random(db)
    s.add(v.seq)
