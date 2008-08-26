# TODO
#  - python interface to indexer for fa, fq
#  - exceptions!

RUN_CRASH_TESTS = True

import sys
sys.path.insert(0, 'bin/')
import _seqdb2

# create index files
import subprocess
subprocess.check_call('./bin/fadbm tests/test.fa', shell=True)
subprocess.check_call('./bin/fqdbm tests/test.fastq', shell=True)

fa = _seqdb2.dbread('tests/test.fa_seqdb2')
fq = _seqdb2.dbread('tests/test.fastq_seqdb2')

# loading out-of-bounds record num -- should raise an error
fa.loadRecord(-1)

# loading a weird field name doesn't return an error
fa.loadRecord(0)
print fa.getFieldValue('foobar')

foo = _seqdb2.dbread('FOO')

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

fa.loadRecord(-1)

sys.exit(0)

