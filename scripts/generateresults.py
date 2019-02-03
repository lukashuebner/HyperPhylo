import subprocess

REPEATS_FILES = [
    "../datasets/extracted/59-s.repeats",
    "../datasets/extracted/404-s.repeats",
    "../datasets/extracted/128-s.repeats",
    "../datasets/extracted/59-l.repeats",
    "../datasets/extracted/404-l.repeats",
    "../datasets/extracted/128-l.repeats",
    "../datasets/extracted/sm_part24_11756.repeats",
    "../datasets/extracted/sm_part12_20753.repeats",
    "../datasets/extracted/sm_part3_31854.repeats",
    "../datasets/extracted/sm_part1_170859.repeats",
]

REPEATS_NAMES = [
    "59s",
    "404s",
    "128s",
    "59l",
    "404l",
    "128l",
    "sm_part24_11756",
    "sm_part12_20753",
    "sm_part3_31854",
    "sm_part1_170859",
]

RESULTS_FOLDER = "../results/jp_vs_naive/quality_134_run4/"

# LOSS = ""
LOSS = "--loss"
# OUTPUT_FILE = "worstrcc.csv"
OUTPUT_FILE = "repeatsloss.csv"
# HEADER = "instance,k,judicious,naive,lowerbound"
HEADER = "instance,k,judicious,naive"

outfile = open(RESULTS_FOLDER + OUTPUT_FILE, "w")
outfile.write(HEADER + "\n")
outfile.close()
for idx, rfile in enumerate(REPEATS_FILES):
    cmd_string = "./create_csv_for_quality_tests.py"
    cmd_string += " --print_name "
    cmd_string += LOSS + " "
    cmd_string += rfile + " "
    cmd_string += RESULTS_FOLDER + " "
    cmd_string += REPEATS_NAMES[idx] + " "
    cmd_string += " | "
    cmd_string += "./csv_transpose.py"

    subprocess.call(cmd_string, shell=True, stdout=open(RESULTS_FOLDER + OUTPUT_FILE, "a"))
