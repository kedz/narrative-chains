import gzip

class ProtagonistModel:
    def __init__(self, marg_counts, joint_counts, marg_total, joint_total):
        self.marg_counts = marg_counts
        self.joint_counts = joint_counts
        self.marg_total = marg_total
        self.joint_total = joint_total

    def pmi(self, event1, event2):
        
        e1_count = self.marg_counts.get(event1, 0)
        e2_count = self.marg_counts.get(event2, 0)
        if e1_count == 0 or e2_count == 0:
            return float('nan')
        
        joint_count = self.joint_counts(self._joint_key(event1, event2), 0)
        if joint_count == 0:   
            return float('nan')
        
        return log(joint_count * self.marg_total * self.marg_total) - \
            log(e1_count * e2_count * self.joint_total)     
             
    def _joint_key(self, event1, event2):
        if event1 < event2:
            return '{}\t{}'.format(event1, event2)
        else:
            return '{}\t{}'.format(event2, event1)

    def avg_pmi(self, events):
        tot_pmi = 0
        tot_pairs = 0
        for idx1, event1 in enumerate(events):
            for event2 in events[idx1+1:]:
                tot_pmi += self.pmi(event1, event2)
                tot_pairs += 1

        return tot_pmi / float(tot_pairs)

def read_chambers_untyped_counts(path):
    marg_counts = {}
    joint_counts = {}
    marg_total = 0 
    joint_total = 0

    with gzip.open(path, u"r") as f:
        for line in f:
            items = line.strip().split('\t')    
            if len(items) == 2:
                event = items[0]
                count = int(items[1])
                marg_counts[event] = count
                marg_total += 1
            if len(items) == 3:
                event1 = items[0]
                event2 = items[1]
                count = int(items[2])
                marg_counts['{}\t{}'.format(event1, event2)] = count
                joint_total += 1
