import UserDict

cdef extern from "dbread.h":
    ctypedef int (*fail)()
    ctypedef int (*is_open)()
    ctypedef char * (*theError)()
    ctypedef char * (*getType)(char*)
    ctypedef char * (*getTypeByIndex)(unsigned)
    ctypedef void (*getRecord)(long long)
    ctypedef void (*clear)()
    ctypedef long long (*getSize)()
    ctypedef unsigned (*getTypesize)()
    ctypedef char * (*getTypekey)(unsigned)
    ctypedef void (*getHashRecord)(char*)
    ctypedef void (*close)()
    ctypedef struct c_dbread "dbread":
        is_open is_open
        fail fail
        theError theError
        getType getType
        getTypeByIndex getTypeByIndex
        getRecord getRecord
        clear clear
        getSize getSize
        getTypesize getTypesize
        getTypekey getTypekey
        getHashRecord getHashRecord
        close close

    c_dbread *new_dbread "new dbread" (char * filename)
    void del_dbread "delete" (c_dbread *db)

class DbException(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

class _dbread_record(UserDict.DictMixin):
    """
    Simple dict-like record interface with bag behavior.
    """
    def __init__(self, *args, **kwargs):
        self.d = dict(*args, **kwargs)
        
    def __getitem__(self, name):
        return self.d[name]
    
    def __getattr__(self, name):
        try:
            return self.d[name]
        except KeyError:
            raise AttributeError, name

    def keys(self):
        return self.d.keys()

cdef class _dbread_record_iter:
    """
    Iterator over dbread database, returning records.
    """
    cdef int i, n
    cdef object db
    
    def __init__(self, db):
        self.db = db
        self.i = 0
        self.n = len(db)

    def __iter__(self):
        return self
        
    def __next__(self):
        if self.i >= self.n:
            raise StopIteration
        
        record = self.db.loadRecordByIndex(self.i)
        self.i = self.i + 1

        return record

cdef class _dbread_name_iter(_dbread_record_iter):
    """
    Iterator over dbread database, returning keys.
    """
    def __next__(self):
        record = _dbread_record_iter.next(self)
        return record.name

cdef class _dbread_name_record_iter(_dbread_record_iter):
    """
    Iterator over dbread database, returning items.
    """
    def __next__(self):
        record = _dbread_record_iter.next(self)
        return record.name, record

cdef class dbread:
    cdef c_dbread *thisptr              # wrapped C++ instance
    cdef readonly list fields           # list of fields in each entry

    def __cinit__(self, filename):
        cdef int n_fields
        self.thisptr = new_dbread(filename)
        if self.thisptr.is_open() == 0:
            raise DbException(self.thisptr.theError())

        n_fields = self.thisptr.getTypesize()
        fields = list()

        self.thisptr.getRecord(0)
        
        for i in range(n_fields):
            name = self.thisptr.getTypekey(i)
            fields.append(name)
        self.fields = fields

    def __iter__(self):
        self.checkopen()
        return _dbread_name_iter(self)

    def __dealloc__(self):
        del_dbread(self.thisptr)
        self.thisptr = NULL

    def _buildRecord(self):
        self.checkopen()
        x = []
        for i, name in enumerate(self.fields):
            value = self.thisptr.getTypeByIndex(i)
            #print i, name, value
            x.append((name, value))

        return _dbread_record(x)

    def loadRecordByIndex(self, idx):
        self.checkopen()
        self.thisptr.getRecord(idx)
        if self.thisptr.fail() == 1:    # IndexError? @CTB
            self.thisptr.clear()
            raise DbException(self.thisptr.theError())

        return self._buildRecord()

    def loadRecordByName(self, name):
        self.checkopen()
        self.thisptr.getHashRecord(name)
        if self.thisptr.fail() == 1:
            self.thisptr.clear()
            raise DbException(self.thisptr.theError())

        return self._buildRecord()

    def close(self):
        self.checkopen()
        self.thisptr.close()

    def __getitem__(self, key):
        self.checkopen()
        try:
            return self.loadRecordByName(key)
        except DbException:
            raise KeyError
    
    def __len__(self):
        """
        Return number of entries in database.
        """
        self.checkopen()
        return self.thisptr.getSize()

    def clearErrorFlag(self):
        self.checkopen()
        self.thisptr.clear()

    def keys(self):
        self.checkopen()
        return list(self.iterkeys())

    def values(self):
        self.checkopen()
        return list(self.itervalues())

    def items(self):
        self.checkopen()
        return list(self.iteritems())

    def iterkeys(self):
        self.checkopen()
        return _dbread_name_iter(self)

    def itervalues(self):
        self.checkopen()
        return _dbread_record_iter(self)

    def iteritems(self):
        self.checkopen()
        return _dbread_name_record_iter(self)

    def __contains__(self, name):
        self.checkopen()
        if self.has_key(name):
            return True
        return False

    def has_key(self, key):
        self.checkopen()
        try:
            if self.loadRecordByName(key):
                return True
        except DbException:
            pass
        
        return False

    def get(self, key, default=None):
        self.checkopen()
        try:
            return self.loadRecordByName(key)
        except DbException:
            return default

    def copy(self):
        self.checkopen()
        return self

    def checkopen(self):
        if not self.thisptr.is_open():
            raise DbException("Database Closed")

    # CTB: no clear(), setdefault(), , pop(), popitem(), copy(), and update();
    # this is a read-only dict interface.
