import gzip
import sys

def main(filename):

    prot_event_pairs = []
    events = set()
    prot2events = {}
  
    idx = 0
    p2idx = {}
    e2idx = {}
    with gzip.open(filename, u'r') as f, gzip.open("sample.in.gz", "w") as o:
        while 1:
            line = f.readline()
            if line == '':
                break
            items = line.strip().split('\t')
            assert len(items) == 3
            
            while 1:
                line = f.readline().strip()
                if line == '':
                    break
                items = line.split('\t')
                if len(items) == 3:
                    prot = items[0]
                    marg_event = items[1]
                    marg_count = int(items[2])
                    for i in range(marg_count):
                        o.write('{}\t{}\n'.format(prot, marg_event))
                    if prot not in p2idx:
                        p2idx[prot] = idx
                        idx += 1 
                    if marg_event not in e2idx:
                        e2idx[marg_event] = idx
                        idx += 1

    with gzip.open("sample.dict.gz", "w") as df:
        for prot, idx in p2idx.iteritems():
            df.write("p\t{}\t{}\n".format(prot, idx))
        for event, idx in e2idx.iteritems():
            df.write("e\t{}\t{}\n".format(event, idx))


if __name__ == '__main__':
    if len(sys.argv) == 2:
        main(sys.argv[1])
