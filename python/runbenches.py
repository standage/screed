import benchmark
import os
import sys
import subprocess

# sys.argv[1] is the filename containing the list of files to run the
# benchmarks on

if __name__ == '__main__':
    filenames = []
    alg = sys.argv[2]
    fp = open(sys.argv[1], 'rb')
    for line in fp:
        line = line.strip()
        if os.path.exists(line):
            filenames.append(line)
    fp.close()
    for file in filenames: # create the databases if necessary
#        if os.path.exists('%s_seqdb2' % file):
#            continue
        if file.endswith('.fa'):
            subprocess.check_call('../bin/fadbm %s' % file,
                    shell=True, stdout=subprocess.PIPE)
        elif file.endswith('.fastq'):
            subprocess.check_call('../bin/fqdbm %s' % file,
                      shell=True, stdout=subprocess.PIPE)
        else: # can't tell if fasta or fastq
            filenames.remove(file)
            print "REMOVED %s" % file

    for file in filenames: # do the benchmarking
        file = '%s_seqdb2' % file
        print "DOING: %s" % os.path.split(file)[1]
        benchmark.doBenchmark(file, alg)
