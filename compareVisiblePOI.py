f1 = f"Bike_Accessibility/Results/visiblePPOICplex.csv"
f2 = f"Bike_Accessibility/Results/visiblePPOIGraph.csv"

def read_file_lines(filename):
    with open(filename, "r") as f:
        next(f)
        lines = f.readlines()
    return [line.strip() for line in lines]
    # return lines.strip()

lines1 = read_file_lines(f1)
lines2 = read_file_lines(f2)

set1 = set(lines1)
set2 = set(lines2)

in_both = set1 & set2

only_in_f1 = set1 - set2
only_in_f2 = set2 - set1

print("Lines in {} not in {}:".format(f1, f2))
for line in sorted(only_in_f1):
    print(line)

print("\nLines in {} not in {}:".format(f2, f1))
for line in sorted(only_in_f2):
    print(line)

print("\nLines in both files:")
for line in sorted(in_both):
	print(line)
