import os, sys, random, subprocess
from cb_generate import generate_cb_file, generate_trace_file
from UPF_simulator import simulate_dump
from evaluation import aggregate_sim_data
from save_state import StateObject
import multiprocessing

def start(seed_dir):
    for i in range(1,11):
        p = multiprocessing.Process(target=simulate_rulesets,args=(seed_dir,"count_"+str(i)))
        p.start()


def simulate_rulesets(seed_dir,output_dir):
    seeds = os.listdir(seed_dir)
    rule_size = list(range(200, 10001, 200))
    block_sizes = list(range(50,501,50))
    opts = ["simple_redundancy", "fdd_redundancy",
            "firewall_compressor", "saxpac", "hypersplit"]
    binths = ["2", "4", "8", "16", "32"]
    complete = ["", "--complete_transform"]
    use_hypersplit = ["", "--use_hypersplit"]
    count = list(range(0,10))
    states = {  "seeds" : 0,
                "opts" : 0,
                "binths" : 0,
                "rule_size" : 0,
                "complete" : 0,
                # "count" : 0,
                "block_sizes" : 0}
    save_file = os.path.join("simulate_rulesets",output_dir,"simulate_rulesets.sav")
    sav = StateObject(save_file,states) # Loads old states if available
    state_loaded = False

    for id_s,seed in enumerate(seeds):
        if not state_loaded and sav.states["seeds"] > id_s:
            continue
        seed_file = os.path.join(seed_dir,seed)
        seed_name = os.path.basename(seed).split('_')[0]
        output = os.path.join("simulate_rulesets",output_dir,seed_name)
        if not os.path.exists(output):
            os.makedirs(output)

        for id_r, num_rules in enumerate(rule_size):
            if not state_loaded and sav.states["rule_size"] > id_r:
                continue
                # for id_c, cnt in enumerate(count):
                #     if not state_loaded and sav.states["count"] > id_c:
                #         continue
            # Generate ClassBench file

            set_name = os.path.basename(seed_file).split('_')[0]
            random_seed = random.randint(0, sys.maxsize)
            cb_file = os.path.join(output,(set_name + "_" + str(num_rules)))
            cb_filepath = generate_cb_file(seed_file, num_rules, 1, 0,
                                               0, random_seed, cb_file)
            # Generate Trace File
            scale = int(float(100000) / num_rules) + 1
            trace_filepath = generate_trace_file(cb_file, scale, num_rules*10)
            for id_b, block in enumerate(block_sizes):
                if not state_loaded and sav.states["block_sizes"] > id_b:
                    continue
                for id_o, opt in enumerate(opts):
                    if not state_loaded and sav.states["opts"] > id_o:
                        continue
                    state_loaded = True
                    # Simulation for each optimization
                    if opt == "simple_redundancy":
                        # threading.Thread(target=simulate_simple_redundancy,
                        #                  args=(output,cb_filepath,trace_filepath,block,
                        #                  complete)).start()
                        simulate_simple_redundancy(output, cb_filepath,
                                                   trace_filepath, block, complete)
                    elif opt == "fdd_redundancy":
                        simulate_fdd_redundancy(output,cb_filepath,trace_filepath,
                                                block,complete)
                    elif opt == "firewall_compressor":
                        simulate_fw_compressor(output,cb_filepath,trace_filepath,
                                               block,complete)
                    elif opt == "saxpac":
                        for switch in use_hypersplit:
                            simulate_saxpac(output,cb_filepath,trace_filepath,
                                            block,switch, complete)
                    elif opt == "hypersplit":
                        for binth in binths:
                            simulate_hypersplit(output,cb_filepath,trace_filepath,
                                                block, binth, complete)
                    sav.update_state({"opts" : id_o})
                sav.update_state({"block_sizes" : id_b})
            simulate_original(output,cb_filepath,trace_filepath)
            os.unlink(cb_filepath)
            os.unlink(trace_filepath)
            # sav.update_state({"count" : id_c})
            sav.update_state({"rule_size" : id_r})
        sav.update_state({"seeds" : id_s})

# Opt Functions
def simulate_original(output,cb_filepath, trace_filepath):
    set_name = os.path.basename(cb_filepath)
    output_dir = os.path.join(output, "original")
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    output_file = os.path.join(output_dir, (set_name + "_dump_original"))
    cmd = "./UPF -i " + cb_filepath + " -cb -o " + output_file + " -dump"
    print(cmd)
    subprocess.call(cmd, shell=True)
    simulate_dump(output_file, trace_filepath, output_dir)
    # os.system("mv " + output_file + "_stats " + os.path.join(output_dir))
    os.unlink(output_file)

def simulate_simple_redundancy(output, cb_filepath, trace_filepath, block_size, complete):
    set_name = os.path.basename(cb_filepath)
    output_dir = os.path.join(output, "simple_redundancy",)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    output_file = os.path.join(output_dir, (set_name + "_dump_simple_redundancy_" + "block_" + str(block_size)))
    cmd = "./UPF -i " + cb_filepath + " -cb -o " + output_file + " -dump" +\
		  " -optimize " + "simple_redundancy " + "--block_size " + \
		str(block_size)
    print(cmd)
    subprocess.call(cmd, shell=True)
    simulate_dump(output_file, trace_filepath, output_dir)
    # os.system("mv " + output_file + "_stats " + os.path.join(output_dir))
    os.unlink(output_file)

def simulate_fdd_redundancy(output, cb_filepath, trace_filepath, block_size, complete):
    set_name = os.path.basename(cb_filepath)
    output_dir = os.path.join(output, "fdd_redundancy",)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    output_file = os.path.join(output_dir, (set_name + "_dump_fdd_redundancy_" + "block_" + str(block_size)))
    cmd = "./UPF -i " + cb_filepath + " -cb -o " + output_file + " -dump" +\
		  " -optimize " + "fdd_redundancy " + "--block_size " + \
		str(block_size)
    print(cmd)
    subprocess.call(cmd, shell=True)
    simulate_dump(output_file, trace_filepath, output_dir)
    # os.system("mv " + output_file + "_stats " + os.path.join(output_dir))
    os.unlink(output_file)

def simulate_fw_compressor(output, cb_filepath, trace_filepath, block_size, complete):
    set_name = os.path.basename(cb_filepath)
    output_dir = os.path.join(output, "firewall_compressor",)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    output_file = os.path.join(output_dir, (set_name + "_dump_firewall_compressor_" + "block_" + str(block_size)))
    cmd = "./UPF -i " + cb_filepath + " -cb -o " + output_file + " -dump" +\
		  " -optimize " + "firewall_compressor " + "--block_size " + \
		str(block_size)
    print(cmd)
    subprocess.call(cmd, shell=True)
    simulate_dump(output_file, trace_filepath, output_dir)
    # os.system("mv " + output_file + "_stats " + os.path.join(output_dir))
    os.unlink(output_file)

def simulate_hypersplit(output, cb_filepath, trace_filepath, block_size, binth, complete):
    set_name = os.path.basename(cb_filepath)
    output_dir = os.path.join(output, "hypersplit",)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    output_file = os.path.join(output_dir, (set_name + "_dump_hypersplit_" + "block_" + str(block_size) + "_binth_" + str(binth)))
    cmd = "./UPF -i " + cb_filepath + " -cb -o " + output_file + " -dump" +\
		  " -optimize " + "hypersplit " + "--binth " + str(binth) +" --block_size " + \
		str(block_size)
    print(cmd)
    subprocess.call(cmd, shell=True)
    simulate_dump(output_file, trace_filepath, output_dir)
    # os.system("mv " + output_file + "_stats " + os.path.join(output_dir))
    os.unlink(output_file)

def simulate_saxpac(output, cb_filepath, trace_filepath, block_size, switch, complete):
    set_name = os.path.basename(cb_filepath)
    output_dir = os.path.join(output, "saxpac",)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    output_file = os.path.join(output_dir, (set_name + "_dump_saxpac_" + "block_" + str(block_size)))
    cmd = "./UPF -i " + cb_filepath + " -cb -o " + output_file + " -dump" +\
		  " -optimize " + "saxpac " + "--block_size " + \
		str(block_size)
    print(cmd)
    subprocess.call(cmd, shell=True)
    simulate_dump(output_file, trace_filepath, output_dir)
    # os.system("mv " + output_file + "_stats " + os.path.join(output_dir))
    os.unlink(output_file)
