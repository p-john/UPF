__author__ = 'Asto_2'
import os,sys
from evaluation import cum_bucket, sum_bucket
from statistics import stdev,mean,median

def cumulative_plots(input_dir, plot_dir):
    files = os.listdir(input_dir)
    buckets = []
    for root, dirs, files in os.walk(input_dir):
        for file in files:
            if "buckets.dat" in file:
                buckets.append(os.path.join(root,file))
    # buckets = [x for x in files if "buckets.dat" in x]

    dict = {}
    for file in buckets:
        num = os.path.basename(file).split("_")[1]
        if num in dict.keys():
            dict[num].append(file)
        else:
            dict.update({num : [file]})

    if not os.path.exists(plot_dir):
        os.makedirs(plot_dir)


    #create cumulative plot data
    for key in dict.keys():
        cum_files = []
        for value in dict[key]:
            sum_file = sum_bucket(input_dir,value)
            cum_files.append(cum_bucket(input_dir,sum_file))
            os.unlink(sum_file)

        create_cumulative_gnuplot(cum_files,plot_dir)
        print(cum_files)
        for file in cum_files:
            os.unlink(file)


def create_cumulative_gnuplot(input_files,plot_dir):
    gnuplot_file = os.path.join(plot_dir, os.path.basename(input_files[0]) + ".gnuplot")
    with open(gnuplot_file, 'a') as g:
        g.write("set terminal png \n")
        g.write("set output \"" + os.path.join(plot_dir,os.path.basename(input_files[0])) + ".png\" \n")
        g.write("plot ")
        for file in input_files[:-1]:
            g.write("\"" + file + "\" u 1:2 w l cum, \" \n")
        g.write("\"" + input_files[-1]+ "\" u 1:2 w l cum \n")
    os.system("gnuplot " + gnuplot_file)
    # os.system("move " + os.path.basename(input_files[0]) + ".png" + " " + plot_dir)


def simulation_plot(input_dir,plot_dir):
    if not os.path.exists(plot_dir):
        os.makedirs(plot_dir)
    sim_files = []
    for root, dirs, files in os.walk(input_dir):
        for file in files:
            if "_sim_stats.dat" in file:
                sim_files.append(os.path.join(root,file))

    for file in sim_files:
        binth = ""
        values = os.path.basename(file).split('_')
        cb_set = values[0]
        number = values[1]
        opt= values[3]
        dict = {}
        medians = []
        means = []
        maxs = []
        mins = []
        dat_dir = os.path.join(plot_dir,cb_set)
        if not os.path.exists(dat_dir):
            os.makedirs(dat_dir)
        dat_file = os.path.join(dat_dir,(cb_set + "_" + opt + "_sum.dat"))
        with open(file,'r') as f:
            for line in f.readlines():
                stats = line.split()
                means.append(float(stats[2]))
                medians.append(int(stats[3]))
                maxs.append(int(stats[1]))
                mins.append(int(stats[0]))
            dict.update({int(number) : (mean(mins),stdev(mins),
                                        mean(maxs),stdev(maxs),
                                        mean(means),stdev(means),
                                        mean(medians),stdev(medians))})
        with open(dat_file,'a') as g:
            # g.write("#Rules #Min #MinError #Max #MaxError #Mean #MeanError #Median #MedianError \n \n")
            for key in sorted(dict.keys()):
                item = dict[key]
                out = '\t'.join([str(number),
                                 str(item[0]),
                                 str(item[1]),
                                 str(item[2]),
                                 str(item[3]),
                                 str(item[4]),
                                 str(item[5]),
                                 str(item[6]),
                                 str(item[7])])
                g.write(out + "\n")

    cb_set_dirs = []
    for root, dirs, files in os.walk(plot_dir):
        for dir in dirs:
            cb_set_dirs.append(os.path.join(dir))

    for cb_set in cb_set_dirs:
        gnuplot_file = os.path.basename(os.path.normpath(cb_set)) + "_median.gnuplot"

        with open(gnuplot_file, 'w') as g:
            g.write("set terminal postscript eps size 3.5,2.62 enhanced color \
                    font 'Helvetica,18' linewidth 1 \n")
            g.write("set xlabel \"Size of Ruleset\" \n")
            g.write("set ylabel \"Number of median checked Rules\" \n")
            g.write("set output \"" + file + ".png\" \n")
            g.write("plot \"<sort -s -n -k 1,1" + file + "\" u 1:8:9 smooth unique w yerrorbars,\
                    \"<sort -s -n -k 1,1" )


        gnuplot_file = file + "_max.gnuplot"
        with open(gnuplot_file, 'w') as g:
            g.write("set terminal postscript eps size 3.5,2.62 enhanced color \
                    font 'Helvetica,18' linewidth 1 \n")
            g.write("set xlabel \"Size of Ruleset\" \n")
            g.write("set ylabel \"Number of maximum checked Rules\" \n")
            g.write("set output \"" + file + ".png\" \n")
            g.write("plot \"" + file + "\" u 1:2 smooth unique w error bars\n")
        # os.system("gnuplot " + gnuplot_file)

def proctime_plot(input_dir,plot_dir):
    if not os.path.exists(plot_dir):
        os.makedirs(plot_dir)
    procs = []
    for root, dirs, files in os.walk(input_dir):
        for file in files:
            if "_stats.dat" in file:
                procs.append(os.path.join(root,file))

    for file in procs:
        opt = ""
        binth = "-"
        block = ""
        values = os.path.basename(file).split('_')
        cb_set = values[0]
        number = values[1]

        if values[2] == "firewall" or values[2] == "simple":
            opt = values[2] + "_" + values[3]
        else:
            opt = values[2]
        if opt == "hypersplit":
            binth = values[values.index("binth")+1]
        block = values[values.index("block")+1]
        threads = values[values.index("threads")+1]
        # if opt == "hypersplit":
        #     binth = values[4]
        #     block = values[6]
        #     threads = values[8]
        # else:
        #     block = values[4]
        #     threads = values[6]

        valids = [x for x in procs if cb_set in x and opt in x]

        for file in valids:
            dict = {}
            means_proc = []
            dat_dir = os.path.join(plot_dir,cb_set,opt)
            if not os.path.exists(dat_dir):
                os.makedirs(dat_dir)
            dat_file = os.path.join(dat_dir,(cb_set + "_" + opt + "_block_" + block
                                             + "_binth_" + binth + "_threads_"
                                             + threads + "_sum.dat"))
            print(file)
            with open(file,'r') as f:
                for line in f.readlines():
                    if "Processing" in line:
                        proc_time = line.split('-')[1].rstrip('\n')
                        means_proc.append(int(proc_time))
                    if "Threads" in line:
                        threads = line.split('-')[1].rstrip('\n')
                if opt == "hypersplit":
                    dict.update({int(number) : (threads,block,mean(means_proc),stdev(means_proc),binth)})
                else:
                    dict.update({int(number) : (threads,block,mean(means_proc),stdev(means_proc))})

            with open(dat_file,'a') as g:
                # if opt == "hypersplit":
                #     #g.write("#Rules #Threads #Blocks #Binths #Processing Time #Proc_Time_Error\n \n")
                # else:
                   #g.write("#Rules #Threads #Blocks #Processing Time #Proc_Time_Error  \n \n")
                for key in sorted(dict.keys()):
                    tuple = dict[key]
                    if opt == "hypersplit":
                        g.write(str(key) + " " +
                                str(tuple[0]) + "  " + str(tuple[1])
                                + "  " + str(tuple[2])+ "  " + str(tuple[3])
                                + "  " + str(tuple[4]) + "\n")
                    else:
                        g.write(str(key) + " " +
                                str(tuple[0])+  "  " + str(tuple[1])
                                + "  " +  str(tuple[2])+ "   " +  str(tuple[3]) + "\n")
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