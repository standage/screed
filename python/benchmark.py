#!/usr/bin/env python2.5
import time
import sys
import sqlite3
import types
from seqdb2 import dbread
import os

dbname = 'times.db'

def createSQ3Db():
    """
    Creates the database. Note: if the database already exists and this is
    called it will delete the existing database first
    """
    if os.path.isfile(dbname):
        os.remove(dbname)
    conn = sqlite3.connect(dbname)
    c = conn.cursor()
    c.execute("CREATE TABLE times (file text, alg text, time real, records int)")
    conn.commit()

def getDBentries(type = ()):
    """
    Selects and prints out the wanted types from the database. type is a tuple
    with the first entry the type of data to select and the second entry
    a value to compare database values to. If the second value is blank, a
    'select *' statement is executed
    examples for type:
    ('records', '40000')
    ('file', 'test.fa_seqdb2')
    """
    if not os.path.isfile(dbname):
        print "ERROR, DB IS NOT YET CREATED! RUN createSQ3Db FIRST"
        return
    types = ['records', 'time', 'alg', 'file']
    conn = sqlite3.connect(dbname)
    c = conn.cursor()
    query = ''
    if len(type) == 0:
        c.execute("SELECT * FROM times")
    else:
        if type[0] not in types:
            print "ERROR: TYPE %s DOES NOT EXIST IN DATABASE" % type[0]
            return
        query = "SELECT * FROM times WHERE %s='%s'" % type
        c.execute(query)
    result = c.fetchall()
    print "File".rjust(5), "Alg".rjust(10), "Time".rjust(13),\
            "Records".rjust(12)
    print '-'*60
    for entry in result:
        toprint = ''
        for data in entry:
            data = str(data)
            if len(data) > 10:
                data = data[:10]
            toprint += data.rjust(10)
        print toprint
    print '-'*60
    return

def doBenchmark(file, algorithm):
    if not os.path.isfile(dbname):
        createSQ3Db()
    fname = os.path.split(file)[1]
    conn = sqlite3.connect(dbname)
    c = conn.cursor()
    db = dbread(file)
    names = []
    for entry in db:
        names.append(entry)
    start = time.time()
    for name in names:
        k = db[name]
    end = time.time()
    totaltime = int(end - start)
    c.execute("INSERT INTO times (file, alg, time, records) VALUES (?, ?, ?, ?)", (fname, algorithm, totaltime, len(db)))
    conn.commit()

if __name__ == '__main__':
    file = str(sys.argv[1])
    algorithm = str(sys.argv[2])
    doBenchmark(file, algorithm)
