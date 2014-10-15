from collections import OrderedDict, defaultdict
import gzip
import random

class ProtagonistModelSampler:
    def __init__(self, idx2event_dict, idx2protag_dict,
        event_samples, prot_event_event_samples, idx2cooc, is_direct):

        self.idx2event_dict_ = idx2event_dict
        self.idx2protag_dict_ = idx2protag_dict
        
        self.event_samples_ = event_samples
        random.shuffle(self.event_samples_)  
        self.neg_sample_idx_ = 0

        self.prot_event_event_samples_ = prot_event_event_samples
        random.shuffle(self.prot_event_event_samples_)
        self.pos_sample_idx_ = 0

        self.idx2cooc_ = idx2cooc
        self.num_events = len(idx2event_dict)
        self.is_direct = is_direct

    def positive_sample(self):
        if self.pos_sample_idx_ >= len(self.prot_event_event_samples_):
            self.pos_sample_idx_ = 0
            random.shuffle(self.prot_event_event_samples_)
        sample = self.prot_event_event_samples_[self.pos_sample_idx_]
        self.pos_sample_idx_ += 1
        return sample

    def negative_sample(self, p_idx):

        while 1:
            if self.neg_sample_idx_ >= len(self.event_samples_):
                self.neg_sample_idx_ = 0
                random.shuffle(self.event_samples_)

            idx1 = self.event_samples_[self.neg_sample_idx_]
            self.neg_sample_idx_ += 1

            if idx1 not in self.idx2cooc_[p_idx]:
                break

        while 1:
            if self.neg_sample_idx_ >= len(self.event_samples_):
                self.neg_sample_idx_ = 0
                random.shuffle(self.event_samples_)

            idx2 = self.event_samples_[self.neg_sample_idx_]
            self.neg_sample_idx_ += 1

            if idx2 not in self.idx2cooc_[p_idx] and idx2 != idx1:
                break

        return idx1, idx2

    def pos_sample_seq_iter(self):
        self.pos_sample_idx_ = 0
        while self.pos_sample_idx_ < len(self.prot_event_event_samples_):
            yield self.positive_sample()


    def get_event_str(self, idx):
        return self.idx2event_dict_.get(idx, None)

    def get_protag_str(self, idx):
        return self.idx2protag_dict_.get(idx, None)


def read_model_counts_to_sampler(path, freq_cutoff, is_directional):

    event_samples = []
    prot_event_event_samples = []

    event2idx = OrderedDict()
    idx2event = OrderedDict()
    protag2idx = OrderedDict()
    idx2protag = OrderedDict()
    e2e = defaultdict(set)

    idx2cooc = []

    idx = 0
    with gzip.open(path, u'r') as f:
        for line in f:

            line = line.strip()
            items = line.split("\t")
            if len(items) == 2:
                count = int(items[1])

                if count > freq_cutoff:
                    if items[0][0] == 'P':
                        protag = intern(items[0][2:])             
                        idx2protag[idx] = protag
                        protag2idx[protag] = idx
                        idx2cooc.append(set())
                        idx += 1

                    elif items[0][0] == 'E':
                        event = intern(items[0][2:])          
                        idx2event[idx] = event
                        event2idx[event] = idx
                        for i in range(count):
                            event_samples.append(idx)
                        #idx2cooc.append(set())
                        idx += 1

                    else:
                        print "Bad type!", items[0]
                        import sys
                        sys.exit()
            


            elif len(items) == 4:
                protag = intern(items[0])
                event1 = intern(items[1])
                event2 = intern(items[2])
                if event1 == event2:
                    continue
                if event1 not in event2idx:
                    continue
                if event2 not in event2idx:
                    continue
                if protag not in protag2idx:
                    continue
                count = int(items[3])
                
                p = protag2idx[protag]
                e1 = event2idx[event1]
                e2 = event2idx[event2]
                idx2cooc[p].add(e1)
                idx2cooc[p].add(e2)
                #if is_directional is False:
                #    idx2cooc[idx2].add(idx1)
                for i in range(count):
                    prot_event_event_samples.append((p, e1, e2))
            
    event2idx = None
    protag2idx = None
    return ProtagonistModelSampler(
        idx2event, idx2protag, event_samples, prot_event_event_samples,
        idx2cooc, is_directional)
