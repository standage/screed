# TODO
#  - python interface to indexer for fa, fq
#  - exceptions!

RUN_CRASH_TESTS = True

import sys
sys.path.insert(0, 'python/')
import _seqdb2


# create index files
import subprocess
def setup():
    subprocess.check_call('./bin/fadbm tests/test.fa', shell=True,
            stdout=subprocess.PIPE)

# loading out-of-bounds record num -- should raise an error
class Test_pyx_err:
    def setup(self):
        self.db = _seqdb2.dbread('tests/test.fa_seqdb2')

    def test_loadRecord(self):
        try:
            self.db.loadRecord(-1)
        except _seqdb2.DbException, e:
            assert(e.value == 'Invalid query')
        finally:
            self.db.clear()

    def test_clear(self):
        try:
            self.db.loadRecord(-1) # Makes sure the error is raised
        except _seqdb2.DbException:
            pass

        try: # Doesn't clear the last error so should raise another exception
            self.db.getFieldValue('name')
        except _seqdb2.DbException, e:
            assert(e.value == 'Invalid query')
        finally:
            self.db.clear()

    def test_typename(self):
        try:
            print fa.getFieldValue('foobar')
        except _seqdb2.DbException, e:
            assert(e.value == 'Invalid typename query')
        finally:
            fa.clear()

        try:
            fa.getFieldName(10)
        except _seqdb2.DbException, e:
            assert(e.value == 'Bad typekey request')
        finally:
            fa.clear()

class Test_pyx_err:
    def setup(self):
        self.fn = 'FOOBAR'
    def test_open(self):
        try:
            foo = _seqdb2.dbread(self.fn)
        except _seqdb2.DbException, e:
            assert(e.value == 'Invalid database filename')
            
###

import seqdb2
db = seqdb2.SeqDB2('tests/test.fa_seqdb2')
print len(db)

print db.fields

x = db[0]
print x.name, x.description, x.sequence

db = seqdb2.SeqDB2('tests/test.fastq_seqdb2')
print len(db)
print db.fields
x = db[0]
print x.name, x.accuracy, x.sequence

###
