from os import walk, system

PATH_TO_TESTS = "./examples"
EXPECTED_OUT_DIR = "tests"
APPEND_TEST_WITH = "_out.txt"
EXE_NAME = "a.out"

f = []
for (dirpath, dirnames, filenames) in walk(PATH_TO_TESTS):
    f.extend(filenames)
    break

system(f"rm -rf {PATH_TO_TESTS}/{EXPECTED_OUT_DIR}/")
system(f"mkdir {PATH_TO_TESTS}/{EXPECTED_OUT_DIR}")

# for every file, 
for file in f:
    outfile = file.split(".rr")[0] + APPEND_TEST_WITH
    system(f"./{EXE_NAME} < {PATH_TO_TESTS}/{file} > {PATH_TO_TESTS}/{EXPECTED_OUT_DIR}/{outfile}")