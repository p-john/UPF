__author__ = 'Patrik John'

import os, argparse, sys
from packet_func import create_packets_from_trace_file
from UPF_simulator import read_rulesets_from_dump
from packet_func import Rule
def get_matched_rule(packet,first_set,rulesets):
    matching_rule = Rule()
    targets = [rulesets[first_set]]
    while targets:
        target = targets.pop()
        for i in range(0,len(target)):
            if packet.matched(target.rules[i]):
                act = target.rules[i].action.action
                if act == "ACCEPT" or act == "BLOCK":
                    matching_rule = target.rules[i]
                    break
                elif act == "JUMP":
                    targets.append(rulesets[str(target.rules[i].action.target)])
                    # print("JUMP TO " +str(target.rules[i].action.target))
                    break
    return matching_rule

def test_packets(rulesets_orig, first_orig, rulesets_opt, first_opt, packets):

    count = 0
    for packet in packets:
        count += 1
        print ("Testing Packet: " + str(count))
        rule1 = get_matched_rule(packet,first_orig,rulesets_orig)
        rule2 = get_matched_rule(packet,first_opt,rulesets_opt)

        if rule1 == rule2:
            continue
        else:
           print("UNEQUAL RULE AT PACKET : " + str(count))
           print(str(packet))
           print("RULE1: " + str(rule1))
           print("RULE2: " + str(rule2))
           return False
    return True

def validate(orig_file, opt_file, trace_file):
    with open(orig_file,'r') as f:
        orig_first_set = f.readline().rstrip('\n')
    with open(opt_file,'r') as f:
        opt_first_set = f.readline().rstrip('\n')
    packets = create_packets_from_trace_file(trace_file)
    total_rules = 0
    rulesets_orig = read_rulesets_from_dump(orig_file)
    rulesets_opt = read_rulesets_from_dump(opt_file)
    print("Processing Packets" + str(len(packets)))
    if test_packets(rulesets_orig, orig_first_set, rulesets_opt, opt_first_set, packets):
        print("Testing Done - All matching rules equal")
