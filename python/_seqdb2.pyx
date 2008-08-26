cdef extern from "dbread.h":
    ctypedef char * (*getType)(char*)
    ctypedef void (*getRecord)(long long)
    ctypedef void (*clear)()
    ctypedef long long (*getSize)()
    ctypedef unsigned (*getTypesize)()
    ctypedef char * (*getTypekey)(unsigned)
    ctypedef struct c_dbread "dbread":
        getType getType
        getRecord getRecord
        clear clear
        getSize getSize
        getTypesize getTypesize
        getTypekey getTypekey

    c_dbread *new_dbread "new dbread" (char * filename)
    void del_dbread "delete" (c_dbread *db)

cdef class dbread:
    cdef c_dbread *thisptr              # wrapped C++ instance

    def __cinit__(self, filename):
        self.thisptr = new_dbread(filename)
        
    def __dealloc__(self):
        pass
#        del_dbread(self.thisptr)

    def loadRecord(self, idx):
        self.thisptr.getRecord(idx)
    
    def getFieldValue(self, name):
        return self.thisptr.getType(name)

    def getNumRecords(self):
        return self.thisptr.getSize()

    def getNumFields(self):
        return self.thisptr.getTypesize()

    def getFieldName(self, idx):
        return self.thisptr.getTypekey(idx)
