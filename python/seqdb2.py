import _seqdb2

class SeqDB2(object):
    """
    Wrapper for _seqdb2 pyx
    """

    def __init__(self, dbfilename):
        self.dbfilename = dbfilename
        self._db = _seqdb2.dbread(dbfilename)

        fields = []
        for k in range(self._db.getNumFields()):
            name = self._db.getFieldName(k)
            fields.append(name)
        self.fields = fields

    def __len__(self):
        return self._db.getNumRecords()

    def __getitem__(self, i):
        if i < 0 or i >= len(self):
            raise IndexError, i

        self._db.loadRecord(i)
        field_values = [ self._db.getFieldValue(n) for n in self.fields ]
        
        return SeqDB2Record(self._db, i, self.fields, field_values)

    def clear(self):
        self._db.clear()
        return
    
class SeqDB2Record(object):
    def __init__(self, _db, _num, fields, values):
        self._db = _db
        self._num = _num
        for field, value in zip(fields, values):
            setattr(self, field, value)
