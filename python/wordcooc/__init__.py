import gzip
import numpy as np
import random

class NarChainCounts:
    def __init__(self, prot_marg_total, 
                 prot_joint_total,
                 prot_marg_counts,
                 prot_joint_counts, events):
    
        self.prot_marg_total = prot_marg_total
        self.prot_joint_total = prot_joint_total
        self.prot_marg_counts = prot_marg_counts
        self.prot_joint_counts = prot_joint_counts
        self.events = events
        self.prot2idx = None
        self.prot_dist = None
        self.prot_event_dist = None

    def init_sampler(self):
        random.shuffle(self.events)
        idx2prot = []
        prot_dist = np.zeros((len(self.prot_marg_total)))
        total = 0
        for idx, (prot, tot) in enumerate(self.prot_marg_total.iteritems()):
            idx2prot.append(prot)
            total += tot
            prot_dist[idx] = float(tot)
        self.prot_dist = prot_dist / float(total)    
        self.idx2prot = idx2prot
      
        prot_event_dist = {} 
        for prot, mc in self.prot_marg_counts.iteritems():
            idx2event = []
            event_dist = np.zeros((len(mc)))
            for idx, (event, cnt) in enumerate(mc.iteritems()):
                event_dist[idx] = float(cnt)
                idx2event.append(event)
            event_dist = event_dist / float(self.prot_marg_total[prot])
            prot_event_dist[prot] = (idx2event, event_dist)
        self.prot_event_dist = prot_event_dist            
    def sample_protag(self):
        idx = np.nonzero(np.random.multinomial(1, self.prot_dist))[0][0]
        return self.idx2prot[idx]
    
    def sample_event(self, prot):
        mc = self.prot_marg_counts[prot]
        idx2event, event_dist = self.prot_event_dist[prot]
        idx = np.nonzero(np.random.multinomial(1, event_dist))[0][0]
        return idx2event[idx]    

    def sample_seq_iter(self):
        
        num_events = len(self.events)
        protags_counts = self.prot_marg_counts.items()
        
        random.shuffle(protags_counts)

        neg_event_idx = 0        
        for prot, mc in protags_counts:
            event = self.sample_event(prot)    
            while 1:
                neg_event = self.events[neg_event_idx]
                neg_event_idx += 1
                if neg_event_idx >= num_events:
                    neg_event_idx = 0
                #p2 = self.sample_protag()
                #neg_event = next_neg_event(prot, mc)
                #self.sample_event(p2)
                if mc.get(neg_event, None) is None:
                    break
            
            yield prot, event, neg_event

    def init_weights(self, d):
        idx = 0
        word2idx = {}
        for event in self.events:
            word2idx[event] = idx
            idx += 1
        for prot in self.prot_marg_counts.iterkeys():
            word2idx[prot] = idx
            idx += 1

        return np.zeros((idx, d)), word2idx
            

def read_nar_chain_counts(filename):

    prot_marg_total = {}
    prot_joint_total = {}
    prot_marg_counts = {}
    prot_joint_counts = {}
    events = set()
    with gzip.open(filename, u'r') as f:
        while 1:
            line = f.readline()
            if line == '':
                break
            items = line.strip().split('\t')
            assert len(items) == 3
            prot = items[0]
            marg_total = int(items[1])
            joint_total = int(items[2])
            
            assert prot_marg_total.get(prot, None) == None
            prot_marg_total[prot] = marg_total

            assert prot_joint_total.get(prot, None) == None
            prot_joint_total[prot] = joint_total
            while 1:
                line = f.readline().strip()
                if line == '':
                    break
                items = line.split('\t')
                if len(items) == 3:
                    prot = items[0]
                    marg_event = items[1]
                    marg_count = int(items[2])
                    events.add(marg_event)
                     
                    mc = prot_marg_counts.get(prot, None)
                    if mc is None:
                        mc = {}
                        prot_marg_counts[prot] = mc

                    assert mc.get(marg_event, None) == None
                    mc[marg_event] = marg_count

                if len(items) == 4:
                    prot = items[0]
                    event1 = items[1]
                    event2 = items[2]
                    joint_event = '{}\t{}'.format(event1, event2)
                    joint_count = int(items[3])
                     
                    jc = prot_joint_counts.get(prot, None)
                    if jc is None:
                        jc = {}
                        prot_joint_counts[prot] = jc

                    assert jc.get(joint_event, None) == None
                    jc[joint_event] = joint_count


    nccounts = NarChainCounts(prot_marg_total, prot_joint_total,
                              prot_marg_counts, prot_joint_counts, 
                              list(events))
 
    return nccounts

def pretty_time(secs):
    hours, remainder = divmod(secs, 3600)
    minutes, seconds = divmod(remainder, 60)
    return '{:6.0f}h{:02.0f}m{:0.3f}s'.format(hours, minutes, seconds)
