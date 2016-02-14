__author__ = 'Asto_2'
import os

import numpy, shutil
from numpy import std,mean,sqrt

def std_error(val):
    return 1.96*(std(val)/sqrt(len(val)))


def cumulative_plots(input_dir, plot_dir):
    if not os.path.exists(plot_dir):
        os.makedirs(plot_dir)
    bucket_files = []
    bucket_set = set()
    for root, dirs, files in os.walk(input_dir):
        for file in files:
            if "buckets.dat" in file:
                bucket_files.append(os.path.join(root,file))
                bucket_set.add(os.path.join(file))
    # buckets = [x for x in files if "buckets.dat" in x]

    for file in bucket_set:
        output_file = os.path.join(plot_dir,file)
        bucket_counts = []
        dict = {}
        valids = [x for x in bucket_files if file in x]

        num = os.path.basename(file).split("_")[1]
        packet_count = 0
        for file in valids:
            with open(file,'r') as f:
                for line in f:
                    values = line.split()
                    key = int(values[0])
                    count = values[1]
                    packet_count += int(count)
                    if key in dict.keys():
                        dict[key].append(int(count))
                    else:
                        dict.update({int(key) : [int(count)]})

        with open(output_file,'w') as g:
            #create cumulative plot data
            current_count = 0
            for key in sorted(dict.keys()):
                sum = numpy.sum(dict[key])
                current_count += sum
                sttdev = 0
                if len(dict[key]) > 1:
                    sttdev = std(dict[key]) / packet_count
                g.write("  ".join([str(key), str(float(current_count) / packet_count),"\n"]))

def simulation_plot(input_dir,plot_dir):
    if not os.path.exists(plot_dir):
        os.makedirs(plot_dir)
    sim_files = []
    sim_files_set = set()
    for root, dirs, files in os.walk(input_dir):
        for file in files:
            if "_sim_stats.dat" in file:
                sim_files.append(os.path.join(root,file))
                sim_files_set.add(os.path.join(file))



    for file in sim_files_set:
        dict = {}
        num_rules = os.path.basename(file).split('_')[1]
        medians = []
        means = []
        maxs = []
        mins = []
        valids = [x for x in sim_files if file in x]
        for valid in valids:
            with open(valid,'r') as f:
                for line in f:
                    values = line.split()
                    #print(values)
                    mins.append(int(values[0]))
                    maxs.append(int(values[1]))
                    means.append(float(values[2]))
                    medians.append(int(values[3]))

        dict.update({int(num_rules) : (mean(mins),std_error(mins),mean(maxs),std_error(maxs),
                                       mean(means),std_error(means),mean(medians),std_error(medians))})

        split_list = os.path.basename(file).split('_')
        split_list.pop(1)
        sum_file = os.path.join('_'.join(split_list))
        with open(os.path.join(plot_dir,sum_file),'a') as f:
            #f.write("#Rules #Min #MinError #Max #MaxError #Mean #MeanError #Median #MedianError \n \n")
            for key, item in dict.iteritems():
                item = dict[key]
                out = '\t'.join([str(key),
                                 str(item[0]),
                                 str(item[1]),
                                 str(item[2]),
                                 str(item[3]),
                                 str(item[4]),
                                 str(item[5]),
                                 str(item[6]),
                                 str(item[7])])
                f.write(out + "\n")


def proctime_plot(input_dir,plot_dir):
    if not os.path.exists(plot_dir):
        os.makedirs(plot_dir)
    procs = []
    for root, dirs, files in os.walk(input_dir):
        for file in files:
            if "_stats.dat" in file:
                procs.append(os.path.join(root,file))

    sets = set()
    binths = set()
    blocks = set()
    opts = set()
    threads = set()

    for file in procs:
        values = os.path.basename(file).split('_')
        cb_set = values[0]
        if values[2] == "firewall":
            opt = values[2] + "_" + values[3]
            opts.add(opt)
        elif values[3] == "simple":
            opt = values[3] + "_" + values[4]
            opts.add(opt)
        elif values[2] == "fwc":
            opt = values[2] + "_" + values[3] + "_" + values[4]
            opts.add(opt)
        else:
            opt = values[2]
            opts.add(opt)
        if opt == "hypersplit":
            binth = values[values.index("binth")+1]
            binths.add(binth)
        block = values[values.index("block")+1]
        thread = values[values.index("threads")+1]
        #print(thread)
        blocks.add(block)
        sets.add(cb_set)
        threads.add(thread)

        # if opt == "hypersplit":
        #     binth = values[4]
        #     block = values[6]
        #     threads = values[8]
        # else:
        #     block = values[4]
        #     threads = values[6]
    for cb_set in sets:
        for opt in opts:
            for block in blocks:
                for thread in threads:
                    if opt == "hypersplit":
                        for binth in binths:
                            valids = [x for x in procs if cb_set in x and opt in x and
                                      "block_"+ str(block) in x and "binth_" + str(binth) in x
                                      and "threads_" + str(thread) in x]
                            dict = {}
                            dat_dir = os.path.join(plot_dir,cb_set,opt)
                            if not os.path.exists(dat_dir):
                                os.makedirs(dat_dir)
                            dat_file = os.path.join(dat_dir,('_'.join([cb_set,opt,"block",block
                                                                          ,"binth", binth,"threads",
                                                                       thread,"sum.dat"])))
                            for file in valids:
                                means_proc = []
                                means_rules = []
                                number = os.path.basename(file).split("_")[1]
                                #print(file)
                                with open(file,'r') as f:
                                    for line in f.readlines():
                                        if "Processing" in line:
                                            proc_time = line.split('-')[1].rstrip('\n')
                                            means_proc.append(int(proc_time))
                                        elif "Threads" in line:
                                            thrd = line.split('-')[1].rstrip('\n')
                                        elif "Rules After" in line:
                                            aft_rules = line.split('-')[1].rstrip('\n')
                                            means_rules.append(int(aft_rules))

                                dict.update({int(number) : (thrd,mean(means_rules),std_error(means_rules),
                                                            mean(means_proc),std_error(means_proc),binth)})

                                    #print(dict)
                            with open(dat_file,'a') as g:
                                # if opt == "hypersplit":
                                #     #g.write("#Rules #Threads #Blocks #Binths #Processing Time #Proc_Time_Error\n \n")
                                # else:
                                #g.write("#Rules #Threads #Blocks #Processing Time #Proc_Time_Error  \n \n")
                                for key in sorted(dict.keys()):
                                    item = dict[key]
                                    out = [str(key)]
                                    for elem in item:
                                        out.append(str(elem))
                                    g.write('\t'.join(out)  + "\n")
                    elif opt == "fwc_and_hyp":
                        for binth in binths:
                            valids = [x for x in procs if cb_set in x and opt in x and
                                      "block_"+ str(block) in x and "binth_" + str(binth) in x
                                      and "threads_" + str(thread) in x]
                            dict = {}
                            dat_dir = os.path.join(plot_dir,cb_set,opt)
                            if not os.path.exists(dat_dir):
                                os.makedirs(dat_dir)
                            dat_file = os.path.join(dat_dir,('_'.join([cb_set,opt,"block",block
                                                                          ,"binth", binth,"threads",
                                                                       thread,"sum.dat"])))
                            for file in valids:
                                means_proc = []
                                means_rules = []
                                number = os.path.basename(file).split("_")[1]
                                #print(file)
                                with open(file,'r') as f:
                                    lines = f.readlines()
                                    for line_num,line in enumerate(lines):
                                        if "Threads" in line:
                                            proc_time_sum = 0
                                            rules_after_final = 0
                                            thrd = 0
                                            for in_line in lines[line_num:]:
                                                if "#" in in_line:
                                                    break
                                                else:
                                                    if "Processing" in in_line:
                                                        proc_time = in_line.split('-')[1].rstrip('\n')
                                                        proc_time_sum += int(proc_time)
                                                    elif "Threads" in in_line:
                                                        thrd = int(in_line.split('-')[1].rstrip('\n'))
                                                    elif "Rules After" in in_line:
                                                         aft_rules = in_line.split('-')[1].rstrip('\n')
                                                         rules_after_final = int(aft_rules)
                                            means_proc.append(int(proc_time_sum))
                                            means_rules.append(int(rules_after_final))
                                dict.update({int(number) : (thrd,mean(means_rules),std_error(means_rules),
                                                            mean(means_proc),std_error(means_proc),binth)})

                                    #print(dict)
                            with open(dat_file,'a') as g:
                                # if opt == "hypersplit":
                                #     #g.write("#Rules #Threads #Blocks #Binths #Processing Time #Proc_Time_Error\n \n")
                                # else:
                                #g.write("#Rules #Threads #Blocks #Processing Time #Proc_Time_Error  \n \n")
                                for key in sorted(dict.keys()):
                                    item = dict[key]
                                    out = [str(key)]
                                    for elem in item:
                                        out.append(str(elem))
                                    g.write('\t'.join(out)  + "\n")
                    else:
                        valids = [x for x in procs if cb_set in x and opt in x and
                                  "block_"+ str(block) in x and "threads_" + str(thread) in x]
                        dict = {}

                        dat_dir = os.path.join(plot_dir,cb_set,opt)
                        if not os.path.exists(dat_dir):
                            os.makedirs(dat_dir)
                        dat_file = os.path.join(dat_dir,('_'.join([cb_set,opt,"block",block,
                                                                   "threads",thread,"sum.dat"])))
                        for file in valids:
                            means_proc = []
                            means_rules = []
                            number = os.path.basename(file).split("_")[1]
                            #print(file)
                            with open(file,'r') as f:
                                for line in f.readlines():
                                    if "Processing" in line:
                                        proc_time = line.split('-')[1].rstrip('\n')
                                        means_proc.append(int(proc_time))
                                    if "Threads" in line:
                                        thrd = line.split('-')[1].rstrip('\n')
                                    if "Rules After" in line:
                                            aft_rules = line.split('-')[1].rstrip('\n')
                                            means_rules.append(int(aft_rules))
                                dict.update({int(number) : (thrd,mean(means_rules),std_error(means_rules),
                                                            mean(means_proc),std_error(means_proc))})

                                #print(dict)
                        with open(dat_file,'a') as g:
                            # if opt == "hypersplit":
                            #     #g.write("#Rules #Threads #Blocks #Binths #Processing Time #Proc_Time_Error\n \n")
                            # else:
                            #g.write("#Rules #Threads #Blocks #Processing Time #Proc_Time_Error  \n \n")
                            for key in sorted(dict.keys()):
                                item = dict[key]
                                out = [str(key)]
                                for elem in item:
                                    out.append(str(elem))
                                g.write('\t'.join(out)  + "\n")
        #
    # sum_files = []
    # for root, dirs, files in os.walk(plot_dir):
    #     for file in files:
    #         sum_files.append(os.path.join(root,file))
    #
    # for file in sum_files:
    #     gnuplot_file = file + ".gnuplot"
    #     with open(gnuplot_file, 'w') as g:
    #         g.write("set terminal png \n")
    #         g.write("set output \"" + file + ".png\" \n")
    #         g.write("plot \"" + file + "\" u 1:2 smooth unique w error bars\n")
    #
    #     os.system("gnuplot " + gnuplot_file)
        # os.system("move " + os.path.b(input_files[0]) + ".png" + " " + plot_dir)

def jumpsets_plot(input_dir,output_dir):
    files = os.listdir(input_dir)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    types = ["non_jump","long_range_jump","jump"]
    engines = ["iptables","ipfw","ipf","pf"]

    for engine in engines:
        for type in types:
            valids = []
            if type == "jump":
                valids = [x for x in files if "_" + engine + "_" in x and
                          "long" not in x and "non" not in x]
            elif type == "long_range_jump":
                valids = [x for x in files if "_" + engine + "_" in x and "long" in x]
            elif type == "non_jump":
                valids = [x for x in files if "_" + engine + "_" in x and "non" in x]

            sum_file = os.path.join(output_dir,type + "_" + engine + "_sum.dat")
            with open(sum_file,"w") as g:
                g.write("#Rules #PCount #PCount Error \n")
                dict = {}
                for file in valids:
                    subs = os.path.basename(file).split('_')
                    num_rules = subs[subs.index("pcount")-1]
                    filepath = os.path.join(input_dir,file)
                    values = []
                    with open(filepath,'r') as f:
                        for line in f.readlines():
                            values.append(int(line.rstrip('\n')))
                    print(values)
                    dict.update({int(num_rules) : (mean(values),std_error(values))})

                for key in sorted(dict.keys()):
                    item = dict[key]
                    out = '\t'.join([str(key),
                                     str(item[0]),
                                     str(item[1])])
                    g.write(out + "\n")



def packet_plot(input_dir,plot_dir):
    if not os.path.exists(plot_dir):
        os.makedirs(plot_dir)
    pcounts = []
    for root, dirs, files in os.walk(input_dir):
        for file in files:
            if "_pcount" in file:
                pcounts.append(os.path.join(root,file))

    sets = set()
    binths = set()
    blocks = set()
    opts = set()
    engines = set()

    for file in pcounts:
        values = os.path.basename(file).split('_')
        cb_set = values[0]
        engines.add(values[2])
        if values[3] == "firewall":
            opt = values[3] + "_" + values[4]
            opts.add(opt)
        elif values[3] == "simple" or values[3] == "fdd":
            opt = values[3] + "_" + values[4]
            opts.add(opt)
        elif values[3] == "fwc":
            opt = values[3] + "_" + values[4] + "_" + values[5]
            opts.add(opt)
        else:
            opt = values[3]
            opts.add(opt)
        if opt == "hypersplit" or opt == "fwc":
            binth = values[values.index("binth")+1]
            binths.add(binth)
        if "block" in file:
            block = values[values.index("block")+1]
            blocks.add(block)
        #print(thread)

        sets.add(cb_set)

        # if opt == "hypersplit":
        #     binth = values[4]
        #     block = values[6]
        #     threads = values[8]
        # else:
        #     block = values[4]
        #     threads = values[6]
    for cb_set in sets:
        for engine in engines:
            for opt in opts:
                if opt == "original":
                    valids = [x for x in pcounts if "_" + engine + "_" in x and cb_set in x and opt in x]
                    if not valids:
                        continue
                    dict = {}
                    dat_dir = os.path.join(plot_dir,cb_set,opt)
                    if not os.path.exists(dat_dir):
                        os.makedirs(dat_dir)
                    dat_file = os.path.join(dat_dir,('_'.join([cb_set,engine,opt,"pcount.dat"])))
                    for file in valids:
                        means_pcount = []
                        number = os.path.basename(file).split("_")[1]
                        #print(file)
                        with open(file,'r') as f:
                            for line in f.readlines():
                                if(int(line) < 160000):
                                    means_pcount.append(int(line))
                        dict.update({int(number) : (mean(means_pcount),std_error(means_pcount))})

                        #print(dict)
                    with open(dat_file,'a') as g:
                        # if opt == "hypersplit":
                        #     #g.write("#Rules #Threads #Blocks #Binths #Processing Time #Proc_Time_Error\n \n")
                        # else:
                        #g.write("#Rules #Threads #Blocks #Processing Time #Proc_Time_Error  \n \n")
                        for key in sorted(dict.keys()):
                            item = dict[key]
                            out = [str(key)]
                            for elem in item:
                                out.append(str(elem))
                            g.write('\t'.join(out)  + "\n")
                for block in blocks:
                    if opt == "hypersplit" or opt == "fwc_and_hyp":
                        for binth in binths:
                            valids = [x for x in pcounts if "_" + engine + "_" in x and cb_set in x and opt in x and
                                      "block_"+ str(block) + "_" in x and "binth_" + str(binth) in x]
                            if not valids:
                                continue
                            dict = {}
                            dat_dir = os.path.join(plot_dir,cb_set,opt)
                            if not os.path.exists(dat_dir):
                                os.makedirs(dat_dir)
                            dat_file = os.path.join(dat_dir,('_'.join([cb_set,engine,opt,"block",block
                                                                          ,"binth", binth,"pcount.dat"])))
                            for file in valids:
                                means_pcount = []
                                number = os.path.basename(file).split("_")[1]
                                #print(file)
                                with open(file,'r') as f:
                                    for line in f.readlines():
                                        if(int(line) < 160000):
                                            means_pcount.append(int(line))
                                dict.update({int(number) : (mean(means_pcount),std_error(means_pcount))})

                                #print(dict)
                            with open(dat_file,'a') as g:
                                # if opt == "hypersplit":
                                #     #g.write("#Rules #Threads #Blocks #Binths #Processing Time #Proc_Time_Error\n \n")
                                # else:
                                #g.write("#Rules #Threads #Blocks #Processing Time #Proc_Time_Error  \n \n")
                                for key in sorted(dict.keys()):
                                    item = dict[key]
                                    out = [str(key)]
                                    for elem in item:
                                        out.append(str(elem))
                                    g.write('\t'.join(out)  + "\n")
                    elif opt != "original":
                        valids = [x for x in pcounts if "_" + engine + "_" in x and cb_set in x and opt in x and
                                  "block_"+ str(block) + "_"  in x]
                        if not valids:
                            continue
                        dict = {}
                        dat_dir = os.path.join(plot_dir,cb_set,opt)
                        if not os.path.exists(dat_dir):
                            os.makedirs(dat_dir)
                        dat_file = os.path.join(dat_dir,('_'.join([cb_set,engine,opt,"block",
                                                                   block,"pcount.dat"])))
                        for file in valids:
                            means_pcount = []
                            number = os.path.basename(file).split("_")[1]
                            #print(file)
                            with open(file,'r') as f:
                                for line in f.readlines():
                                    if(int(line) < 160000):
                                        means_pcount.append(int(line))
                            dict.update({int(number) : (mean(means_pcount),std_error(means_pcount))})

                            #print(dict)
                        with open(dat_file,'a') as g:
                            # if opt == "hypersplit":
                            #     #g.write("#Rules #Threads #Blocks #Binths #Processing Time #Proc_Time_Error\n \n")
                            # else:
                            #g.write("#Rules #Threads #Blocks #Processing Time #Proc_Time_Error  \n \n")
                            for key in sorted(dict.keys()):
                                item = dict[key]
                                out = [str(key)]
                                for elem in item:
                                    out.append(str(elem))
                                g.write('\t'.join(out)  + "\n")
                # if opt == "fwc_and_hyp":
                #     for binth in binths:
                #         valids = [x for x in pcounts if "_" + engine + "_" in x and cb_set in x and opt in x and "binth_" + str(binth) in x]
                #         if not valids:
                #             continue
                #         dict = {}
                #         dat_dir = os.path.join(plot_dir,cb_set,opt)
                #         if not os.path.exists(dat_dir):
                #             os.makedirs(dat_dir)
                #         dat_file = os.path.join(dat_dir,('_'.join([cb_set,engine,opt,"binth", binth,"pcount.dat"])))
                #         for file in valids:
                #             means_pcount = []
                #             number = os.path.basename(file).split("_")[1]
                #             #print(file)
                #             with open(file,'r') as f:
                #                 for line in f.readlines():
                #                     if(int(line) < 150000):
                #                         means_pcount.append(int(line))
                #             dict.update({int(number) : (mean(means_pcount),std(means_pcount))})
                #
                #             #print(dict)
                #         with open(dat_file,'a') as g:
                #             # if opt == "hypersplit":
                #             #     #g.write("#Rules #Threads #Blocks #Binths #Processing Time #Proc_Time_Error\n \n")
                #             # else:
                #             #g.write("#Rules #Threads #Blocks #Processing Time #Proc_Time_Error  \n \n")
                #             for key in sorted(dict.keys()):
                #                 item = dict[key]
                #                 out = [str(key)]
                #                 for elem in item:
                #                     out.append(str(elem))
                #                 g.write('\t'.join(out)  + "\n")

def add_zerocount(input_dir,output_dir):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    iptables_file = os.path.join("iptables_zero_pcount")
    ipfw_file = os.path.join("ipfw_zero_pcount")
    pf_file = os.path.join("pf_zero_pcount")
    ipf_file = os.path.join("ipf_zero_pcount")

    pcounts = []
    for root, dirs, files in os.walk(input_dir):
        for file in files:
            if "_pcount" in file:
                pcounts.append(os.path.join(root,file))

    sets = set()
    binths = set()
    blocks = set()
    opts = set()
    engines = set()

    for file in pcounts:
        values = os.path.basename(file).split('_')
        cb_set = values[0]
        engines.add(values[2])
        if values[3] == "firewall":
            opt = values[3] + "_" + values[4]
            opts.add(opt)
        elif values[3] == "simple" or values[3] == "fdd":
            opt = values[3] + "_" + values[4]
            opts.add(opt)
        elif values[3] == "fwc":
            opt = values[3] + "_" + values[4] + "_" + values[5]
            opts.add(opt)
        else:
            opt = values[3]
            opts.add(opt)
        if opt == "hypersplit" or opt == "fwc":
            binth = values[values.index("binth")+1]
            binths.add(binth)
        if "block" in file:
            block = values[values.index("block")+1]
            blocks.add(block)
        #print(thread)

        sets.add(cb_set)

    for cb_set in sets:
        for engine in engines:
            for opt in opts:
                for binth in binths:
                    for block in blocks:
                        engine = "ipf"
                        if opt == "hypersplit" or opt == "fwc_and_hyp":
                            if engine == "iptables":
                                output_file = "_".join([cb_set,"0","iptables",opt,"binth",binth,"block",block,"pcount"])
                                shutil.copyfile(iptables_file,os.path.join(output_dir,output_file))
                            elif engine == "ipfw":
                                output_file = "_".join([cb_set,"0","ipfw",opt,"binth",binth,"block",block,"pcount"])
                                shutil.copyfile(ipfw_file,os.path.join(output_dir,output_file))
                            elif engine == "pf":
                                output_file = "_".join([cb_set,"0","pf",opt,"binth",binth,"block",block,"pcount"])
                                shutil.copyfile(pf_file,os.path.join(output_dir,output_file))
                            elif engine == "ipf":
                                output_file = "_".join([cb_set,"0","ipf",opt,"binth",binth,"block",block,"pcount"])
                                shutil.copyfile(ipf_file,os.path.join(output_dir,output_file))
                        else:
                            if engine == "iptables":
                                output_file = "_".join([cb_set,"0","iptables",opt,"block",block,"pcount"])
                                shutil.copyfile(iptables_file,os.path.join(output_dir,output_file))
                            elif engine == "ipfw":
                                output_file = "_".join([cb_set,"0","ipfw",opt,"block",block,"pcount"])
                                shutil.copyfile(ipfw_file,os.path.join(output_dir,output_file))
                            elif engine == "pf":
                                output_file = "_".join([cb_set,"0","pf",opt,"block",block,"pcount"])
                                shutil.copyfile(pf_file,os.path.join(output_dir,output_file))
                            elif engine == "ipf":
                                output_file = "_".join([cb_set,"0","ipf",opt,"block",block,"pcount"])
                                shutil.copyfile(ipf_file,os.path.join(output_dir,output_file))

