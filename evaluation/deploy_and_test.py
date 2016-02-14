import os
from scapy.all import *
from scapy.layers.inet import IP, TCP, UDP, ICMP, GRE
import time
from multiprocessing import Process
 
def deploy_and_test(ruleset, trace_file, engine, output_dir):
    print("Testing Ruleset: " + os.path.basename(ruleset))

    # Determine target machine
    target = ""
    interface = ""
    if engine == "iptables":
        target = "pi1"
        interface = "eth4"
    elif engine == "pf":
        target = "pi2"
        interface = "eth3"
    elif engine == "ipfw":
        target = "pi3"
        interface = "eth2"

    # Deploy Ruleset
    deploy_ruleset(ruleset, engine, target)

    # # Get packet count before testing
    packet_count_before = get_packet_count(interface)

    # Bombard target with packets for <duraction> seconds
    send_packets(trace_file,interface)

    #Testing Done, get new packet count
    packet_count_after = get_packet_count(interface)
    packet_count = packet_count_after - packet_count_before

    #Clean Up
    clean_target="ssh " + target + "\"rm " + os.path.basename(ruleset) + "\""
    os.system(clean_target)

    #Export results into stats file
    stats_file = os.path.join(output_dir,os.path.basename(ruleset) + "_pcount")
    with open(stats_file,'a') as f:
        f.write(str(packet_count))


def deploy_ruleset(ruleset, engine, target):
    if engine == "iptables":
        cmd = "iptables-restore < " + os.path.basename(ruleset)
        put_command_on_target(cmd, ruleset, target)
    elif engine == "ipfw":
        cmd = "csh " + os.path.basename(ruleset)
        put_command_on_target(cmd, ruleset,target)
    elif engine == "pf":
        cmd = "pfctl -F all; pfctl -o none -f " + os.path.basename(ruleset)
        put_command_on_target(cmd, ruleset, target)

def put_command_on_target(command, ruleset, target):
    os.system("scp " + ruleset + " " + target + ":")
    cmd = "ssh " + target + " \"" + command + "\""
    print(cmd)
    os.system(cmd)


# Internal optimizations


def deploy_and_test_optimized(ruleset, trace_file, engine, output_dir):
    print("Testing Ruleset: " + os.path.basename(ruleset))

    # Determine target machine
    target = ""
    interface = ""
    if engine == "iptables":
        target = "pi1"
        interface = "eth4"
    elif engine == "pf":
        target = "pi2"
        interface = "eth3"
    elif engine == "ipfw":
        target = "pi3"
        interface = "eth2"

    # Deploy Ruleset
    deploy_ruleset_optimized(ruleset, engine, target)

    # # Get packet count before testing
    packet_count_before = get_packet_count(target)

    # Bombard target with packets for <duraction> seconds
    send_packets(trace_file,interface)

    #Testing Done, get new packet count
    packet_count_after = get_packet_count(target)
    packet_count = packet_count_after - packet_count_before

    #Clean Up
    clean_target="ssh " + target + "\"rm " + os.path.basename(ruleset) + "\""
    os.system(clean_target)

    #Export results into stats file
    stats_file = os.path.join(output_dir,os.path.basename(ruleset) + "_pcount")
    with open(stats_file,'a') as f:
        f.write(str(packet_count))


def deploy_ruleset_optimized(ruleset, engine, target):
    if engine == "iptables":
        cmd = "iptables-restore < " + os.path.basename(ruleset)
        put_command_on_target(cmd, ruleset, target)
    elif engine == "ipfw":
        cmd = "csh " + os.path.basename(ruleset)
        put_command_on_target(cmd, ruleset,target)
    elif engine == "pf":
        cmd = "pfctl -F all; pfctl -f " + os.path.basename(ruleset)
        put_command_on_target(cmd, ruleset, target)


def send_process(packets,interface):
    sendp(packets,iface=interface,loop=1)

def send_packets(trace_file, interface):
    packets = get_scapy_packets_from_tracefile(trace_file)
    p = Process(target=send_process,args=(packets,interface))
    p.start()
    timeout = time.time() + 10
    while 1:
        if timeout < time.time():
            p.terminate()
            break



def get_scapy_packets_from_tracefile(trace_file):
    src_mac = "aa:bb:cc:dd"
    dst_mac = "b8:27:eb:03:80:c0"
    packets = []
    for line in open(trace_file, 'r'):
        values = line.split()
        l3src = values[0]
        l3dst = values[1]
        l4src = int(values[2])
        l4dst = int(values[3])
        protocol = values[4]
        if(protocol == "6"):
            packets.append(Ether(dst=dst_mac)/\
            IP(src=l3src,dst=l3dst)/\
            TCP(sport=l4src, dport=l4dst))
        elif(protocol == "17"):
            packets.append(Ether(dst=dst_mac)/\
            IP(src=l3src,dst=l3dst)/\
            UDP(sport=l4src, dport=l4dst))
        elif(protocol == "1"):
            packets.append(Ether(dst=dst_mac)/\
            IP(src=l3src,dst=l3dst)/\
            ICMP())
        elif(protocol == "47"):
            packets.append(Ether(dst=dst_mac)/\
            IP(src=l3src,dst=l3dst)/\
            GRE())
        else:
            packets.append(Ether(dst=dst_mac)/\
            IP(src=l3src,dst=l3dst))
    return packets


def get_packet_count(target):
    cmd = "ssh s2 \"netstat -s | grep -A1 -i \"ip:\" | grep -v -i \"ip:\""
    packet_count = subprocess.check_output(cmd, shell=True)
    print("Counted " + str(packet_count) + " packets")
    return int(packet_count.split()[0])

