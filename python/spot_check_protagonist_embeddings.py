import gzip
import numpy as np
import time
import scipy.spatial
import os

dict_path = u'weight-matrix-dict.txt.gz'
weight_matrix_path = u'weights.txt'
W = None


e2i = {}
i2e = {}

with gzip.open(dict_path, u'r') as f:

    for line in f:
        items = line.strip().split('\t')
        assert len(items) == 2
        idx = int(items[0])
        event = items[1]
        e2i[event] = idx
        i2e[idx] = event



while 1:

    for idx, event in i2e.iteritems():
        if W is None:
            W = np.loadtxt(weight_matrix_path)
            D = scipy.spatial.distance.pdist(W, u'cosine')
            last_mod = time.ctime(os.path.getmtime(weight_matrix_path))
#        elif last_mod != time.ctime(os.path.getmtime(weight_matrix_path)):
#            print "Reloading weights..."
#            W = np.loadtxt(weight_matrix_path)
#            D = scipy.spatial.distance.pdist(W, u'cosine')
#            last_mod = time.ctime(os.path.getmtime(weight_matrix_path))



        v = W[idx,:].T
        print event
        dists = W[idx,:].T
        II_dists = np.argsort(dists)

        dots = np.dot(W, v)
        II = np.argsort(dots)[::-1]
        for i1, i2 in zip(II[1:11], II_dists[0:10]):
            print "\t{:20s} {:0.3f}\t {:20s} {:0.3f}".format(
                i2e[i1], dots[i1], i2e[i2],  -(dists[i2] - 1))
        
        time.sleep(5)

