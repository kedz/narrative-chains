import argparse
import os
import gzip

import numpy as np
import wordcooc.protagonist
import wordcooc.timer as timer

def write_dictionary(sampler, dict_file):
    if dict_file.endswith(u'.gz'):
        f = gzip.open(dict_file, u'w')
    else:
        f = open(dict_file, u'w')
    
    for idx, token in sampler.idx2protag_dict_.iteritems():
        f.write('P_{}\t{}\n'.format(idx, token))
        f.flush() 
    for idx, token in sampler.idx2event_dict_.iteritems():
        f.write('E_{}\t{}\n'.format(idx, token))
        f.flush() 
    f.close()

def init_mem(vocab_size, num_dims):
    W = np.random.normal(size=(vocab_size, num_dims)) 
    p_update = np.zeros((1,num_dims))
    v1_update = np.zeros((1,num_dims))
    v2_update = np.zeros((1,num_dims))
    vn1_update = np.zeros((1,num_dims))
    vn2_update = np.zeros((1,num_dims))
    return (W, p_update, v1_update, v2_update, vn1_update, vn2_update)
   
def start_msg(counts_file, dict_file, matrix_file, freq_cutoff, num_dims, 
              max_iters, find_max, is_directional, learning_rate, alert_rate):
    print 'Narrative Chain Embeddings'
    print '=========================='
    print 'Reading counts from\n\t\t==> {}'.format(counts_file)
    print 'Writing dictionary to\n\t\t==> {}'.format(dict_file)
    print 'Writing embedding matrix\n\t\t==> {}'.format(matrix_file)
    print
    print '========================== Settings =========================='
    print
    print '{:45s}\t{}'.format('Embedding dimensionality', num_dims)
    print '{:45s}\t{}'.format('(Sampler) Event frequency cutoff',freq_cutoff)
    print '{:45s}\t{}'.format('(Sampler) Directionality on', is_directional)
    print '{:45s}\t{}'.format('Find max loss?', find_max)
    print '{:45s}\t{}'.format('Max iterations', max_iters)
    print '{:45s}\t{}'.format('Learning rate', learning_rate)
    print '{:45s}\t{}'.format('Alert Rate (secs)', alert_rate)

def main(counts_file, dict_file, matrix_file, freq_cutoff, num_dims, 
         max_iters, find_max, is_directional, learning_rate, alert_rate):

    start_msg(counts_file, dict_file, matrix_file, freq_cutoff, num_dims, 
              max_iters, find_max, is_directional, learning_rate, alert_rate)

    print 
    print '========================== Initializing =========================='
    print 

    sampler = timer.timed_function(
        wordcooc.protagonist.read_model_counts_to_sampler,
        [counts_file, freq_cutoff, False],
        'Reading counts file...              ')

    timer.timed_function(
        write_dictionary, [sampler, dict_file],
        'Writing matrix indices to file...   ')    
    
    num_events = len(sampler.idx2event_dict_)
    num_protags = len(sampler.idx2protag_dict_)
    vocab_size = num_events + num_protags 
    W, vp_update, v1_update, v2_update, vn1_update, vn2_update = \
        timer.timed_function(
            init_mem, [vocab_size, num_dims],
            'Initializing matrix/vector memory...')

    print '{:45s}\t{}'.format('Corpus Size', 
        len(sampler.prot_event_event_samples_))
    print '{:45s}\t{}'.format('Vocab Size', 
        '{} (protags {}, events: {}'.format(vocab_size, 
            num_protags, num_events))

    def find_max_loss(r1, r2):
        v1_col = W[r1,:].T
        P = np.dot(W, v1_col)
        II = np.argsort(-P)
        is_loss = False
        for r_neg in II:
            if r_neg != r1 and r_neg != r2:
                if r_neg not in sampler.idx2cooc_[r1]:
                    is_loss = True
                    break 
            
        if is_loss is True:
            loss = 1 - np.dot(W[r2,:], v1_col) + P[r_neg]
            if loss > 0:
                return (loss, r_neg)
        return None

    global total_neg_samples
    total_neg_samples = 0
    def sample_loss(p, e1, e2):
        global total_neg_samples
        num_neg_samples = 0
        vp = W[p,:]
        v1 = W[e1,:]
        v2 = W[e2,:]
        sim = np.dot(vp, (v1+v2).T)

        while num_neg_samples < 15: #len(sampler.event_samples_):
            num_neg_samples += 1
            neg1, neg2 = sampler.negative_sample(p)
            vn1 = W[neg1,:]
            vn2 = W[neg2,:]

            loss = 1 - sim + np.dot(vp, (vn1+vn2).T) # hinge_loss(v1, v2, v_neg)
            if loss > 0: # or num_neg_samples > sampler.num_events:
                total_neg_samples += num_neg_samples
                return (loss, neg1, neg2)
        total_neg_samples += num_neg_samples
        return None        

    if find_max is True:
        compute_loss = None # find_max_loss
    else:
        compute_loss = sample_loss

    
    fmt = "==> Iteration #{} || Sample {} / {} || Avg Loss {:0.3f}"
    if find_max is False:
        fmt += "\n\tAvg Neg Sample / Pos Sample {}"

    def update_weights(p, r1, r2, rn1, rn2):

        # Get current rows
        vp = W[p,:]
        v1 = W[r1,:]
        v2 = W[r2,:]
        vn1 = W[rn1,:]
        vn2 = W[rn2,:]

        # Compute updates
        vp_update = vp - learning_rate * (-v1 - v2 + vn1 + vn2)
        v1_update = v1 - learning_rate * (-vp)
        v2_update = v2 - learning_rate * (-vp)
        vn1_update = vn1 - learning_rate * (vp)
        vn2_update = vn2 - learning_rate * (vp)

        # Project onto the L2 ball updates that have a magnitude > 1.
        v1_norm = np.linalg.norm(v1_update)
        if (v1_norm > 1):
            W[r1,:] = v1_update / v1_norm 
        else:
            W[r1,:] = v1_update  

        v2_norm = np.linalg.norm(v2_update)
        if (v2_norm > 1):
            W[r2,:] = v2_update / v2_norm
        else:
            W[r2,:] = v2_update 

        vn1_norm = np.linalg.norm(vn1_update)
        if (vn1_norm > 1):
            W[rn1,:] = vn1_update / vn1_norm
        else:
            W[rn1,:] = vn1_update

        vn2_norm = np.linalg.norm(vn2_update)
        if (vn2_norm > 1):
            W[rn2,:] = vn2_update / vn2_norm
        else:
            W[rn2,:] = vn2_update

        vp_norm = np.linalg.norm(vp_update)
        if (vp_norm > 1):
            W[p,:] = vp_update / vp_norm 
        else:
            W[p,:] = vp_update  

       

    total_num_sample = 0
    t = timer.Timer()

    print 
    print '============================ Training ============================'
    print 

    t.start()
    t.set_timer(alert_rate)
    for num_iter in range(1, max_iters+1):
    
        print "Iteration #{}\t (elapsed time: {})".format(
            num_iter, timer.pretty_time(t.elapsed()))
            #total_loss = 0.0
            #tot_neg_samples = 0
        total_neg_samples = 0
        tot_loss = 0    
            
        for num_sample, (p, e1, e2) in enumerate(
            sampler.pos_sample_seq_iter(), 1):
            total_num_sample += 1
            #print sampler.get_protag_str(p),
            #print sampler.get_event_str(e1), sampler.get_event_str(e2)


            loss_row_neg = compute_loss(p, e1, e2)
            if loss_row_neg is not None:
                loss, neg1, neg2 = loss_row_neg
                #print '\t', sampler.get_event_str(neg1), 
                #print sampler.get_event_str(neg2), loss
                #print sampler.get_word(rn), loss  
                update_weights(p, e1, e2, neg1, neg2)
                tot_loss += loss

            if num_sample % 10000 == 0:
                
                if find_max is True:
                    t.check_timer(fmt.format(num_iter, num_sample,
                        len(sampler.event_event_samples_),
                        tot_loss / float(num_sample)))
                else:
                    t.check_timer(fmt.format(num_iter, num_sample,
                        len(sampler.prot_event_event_samples_),
                        tot_loss / float(num_sample),
                        total_neg_samples / float(num_sample)))


        print "\t{:0.3f} samples/sec".format(
            float(total_num_sample) / t.lap())
        
        timer.timed_function(np.savetxt, [matrix_file, W],
            'Writing embedding matrix to file {} ...'.format(matrix_file))

def validate_file(parser, arg):
    path = os.path.dirname(arg)
    if path != u'' and not os.path.exists(path):
        os.makedirs(path)
    return arg

def validate_pos_int(parser, arg):
    iarg = int(arg)
    if iarg <= 0:
        parser.error(u'Argument {} must be a positive integer.'.format(arg))
    else:
        return iarg

def validate_pos_float(parser, arg):
    farg = float(arg)
    if farg <= 0:
        parser.error(
            u'Argument {} must be a positive floating point num.'.format(arg))
    else:
        return farg
       

def validate_input(parser, arg):
    if not os.path.exists(arg):
        parser.error(u'The file {} does not exist!'.format(arg))
    else:
        return arg

if __name__ == u'__main__':

    parser = argparse.ArgumentParser(description=u'Train word embeddings.')
    parser.add_argument(u'--find-max', dest=u'find_max',
                        action=u'store_const', const=True, default=False,
                        help=u'Find max possible loss at each iteration.')

    parser.add_argument(u'--freq-cutoff', dest=u'freq_cutoff', type=int,
                        default=0, metavar=u'F',
                        help=u'Prune events that occur less than F times.')

    parser.add_argument(u'--matrix-file', u'-m', dest=u'matrix_file',
                        type=lambda x: validate_file(parser, x), 
                        default=u'embeddings.gz', 
                        metavar=u'FILE',
                        help=u'Location to write embedding matrix.')

    parser.add_argument(u'--dict-file', u'-d', dest=u'dict_file',
                        type=lambda x: validate_file(parser, x), 
                        default=u'embeddings-dict.gz', 
                        metavar=u'FILE',
                        help=u'Location to write dict mapping words to rows.')
    
    parser.add_argument(u'--counts-file', u'-c', dest=u'counts_file',
                        required=True,
                        type=lambda x: validate_input(parser, x),
                        metavar=u'FILE', help=u'Location to read counts.')

    parser.add_argument(u'--dim', dest=u'num_dims',
                        type=lambda x: validate_pos_int(parser, x),
                        metavar=u'D', help=u'Dimension of embeddings.',
                        default=50)
 
    parser.add_argument(u'--iters', dest=u'max_iters',
                        type=lambda x: validate_pos_int(parser, x),
                        metavar=u'I', help=u'Number of training iterations',
                        default=20)

    parser.add_argument(u'--directional', dest=u'is_directional',
                        action=u'store_true', default=False,
                        help=u'When \'directional\', event order matters.')
    
    parser.add_argument(u'--learning-rate', dest=u'learning_rate',
                        type=lambda x: validate_pos_float(parser, x),
                        default=0.1,
                        help=u'Learning rate for gradient descent.')

    parser.add_argument(u'--alert-rate', dest=u'alert_rate',
                        type=lambda x: validate_pos_int(parser, x),
                        default=600, metavar=u'SECS',
                        help=u'Time in seconds to print training updates.')

 
    args = parser.parse_args()
    main(args.counts_file, args.dict_file, args.matrix_file,  
         args.freq_cutoff, args.num_dims, args.max_iters, args.find_max,
         args.is_directional, args.learning_rate, args.alert_rate)
    #print args.accumulate(args.integers)


