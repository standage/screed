cdef extern from "dbread.h":
    ctypedef int (*fail)()
    ctypedef int (*is_open)()
    ctypedef char * (*theError)()
    ctypedef char * (*getType)(char*)
    ctypedef void (*getRecord)(long long)
    ctypedef void (*clear)()
    ctypedef long long (*getSize)()
    ctypedef unsigned (*getTypesize)()
    ctypedef char * (*getTypekey)(unsigned)
    ctypedef struct c_dbread "dbread":
        is_open is_open
        fail fail
        theError theError
        getType getType
        getRecord getRecord
        clear clear
        getSize getSize
        getTypesize getTypesize
        getTypekey getTypekey

    c_dbread *new_dbread "new dbread" (char * filename)
    void del_dbread "delete" (c_dbread *db)

class DbException(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

cdef class dbread:
    cdef c_dbread *thisptr              # wrapped C++ instance

    def __cinit__(self, filename):
        self.thisptr = new_dbread(filename)
        if self.thisptr.is_open() == 0:
            raise DbException(self.thisptr.theError())
        
    def __dealloc__(self):
        pass
#        del_dbread(self.thisptr)

    def loadRecord(self, idx):
        self.thisptr.getRecord(idx)
        if self.thisptr.fail() == 1:
            self.thisptr.clear()
            raise DbException(self.thisptr.theError())
    
    def getFieldValue(self, name):
        result = self.thisptr.getType(name)
        if self.thisptr.fail() == 1:
           self.thisptr.clear()
           raise DbException(self.thisptr.theError())
        return result

    def getNumRecords(self):
        result = self.thisptr.getSize()
        if self.thisptr.fail() == 1:
           self.thisptr.clear()
           raise DbException(self.thisptr.theError())
        return result

    def getNumFields(self):
        result = self.thisptr.getTypesize()
        if self.thisptr.fail() == 1:
           self.thisptr.clear()
           raise DbException(self.thisptr.theError())
        return result

    def getFieldName(self, idx):
        result = self.thisptr.getTypekey(idx)
        if self.thisptr.fail() == 1:
           self.thisptr.clear()
           raise DbException(self.thisptr.theError())
        return result

    def clear(self):
        self.thisptr.clear()
        return

class SeqDB2(object):
    """
    Wrapper for Pyrex dbread class
    """

    def __init__(self, dbfilename):
        self.dbfilename = dbfilename
        self._db = dbread(dbfilename)

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
        field_values = []
        for n in self.fields:
            field_values.append(self._db.getFieldValue(n))
#        field_values = [ self._db.getFieldValue(n) for n in self.fields ]
        
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
