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
    def __init__(self):
        self.value = self.thisptr.theError()
    def __str__(self):
        return repr(self.value)

cdef class dbread:
    cdef c_dbread *thisptr              # wrapped C++ instance

    def __cinit__(self, filename):
        self.thisptr = new_dbread(filename)
        if self.thisptr.is_open() == 0:
            raise DbException
        
    def __dealloc__(self):
        pass
#        del_dbread(self.thisptr)

    def loadRecord(self, idx):
        self.thisptr.getRecord(idx)
        if self.thisptr.fail() == 1:
            self.thisptr.clear()
            raise DbException
    
    def getFieldValue(self, name):
        result = self.thisptr.getType(name)
        if self.thisptr.fail() == 1:
            self.thisptr.clear()
            raise DbException
        return result

    def getNumRecords(self):
        result = self.thisptr.getSize()
        if self.thisptr.fail() == 1:
            self.thisptr.clear()
            raise DbException
        return result

    def getNumFields(self):
        result = self.thisptr.getTypesize()
        if self.thisptr.fail() == 1:
            self.thisptr.clear()
            raise DbException
        return result

    def getFieldName(self, idx):
        result = self.thisptr.getTypekey(idx)
        if self.thisptr.fail() == 1:
            self.thisptr.clear()
            raise DbException
        return result
