# TODO
#  - python interface to indexer for fa, fq
#  - exceptions!

RUN_CRASH_TESTS = True

import sys
sys.path.insert(0, 'python/')
import _seqdb2
import seqdb2


# create index files
import subprocess
def setup():
    subprocess.check_call('./bin/fadbm tests/test.fa', shell=True,
            stdout=subprocess.PIPE)
    subprocess.check_call('./bin/fqdbm tests/test.fastq', shell=True,
            stdout=subprocess.PIPE)

# loading out-of-bounds record num -- should raise an error
class Test_pyx_err:
    def setup(self):
        self.db = _seqdb2.dbread('tests/test.fa_seqdb2')

    def tearDown(self):
        self.db.clear()

    def test_open(self):
        try:
            foo = _seqdb2.dbread('FOOBAR')
        except _seqdb2.DbException, e:
            assert(e.value == 'Invalid database filename')

    def test_loadRecord(self): 
        try:
            self.db.loadRecord(-1)
        except _seqdb2.DbException, e:
            assert(e.value == 'Invalid query')

    def test_clear(self):
        try:
            self.db.loadRecord(-1) # Makes sure the error is raised
        except _seqdb2.DbException:
            pass

        try: # Doesn't clear the last error so should raise another exception
            self.db.getFieldValue('name')
        except _seqdb2.DbException, e:
            assert(e.value == 'Invalid query')

    def test_typename(self):
        try:
            print self.db.getFieldValue('foobar')
        except _seqdb2.DbException, e:
            assert(e.value == 'Invalid typename query')

    def test_typekey(self):
        try:
            self.db.getFieldName(10)
        except _seqdb2.DbException, e:
            assert(e.value == 'Bad typekey request')

class Test_seqdb2:
    def setup(self):
        self.fadb = seqdb2.SeqDB2('tests/test.fa_seqdb2')
        self.fqdb = seqdb2.SeqDB2('tests/test.fastq_seqdb2')

    def tearDown(self):
        self.fadb.clear()
        self.fqdb.clear()

    def test_fa_length(self):
        assert(len(self.fadb) == 22)

    def test_fa_fields(self):
        assert(self.fadb.fields == ['description','name','sequence'])

    def test_fa_fields2(self):
        k = self.fadb[0]
        assert(k.description == 'cdna:pseudogene scaffold:micMur1:scaffold_\
185008:9:424:1 gene:ENSMICG00000012730')
        assert(k.name == 'ENSMICT00000012722')
        assert(k.sequence.startswith('TGCAGAAAATATCAAGAGTCAGCAGAAAAACTATACAAG'))

    def test_fq_length(self):
        assert(len(self.fqdb) == 49)

    def test_fq_fields(self):
        assert(self.fqdb.fields == ['accuracy','name','sequence'])

    def test_fq_fields2(self):
        k = self.fqdb[0]
        assert(k.accuracy == 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAADDDAAD')
        assert(k.name == 'HWI-EAS_4_PE-FC20GCB:1:1:62:922/1')
        assert(k.sequence == 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA')
