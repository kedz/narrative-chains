import wordcooc
import time
import sys

def main(path):

    print "Reading counts...",
    start = time.time()
    nc = wordcooc.read_nar_chain_counts(path)
    stop = time.time()
    print "{} elapsed".format(wordcooc.pretty_time(stop-start))

    print "Initializing sampler...",
    start = time.time()
    nc.init_sampler()
    stop = time.time()
    print "{} elapsed".format(wordcooc.pretty_time(stop-start))

    total_updates = 0

    W, word2row = nc.init_weights(100)

    start = time.time()
    for num_iter in range(10):
        for prot, event, neg_event in nc.sample_seq_iter():
            total_updates += 1
            #print prot, event, neg_event
            
            ###
            # Do dot product and grad descent here
            ###

    stop = time.time()
    print "Total time: {}".format(wordcooc.pretty_time(stop-start))
    print "Time 100,000 protag per sec {:0.5f}".format(100000 * \
        (stop-start) / float(total_updates))

if __name__ == u'__main__':

    if len(sys.argv) != 2:
        print "Please supply a counts file."
    else:
        main(sys.argv[1])
