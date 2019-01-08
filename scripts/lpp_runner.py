import sys
import shlex
import subprocess
import tempfile
import os

input_file = sys.argv[1]
corelist = sorted([int(x) for x in sys.argv[2].split(",")], reverse=True)

with tempfile.NamedTemporaryFile("w") as hmetis:
    # convert to hmetis
    subprocess.call(["./partitionToMetis.py", input_file], stdout=hmetis)

    for cores in corelist:
        # run lpp
        cmd = "../kahypar/build/kahypar/application/HyperedgeLPP -h {} --blocks={} --epsilon=0.03 --iterations=100 --mode=direct" \
              " --objective=km1 --seed=42 -p ../kahypar/config/km1_direct_kway_sea18.ini".format(hmetis.name, cores)
        subprocess.call(shlex.split(cmd), stdout=subprocess.DEVNULL)

        # convert to ddf
        filename = "{}.part{}.epsilon0.03.seed42.LPP".format(hmetis.name, cores)
        process = subprocess.Popen(["./kahyparOutput2partition.py", filename, str(cores)],
                                   stdout=subprocess.PIPE,
                                   universal_newlines=True)
        stdout, _ = process.communicate()
        print(stdout, end="")
        os.remove(filename)
