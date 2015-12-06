from __future__ import division
import os, sys
from collections import Counter


def aggregate_sim_data(output_dir):
    files = os.listdir(output_dir)
    buckets = [x for x in files if "bucket" in x]
    sim_stats = [x for x in files if "sim_stats" in x]
    # for bucket in buckets:
    #     sum_bucket(output_dir, bucket)

    # for stat in sim_stats:
    #     sum_sim_stats(output_dir, stat)


def cum_bucket(output_dir,bucket_path):
    current_count = 0
    last_number = 0
    cum_path = os.path.join(bucket_path + "_cum.dat")
    with open(cum_path,'a') as g:
        with open(bucket_path,'r') as f:
            for line in f:
                if "#" in line:
                    continue
                number = int(line.split()[0])
                if number < last_number:
                    current_count = 0
                current_count += int(line.split()[1])
                cum = current_count / 1000000
                g.write(line.split()[0] + "     " + str(cum) + "\n")
                last_number = int(line.split()[0])
    return cum_path

def aggregate_test_data(output):
    files = os.listdir(output)
    pcount = [x for x in files if "pcount" in x]

    for pc in pcount:
        mean_packet_count(output, pc)

def aggregate_stats(sim_dir):
    files = os.listdir(sim_dir)
    stats = [x for x in files if "sim_stats" in x]
    set = stats[0].split('_')[0]
    stats = sorted(stats, key=lambda elem : int(elem.split('_')[1]))
    with open(os.path.join(sim_dir,set + "_result.dat"),'a') as g:
        g.write("#Rules      #Min       #Max        #Mean       #Median \n")
        for file in stats:
            num_rules = int(file.split('_')[1])
            with open(os.path.join(sim_dir,file),'r') as f:
                for line in f:
                    values = line.split()
                    g.write(str(num_rules) +"           " + values[0]+
                            "          " +values[1]+ "           " +
                            values[2]+ "         " + values[3]+ "\n")


def aggregate_proc_data(output):
    files = os.listdir(output)
    procs = [x for x in files if "proc" in x]

    for proc in procs:
        sum_procs(output, proc)


def aggregate_size_data(output):
    files = os.listdir(output)
    sizes = [x for x in files if "size" in x]

    for size in sizes:
        sum_sizes(output, size)


def sum_sizes(output, size):
    size_count_before = 0
    size_count_after = 0
    num = 0
    with open(os.path.join(output, size), 'r') as f:
        for line in f:
            num += 1
            if "Rules Before" in line:
                value = int(line.split("-")[1])
                size_count_before = int(value)
            if "Rules After" in line:
                value = int(line.split("-")[1])
                size_count_after += int(value)
    size_count_after = size_count_after / num

    with open(os.path.join(output, size), 'w') as f:
        f.write("#Rules Before      #Rules After \n")
        f.write(str(size_count_before) + "      "
                + str(size_count_after) + "\n")


def sum_procs(output, proc):
    proc_count = 0
    thread = 0
    num = 0
    with open(os.path.join(output, proc), 'r') as f:
        for line in f:
            num += 1
            if "Processing Time" in line:
                value = int(line.split("-")[1])
                proc_count += int(value)
            if "Thread" in line:
                thread = int(line.split("-")[1])

    proc_count = proc_count / num

    with open(os.path.join(output, proc), 'w') as f:
        f.write("#Threads       #Processing Time \n")
        f.write(str(thread)+ "      "+ str(proc_count) + "\n")


def mean_packet_count(output, stat):
    packet_count = 0
    num = 0
    with open(os.path.join(output, stat), 'r') as f:
        for line in f:
            value = line.split()[0]
            num += 1
            packet_count += int(value)

    packet_count = round(packet_count / num)

    with open(os.path.join(output, stat), 'w') as f:
        f.write("#Packet Count \n")
        f.write(str(packet_count) + "\n")


def sum_bucket(output_dir, bucket_path):
    filepath = os.path.join(bucket_path + "_sum.dat")
    sum_bucket = Counter()
    with open(bucket_path, 'r') as f:
        for line in f:
            if "#Bucket     #Count" in line:
                return
            values = line.split()
            sum_bucket.update({int(values[0]): int(values[1])})

    with open(filepath, 'w') as g:
        g.write("#Bucket     #Count \n")
        for key, value in sorted(sum_bucket.items()):
            g.write(str(key) + "      " + str(value) + "\n")
    return filepath

def sum_sim_stats(output_dir, stat):
    min_count = 0
    max_count = 0
    mean_count = 0
    median_count = 0

    with open(os.path.join(output_dir, stat), 'r') as f:
        length = 0
        for line in f:
            if "#Min     #Max    #Mean   #Median" in f:
                return
            length += 1
            values = line.split()
            min_count += int(values[0])
            max_count += int(values[1])
            mean_count += float(values[2])
            median_count += int(values[3])
        print(length)
        min_count = round(min_count / length)
        max_count = round(max_count / length)
        mean_count = round(mean_count / length)
        median_count = round(median_count / length)

    with open(os.path.join(output_dir, stat), 'w') as f:
        f.write("#Min     #Max    #Mean   #Median + \n")
        f.write(str(min_count) + "     " + str(max_count) + "      " +
                str(mean_count) + "     " + str(median_count) + "\n")
