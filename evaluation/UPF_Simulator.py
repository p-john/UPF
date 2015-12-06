import os
import sys
import math
from packet_func import Action, Rule, Ruleset, Range
from packet_func import create_packets_from_trace_file
from collections import Counter
import multiprocessing as mp
import copy
import threading

def number_of_checked_rules(rulesets, packet, first_set):
    count = 0
    jumps = 0
    targets = [rulesets[first_set]]
    while targets:
        target = targets.pop()
        for i in range(0, len(target)):
            if packet.matched(target.rules[i]):
                count += 1
                jumps += 1
                if target.rules[i].action.action == "JUMP":
                    targets.append(
                        rulesets[str(target.rules[i].action.target)])
                break
            else:
                count += 1
    return count

# def get_bucket_of_packets(rulesets, packets, first_set):
#     cores = mp.cpu_count()
#     print(cores)
#     sum_bucket = Counter()
#     buckets=[]
#     threads = []
#     for x in range(1,cores):
#         t_packets = []
#         for packet in packets[::x]:
#             t_packets.append(packet)
#         t = threading.Thread(target=get_bucket_of_packets_threaded(rulesets,t_packets,first_set))
#         threads.append(t)
#         t.start()
#         # processes = [mp.Process(target=number_of_checked_rules,args=(rulesets, packets[i], first_set)) for i in range(0,len(packets))]
#     for thread in threads:
#         thread.join()
#     for bucket in buckets:
#         sum_bucket += bucket
#     return sum_bucket

def get_bucket_of_packets(rulesets,packets,first_set):
    bucket = Counter()
    for packet in packets:
        count = number_of_checked_rules(rulesets,packet,first_set)
        bucket.update({count : 1})
    return bucket


def simulate_dump(file, trace, output_dir):
    packets = create_packets_from_trace_file(trace)
    total_rules = 0
    rulesets = read_rulesets_from_dump(file)
    for key in rulesets:
        total_rules += len(rulesets[key])
    average_count = 0
    max_count = 0
    min_count = 9999999
    i = 0
    print("Processing Packets")
    with open(file,'r') as f:
        first_set = f.readline().rstrip('\n')
    bucket = get_bucket_of_packets(rulesets, packets, first_set)
    elements = list(bucket.keys())

    for key,value in bucket.items():
        average_count += key*value
    average_count = round(average_count / sum(bucket.values()))
    median_count = elements[int(len(elements)/2)]
    min_count = min(elements)
    max_count = max(elements)
    bucket_file = os.path.join(output_dir, (os.path.basename(file) + "_buckets.dat"))
    with open(os.path.join(bucket_file), 'a+') as f:
        # f.write("#Bucket  #Count \n ")
        for key, value in sorted(bucket.items()):
            f.write(str(key) + "       " + str(value) + "\n")

    stat_file = os.path.join(output_dir, (os.path.basename(file) + "_sim_stats.dat"))
    with open(stat_file, 'a+') as f:
        # f.write("#Min   #Max    #Mean   #Median \n")
        f.write(str(min_count)+"      "+str(max_count)+"      "
                + str(average_count)+"      " + str(median_count) + "\n")

def read_rulesets_from_dump(file):
    rulesets = {}
    with open(file, 'r') as f:
        in_ruleset = False
        in_rule = False
        valid_numbers = [5, 6, 7, 8, 9]
        f.seek(0)
        count = 0
        num_lines = sum(1 for line in open(file))
        pct = math.ceil(num_lines / 100)
        for line in f:
            # count += 1
            # if (count % pct == 0):
            #     sys.stdout.flush()
            #     sys.stdout.write(
            #         "Loading Rulesets: %d%%   \r" % (
            #             count * 100 / num_lines))
            if not line.strip():
                continue
            else:
                if "{" in line:
                    in_ruleset = True
                    continue
                if in_ruleset == False:
                    ruleset = Ruleset(line.rstrip('\n'))
                if in_ruleset == True:
                    if in_rule == False:
                        rule = Rule()
                        in_rule = True
                    if "Field" in line:
                        number = line.split()[1].rstrip(":")
                        if int(number) not in valid_numbers:
                            continue
                        val = line.split("[")[1].split("]")[0].split(",")
                        rule.set_field(Range(int(val[0]), int(val[1])),
                                       int(number))
                    elif "ACCEPT" in line:
                        rule.set_action(Action("ACCEPT", "NONE"))
                        in_rule = False
                        ruleset.add_rule(rule)
                    elif "BLOCK" in line:
                        rule.set_action(Action("BLOCK", "NONE"))
                        in_rule = False
                        ruleset.add_rule(rule)
                    elif "JUMP" in line:
                        target = line.split(":")[1].strip().rstrip('\n')
                        rule.set_action(Action("JUMP", target))
                        ruleset.add_rule(rule)
                        in_rule = False
                if "}" in line:
                    rulesets.update({ruleset.name: ruleset})
                    in_ruleset = False
    print("\n" + "DONE" + "\n")
    return rulesets