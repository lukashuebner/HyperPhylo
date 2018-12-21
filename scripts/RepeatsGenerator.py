import random
import os
import tempfile
from subprocess import Popen

count = 0

while True:
    partitionCount = 1
    innerNodeCount = 3
    siteCount = random.randint(3, 10)

    finished = True

    output = "{} {}\n".format(partitionCount, innerNodeCount)
    output += "partition_0 {}\n".format(siteCount)
    for innerNode in range(innerNodeCount):
        repeatClassCount = random.randint(2, 5)
        line = ""
        for site in range(siteCount):
            line += "{} ".format(random.randint(0, repeatClassCount - 1))

        # Check if line is valid
        linelist = list(map(int, list(filter(None, line.split(" ")))))
        listmin = min(linelist)
        listmax = max(linelist)
        if listmin != 0 or set(linelist) != set(range(listmin, listmax + 1)):
            print("INVALID, SKIPPING!")
            finished = False
            break

        output += line + "\n"

    if not finished:
        continue

    print(output)
    with tempfile.NamedTemporaryFile(mode="w") as rfile:
        rfile.write(output)
        rfile.flush()

        process = Popen(["../JudiciousPartitioning/cmake-build-debug/JudiciousCpp", rfile.name, "4,8,12,16,24,32,48,64"])
        process.communicate()

        if process.returncode and "y" in input("Do you want to save the file?"):
            with open(os.path.join(os.path.expanduser("~"), "output.repeats"), "w") as ofile:
                ofile.write(output)
        else:
            count += 1
            print("[1;30mSuccessfully ran {} inputs.[0m".format(count))
