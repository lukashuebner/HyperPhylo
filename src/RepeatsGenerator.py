import random
import os
import tempfile
from subprocess import Popen

partitionCount = 1
innerNodeCount = 3
siteCount = random.randint(3, 10)

output = "{} {}\n".format(partitionCount, innerNodeCount)
output += "partition_0 {}\n".format(siteCount)
for innerNode in range(innerNodeCount):
    repeatClassCount = random.randint(2, 5)
    for site in range(siteCount):
        output += "{} ".format(random.randint(0, repeatClassCount - 1))
    output += "\n"

print(output)

with tempfile.NamedTemporaryFile(mode="w") as rfile:
    rfile.write(output)
    rfile.flush()

    process = Popen(["../JudiciousCppOptimized/cmake-build-debug/JudiciousCpp", rfile.name, "2"])
    process.communicate()

    if "y" in input("Do you want to save the file?"):
        with open(os.path.join(os.path.expanduser("~"), "output.repeats"), "w") as ofile:
                ofile.write(output)
