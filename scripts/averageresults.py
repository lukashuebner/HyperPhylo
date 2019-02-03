import csv

RESULT_FOLDERS = [
    "../results/jp_vs_naive/quality_129_run3/",
    "../results/jp_vs_naive/quality_130_run2/",
    "../results/jp_vs_naive/quality_130_run5/",
    "../results/jp_vs_naive/quality_134_run1/",
    "../results/jp_vs_naive/quality_134_run4/"
]

OUT_FOLDER = "../results/jp_vs_naive/"

# Read everything worstrcc
allresults = []
header = None
for result_folder in RESULT_FOLDERS:
    with open(result_folder + "worstrcc.csv", "r") as csvfile:
        r = csv.reader(csvfile, delimiter=",")
        header = next(r)
        results = [row for row in r]
    allresults.append(results)

# Average worstrcc
average = []
for idx_row in range(0, len(allresults[0])):
    elements = [list(map(int, allresults[x][idx_row][2:4])) for x in range(0, len(allresults))]
    sums = [sum(x) for x in zip(*elements)]
    avg = [str(round(x / len(allresults))) for x in sums]
    average.append(allresults[0][idx_row][0:2] + avg + allresults[0][idx_row][4:])

# Write average csv repeatloss
with open(OUT_FOLDER + "averaged_worstrcc.csv", "w") as outfile:
    w = csv.writer(outfile, delimiter=",")
    w.writerow(header)
    for row in average:
        w.writerow(row)


# Read everything repeatloss
allresults = []
header = None
for result_folder in RESULT_FOLDERS:
    with open(result_folder + "repeatsloss.csv", "r") as csvfile:
        r = csv.reader(csvfile, delimiter=",")
        header = next(r)
        results = [row for row in r]
    allresults.append(results)

# Average repeatloss
average = []
for idx_row in range(0, len(allresults[0])):
    elements = [list(map(int, allresults[x][idx_row][2:4])) for x in range(0, len(allresults))]
    sums = [sum(x) for x in zip(*elements)]
    avg = [str(round(x / len(allresults))) for x in sums]
    average.append(allresults[0][idx_row][0:2] + avg)

# Write average csv repeatloss
with open(OUT_FOLDER + "averaged_repeatsloss.csv", "w") as outfile:
    w = csv.writer(outfile, delimiter=",")
    w.writerow(header)
    for row in average:
        w.writerow(row)
