import os, argparse, random, sys, copy
from cb_generate import generate_cb_file, generate_trace_file
from test_rulesets import test_hypersplit,test_original
from simulate_rulesets import simulate_hypersplit, simulate_original
from evaluation import aggregate_test_data, aggregate_sim_data
from save_state import StateObject
import multiprocessing

def minsets_performance(seed_dir):

    seeds = os.listdir(seed_dir)
    rule_size = list(range(1, 101, 1))
    engines = ["iptables", "ipfw", "pf"]
    opts = ["hypersplit"]
    complete = ""
    count = list(range(0,10))
    states = {  "seeds" : 0,
                "engines" : 0,
                "opts" : 0,
                "rule_size" : 0,
                "complete" : 0,
                "count" : 0}
    save_file = os.path.join("minimal_sets_performance","minsets_perf.sav")
    sav = StateObject(save_file,states) # Loads old states if available
    last_seed = seeds[sav.states["seeds"]]
    last_engine = engines[sav.states["engines"]]
    last_opt = opts[sav.states["opts"]]
    last_num = rule_size[sav.states["rule_size"]]
    for root, dirs, files in os.walk("minimal_sets_performance"):
        for x in files:
            if (last_seed in x and last_engine in x and last_opt in x
                and last_num in x):
                os.unlink(x)

    state_loaded = False
    for id_s, seed in enumerate(seeds):
        if not state_loaded and sav.states["seeds"] > id_s:
            continue
        seed_file = os.path.join(seed_dir,seed)
        seed_name = os.path.basename(seeds[0]).split('_')[0]
        output = os.path.join("minimal_sets_simulation", seed_name)
        if not os.path.exists(output):
            os.makedirs(output)
        for id_r, num_rules in enumerate(rule_size):
            if not state_loaded and sav.states["rule_size"] > id_r:
                continue
            for id_c, cnt in enumerate(count):
                if not state_loaded and sav.states["count"] > id_c:
                    continue
                # Generate ClassBench file
                set_name = os.path.basename(seed_file).split('_')[0]
                random_seed = random.randint(0, sys.maxsize)
                cb_file = os.path.join(output,(set_name + "_" + str(num_rules)))
                cb_filepath = generate_cb_file(seed_file, num_rules, 1, 0,
                                               0, random_seed, cb_file)
                # Generate Trace File
                scale = int(float(100000) / num_rules) + 1
                trace_filepath = generate_trace_file(cb_file, scale, 100000)
                for id_o, opt in enumerate(opts):
                    if not state_loaded and sav.states["opts"] > id_o:
                        continue
                    for id_e, engine in enumerate(engines):
                        if not state_loaded and sav.states["engines"] > id_e:
                            continue
                        state_loaded = True
                        # Test for each optimization
                        if opt == "hypersplit":
                            test_hypersplit(output, cb_filepath, trace_filepath,
                                            engine, 100, 1, complete)
                        sav.update_state({"opts" : id_o})
                        test_original(output,cb_filepath,trace_filepath,engine)
                        os.unlink(cb_filepath)
                        os.unlink(trace_filepath)
                sav.update_state({"count" : id_c})
            sav.update_state({"rule_size" : id_r})
        sav.update_state({"seeds" : id_s})

def simulation_start(seed_dir):
    for x in range(0,10):
        p = multiprocessing.Process(target=minsets_simulation, args=(seed_dir,"count_" + str(x)))
        p.start()

def minsets_simulation(seed_dir, output_dir):
    seeds = os.listdir(seed_dir)
    rule_size = list(range(1, 101, 1))
    opts = ["hypersplit"]
    complete = ""
    binths = [2,4,8,16,32]

    states = {  "seeds" : 0,
                "opts" : 0,
                "rule_size" : 0,
                "complete" : 0,
                "binths" : 0
                }
    save_file = os.path.join("minimal_sets_simulation",output_dir, "minsets_sim.sav")
    sav = StateObject(save_file,states) # Loads old states if available
    state_loaded = False

    for id_s, seed in enumerate(seeds):
        if not state_loaded and sav.states["seeds"] > id_s:
            continue
        seed_file = os.path.join(seed_dir,seed)
        seed_name = os.path.basename(seeds[0]).split('_')[0]
        output = os.path.join("minimal_sets_simulation",output_dir, seed_name)
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
            trace_filepath = generate_trace_file(cb_file, scale, 100000)
            for id_o, opt in enumerate(opts):
                if not state_loaded and sav.states["opts"] > id_o:
                    continue
                for id_b, binth in enumerate(binths):
                    if not state_loaded and sav.states["binths"] > id_b:
                        continue
                    state_loaded = True
                    # Test for each optimization
                    if opt == "hypersplit":
                        simulate_hypersplit(output, cb_filepath, trace_filepath,
                                                100, binth, complete)
                    sav.update_state({"binths" : id_b})
                sav.update_state({"opts" : id_o})
                simulate_original(output,cb_filepath,trace_filepath)
                os.unlink(cb_filepath)
                os.unlink(trace_filepath)
                # sav.update_state({"count" : id_c})
                sav.update_state({"rule_size" : id_r})
            sav.update_state({"seeds" : id_s})
