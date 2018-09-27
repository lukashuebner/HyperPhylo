str = """<1%: 772934ms
<10%: 6720.19ms
<20%: 2278.77ms
<50%: 6594.23ms
<75%: 3867.87ms
Runtime: 1.57191e+06ms
totalTime: 792434ms

"""

totalTime = 0
for line in str.splitlines():
    if "totalTime" in line:
        totalTime = float(line.split(" ")[1].replace("ms", ""))

for line in str.splitlines():
    if "<0.01%" in line:
        print("<0.01%: {}%".format((float(line.split(" ")[1].replace("ms", "")) / totalTime) * 100))
    if "<0.1%" in line:
        print("<0.10%: {}%".format((float(line.split(" ")[1].replace("ms", "")) / totalTime) * 100))
    if "<1%" in line:
        print("<1.00%: {}%".format((float(line.split(" ")[1].replace("ms", "")) / totalTime) * 100))
    if "<10%" in line:
        print("<10.0%: {}%".format((float(line.split(" ")[1].replace("ms", "")) / totalTime) * 100))
    if "<20%" in line:
        print("<20.0%: {}%".format((float(line.split(" ")[1].replace("ms", "")) / totalTime) * 100))
    if "<50%" in line:
        print("<50.0%: {}%".format((float(line.split(" ")[1].replace("ms", "")) / totalTime) * 100))
    if "<75%" in line:
        print("<75.0%: {}%".format((float(line.split(" ")[1].replace("ms", "")) / totalTime) * 100))
    if "<100%" in line:
        print("<100.%: {}%".format((float(line.split(" ")[1].replace("ms", "")) / totalTime) * 100))
