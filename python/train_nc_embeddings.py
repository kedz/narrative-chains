import numpy as np
import theano
import theano.tensor as T
import wordcooc
import time
import sys
from sklearn.metrics.pairwise import cosine_similarity
import signal

def main(pair_file, dict_file):



    #get_loss = theano.function(inputs=[p, u, v], outputs=loss)

    print "Reading counts...",
    start = time.time()
    sampler = wordcooc.load_sampler(pair_file, dict_file)
    stop = time.time()
    print "{} elapsed".format(wordcooc.pretty_time(stop-start))

    print "Sampler loaded with:"
    print "\t{} positive events".format(sampler.num_pe_pairs)
    print "\t{} protagonists".format(sampler.num_protags)
    print "\t{} events".format(sampler.num_events) 



    total_updates = 0

    W_py = sampler.init_weights(50)
    ##W_py.fill(10)
#    t_index = np.zeros((1, W_py.shape[0]))
#    u_index = np.zeros((1, W_py.shape[0]))
#    v_index = np.zeros((1, W_py.shape[0]))
#    t = T.matrix('t', dtype='float64')



    u = T.matrix('u', dtype='float64')
    v = T.matrix('v', dtype='float64')

    W = theano.shared(W_py, name='W')

    hinge_loss = T.sum(T.maximum(0, \
        1 + .25 * T.dot(T.dot(u, W), T.transpose(T.dot(v, W)))))

    learning_rate = 1
    g_W = T.grad(cost=hinge_loss, wrt=W)
    updates = [(W, W - learning_rate * g_W)]
    learn = theano.function(inputs=[u, v],
                            outputs=hinge_loss,
                            updates=updates)        
                       # //givens={p: p_real, u: u_real, v: v_real})


    V = W_py.shape[0]
    batchsize = 2000
    start = time.time()
    alert_start = time.time()

    def signal_handler(signal, frame):
            print('Writing...')
            np.savetxt("weights.gz", W.get_value())
    signal.signal(signal.SIGINT, signal_handler)

    num_batches = 0
    for num_iter in range(100):
        #print "iter", num_iter
        print "Iter", num_iter 
            

        for P, N in sampler.batch_sample_seq_iter(batchsize):
            num_batches += 1
                
            bloss = learn(P, N)
            this_bsize = P.shape[0]
            total_updates += this_bsize
            avg_loss = bloss / float(this_bsize)
            now = time.time()
            if now - alert_start > 60:
                up_per_sec = total_updates / float(now-start)
                print "{:0.5f} samples per sec".format(up_per_sec)
                print "Batchsize: {} / Batchloss: {}".format(this_bsize, avg_loss)
                alert_start = now
                
                
        np.savetxt("weights.gz", W.get_value())

    stop = time.time()
    print "Total time: {}".format(wordcooc.pretty_time(stop-start))
    print "Time 100,000 protag per sec {:0.5f}".format(100000 * \
        (stop-start) / float(total_updates))
#    print W_k

if __name__ == u'__main__':

    if len(sys.argv) != 3:
        print "Please supply a pair file and dict file"
    else:
        main(sys.argv[1], sys.argv[2])
