import sys, os
import subprocess

thisdir = os.path.dirname(__file__)
libdir = os.path.abspath(thisdir + '/../python')
sys.path.insert(0, libdir)
import _seqdb2
import seqdb2

def setup():
    # index databases
    subprocess.check_call(thisdir + '/../bin/fadbm tests/test.fa',
                          shell=True, stdout=subprocess.PIPE)
    subprocess.check_call(thisdir + '/../bin/fqdbm tests/test.fastq',
                          shell=True, stdout=subprocess.PIPE)

class Test_pyx_Fasta:
    def setup(self):
        self.db = _seqdb2.dbread(thisdir + '/test.fa_seqdb2')

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
        self.db = _seqdb2.dbread(thisdir + '/test.fastq_seqdb2')

    def test_simple(self):
        db = self.db
        assert db.getNumFields() == 3

        fields = [ db.getFieldName(i) for i in range(3) ]
        fields.sort()
        assert fields == ['accuracy', 'name', 'sequence']

        assert db.getNumRecords() == 49

        db.loadRecord(0)
        for field in fields:
            print db.getFieldValue(field)

        assert db.getFieldValue('accuracy') == \
               'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAADDDAAD'
        assert db.getFieldValue('name') == 'HWI-EAS_4_PE-FC20GCB:1:1:62:922/1'
        assert db.getFieldValue('sequence') == \
               'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA'

class Test_FastQ:
    def setup(self):
        self.db = seqdb2.SeqDB2(thisdir + '/test.fastq_seqdb2')

    def test_simple(self):
        assert len(self.db) == 49

        record = self.db[0]
        assert record.accuracy == 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAADDDAAD'
        assert record.name == 'HWI-EAS_4_PE-FC20GCB:1:1:62:922/1'
        assert record.sequence == 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA'
        
class Test_Fasta:
    def setup(self):
        self.db = seqdb2.SeqDB2(thisdir + '/test.fa_seqdb2')

    def test_simple(self):
        assert len(self.db) == 22

        record = self.db[0]
        assert record.description.startswith('cdna:pseudogene ')
        assert record.name == 'ENSMICT00000012722'
        assert record.sequence.startswith('TGCAGAAAATATCAAGAGTCA')
