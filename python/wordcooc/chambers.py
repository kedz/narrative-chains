import gzip
from math import log, isnan
from collections import OrderedDict, defaultdict
import random

class ProtagonistModel(object):
    def __init__(self, marg_counts, joint_counts,
                 marg_total, joint_total, directional=False):
        self.marg_counts = marg_counts
        self.joint_counts = joint_counts
        self.marg_total = marg_total
        self.joint_total = joint_total
        self.directional = directional


    def pmi(self, event1, event2):
        
        e1_count = self.marg_counts.get(event1, 0)
        e2_count = self.marg_counts.get(event2, 0)
        if e1_count == 0 or e2_count == 0:
            return float('nan')
        
        if self.directional is False:
            joint_key = self._joint_key(event1, event2)
        else:
            joint_key = '{}\t{}'.format(event1, event2)

        joint_count = self.joint_counts.get(self._joint_key(event1, event2), 0)
        if joint_count == 0:   
            return float('nan')
        
        return log(joint_count * self.marg_total * self.marg_total) - \
            log(e1_count * e2_count * self.joint_total)     
             
    def _joint_key(self, event1, event2):
        if event1 < event2:
            return '{}\t{}'.format(event1, event2)
        else:
            return '{}\t{}'.format(event2, event1)

    def avg_pmi(self, events, skip_nan=True):
        tot_pmi = 0
        tot_pairs = 0
        for idx1, event1 in enumerate(events):
            for event2 in events[idx1+1:]:
                pmi = self.pmi(event1, event2)
                if pmi is not float('nan') or skip_nan is True:
                    tot_pmi += pmi
                    tot_pairs += 1


        return tot_pmi / float(tot_pairs)

    def rank(self, event1):
        ranked_items = []
        for event2 in self.marg_counts.iterkeys():
            pmi = self.pmi(event1, event2)
            if isnan(pmi) is not True:
                ranked_items.append((pmi, intern(event2)))
        ranked_items.sort(key=lambda x: x[0])
        return ranked_items


        
def read_protagonist_model_counts(path):
    marg_counts = OrderedDict()
    joint_counts = {}
    marg_total = 0 
    joint_total = 0

    with gzip.open(path, u"r") as f:
        for line in f:
            items = line.strip().split('\t')    
            if len(items) == 2:
                event = items[0]
                count = int(items[1])
                marg_counts[intern(event)] = count
                marg_total += 1
            if len(items) == 3:
                event1 = items[0]
                event2 = items[1]
                count = int(items[2])
                joint_counts['{}\t{}'.format(event1, event2)] = count
                joint_total += 1

    return ProtagonistModel(marg_counts, joint_counts, marg_total, joint_total)


class ProtagonistModelSampler:
    def __init__(self, idx2event_dict, 
        event_samples, event_event_samples, idx2cooc, is_direct):

        self.idx2event_dict_ = idx2event_dict
        
        self.event_samples_ = event_samples
        random.shuffle(self.event_samples_)  
        self.neg_sample_idx_ = 0

        self.event_event_samples_ = event_event_samples
        random.shuffle(self.event_event_samples_)
        self.pos_sample_idx_ = 0

        self.idx2cooc_ = idx2cooc
        self.num_events = len(idx2event_dict)
        self.is_direct = is_direct


    def positive_sample(self):
        if self.pos_sample_idx_ >= len(self.event_event_samples_):
            self.pos_sample_idx_ = 0
            random.shuffle(self.event_event_samples_)
        sample = self.event_event_samples_[self.pos_sample_idx_]
        self.pos_sample_idx_ += 1
        return sample

    def negative_sample(self, idx1):
        
        while 1:
            if self.neg_sample_idx_ >= len(self.event_samples_):
                self.neg_sample_idx_ = 0
                random.shuffle(self.event_samples_)

            idx2 = self.event_samples_[self.neg_sample_idx_]      
            self.neg_sample_idx_ += 1

            if idx2 not in self.idx2cooc_[idx1]:
                break
        return idx2              

    def pos_sample_seq_iter(self):
        self.pos_sample_idx_ = 0
        while self.pos_sample_idx_ < len(self.event_event_samples_):
            yield self.positive_sample()


    def get_word(self, idx):
        return self.idx2event_dict_.get(idx, None)

    
    
def read_protagonist_model_counts_to_sampler(path, freq_cutoff,
                                             is_directional):
 
    event_samples = []
    event_event_samples = []  

    event2idx = OrderedDict()
    idx2event = OrderedDict()
    e2e = defaultdict(set)

    idx2cooc = []

    idx = 0
    with gzip.open(path, u'r') as f:
        for line in f:
            line = line.strip()
            items = line.split("\t")
            if len(items) == 2:
                event = intern(items[0])
                count = int(items[1])
                if count > freq_cutoff:
                    if idx2event.get(idx, None) is None:
                        idx2event[idx] = event
                        event2idx[event] = idx
                        for i in range(count):
                            event_samples.append(idx)
                        idx2cooc.append(set())
                        idx += 1
    
            elif len(items) == 3:
                event1 = intern(items[0])
                event2 = intern(items[1])
                if event1 == event2:
                    continue
                if event1 not in event2idx:
                    continue
                if event2 not in event2idx:
                    continue

                count = int(items[2]) 
                idx1 = event2idx[event1]
                idx2 = event2idx[event2]
                idx2cooc[idx1].add(idx2)
                if is_directional is False:
                    idx2cooc[idx2].add(idx1)
                for i in range(count):
                    event_event_samples.append((idx1, idx2))

    event2idx = None
    return ProtagonistModelSampler(
        idx2event, event_samples, event_event_samples,
        idx2cooc, is_directional)
                     
               # print event, count
                #prot_event_pairs.append((intern(prot), intern(event)))
            #prot2events[intern(prot)].add(intern(event))
            #events.add(intern(event))
#    with gzip.open(dict_file, u'r') as f:
#        for line in f:
#            items = line.strip().split("\t")
#            assert len(items) == 3
#            typ, token, idx = items
#            idx = int(idx)
#            if typ == "p":
#                p2idx[intern(token)] = idx
#            else:
#                e2idx[intern(token)] = idx
#            if idx > max_idx:
#                max_idx = idx
#            vocab_size += 1
#    assert vocab_size == max_idx + 1
#    assert vocab_size == len(p2idx) + len(e2idx)
#
#    return ProtEventSample(
#        prot_event_pairs, events, prot2events, p2idx, e2idx)


