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

sets = set()
number_of_rules = set()
engines = set()
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


pkt_thrpt = os.path.join(output_dir, "packet_throughput")
if not os.path.exists(pkt_thrpt):
    os.makedirs(pkt_thrpt)

# Original Set Evaluation
for set in sets:
    for engine in engines:
        files = [x for x in filelist if set in x and "_" + engine in x
                 and "original" in x]
        for file in files:
            number = file.split('_')[1]
            print(file)
            file_path = os.path.join(input_dir, file)
            with open(file_path,'r') as r:
                value = r.readline().split()
                packet_count = value[2]
            stat_file = os.path.join(pkt_thrpt, (set + "_" + engine + "_original"))
            f = open(stat_file,'a')
            f.write(str(number) + "     " + str(packet_count) + "\n")


#Optimized Sets Evaluation
for set in sets:
    for engine in engines:
        for binth in binths:
            for subtree in subtrees:
                files = [x for x in filelist if set in x and "_" + engine in x
                         and "binth_" + binth in x and "subtrees_" +
                         subtree in x ]
                for file in files:
                    number = file.split('_')[1]
                    print(file)
                    file_path = os.path.join(input_dir, file)
                    with open(file_path,'r') as r:
                        value = r.readline().split()
                        packet_count = value[2]
                    stat_file = os.path.join(pkt_thrpt,
                        (set + "_" + engine + "_binth_" + binth +
                         "_subtrees_" + subtree))
                    f = open(stat_file,'a')
                    f.write(str(number) + "     " + str(packet_count) + "\n")



# # Make Gnuplots - maybe
#
# filelist = os.listdir(pkt_thrpt)
#
# for file in filelist:
#
#     gnuplot_file = os.path.join(output_dir, ("gnuplot_" + str(file)))
#
#     stream = ""
#     stream += ("set terminal png noenhanced\n")
#     stream +=("set output \"" + os.path.join(output_dir, "plots", str(file)).replace("\\","/") + ".png\"" + "\n")
#     stream +=("set title \" Packet Throughput by Number of Rules for " +
#               str(file) + "\"" + " font \"Arial, 14\"\n")
#     stream +=("set style fill solid 1.0000000 border -1\n")
#     stream +=("set xlabel \"Number of Subtrees\" font \"Arial, 12\"\n")
#     stream +=("set ylabel \"Total Depth of all subtrees combined\" font \"Arial, 12\"\n")
#     stream +=("set pointsize 1.2\n")
#     stream +=("plot \"" + str(file)+ "\"" + " u 1:2 w boxes notitle lc rgb \"red\"\n")
#
#     with open(gnuplot_file,'w') as f:
#         f.write(stream)
#
#     if not os.path.exists(os.path.join(output_dir,"plots")):
#         os.makedirs(os.path.join(output_dir, "plots"))
#     os.system("gnuplot " + gnuplot_file)
#





