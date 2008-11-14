#!/usr/bin/env python2.5
import time
import sys
import sqlite3
import types
from seqdb2 import SeqDB2

def doBenchmark(file, desc, sizeratio):
    conn = sqlite3.connect('times.db')
    c = conn.cursor()
    db = SeqDB2(file)
    names = []
    for entry in db:
        names.append(entry.name)
    print "%d RECORDS" % len(db)
    start = time.time()
    for name in names:
        k = db[name]
    end = time.time()
    totaltime = int(end - start)
    print "%d SECONDS TO ITERATE THROUGH ALL" % totaltime
    query = "INSERT INTO times (desc, recordnum, itertime, sizeratio) VALUES "+\
            "('%s', %d, %d, %f)" % (desc, len(db), totaltime, sizeratio)
    c.execute(query)
    conn.commit()

if __name__ == '__main__':
    file = str(sys.argv[1])
    desc = str(sys.argv[2])
    sizeratio = float(sys.argv[3])
    doBenchmark(file, desc, sizeratio)
