"""
A simple wrapper implementing a pygr-compatible SequenceDB based on screed.

There are two implementions:
 - ScreedSequenceDB
 - ScreedSequenceDB_ByIndex

ScreedSequenceDB uses the sequence name as the sequence ID, which
mimics the behavior of pygr's SequenceFileDB and is good for
small-to-medium sized collections of sequences.
ScreedSequenceDB_ByIndex uses the sequence's index (0...size of
database) as a sequence ID, rather than the sequence name; this is
much faster for databases with many, many sequences.

Unlike the normal seqdb, screed will load the entire sequence record
into memory on request, so it's not good for large sequences.

All screed records are guaranteed to have a name and a sequence; anything
else is specific to the database writer you use.  The raw screed record
is available under seq.record.

Note: the underlying screed database must already have been built with
fadbm or fqdbm.

CTB 3/15/09
"""

import UserDict

import screed

from pygr.sequence import SequenceBase
from pygr.seqdb import SequenceDB
from pygr.sequtil import DNA_SEQTYPE

###

class _ScreedSequence(SequenceBase):
    """Sequence implementation based on screed; stores screed record info."""
    def _init_subclass(cls, db, **kwargs):
        cls.db = db
    _init_subclass = classmethod(_init_subclass)

    def __init__(self, db, id):
        self.id = id
        SequenceBase.__init__(self)
        info = db.seqInfoDict[id]
        
        self.record = info.record
        self.name = info.record.name
        self.seq = info.seq

class ScreedSequenceDB(SequenceDB):
    """SequenceDB implementation based on screed; retrieve seqs by name."""
    itemClass = _ScreedSequence
    
    def __init__(self, filename):
        SequenceDB.__init__(self)
        self.seqInfoDict = _ScreedSeqInfoDict_ByName(filename)
        
    def set_seqtype(self):
        self._seqtype = DNA_SEQTYPE

class ScreedSequenceDB_ByIndex(SequenceDB):
    """SequenceDB implementation based on screed; retrieve seqs by index."""
    itemClass = _ScreedSequence
    
    def __init__(self, filename):
        SequenceDB.__init__(self)
        self.seqInfoDict = _ScreedSeqInfoDict_ByIndex(filename)
        
    def set_seqtype(self):
        self._seqtype = DNA_SEQTYPE

class _ScreedSequenceInfo(object):
    """Objects to put in seqInfoDict values, for holding screed record info."""
    def __init__(self, id, record):
        self.id = id
        self.record = record
        self.seq = record.sequence
        self.length = len(self.seq)

class _ScreedSeqInfoDict_ByName(object, UserDict.DictMixin):
    """seqInfoDict implementation that uses names to retrieve records."""
    def __init__(self, filename):
        self.sdb = screed.dbread(filename)

    def __getitem__(self, k):
        v = self.sdb[k]
        return _ScreedSequenceInfo(k, v)

    def keys(self):
        return self.sdb.keys()

class _ScreedSeqInfoDict_ByIndex(object, UserDict.DictMixin):
    """seqInfoDict implementation that uses indices to retrieve records."""
    def __init__(self, filename):
        self.sdb = screed.dbread(filename)

    def __getitem__(self, k):
        n = int(k)
        v = self.sdb.loadRecordByIndex(n)
        return _ScreedSequenceInfo(k, v)

    def keys(self):
        return xrange(0, len(self.sdb))

    def iterkeys(self):
        i = 0
        max_index = len(self.sdb)
        while i < max_index:
            yield i
            i += 1

###

if __name__ == '__main__':
    import sys
    filename = sys.argv[1]

    db = ScreedSequenceDB(filename)
    for k in db:
        print k, repr(db[k]), db[k].name

    db = ScreedSequenceDB_ByIndex(filename)
    for k in db:
        print k, repr(db[k]), db[k].name
