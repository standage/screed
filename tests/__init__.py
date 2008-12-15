import sys, os, gc
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

class Test_dict_methods:
    """
    Make sure that seqdb2 returns sensible results for standard dictionary
    queries.
    """
    def setup(self):
        self.db = seqdb2.dbread('tests/test.fa_seqdb2')

    def test_iter_stuff(self):
        db = self.db
        keys = db.keys()
        ikeys = list(db.iterkeys())
        assert sorted(keys) == sorted(ikeys)

        values = db.values()
        ivalues = list(db.itervalues())
        assert sorted(values) == sorted(ivalues)

        items = db.items()
        iitems = list(db.iteritems())
        assert sorted(items) == sorted(iitems)

    def test_contains(self):
        for k in self.db:
            assert self.db.has_key(k)

        assert not self.db.has_key('FOO')
            
    def test_contains(self):
        for k in self.db:
            assert k in self.db

        assert not 'FOO' in self.db

    def test_get(self):
        for k in self.db:
            record = self.db.get(k)
            assert record.name == k

            record = self.db[k]
            assert record.name == k

        assert self.db.get('FOO') == None
        try:
            self.db['FOO']
            assert False, "the previous line should raise a KeyError"
        except KeyError:
            pass

    def test_missing(self):
        """
        Make sure that unsupported dict attributes are actually missing.
        """
        db = self.db

        try:
            db.clear()
            assert 0
        except AttributeError:
            pass

        try:
            db.update({})
            assert 0
        except AttributeError:
            pass

        try:
            db.clear()
            assert 0
        except AttributeError:
            pass

        try:
            db.setdefault(None)
            assert 0
        except AttributeError:
            pass

        try:
            db.pop()
            assert 0
        except AttributeError:
            pass

        try:
            db.popitem()
            assert 0
        except AttributeError:
            pass

class Test_pyx_err:
    def setup(self):
        self.db = seqdb2.dbread('tests/test.fa_seqdb2')

    def teardown(self):
        self.db.clearErrorFlag()

    def test_open(self):
        try:
            foo = seqdb2.dbread('foobar')
        except seqdb2.DbException, e:
            assert e.value == 'Invalid database filename'

    def test_loadrecord(self): 
        try:
            self.db.loadRecordByIndex(-1)
        except seqdb2.DbException, e:
            assert e.value == 'Invalid query'

    def test_clear(self):
        try:
            self.db.loadRecordByIndex(-1) # makes sure the error is raised
            assert 1 == 0, "Not raising an exception with loading bad record"
        except seqdb2.DbException:
            pass

#        try: # doesn't clear the last error so should raise another exception
#            self.db.getFieldValue('name')
#        except seqdb2.DbException, e:
#            assert e.value == 'Invalid query'
# @CTB

    def test_typename(self):
        if 0:                           # @@CTB
            try:
                print self.db.getFieldValue('FOOBAR')
            except seqdb2.DbException, e:
                assert e.value == 'Invalid typename query'

    def test_typekey(self):
        if 0:                           # @@CTB
            try:
                self.db.getFieldName(10)
            except seqdb2.DbException, e:
                assert e.value == 'Bad typekey request'

class Test_pyx_Fasta:
    def setup(self):
        self.db = seqdb2.dbread(thisdir + '/test.fa_seqdb2')

    def test_delete(self):
        gc.collect()
        
        db = seqdb2.dbread(thisdir + '/test.fa_seqdb2')
        del db

        gc.collect()
        

    def test_simple(self):
        db = self.db
        assert len(db.fields) == 3

        fields = list(db.fields)
        fields.sort()
        assert fields == ['description', 'name', 'sequence']
        
        assert len(db) == 22

        record = db.loadRecordByIndex(0)
        
        assert record['description'].startswith('cdna:pseudogene ')
        assert record['name'] == 'ENSMICT00000012722'
        assert record['sequence'].startswith('TGCAGAAAATATCAAGAGTC')

    def test_bag_attr(self):
        db = self.db
        assert len(db.fields) == 3

        record = db.loadRecordByIndex(0)

        assert record['description'] == record.description
        assert record['name'] == record.name
        assert record['sequence'] == record.sequence
        
    def test_iter(self):
        for i, k in enumerate(self.db):
            record = self.db[k]
            record2 = self.db.loadRecordByIndex(i)
            assert record.name == record2.name

    def test_name_hashing(self):
        db = self.db
        for k in db:
            record = db[k]
            record2 = db.loadRecordByName(record['name'])
            assert record == record2

class Test_pyx_FastQ:
    def setup(self):
        self.db = seqdb2.dbread(thisdir + '/test.fastq_seqdb2')

    def test_simple(self):
        db = self.db
        assert len(db.fields) == 3
        assert len(db) == 125

        fields = list(db.fields)
        fields.sort()
        assert fields == ['accuracy', 'name', 'sequence']

        record = db.loadRecordByIndex(0)

        assert record['accuracy'] == 'AA7AAA3+AAAAAA.AAA.;7;AA;;;;*;<1;<<<'
        assert record['name'] == 'HWI-EAS_4_PE-FC20GCB:2:1:492:573/2'
        assert record['sequence'] == 'ACAGCAAAATTGTGATTGAGGATGAAGAACTGCTGT'

    def test_bag(self):
        db = self.db

        record = db.loadRecordByIndex(0)
        assert record['accuracy'] == record.accuracy
        assert record['name'] == record.name
        assert record['sequence'] == record.sequence

    def test_iter(self):
        for i, k in enumerate(self.db):
            record = self.db[k]
            record2 = self.db.loadRecordByIndex(i)
            assert record.name == record2.name

    def test_name_hashing(self):
        db = self.db
        for k in db:
            record = db[k]
            record2 = db.loadRecordByName(record['name'])
            assert record == record2
            
if 0:
    class Test_Hashing_nondefault_1:
        # @CTB what is this testing?
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
            pass
    #        for record in self.fqdb:
    #            assert record.name == self.fqdb[record.name].name

        def test_fasta_simple(self):
            pass
    #        for record in self.fadb:
    #            assert record.name == self.fadb[record.name].name
