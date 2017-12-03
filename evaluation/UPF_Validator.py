from UPF_Simulator import *
from packet_func import Action, Rule, Ruleset, Range
from packet_func import create_packets_from_trace_file
import sys



def get_action_for_packet(rulesets, packet, first_set):
    count = 0
    jumps = 0
    targets = [rulesets[first_set]]
    while targets:
        target = targets.pop()
        #print("Next Target %s " % target.name)
        for i in range(0, len(target)):
            if packet.matched(target.rules[i]):
                #print("Match : %s" % packet)
                #print("On %i : %s" % (i,target.rules[i]))
                if target.rules[i].action.action == "JUMP":
                    #print ("Jump to %s" %(str(target.rules[i].action.target)))
                    targets.append(
                        rulesets[str(target.rules[i].action.target)])
                else:
                   action = target.rules[i].action.action
                   rule = target.rules[i]
                   return {"action" : action, "rule"  : rule} 
                break
            else:
                 return {"action" : "ACCEPT", "rule" : "default_rule"}

if __name__=="__main__":
    dump1_file = sys.argv[1]
    dump2_file = sys.argv[2]
    trace_file = sys.argv[3]
    rulesets_1 = read_rulesets_from_dump(dump1_file)
    rulesets_2 = read_rulesets_from_dump(dump2_file)

    with open(dump1_file, 'r') as f:
       rulesets_1_first = f.readline().strip()

    with open(dump2_file, 'r') as f:
       rulesets_2_first = f.readline().strip()
    count = 0
    packets = create_packets_from_trace_file(trace_file)
    for i,packet in enumerate(packets):
        #print("Packet %i" % i)
        res1 = get_action_for_packet(rulesets_1,packet,rulesets_1_first)
        #print("Ruleset 2: ---------------------------------")
        res2 = get_action_for_packet(rulesets_2,packet,rulesets_2_first)
        
   
        action1 = res1["action"]
        action2 = res2["action"]

        if action1 != action2:
            print("Action 1: %s - Action 2: %s - Packet : %s" % (action1,action2,packet))
            print("Rule 1")
            print(res1["rule"])
            print("---")
            print("Rule 2")
            print(res2["rule"])
            count += 1
    print("Wrong Matches : %i" % count) 


