import os, argparse
parser = argparse.ArgumentParser(description='Evaluate all cb_sets in target folder')
parser.add_argument('-i','--seeds', help='Seed Directory', required= True)
parser.add_argument('-o','--output', help='Output Directory', required=True)
parser.add_argument('-s','--simulate',help='Simulation Switch and Simulation Output Dir', required=False)
args = parser.parse_args()

rule_size = list(range(0,10001,200))
engines = ["iptables","ipfw", "pf"]
block_sizes = [50,501,50]
opts = ["simple_redundancy", "fdd_redundancy",
        "firewall_compressor", "saxpac", "hypersplit"]
binths=["2","4","8","16","32"]
threads=["1","2","4","8","16"]




#------------------------------------------------------------------------------#
# 	Build UPF, DB Generator and Trace Generator							       #
#------------------------------------------------------------------------------#

# os.system("make -j")


#------------------------------------------------------------------------------#
# 	Generation of ClassBench Rule Sets and Traces							   #
#------------------------------------------------------------------------------#


# seeds = os.path.join(args.seeds)
cb_set_dir = os.path.join("cb_sets")
# if not os.path.exists(cb_set_dir):
#     os.makedirs(cb_set_dir)
#
# os.system("python cb_generate.py -i " + seeds +  " -o " + cb_set_dir)


#------------------------------------------------------------------------------#
# 	Processing of rule sets for Evaluation							    	   #
#------------------------------------------------------------------------------#

opt_sets = args.output
if not os.path.exists(opt_sets):
    os.makedirs(opt_sets)



#------------------------------------------------------------------------------#
# 	Testing Rulesets                        						    	   #
#------------------------------------------------------------------------------#



#------------------------------------------------------------------------------#
# 	Optional Simulation                        						    	   #
#------------------------------------------------------------------------------#

if(args.simulate):
    sim_dir = os.path.join(args.simulate)
    if not os.path.exists(sim_dir):
        os.makedirs(sim_dir)
    os.system("python simulate_rulesets.py -r " + opt_sets + " -t " + cb_set_dir)