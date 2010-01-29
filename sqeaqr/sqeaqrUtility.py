from sqeaqrExtension import fileExtension
import UserDict
import os
import sqlite3

# [AN] switch to cursor?

_SQEAQRADMIN = 'SQEAQRADMIN'
_DICT_TABLE = 'DICTIONARY_TABLE'
_PRIMARY_KEY = 'ID'

class _sqeaqr_record(UserDict.DictMixin):
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

def getSqeaqrDB(filepath, queryBy=None, fields=None):
    """
    Opens and prepares a sqeaqrDB. The order of arguments is:
    filepath: the string path to the sqeaqr database to open
    queryBy: either None to enable querying by primary key or the
        name of a specified field to perform name lookups with
    fields: either None if the the database has already been created or
        a tuple of 2-tuples specifying the names and types of fields
        to be used in the creation of the sql database
    Returns a 7 tuple containing:
    (database object, _standardStub, _fieldTuple, _qMarks, _queryBy, _DICT_TABLE,
    _PRIMARY_KEY)
    """
    # Ensure the filepath is correctly formatted with the extension to
    # the database filename
    if not filepath.endswith(fileExtension):
        filepath += fileExtension

    sqdb = None
    if not os.path.isfile(filepath): # Db file doesn't exist, should I create?
        if fields == None: # Need to create, but don't have fields!
            raise ValueError("Fields not specified and database doesn't exist")
        sqdb = _createSqDb(filepath, fields)
    else:
        sqdb = sqlite3.connect(filepath)

    # Create the standard sql query stub
    _standardStub = _retrieveStandardStub(sqdb)

    # Create the ordered tuple of fields
    _fieldTuple = _getFieldTuple(sqdb)

    # Create the string of question marks
    _qMarks = _toQmarks(sqdb)

    # Set the queryBy string (PRIMARY KEY/NAME)
    _queryBy = _setQueryBy(sqdb, queryBy)

    return (sqdb, _standardStub, _fieldTuple, _qMarks, _queryBy, _DICT_TABLE,
            _PRIMARY_KEY)

def _getFieldTuple(sqdb):
    """
    Creates the ordered tuple of fields from the database
    e.x, returns: (id, name, description)
    """
    query = 'SELECT * FROM %s' % _SQEAQRADMIN
    result = [_PRIMARY_KEY.lower()]
    for fieldName, fieldType, in sqdb.execute(query):
        result.append(str(fieldName.lower()))

    return tuple(result)

def _setQueryBy(sqdb, queryBy):
    """
    Returns the sub-string needed for SQL 'WHERE' clauses when selecting items
    from the dictionary table
    """
    if queryBy == None: # User didn't set queryBy, default to ID
        return _PRIMARY_KEY

    # Pull field data from the admin table to make sure the user's selection
    # matches something there
    query = 'SELECT * FROM %s' % _SQEAQRADMIN
    for fieldName, fieldType, in sqdb.execute(query):
        if fieldName == queryBy.upper():
            return queryBy.upper()

    raise ValueError("Non-existant queryBy field specified: '%s'" % queryBy)

def _createSqDb(filepath, fields):
    """
    Creates the sqeaqr database. This consists of a small 'SQEAQRADMIN' table which
    holds accounting information such as name and number of fields and a
    'DICTIONARY_TABLE' which holds the actual information to be handled for the user
    """
    sqdb = sqlite3.connect(filepath)

    # Create the admin table
    sqdb.execute('CREATE TABLE %s (FIELDNAME TEXT, FIELDTYPE TEXT)' %
                      _SQEAQRADMIN)
    
    query = 'INSERT INTO %s (FIELDNAME, FIELDTYPE) VALUES (?, ?)' % \
            _SQEAQRADMIN

    for fieldName, fieldType in fields:
        sqdb.execute(query, (fieldName.upper(), fieldType.upper()))

    # Create the dictionary table
    sqdb.execute('CREATE TABLE %s (%s INTEGER PRIMARY KEY, %s)' %
                      (_DICT_TABLE, _PRIMARY_KEY, toCreateStub(fields)))
    return sqdb

def _retrieveStandardStub(sqdb):
    """
    Retrieves the names of the fields from the admin table and returns a sql sub-
    string that can be used for querying.
    e.x: table contains:
    FIELDNAME      FIELDTYPE
    'NAME'         TEXT
    'DESCRIPTION'  TEXT
    returns: 'NAME, DESCRIPTION'
    """
    sqlList = []
    query = "SELECT * FROM %s" % _SQEAQRADMIN
    for fieldName, fieldType in sqdb.execute(query):
        sqlList.append('%s' % fieldName)
        sqlList.append(', ')
    sqlList.pop()
    return str("".join(sqlList))

def _toQmarks(sqdb):
    """
    Retrieves the count of items in the admin table and builds the sql question
    marks in the format needed for substitution into an sql query
    e.x: table has 3 elements
    returns: '?, ?, ?'
    """
    sqlList = []
    query = "SELECT COUNT(1) FROM %s" % _SQEAQRADMIN
    result, = sqdb.execute(query).next()
    for i in xrange(0, result):
        sqlList.append("?")
        sqlList.append(", ")
    sqlList.pop()
    return "".join(sqlList)

def _retrieve(sqdb, getQuery):
    retrieved = sqdb.execute(getQuery)

def toCreateStub(fieldTuple):
    """
    Parses the ordered name, value pairs in fieldTuple into a create stub
    ready to be inserted into the command to create the sql table.
    e.x: given input (('name', 'text'), ('description', 'text'))
    returns: 'NAME TEXT, DESCRIPTION TEXT'
    """
    sqlList = []
    for fieldName, fieldType in fieldTuple:
        sqlList.append('%s %s' % (fieldName.upper(), fieldType.upper()))
        sqlList.append(', ')
    sqlList.pop()
    return "".join(sqlList)
