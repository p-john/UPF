__author__ = 'Asto_2'
import os, argparse, random, sys
from cb_generate import generate_cb_file, generate_trace_file
from simulate_rulesets import simulate_simple_redundancy
from evaluation import aggregate_sim_data


parser = argparse.ArgumentParser(description='Evaluate all cb_sets in target folder')
parser.add_argument('-i','--seeds', help='Seed Directory', required= True)
parser.add_argument('-o','--output', help='Output Directory', required=True)
parser.add_argument('-s','--simulate',help='Simulation Switch and Simulation Output Dir', required=False)
args = parser.parse_args()

seed_file = args.seeds
num_rules = 1000
opt = "simple_redundancy"
output = args.output
engine = "iptables"
block_sizes = 100
complete = ""

if not os.path.exists(output):
    os.makedirs(output)
for i in range(0,10):
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

    # Test for each optimization

    if opt == "simple_redundancy":
        simulate_simple_redundancy(output, cb_filepath,
                                       trace_filepath, block_sizes, complete)
aggregate_sim_data(output,opt)