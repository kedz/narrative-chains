import numpy as np
import gzip
import wordcooc.chambers
import wordcooc.timer as timer


import sys

if len(sys.argv) != 2:
    sys.exit()

path = sys.argv[1]

dims = 500
max_iters = 100
learning_rate = 0.1


#path = u'/home/kedz/projects/narrative-chains/narrative-chains/python/nc-protagonist-counts-small.gz'
#path = u'/home/kedz/projects/narrative-chains/narrative-chains/python/nc-protagonist-counts.gz'

sampler = timer.timed_function(
    wordcooc.chambers.read_protagonist_model_counts_to_sampler, path,
    "Reading counts file...")

print "Writing matrix indices to file..."
with gzip.open("weight-matrix-dict.txt.gz", "w") as f:
    for idx, token in sampler.idx2event_dict_.iteritems():
        f.write('{}\t{}\n'.format(idx, token))
        f.flush() 


t = timer.Timer()
t.start("Initializing weight matrix memory...")
def hinge_loss(u, v, v_neg):
    return 1 - np.dot(u.T, v) + np.dot(u.T, v_neg)

W = np.random.normal(size=(sampler.num_events, dims))

v1_update = np.zeros((1,dims))
v2_update = np.zeros((1,dims))
v_neg_update = np.zeros((1,dims))

num_samples = 0
print '\t{}'.format(timer.pretty_time(t.stop()))

t.reset()

print "Training narrative chains (protagonist) model verb embeddings",
print "for {} iterations".format(max_iters)

t.start()
t.set_timer(1800)
for num_iter in range(max_iters):
    
    print "Iteration #{}\t (elapsed time: {})".format(
        num_iter+1, timer.pretty_time(t.elapsed()))
    total_loss = 0.0
    tot_neg_samples = 0
    
    for num_sampled, (idx1, idx2) in enumerate(sampler.pos_sample_seq_iter()):
        v1 = W[idx1,:]
        v2 = W[idx2,:]
        
        num_neg_samples = 0
        while 1:
            num_neg_samples += 1
            idx_neg = sampler.negative_sample(idx1)
            v_neg = W[idx_neg,:]
            loss = hinge_loss(v1, v2, v_neg)

            if loss > 0 or num_neg_samples > sampler.num_events:
                break
        
        if num_neg_samples > sampler.num_events:
            print sampler.get_word(idx1), sampler.get_word(idx2), "?"
            continue
        tot_neg_samples += num_neg_samples        
        num_samples += 1
        total_loss += loss
       
        v1_update.fill(0.0)
        v2_update.fill(0.0)
        v_neg_update.fill(0.0)
         
        v1_update = v1 - learning_rate * (-v1 + v_neg)
        v2_update = v2 - learning_rate * (-v1)
        v_neg_update = v_neg - learning_rate * v1

        W[idx1,:] = v1_update / np.linalg.norm(v1_update)    
        W[idx2,:] = v2_update / np.linalg.norm(v2_update)       
        W[idx_neg,:] = v_neg_update / np.linalg.norm(v_neg_update)   

        if num_sampled % 10000 == 0:
            t.check_timer("==> Iteration #{} || Sample {} / {} <==".format(
                num_iter+1, num_sampled, len(sampler.event_event_samples_)))
    
    print "\t{:0.3f} samples/sec".format(
        float(num_sampled + 1) / t.lap())
    print "\tAvg. Loss: {:0.5f}".format(total_loss / float(num_sampled + 1))
    print "\tAvg. Neg. Samples: {}\n".format(
        tot_neg_samples / float(num_sampled + 1))
    np.savetxt("weights.txt", W)
            
print "Training complete ({} total samples, time elapsed: {})".format(
    num_samples, timer.pretty_time(t.stop()))

np.savetxt("weights.txt", W)

