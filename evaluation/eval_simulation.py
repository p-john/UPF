import os
# parser = argparse.ArgumentParser(description='Gather data and gnuplot them')
# parser.add_argument('-i','--input', help='Input Directory', required= True)
# parser.add_argument('-o','--output', help='Output Directory', required=True)
# args = parser.parse_args()



# # files = os.listdir(args.input)
# files = []
#
# for root, subdirs, filenames in os.walk(args.input):
#     # for file in files:
#     files.extend([ os.path.join(root, f) for f in filenames ])
#
#
# #
# files = [x for x in files if "sim_results" in x]
#
# rulesets = set()
# for file in files:
#     temp = os.path.basename(file).split('_')
#     rulesets.add(temp[0])
#
# def get_result_value(filepath, valuetype):
#     value = 0
#     for line in open(filepath,'r'):
#         if valuetype in line:
#             value = (''.join(ele for ele in line if ele.isdigit() or ele == '.'))
#             return value
#
# def aggregate_values(files, aggregate_by, valuetype):
#     for file in files:
#         if aggregate_by not in file:
#             continue
#
#         agg_value = next(x for x in file.split(os.path.sep) if aggregate_by in x).split(aggregate_by)[1]
#         agg_value = agg_value.split('_')[1]
#         # if(aggregate_by == "partitions"):
#         #     print(next(x for x in file.split(os.path.sep) if aggregate_by in x))
#         #     print(agg_value)
#         orig_dir = os.path.dirname(file)
#         dirlist = orig_dir.split(os.path.sep)
#         # print(dirlist)
#         # print("AGGREGATE BY " + aggregate_by)
#         dirlist = [x for x in dirlist if not x.startswith(aggregate_by)]
#         dir = os.path.join(*dirlist)
#         # print("AGGREGATE BY " + aggregate_by)
#         # print(dir)
#         # print(file)
#         basenamelist = os.path.basename(file).split('_')
#         del basenamelist[basenamelist.index(aggregate_by) + 1]
#         basename = '_'.join(basenamelist)
#         output_dir = os.path.join(args.output, "aggregated_" + str(aggregate_by), dir)
#         if not os.path.exists(output_dir):
#             os.makedirs(output_dir)
#         output_file = os.path.join(output_dir, basename + "_aggregated_"+ str(aggregate_by) + "_" + valuetype)
#         with open(file, 'r'):
#             value = get_result_value(file,valuetype)
#             with open(output_file,'a') as f:
#                 f.write(agg_value + "  " + value + "\n")
#
#
# saxpac_all = [x for x in files if "sax_pac" in x]
# saxpac_complete_hypersplit = [x for x in saxpac_all if "complete" in x and "use_hypersplit" in x]
# saxpac_complete = [x for x in saxpac_all if "complete" in x and "use_hypersplit" not in x]
# saxpac = [x for x in saxpac_all if "complete" not in x and "use_hypersplit" not in x]
#
# hypersplit_all = [x for x in files if "binth" in x]
# hypersplit_complete = [x for x in files if "binth" in x and "complete" in x]
# hypersplit = [x for x in files if "binth" in x and "complete" not in x]
#
# originals = [x for x in files if "original" in x]
#
# evals = [saxpac,
#          saxpac_complete,
#          saxpac_complete_hypersplit,
#          hypersplit,
#          hypersplit_complete,
#          originals]
#
# types =  ["Mean","Median","Minimum", "Maximum"]
# aggregates = ["partitions", "binth", "threshold"]
# aggregates.extend(rulesets)
#
# for list in evals:
#     for aggregate in aggregates:
#         for type in types:
#             aggregate_values(list,aggregate, type)
#
#
