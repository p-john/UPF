__author__ = 'Asto_2'
import os
from deploy_and_test import deploy_and_test


def test_jumpsets(jump_dir):
    files = os.listdir(jump_dir)
    sets = [x for x in files if "trace" not in x]
    trace_file = os.path.join(jump_dir, next(x for x in files if "trace" in x))
    engines = ["pf"]
    output_dir = os.path.join("test_jumpsets")
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    for engine in engines:
        for set in sets:
            set_file = os.path.join(jump_dir, set)
            if "_"+ engine+"_" in set:
                deploy_and_test(set_file, trace_file, engine, output_dir)
