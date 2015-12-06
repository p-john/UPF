__author__ = 'Patrik John'
import os
import sys
import UPF_simulator
import argparse
from UPF_simulator import simulate_dump

parser = argparse.ArgumentParser(description='Evaluate all cb_sets in target folder')
parser.add_argument('-i','--ruleset', help='Ruleset Directory', required= True)
parser.add_argument('-t','--traces', help='Trace Directory', required= True)
parser.add_argument('-o','--output', help='Output Directory', required=False)
args = parser.parse_args()
#
if not os.path.exists(args.output):
    os.mkdir(args.output)
simulate_dump(args.ruleset,args.traces,args.output)


