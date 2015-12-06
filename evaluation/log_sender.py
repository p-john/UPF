__author__ = 'Asto_2'
from cb_generate import generate_trace_file, generate_cb_file
from scapy.layers.inet import IP, TCP, UDP, ICMP, GRE
from scapy.all import *
from multiprocessing import *
from test_rulesets import test_original, test_simple_redundancy,test_hypersplit,test_saxpac,test_fw_compressor,test_fdd_redundancy
import os, argparse

parser = argparse.ArgumentParser(description='Log Sender')
parser.add_argument('-s', '--seed', help='Seed File', required=True)
parser.add_argument('-o', '--output', help='Output Dir', required=True)
args = parser.parse_args()

def send_process(packets,interface):
    while True:
        time.sleep(0.1)
        sendp(packets,iface=interface,loop=0)

def send_packets(trace_file, interface):
    packets = get_scapy_packets_from_tracefile(trace_file)
    for packet in packets:
        sendp(packet,iface=interface)

def get_scapy_packets_from_tracefile(trace_file):
    dst_mac = "b8:27:eb:04:c6:86"
    packets = []
    count = 0
    for line in open(trace_file, 'r'):
        count += 1
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
            UDP(sport=l4src, dport=l4dst)/"Packet_"+str(count))
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

    packets.append(Ether(dst_mac)/\
    IP(src="1.2.3.4",dst="2.3.4.5")/\
    UDP(sport="80",dport="5555")/"FINAL PACKET")

    return packets

def put_command_on_target(command, ruleset, target):
    os.system("scp " + ruleset + " " + target + ":")
    cmd = "ssh " + target + " \"" + command + "\""
    print(cmd)
    os.system(cmd)

def deploy_ruleset(ruleset, engine, target):
    if engine == "iptables":
        cmd = "iptables-restore < " + os.path.basename(ruleset)
        put_command_on_target(cmd, ruleset, target)
    elif engine == "ipfw":
        cmd = "service netif restart ue1; service routing restart; sysctl -w net.inet.ip.forwarding=1; csh " + os.path.basename(ruleset)
        put_command_on_target(cmd, ruleset,target)
    elif engine == "pf":
        cmd = "pfctl -F all; pfctl -o none -f " + os.path.basename(ruleset)
        put_command_on_target(cmd, ruleset, target)
    elif engine == "ipf":
        cmd = "service netif restart ue1; service routing restart; sysctl -w net.inet.ip.forwarding=1; csh " + os.path.basename(ruleset)
        put_command_on_target(cmd, ruleset, target)

def start_sender(output):
    cmd = "ssh s2 \"sudo python log_receiver.py -o " + output + " \""
    os.system(cmd)

def log_test(ruleset, trace_file,engine,output_dir):

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
    elif engine == "ipf":
        target = "pi4"
        interface = "eth1"


    deploy_ruleset(ruleset,engine,target)
    output = os.path.join(output_dir,"test_" + engine)
    start_sender(output)
    send_packets(trace_file,interface)

def log_original(output, cb_filepath, trace_filepath, engine):
    set_name = os.path.basename(cb_filepath)
    output_dir = os.path.join(output, "original", engine)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    output_file = os.path.join(output_dir, (set_name + "_" + engine +
                                            "_original"))
    cmd = "./UPF -i " + cb_filepath + " -cb -o " + output_file + " -" + \
          engine
    print(cmd)
    subprocess.call(cmd, shell=True)
    log_test(output_file, trace_filepath, engine, output_dir)
    os.system("rm " + output_file)


def log_simple_redundancy(output, cb_filepath, trace_filepath, engine,
                           block_size, complete):
    set_name = os.path.basename(cb_filepath)
    output_dir = os.path.join(output, "simple_redundancy", engine)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    output_file = os.path.join(output_dir, (set_name + "_" + engine +
                                            "_simple_redundancy_" + "block_" + str(
        block_size)))
    cmd = "./UPF -i " + cb_filepath + " -cb -o " + output_file + " -" + \
          engine + " -optimize " + "simple_redundancy " + "--block_size " + str(
        block_size)
    print(cmd)
    subprocess.call(cmd, shell=True)
    log_test(output_file, trace_filepath, engine, output_dir)
    os.system("rm " + output_file)


def log_fdd_redundancy(output, cb_filepath, trace_filepath, engine, block_size,
                        complete):
    set_name = os.path.basename(cb_filepath)
    output_dir = os.path.join(output, "fdd_redundancy", engine)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    output_file = os.path.join(output_dir, (set_name + "_" + engine +
                                            "_fdd_redundancy_" + "block_" + str(
        block_size)))
    cmd = "./UPF -i " + cb_filepath + " -cb -o " + output_file + " -" + \
          engine + " -optimize " + "fdd_redundancy " + "--block_size " + str(
        block_size)
    print(cmd)
    subprocess.call(cmd, shell=True)
    log_test(output_file, trace_filepath, engine, output_dir)
    os.system("rm " + output_file)


def log_fw_compressor(output, cb_filepath, trace_filepath, engine, block_size,
                       complete):
    set_name = os.path.basename(cb_filepath)
    output_dir = os.path.join(output, "firewall_compressor", engine)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    output_file = os.path.join(output_dir, (set_name + "_" + engine +
                                            "_firewall_compressor_block_" + str(
        block_size)))
    cmd = "./UPF -i " + cb_filepath + " -cb -o " + output_file + " -" + \
          engine + " -optimize " + "firewall_compressor " + "--block_size " + str(
        block_size)
    print(cmd)
    subprocess.call(cmd, shell=True)
    log_test(output_file, trace_filepath, engine, output_dir)
    os.system("rm " + output_file)


def log_saxpac(output, cb_filepath, trace_filepath, engine, block_size,
                switch, complete):
    set_name = os.path.basename(cb_filepath)
    output_dir = os.path.join(output, "saxpac", engine)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    output_file = os.path.join(output_dir, (set_name + "_" + engine +
                                            "_saxpac_" + "block_" + str(
        block_size)))
    cmd = "./UPF -i " + cb_filepath + " -cb -o " + output_file + " -" + \
          engine + " -optimize " + "sax_pac " + "--block_size " + str(
        block_size)
    print(cmd)
    subprocess.call(cmd, shell=True)
    log_test(output_file, trace_filepath, engine, output_dir)
    os.system("rm " + output_file)


def log_hypersplit(output, cb_filepath, trace_filepath, engine, block_size,
                    binth, complete):
    set_name = os.path.basename(cb_filepath)
    output_dir = os.path.join(output, "hypersplit", engine)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    output_file = os.path.join(output_dir, (set_name + "_" + engine +
                                            "_hypersplit_" + "binth_" +
                                            str(binth) + "_block_" + str(
        block_size)))
    cmd = "./UPF -i " + cb_filepath + " -cb -o " + output_file + " -" + \
          engine + " -optimize " + "hypersplit " + "--binth " + str(binth) \
          + " --block_size " + str(block_size)
    print(cmd)
    subprocess.call(cmd, shell=True)
    log_test(output_file, trace_filepath, engine, output_dir)
    os.system("rm " + output_file)


seed = args.seed
output_dir = args.output
cb_file = os.path.join(os.path.basename(seed)[0] + "_100")
cb_filepath = generate_cb_file(seed,100,1,0,0,5,cb_file)
trace_filepath = generate_trace_file(cb_filepath,1,100)
engines = ["iptables","pf","ipfw","ipf"]
opts = ["simple_redundancy","fdd_redundancy","firewall_compressor","hypersplit","saxpac"]
binth = "4"
block = "100"
complete = ""
switch = "use_hypersplit"
output = os.path.join("log_temp")
packets = get_scapy_packets_from_tracefile(trace_filepath)

for engine in engines:
    for opt in opts:
             if opt == "simple_redundancy":
                 log_simple_redundancy(output, cb_filepath,
                                        trace_filepath,  engine,
                                        block,  complete)
             elif opt == "fdd_redundancy":
                 log_fdd_redundancy(output, cb_filepath,
                                     trace_filepath,
                                     engine, block, complete)
             elif opt == "firewall_compressor":
                 log_fw_compressor(output, cb_filepath,
                                    trace_filepath,
                                    engine, block, complete)
             elif opt == "saxpac":
                 log_saxpac(output, cb_filepath,
                                 trace_filepath,
                                 engine, block, switch, complete)
             elif opt == "hypersplit":
                 log_hypersplit(output, cb_filepath,
                                     trace_filepath,
                                     engine, block, binth,
                                     complete)

