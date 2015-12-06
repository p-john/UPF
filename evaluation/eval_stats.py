import os, argparse, io, shutil
parser = argparse.ArgumentParser(description='Gather data and gnuplot them')
parser.add_argument('-i','--input', help='Input Directory', required= True)
parser.add_argument('-o','--output', help='Output Directory', required=True)
args = parser.parse_args()


input_dir = os.path.join(args.input)
output_dir = os.path.join(args.output)

if not os.path.exists(output_dir):
    os.makedirs(output_dir)

filelist = os.listdir(input_dir)
filelist = [x for x in filelist if "stats" in x]

sets = set()
engines = set()
number_of_rules = set()
binths = set()
subtrees = set()

for file in filelist:
    values = file.split('_')
    sets.add(values[0])
    number_of_rules.add(values[1])
    engines.add(values[2])
    if len(values) > 6:
        print(values)
        binths.add(values[4])
        subtrees.add(values[6])

stat_file = "stats"
stat_filepath = os.path.join(output_dir, stat_file)
with open(stat_filepath,'w') as f:
    f.write("#Rule Set   " + "#Rules    " + "#Binth   " +
            "#Subtrees  " + "#Processing Time   " + "#Total Depth   \n")
    f.close()

for set in sets:
    for binth in binths:
        files = [x for x in filelist if set in x and "_binth_" + binth in x]
        for file in files:
            number = file.split('_')[1]
            subtree = file.split('_')[6]
            with open(os.path.join(input_dir, file),'r') as f:
                max_depth = 0
                proc_time = 0
                for line in f.readlines():
                    if "Processing" in line:
                        proc_time = line.split('-')[1].rstrip('\n')
            write_line = set +"           " + number +"       " + binth + \
                         "           " + subtree + "         " + str(proc_time) + \
                         "            " +  str(max_depth + "\n")
            print(write_line)
            with open(stat_filepath, 'a') as g:
                g.write(write_line)
