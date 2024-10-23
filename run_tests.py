# I'm a PY hater and I use PY. I'm a hypocrite, yes.
# in PATH_TO_TESTS take all .rr files and compare their output with _out.txt files with the same name in EXPECTED_OUT_DIR

from os import walk, system

PATH_TO_TESTS = "./examples"
EXPECTED_OUT_DIR = "tests"
APPEND_TEST_WITH = "_out.txt"
EXE_NAME = "a.out"

f = []
for (dirpath, dirnames, filenames) in walk(PATH_TO_TESTS):
    f.extend(filenames)
    break

# for every file, 
for file in f:
    # f = open(file, "r")
    outfile = file.split(".rr")[0] + APPEND_TEST_WITH
    print("--" + file + ":")
    system(f"./{EXE_NAME} < {PATH_TO_TESTS}/{file} | diff {PATH_TO_TESTS}/{EXPECTED_OUT_DIR}/{outfile} -")