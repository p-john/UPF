__author__ = 'Asto_2'
import os, argparse, random, sys
from cb_generate import generate_cb_file, generate_trace_file
from test_rulesets import test_simple_redundancy, test_fdd_redundancy,test_fw_compressor,test_saxpac,test_hypersplit,test_original
parser = argparse.ArgumentParser(description='Evaluate all cb_sets in target folder')
parser.add_argument('-i','--seeds', help='Seed Directory', required= True)
parser.add_argument('-o','--output', help='Output Directory', required=True)
parser.add_argument('-s','--simulate',help='Simulation Switch and Simulation Output Dir', required=False)
args = parser.parse_args()

seed_file = args.seeds
num_rules = 1000

output = args.output
opts = ["simple_redundancy", "fdd_redundancy",
            "firewall_compressor", "saxpac", "hypersplit"]
engines = ["iptables", "ipfw", "pf"]
binth = 2
block = 100
complete = ""
switch = ""

if not os.path.exists(output):
    os.makedirs(output)

# Generate ClassBench file
set_name = os.path.basename(seed_file).split('_')[0]
random_seed = random.randint(0, sys.maxsize)
cb_file = os.path.join(output, set_name + "_" + str(num_rules))
cb_filepath = generate_cb_file(seed_file, num_rules, 1, 0,
                               0,
                               random_seed, cb_file)

# Generate Trace File
scale = int(float(100000) / num_rules) + 1
trace_filepath = generate_trace_file(cb_file, scale)



for engine in engines:
    test_original(output,cb_filepath,trace_filepath,engine)
    for opt in opts:
        # Test for each optimization
        if opt == "simple_redundancy":
            test_simple_redundancy(output, cb_filepath,
                                   trace_filepath, engine,
                                   block, complete)
        elif opt == "fdd_redundancy":
            test_fdd_redundancy(output, cb_filepath,
                                trace_filepath, engine,
                                block, complete)
        elif opt == "firewall_compressor":
            test_fw_compressor(output, cb_filepath,
                               trace_filepath, engine,
                               block, complete)
        elif opt == "saxpac":
            test_saxpac(output, cb_filepath, trace_filepath,
                            engine,block, switch, complete)
        elif opt == "hypersplit":
            test_hypersplit(output, cb_filepath,trace_filepath, engine,
                            block, binth, complete)