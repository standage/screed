import sys, os, gc
import subprocess
import urllib, tarfile

thisdir = os.path.dirname(__file__)
libdir = os.path.abspath(os.path.join(thisdir, '..', 'python'))
sys.path.insert(0, libdir)
import screed

fadbm = os.path.join(thisdir, '..', 'bin', 'fadbm')
fqdbm = os.path.join(thisdir, '..', 'bin', 'fqdbm')
tests22 = os.path.join(thisdir,  's_2_2_sequence.fastq')
tests31 = os.path.join(thisdir, 's_3_1_sequence.fastq')
tests42 = os.path.join(thisdir, 's_4_2_sequence.fastq')
pongo = os.path.join(thisdir, 'Pongo_pygmaeus.PPYG2.50.cdna.abinitio.fa')
tri = os.path.join(thisdir, 'triCas2.fa')
mus = os.path.join(thisdir, 'Mus_musculus.NCBIM37.50.dna_rm.chromosome.9.fa')
xeno = os.path.join(thisdir, 'Xenopus_tropicalis.JGI4.1.50.dna.toplevel.fa')

def getfile(f):
    """
    Downloads and extracts the given file
    """
    filetype = f[1]
    filename = "%s.tar.gz" % f[0]
    urlname = os.path.split(filename)[1]
    base_url = 'http://lyorn.idyll.org/~nolleyal/genomes/%s/%s' % \
        (filetype, urlname)

    fp = open(filename, 'wb')
    try:
        up = urllib.urlopen(base_url)
    except IOError:
        raise IOError, "Error downloading testfiles, are you connected to\
 the internet?"
    fp.write(up.read())
    fp.close()

    tar = tarfile.open(filename)
    tar.extractall(path=thisdir)
    tar.close()
    os.unlink(filename)
    return

def setup():
    # Create databases
    endings = ['_screed', '_screed_idx', '_screed_hash']
    filenames = [(tests22, 'fastq'), (tests31, 'fastq'), (tests42, 'fastq'),
            (pongo, 'fasta'), (tri, 'fasta'), (mus, 'fasta'), (xeno, 'fasta')]
    for f in filenames:
        fname = f[0]
        if not os.path.isfile(fname): # Download files if necessary
            getfile(f)
        parser = None
        if f[1] == 'fasta':
            parser = fadbm
        elif f[1] == 'fastq':
            parser = fqdbm
        created = True
        for end in endings:
            if not os.path.isfile(fname + end):
                created = False
        if created == False:
            subprocess.check_call(parser + ' ' + fname, shell=True, stdout=subprocess.PIPE)

class Test_s22_fastq:
    """
    Test screed methods on the s22 fastq file
    """
    def setup(self):
        self.db = screed.dbread(tests22 + '_screed')

    def tearDown(self):
        del self.db
        gc.collect()

    def test_iteration(self):
        """
        Runs through the database, accessing each element by index and then by
        name
        """
        names = []
        for i in range(0, len(self.db)):
            names.append(self.db.loadRecordByIndex(i)['name'])
        assert names == self.db.keys()

        for i in names:
            l = self.db[i]

        del names
        gc.collect()

    def test_dict_stuff(self):
        """
        Tests some dictionary methods on the database
        """
        keys = self.db.keys()
        ikeys = list(self.db.iterkeys())
        assert sorted(keys) == sorted(ikeys)
        del keys
        del ikeys
        gc.collect()
        
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

    def test_certain_records(self):
        """
        Pulls first, last, middle and few other records out of database and
        compares them to known quantities
        """
        testcases = {}
        testcases['HWI-EAS_4_PE-FC20GCB:2:1:492:573/2'] = {
		'index': 0L,
                'accuracy': 'AA7AAA3+AAAAAA.AAA.;7;AA;;;;*;<1;<<<',
                'name' : 'HWI-EAS_4_PE-FC20GCB:2:1:492:573/2',
                'sequence': 'ACAGCAAAATTGTGATTGAGGATGAAGAACTGCTGT'}
        testcases['HWI-EAS_4_PE-FC20GCB:2:162:131:826/2'] = {
		'index': 1895228L,
                'accuracy': 'AAAAAAAAAAAAAAAAAAAAAA+@6=7A<05<*15:',
                'name': 'HWI-EAS_4_PE-FC20GCB:2:162:131:826/2',
                'sequence': 'ATGAATACAAACAATGCGGCAGTCATAATGCCCCTC'}
        testcases['HWI-EAS_4_PE-FC20GCB:2:330:88:628/2'] = {
		'index': 3790455L,
                'accuracy' : 'AA;AA??A5A;;+AA?AAAA;AA;9AA.AA?????9',
                'name': 'HWI-EAS_4_PE-FC20GCB:2:330:88:628/2',
                'sequence': 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAA'}
        testcases['HWI-EAS_4_PE-FC20GCB:2:4:707:391/2'] = {
		'index': 29999L,
                'accuracy': 'AAAAAAAAAA@<)A*AAA6A::<@AA<>A>-8?>4<',
                'name': 'HWI-EAS_4_PE-FC20GCB:2:4:707:391/2',
                'sequence': 'ATTAATCTCCAGTTTCTGGCAAACATTCAGGCCATT'}
        testcases['HWI-EAS_4_PE-FC20GCB:2:36:158:208/2'] = {
		'index': 342842L,
                'accuracy': 'AA5?AAAAA?AAAA5?AAA5A???5A>AAA4?;.;;',
                'name': 'HWI-EAS_4_PE-FC20GCB:2:36:158:208/2',
                'sequence': 'TTTCCCTACAGAAGTGTCTGTACCGGTAATAAAGAA'}

        for case in testcases:
            assert testcases[case] == self.db[case]
            
            
class Test_s31_fastq:
    """
    Test screed methods on the s31 fastq file
    """
    def setup(self):
        self.db = screed.dbread(tests31 + '_screed')

    def tearDown(self):
        del self.db
        gc.collect()

    def test_iteration(self):
        """
        Runs through the database, accessing each element by index and then by
        name
        """
        names = []
        for i in range(0, len(self.db)):
            names.append(self.db.loadRecordByIndex(i)['name'])
        assert names == self.db.keys()

        for i in names:
            l = self.db[i]

        del names
        gc.collect()

    def test_dict_stuff(self):
        """
        Tests some dictionary methods on the database
        """
        keys = self.db.keys()
        ikeys = list(self.db.iterkeys())
        assert sorted(keys) == sorted(ikeys)
        del keys
        del ikeys
        gc.collect()

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

    def test_certain_records(self):
        """
        Pulls first, last, middle and few other records out of database and
        compares them to known quantities
        """
        testcases = {}
        testcases['HWI-EAS_4_PE-FC20GCB:3:1:71:840/1'] = {
		'index': 0L,
                'accuracy': 'CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC',
                'name' : 'HWI-EAS_4_PE-FC20GCB:3:1:71:840/1',
                'sequence': 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA'}
        testcases['HWI-EAS_4_PE-FC20GCB:3:330:957:433/1'] = {
                'index': 4439695L,
		'accuracy': 'AAAAAAAAAAA<A?<AA<AAAAA?AAA?<:*??&::',
                'name': 'HWI-EAS_4_PE-FC20GCB:3:330:957:433/1',
                'sequence': 'CTTTGTGGAGAAGAGGGCGTGGGCAAGGCACTGATA'}
        testcases['HWI-EAS_4_PE-FC20GCB:3:166:443:410/1'] = {
                'index': 2219847L,
		'accuracy' : 'AAAAAAAAAAAAAAAAAAAAAAAA6<@AA959???%',
                'name': 'HWI-EAS_4_PE-FC20GCB:3:166:443:410/1',
                'sequence': 'TGGCATTCGCACACATCATGATGGTGCTGACCGTAA'}
        testcases['HWI-EAS_4_PE-FC20GCB:3:1:803:878/1'] = {
                'index': 2999L,
		'accuracy': '?6AAA6A<A6AA<<AA?A&A066/6:/&?&1191+0',
                'name': 'HWI-EAS_4_PE-FC20GCB:3:1:803:878/1',
                'sequence': 'AAGATGCTGTAGTGGCCGCATGTGTAATAGGCTTTA'}
        testcases['HWI-EAS_4_PE-FC20GCB:3:245:54:506/1'] = {
                'index': 3329772L,
		'accuracy': "AAAAAAAAAAAAAAAA>A+AAA+@AA+A>A%8*?'%",
                'name': 'HWI-EAS_4_PE-FC20GCB:3:245:54:506/1',
                'sequence': 'CTTCGTTGCTGTTTATCAGTAACTTTTTCTGGCTAG'}

        for case in testcases:
            assert testcases[case] == self.db[case]
            

class Test_s42_fastq:
    """
    Test screed methods on the s42 fastq file
    """
    def setup(self):
        self.db = screed.dbread(tests42 + '_screed')

    def tearDown(self):
        del self.db
        gc.collect()

    def test_iteration(self):
        """
        Runs through the database, accessing each element by index and then by
        name
        """
        names = []
        for i in range(0, len(self.db)):
            names.append(self.db.loadRecordByIndex(i)['name'])
        assert names == self.db.keys()

        for i in names:
            l = self.db[i]

        del names
        gc.collect()

    def test_dict_stuff(self):
        """
        Tests some dictionary methods on the database
        """
        keys = self.db.keys()
        ikeys = list(self.db.iterkeys())
        assert sorted(keys) == sorted(ikeys)
        del keys
        del ikeys
        gc.collect()
        
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

    def test_certain_records(self):
        """
        Pulls first, last, middle and few other records out of database and
        compares them to known quantities
        """
        testcases = {}
        testcases['HWI-EAS_4_PE-FC20GCB:4:1:257:604/2'] = {
                'index': 0L,
		'accuracy': 'AAAAAAAA:4>>AAA:44>>->-&4;8+8826;66.',
                'name' : 'HWI-EAS_4_PE-FC20GCB:4:1:257:604/2',
                'sequence': 'TGTGGATAGTCGCCCGTGATGGCGTCGAAGTTCCGG'}
        testcases['HWI-EAS_4_PE-FC20GCB:4:330:96:902/2'] = {
                'index': 4148632L,
		'accuracy': 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA??????',
                'name': 'HWI-EAS_4_PE-FC20GCB:4:330:96:902/2',
                'sequence': 'CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC'}
        testcases['HWI-EAS_4_PE-FC20GCB:4:166:158:532/2'] = {
                'index': 2074316L,
		'accuracy' : 'AAAAAAA?A?AAAAAAA?A>A?A?AAAAAA?.<?-?',
                'name': 'HWI-EAS_4_PE-FC20GCB:4:166:158:532/2',
                'sequence': 'ATCGCCAATGCCCAGGCCTGGTTCTCTTTAACCTAT'}
        testcases['HWI-EAS_4_PE-FC20GCB:4:1:332:634/2'] = {
                'index': 3000L,
		'accuracy': '?A?AAAAAAAAA8>AAAAAA*AA?A?AA.?)<9)9?',
                'name': 'HWI-EAS_4_PE-FC20GCB:4:1:332:634/2',
                'sequence': 'ACCGTGCCAGATCAGAACCTAGTGGCGATTCCAATT'}
        testcases['HWI-EAS_4_PE-FC20GCB:4:242:843:13/2'] = {
                'index': 3111474L,
		'accuracy': "ABAAACA?CAAA??%A;2A;/5/&:?-*1-'11%71",
                'name': 'HWI-EAS_4_PE-FC20GCB:4:242:843:13/2',
                'sequence': 'GTTTCTATATTCTGGCGTTAGTCGTCGCCGATAATT'}

        for case in testcases:
            assert testcases[case] == self.db[case]
            
            
class Test_po_fasta:
    """
    Test screed methods on the pongo fasta file
    """
    def setup(self):
        self.db = screed.dbread(pongo + '_screed')

    def tearDown(self):
        del self.db
        gc.collect()

    def test_iteration(self):
        """
        Runs through the database, accessing each element by index and then by
        name
        """
        names = []
        for i in range(0, len(self.db)):
            names.append(self.db.loadRecordByIndex(i)['name'])
        assert names == self.db.keys()

        for i in names:
            l = self.db[i]

        del names
        gc.collect()

    def test_dict_stuff(self):
        """
        Tests some dictionary methods on the database
        """
        keys = self.db.keys()
        ikeys = list(self.db.iterkeys())
        assert sorted(keys) == sorted(ikeys)
        del keys
        del ikeys
        gc.collect()
        
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

    def test_certain_records(self):
        """
        Pulls first, last, middle and few other records out of database and
        compares them to known quantities
        """
        testcases = {}
        testcases['GENSCAN00000032971'] = {
		'index': 0L,
                'description': 'cdna:Genscan chromosome:PPYG2:6_qbl_hap2_random:95622:98297:1  ',
                'name' : 'GENSCAN00000032971',
                'sequence': 'ATGGCGCCCCGAACCCTCCTCCTGCTGCTCTCGGCGGCCCTGGCCCCGACCGAGACCTGG'}
        testcases['GENSCAN00000042282'] = {
		'index': 53997L,
                'description': 'cdna:Genscan chromosome:PPYG2:1:229892060:229892800:1  ',
                'name': 'GENSCAN00000042282',
                'sequence': 'ATGATGCCATTGCAAGGACCCTCTGCAGGGCCTCAGTCCCGAGGATGGCACACAGCCTTC'}
        testcases['GENSCAN00000051311'] = {
		'index': 30780L,
                'description' : 'cdna:Genscan chromosome:PPYG2:10:132962172:132962871:1  ',
                'name': 'GENSCAN00000051311',
                'sequence': 'ATGACCCAGCCACCTACCAGGCCGCTCTGCAGACCCCCCACGGGAGCAGCCTCTGCCCCC'}
        testcases['GENSCAN00000006030'] = {
		'index': 1469L,
                'description': 'cdna:Genscan chromosome:PPYG2:14_random:1765749:1766075:-1  ',
                'name': 'GENSCAN00000006030',
                'sequence': 'ATGTGTGGCAACAAGGGCATTTCTGCCTTCCCTGAATCAGACCACCTTTTCACATGGGTA'}
        testcases['GENSCAN00000048263'] = {
		'index': 43029L,
                'description': "cdna:Genscan chromosome:PPYG2:6:100388173:100485454:-1  ",
                'name': 'GENSCAN00000048263',
                'sequence': 'ATGTGTCCCTTTGAATATGCCGGAGAACAACAGTTGCCATGGATGTGTTCTGGGGAGCCC'}

        for case in testcases:
            assert testcases[case]['name'] == self.db[case]['name']
            assert testcases[case]['description'] == self.db[case]['description']
            assert self.db[case]['sequence'].startswith(testcases[case]['sequence'])

class Test_mus_fasta:
    """
    Test screed methods on the mus_musculus fasta file
    """
    def setup(self):
        self.db = screed.dbread(mus + '_screed')

    def tearDown(self):
        del self.db
        gc.collect()

    def test_iteration(self):
        """
        Runs through the database, accessing each element by index and then by
        name
        """
        names = []
        for i in range(0, len(self.db)):
            names.append(self.db.loadRecordByIndex(i)['name'])
        assert names == self.db.keys()

        for i in names:
            l = self.db[i]

        del names
        gc.collect()

    def test_dict_stuff(self):
        """
        Tests some dictionary methods on the database
        """
        keys = self.db.keys()
        ikeys = list(self.db.iterkeys())
        assert sorted(keys) == sorted(ikeys)
        del keys
        del ikeys
        gc.collect()
        
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

    def test_certain_records(self):
        """
        Pulls first, last, middle and few other records out of database and
        compares them to known quantities
        """
        testcases = {}
        testcases['9'] = {
		'index': 0L,
                'description': 'dna_rm:chromosome chromosome:NCBIM37:9:1:124076172:1',
                'name' : '9',
                'sequence': 'NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN'}

        for case in testcases:
            assert testcases[case]['name'] == self.db[case]['name']
            assert testcases[case]['description'] == self.db[case]['description']
            assert self.db[case]['sequence'].startswith(testcases[case]['sequence'])
            
            
class Test_tri_fasta:
    """
    Test screed methods on the tri fasta file
    """
    def setup(self):
        self.db = screed.dbread(tri + '_screed')

    def tearDown(self):
        del self.db
        gc.collect()

    def test_iteration(self):
        """
        Runs through the database, accessing each element by index and then by
        name
        """
        names = []
        for i in range(0, len(self.db)):
            names.append(self.db.loadRecordByIndex(i)['name'])
        assert names == self.db.keys()

        for i in names:
            l = self.db[i]

        del names
        gc.collect()

    def test_dict_stuff(self):
        """
        Tests some dictionary methods on the database
        """
        keys = self.db.keys()
        ikeys = list(self.db.iterkeys())
        assert sorted(keys) == sorted(ikeys)
        del keys
        del ikeys
        gc.collect()
        
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

    def test_certain_records(self):
        """
        Pulls first, last, middle and few other records out of database and
        compares them to known quantities
        """
        testcases = {}
        testcases['singleUn_100'] = {
		'index': 0L,
                'description': '',
                'name' : 'singleUn_100',
                'sequence': 'TTTAAACACGTGTCCGCGCCATTTTTTTATTTATTTACCGATCAAGTGCA'}
        testcases['singleUn_9'] = {
		'index': 2210L,
                'description': '',
                'name': 'singleUn_9',
                'sequence': 'TTTAATTTTTTTACAACTCAAAATTTTGAGTAGTGTTTTAAATAGTACAC'}
        testcases['ChLG6'] = {
		'index': 2016L,
                'description' : '',
                'name': 'ChLG6',
                'sequence': 'CAAAAAAATTCATAACTCAAAAACTAAAAGTCGTAGAGCAATGCGGTTTG'}
        testcases['singleUn_286'] = {
		'index': 186L,
                'description': '',
                'name': 'singleUn_286',
                'sequence': 'AAACTAAAACATCCTTTTCAGCATATTATTTGTTATATTTAAAAAAAAAC'}
        testcases['ChLG9'] = {
		'index': 2019L,
                'description': '',
                'name': 'ChLG9',
                'sequence': 'CTGCCGATAATATTTCCTACCAGAAATAACCAATTTATTTTACGTATTAC'}

        for case in testcases:
            assert testcases[case]['name'] == self.db[case]['name']
            assert testcases[case]['description'] == self.db[case]['description']
            assert self.db[case]['sequence'].startswith(testcases[case]['sequence'])
            

class Test_xeno_fasta:
    """
    Test screed methods on the xeno fasta file
    """
    def setup(self):
        self.db = screed.dbread(xeno + '_screed')

    def tearDown(self):
        del self.db
        gc.collect()

    def test_iteration(self):
        """
        Runs through the database, accessing each element by index and then by
        name
        """
        names = []
        for i in range(0, len(self.db)):
            names.append(self.db.loadRecordByIndex(i)['name'])
        assert names == self.db.keys()

        for i in names:
            l = self.db[i]

        del names
        gc.collect()

    def test_dict_stuff(self):
        """
        Tests some dictionary methods on the database
        """
        keys = self.db.keys()
        ikeys = list(self.db.iterkeys())
        assert sorted(keys) == sorted(ikeys)
        del keys
        del ikeys
        gc.collect()
        
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

    def test_certain_records(self):
        """
        Pulls first, last, middle and few other records out of database and
        compares them to known quantities
        """
        testcases = {}
        testcases['scaffold_20095'] = {
		'index': 0L,
                'description': 'dna:scaffold scaffold:JGI4.1:scaffold_20095:1:2001:1',
                'name' : 'scaffold_20095',
                'sequence': 'GATGAGATCACCTTTCATGCTTTTTGTATCCCTATTATCTAGAGACAACAAATCAGTTGC'}
        testcases['scaffold_1'] = {
		'index': 19500L,
                'description': 'dna:scaffold scaffold:JGI4.1:scaffold_1:1:7817814:1',
                'name': 'scaffold_1',
                'sequence': 'CCTCCCTTTTTGGCTGTCTTTTCACTGTATCATAGCCTGGCGTGAACCAAGCCTCAAAAA'}
        testcases['scaffold_271'] = {
		'index': 19230L,
                'description' : 'dna:scaffold scaffold:JGI4.1:scaffold_271:1:1567461:1',
                'name': 'scaffold_271',
                'sequence': 'CGATTTTTGCGGAAAAACGCGAGTTTTTGGTAGCCATTCCGAAAGTTGCGATTTTTTGTA'}
        testcases['scaffold_19901'] = {
		'index': 329L,
                'description': 'dna:scaffold scaffold:JGI4.1:scaffold_19901:1:2256:1',
                'name': 'scaffold_19901',
                'sequence': 'ATACCGCAAAGGTTTCTTTCTTCTCAGTGCTCCATGCTGCCTCTCTTGTTTTGCCTCCCT'}
        testcases['scaffold_95'] = {
		'index': 19408L,
                'description': 'dna:scaffold scaffold:JGI4.1:scaffold_95:1:2899670:1',
                'name': 'scaffold_95',
                'sequence': 'CCCTCCTGGTGATCCCACTTCAATCTCCCCATAGGCACACATCACTTCTAGCAGTTCACA'}

        for case in testcases:
            assert testcases[case]['name'] == self.db[case]['name']
            assert testcases[case]['description'] == self.db[case]['description']
            assert self.db[case]['sequence'].startswith(testcases[case]['sequence'])
