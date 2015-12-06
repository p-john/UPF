import os, sys, random, subprocess
from cb_generate import generate_cb_file, generate_trace_file
from deploy_and_test import deploy_and_test
from evaluation import aggregate_test_data
from save_state import StateObject


def test_rulesets(seed_dir):
    seeds = os.listdir(seed_dir)
    rule_size = list(range(200, 10001, 200))
    block_sizes = list(range(50, 501, 50))
    engines = ["iptables", "ipfw", "pf"]
    opts = ["simple_redundancy", "fdd_redundancy",
            "firewall_compressor", "saxpac", "hypersplit"]
    binths = ["2", "4", "8", "16", "32"]
    complete = ["", "--complete_transform"]
    use_hypersplit = ["", "--use_hypersplit"]
    count = list(range(0, 10))
    states = {"seeds": 0,
              "opts": 0,
              "binths": 0,
              "engines": 0,
              "rule_size": 0,
              "complete": 0,
              "count": 0,
              "block_sizes": 0}
    save_file = os.path.join("test_rulesets", "test_rulesets.sav")
    sav = StateObject(save_file, states)  # Loads old states if available

    state_loaded = False
    for id_s, seed in enumerate(seeds):
        if not state_loaded and sav.states["seeds"] > id_s:
            continue
        seed_file = os.path.join(seed_dir, seed)
        seed_name = os.path.basename(seed).split('_')[0]
        output = os.path.join("simulate_rulesets", seed_name)
        if not os.path.exists(output):
            os.makedirs(output)

        for id_r, num_rules in enumerate(rule_size):
            if not state_loaded and sav.states["rule_size"] > id_r:
                continue
            for id_c, cnt in count[sav.states["count"]:]:
                if not state_loaded and sav.states["count"] > id_c:
                    continue
                # Generate ClassBench file
                set_name = os.path.basename(seed_file).split('_')[0]
                random_seed = random.randint(0, sys.maxsize)
                cb_file = os.path.join(output,
                                       (set_name + "_" + str(num_rules)))
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
                        for id_e, engine in enumerate(engines):
                            if not state_loaded and sav.states["engines"] > id_e:
                                continue
                            state_loaded = True
                            # Test for each optimization
                            if opt == "simple_redundancy":
                                test_simple_redundancy(output, cb_filepath,
                                                       engine,
                                                       trace_filepath, block,
                                                       complete)
                            elif opt == "fdd_redundancy":
                                test_fdd_redundancy(output, cb_filepath,
                                                    trace_filepath,
                                                    engine, block, complete)
                            elif opt == "firewall_compressor":
                                test_fw_compressor(output, cb_filepath,
                                                   trace_filepath,
                                                   engine, block, complete)
                            elif opt == "saxpac":
                                for switch in use_hypersplit:
                                    test_saxpac(output, cb_filepath,
                                                trace_filepath,
                                                engine, block, switch, complete)
                            elif opt == "hypersplit":
                                for binth in binths:
                                    test_hypersplit(output, cb_filepath,
                                                    trace_filepath,
                                                    engine, block, binth,
                                                    complete)
                            sav.update_state({"engines" : id_e})
                        sav.update_state({"opts": id_o})
                    sav.update_state({"block_sizes": id_b})
                test_original(output, cb_filepath, trace_filepath)
                os.unlink(cb_filepath)
                os.unlink(trace_filepath)
                sav.update_state({"count": id_c})
            sav.update_state({"rule_size": id_r})
        sav.update_state({"seeds": id_s})


def test_original(output, cb_filepath, trace_filepath, engine):
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
    deploy_and_test(output_file, trace_filepath, engine, output_dir)
    os.system("rm " + output_file)


def test_simple_redundancy(output, cb_filepath, trace_filepath, engine,
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
    deploy_and_test(output_file, trace_filepath, engine, output_dir)
    os.system("rm " + output_file)


def test_fdd_redundancy(output, cb_filepath, trace_filepath, engine, block_size,
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
    deploy_and_test(output_file, trace_filepath, engine, output_dir)
    os.system("rm " + output_file)


def test_fw_compressor(output, cb_filepath, trace_filepath, engine, block_size,
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
    deploy_and_test(output_file, trace_filepath, engine, output_dir)
    os.system("rm " + output_file)


def test_saxpac(output, cb_filepath, trace_filepath, engine, block_size,
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
    deploy_and_test(output_file, trace_filepath, engine, output_dir)
    os.system("rm " + output_file)


def test_hypersplit(output, cb_filepath, trace_filepath, engine, block_size,
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
    deploy_and_test(output_file, trace_filepath, engine, output_dir)
    os.system("rm " + output_file)
