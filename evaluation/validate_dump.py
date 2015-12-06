__author__ = 'Patrik John'
import os
import sys
import UPF_simulator
import argparse
from UPF_validator import validate
parser = argparse.ArgumentParser(description='Evaluate all cb_sets in target folder')
parser.add_argument('-or','--orig', help='Ruleset Directory', required= True)
parser.add_argument('-op','--opt', help='Ruleset Directory', required= True)
parser.add_argument('-t','--traces', help='Trace Directory', required= True)
# parser.add_argument('-o','--output', help='Output Directory', required=True)
args = parser.parse_args()

validate(args.orig,args.opt,args.traces)


