import os, subprocess,re


def generate_cb_file(seed, num_rules, smoothness, address_scope, port_scope, random_seed, output_file):
    name = os.path.basename(seed).split('_')[0]
    # target = os.path.join(output_file,str(name) + "_" + str(num_rules))
    target = output_file
    n = num_rules
    done = False
    while not done:
       # seed = "%s/%s_seed" % (PATH_TO_SEED_FILES, name)
        #target = "synth_rules/%s_%d_%d" % (name, num_rules, i)
        # params = {"seed": seed, "target": target, "n": n}
        cmd =   "./db_generator -c " +  seed + " " + str(n) + " " + str(smoothness) + " " + str(address_scope) +\
                " " + str(port_scope) + " " + target + " " + str(random_seed)
        print(cmd)
        # os.system(cmd)
        try:
            output = subprocess.check_output(cmd, shell=True)
        except:
            return
        num_gen_filters = int(output.split("\n")[11].split(" ")[0])
        print("Number of Generated Filters: " + str(num_gen_filters))
        print("Target Filters: " + str(n))
        if num_gen_filters >= num_rules:
            done = True
        else:
            n *= 2
    # reduce generated file to number of required rules
    with open(target, "r") as f:
        lines = f.readlines()
    lines = lines[:num_rules]
    with open(target, "w") as f:
        for line in lines:
            parts = re.split("[ \t]+", line.strip())
            f.write("%s\n" % " ".join(parts[:-1]))
    return str(target)


def generate_trace_file(cb_file, scale, target_headers):
    trace_filepath = os.path.join(cb_file + "_trace")
    done = False
    while not done:
        # key = (PATH_TO_RULE_DIR, name, num_rules, i)
        # rule_file = "%s/%s_%d_%d" % key
        # trace_file = "%s/%s_%d_%d_trace" % ((PATH_TO_TRACE_DIR,)
        #                                     + key[1:])
        cmd = "./trace_generator 1 0 %d %s" % (scale, cb_file)
        print(cmd)
        output = subprocess.check_output(cmd, shell=True)
        num_headers = int(output.split("\n")[1].split(" ")[4])
        if num_headers >= target_headers:
            done = True
            with open(trace_filepath, "r") as f:
                lines = f.readlines()
            with open(trace_filepath, "w") as f:
                for index in range(100000):
                    line = lines[index].strip()
                    parts = re.split("[ \t]+", line)
                    assert len(parts) == 6
                    line = " ".join(parts)
                    f.write("%s\n" % line)

            print("generated trace %s" + trace_filepath)
        else:
            os.unlink(trace_filepath)
            scale *= 2

    return trace_filepath

    # #Generate Trace from sets
    # setname = os.path.basename(cb_file)
    # trace_cmd = "trace_generator " + str(trace_param_A) + " " + \
    #              str(trace_param_B)+  " " + str(scale) + " " + cb_file
    # print(trace_cmd)
    # os.system(trace_cmd)
    # trace_file = next(x for x in os.listdir(os.path.dirname(cb_file)) if setname in x and str(number) in x and "trace" in x)
    # return os.path.join(os.path.dirname(cb_file),trace_file)

