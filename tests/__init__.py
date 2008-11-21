import sys, os
import subprocess

thisdir = os.path.dirname(__file__)
libdir = os.path.abspath(thisdir + '/../python')
sys.path.insert(0, libdir)
import seqdb2

def setup():
    # index databases
    subprocess.check_call(thisdir + '/../bin/fadbm tests/test.fa',
                          shell=True, stdout=subprocess.PIPE)
    subprocess.check_call(thisdir + '/../bin/fqdbm tests/test.fastq',
                          shell=True, stdout=subprocess.PIPE)

class Test_pyx_err:
    def setup(self):
        self.db = seqdb2.dbread('tests/test.fa_seqdb2')

    def teardown(self):
        self.db.clear()

    def test_open(self):
        try:
            foo = seqdb2.dbread('foobar')
        except seqdb2.DbException, e:
            assert e.value == 'Invalid database filename'

    def test_loadrecord(self): 
        try:
            self.db.loadRecord(-1)
        except seqdb2.DbException, e:
            assert e.value == 'Invalid query'

    def test_clear(self):
        try:
            self.db.loadRecord(-1) # makes sure the error is raised
            assert 1 == 0, "Not raising an exception with loading bad record"
        except seqdb2.DbException:
            pass

        try: # doesn't clear the last error so should raise another exception
            self.db.getFieldValue('name')
        except seqdb2.DbException, e:
            assert e.value == 'Invalid query'

    def test_typename(self):
        try:
            print self.db.getFieldValue('FOOBAR')
        except seqdb2.DbException, e:
            assert e.value == 'Invalid typename query'

    def test_typekey(self):
        try:
            self.db.getFieldName(10)
        except seqdb2.DbException, e:
            assert e.value == 'Bad typekey request'

class Test_pyx_Fasta:
    def setup(self):
        self.db = seqdb2.dbread(thisdir + '/test.fa_seqdb2')

    def test_simple(self):
        db = self.db
        assert db.getNumFields() == 3

        fields = [ db.getFieldName(i) for i in range(3) ]
        fields.sort()
        assert fields == ['description', 'name', 'sequence']
        
        assert db.getNumRecords() == 22

        db.loadRecord(0)
        for field in fields:
            print db.getFieldValue(field)

        assert db.getFieldValue('description').startswith('cdna:pseudogene ')
        assert db.getFieldValue('name') == 'ENSMICT00000012722'
        assert db.getFieldValue('sequence').startswith('TGCAGAAAATATCAAGAGTC')

class Test_pyx_FastQ:
    def setup(self):
        self.db = seqdb2.dbread(thisdir + '/test.fastq_seqdb2')

    def test_simple(self):
        db = self.db
        assert db.getNumFields() == 3

        fields = [ db.getFieldName(i) for i in range(3) ]
        fields.sort()
        assert fields == ['accuracy', 'name', 'sequence']

        assert db.getNumRecords() == 125

        db.loadRecord(0)
        for field in fields:
            print db.getFieldValue(field)

        assert db.getFieldValue('accuracy') == \
               'AA7AAA3+AAAAAA.AAA.;7;AA;;;;*;<1;<<<'
        assert db.getFieldValue('name') == 'HWI-EAS_4_PE-FC20GCB:2:1:492:573/2'
        assert db.getFieldValue('sequence') == \
               'ACAGCAAAATTGTGATTGAGGATGAAGAACTGCTGT'

class Test_FastQ:
    def setup(self):
        self.db = seqdb2.SeqDB2(thisdir + '/test.fastq_seqdb2')
    
    def tearDown(self):
        self.db.clear()

    def test_simple(self):
        assert len(self.db) == 125
        assert self.db.fields == ['accuracy','name','sequence']

        record = self.db[0]
        assert record.accuracy == 'AA7AAA3+AAAAAA.AAA.;7;AA;;;;*;<1;<<<'
        assert record.name == 'HWI-EAS_4_PE-FC20GCB:2:1:492:573/2'
        assert record.sequence == 'ACAGCAAAATTGTGATTGAGGATGAAGAACTGCTGT'
        
class Test_Fasta:
    def setup(self):
        self.db = seqdb2.SeqDB2(thisdir + '/test.fa_seqdb2')

    def test_simple(self):
        assert len(self.db) == 22
        assert self.db.fields == ['description','name','sequence']

        record = self.db[0]
        assert record.description.startswith('cdna:pseudogene ')
        assert record.name == 'ENSMICT00000012722'
        assert record.sequence.startswith('TGCAGAAAATATCAAGAGTCA')

class Test_Hashing_fastq:
    def setup(self):
        self.db = seqdb2.SeqDB2(thisdir + '/test.fastq_seqdb2')

    def test_simple(self):
        for record in self.db:
            assert record.name == self.db[record.name].name

class Test_Hashing_fa:
    def setup(self):
        self.db = seqdb2.SeqDB2(thisdir + '/test.fa_seqdb2')

    def test_simple(self):
        for record in self.db:
            assert record.name == self.db[record.name].name

class Test_Hashing_nondefault_1:
    def setup(self):
        # Setup non-default database with length=1
        subprocess.check_call(thisdir + '/../bin/fqdbm tests/test.fastq 1',
                shell=True, stdout=subprocess.PIPE)
        subprocess.check_call(thisdir + '/../bin/fadbm tests/test.fa 1',
                shell=True, stdout=subprocess.PIPE)
        self.fqdb = seqdb2.SeqDB2(thisdir + '/test.fastq_seqdb2')
        self.fadb = seqdb2.SeqDB2(thisdir + '/test.fa_seqdb2')

    def tearDown(self):
         # remake the normal database
        subprocess.check_call(thisdir + '/../bin/fadbm tests/test.fa',
                shell=True, stdout=subprocess.PIPE)
        subprocess.check_call(thisdir + '/../bin/fqdbm tests/test.fastq',
                shell=True, stdout=subprocess.PIPE)

    def test_fastq_simple(self):
        for record in self.fqdb:
            assert record.name == self.fqdb[record.name].name

    def test_fasta_simple(self):
        for record in self.fadb:
            assert record.name == self.fadb[record.name].name
