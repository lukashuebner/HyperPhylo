#!/usr/bin/env python3
import itertools
import argparse

from Elem import Elem


def parse_arguments():
	parser = argparse.ArgumentParser(description="Split one partition of a MSA to k CPUs just by splitting the sites "
												 "into k blocks.")

	parser.add_argument('partition_file', help="the partitions file you want to split")
	parser.add_argument('k', type=int, help="the number of blocks you want to split the partition into")

	args = parser.parse_args()
	return args


def split_very_naive(n, k):
	"""Split n sites into k blocks of roughly same size."""
	split = []
	splitsize = float(n) / k
	n_range = range(n)
	for i in range(k):
		split.append(n_range[int(round(i * splitsize)):int(round((i + 1) * splitsize))])
	return split


def generate_T(H, cm):
	return set(itertools.combinations(H, cm))


def generate_S(T, E):
	S = set()
	# for all sets in T create a set in S
	for t in T:
		S_j = set()
		# in the set in S are all e that are a subset of t
		for e in E:
			if all(x in t for x in e):
				S_j.add(e)
		S.add(Elem("S_" + "".join(map(str, S_j)), S_j))

	return S


def find_minimum_set_in_S_covering_E(E, S):
	P = set()
	result = set()
	while P != E:
		# find longest difference set
		longest_diffset = set()
		for S_j in S:
			diff = S_j - P
			if len(diff) > len(longest_diffset):
				longest_diffset = diff

		P.update(longest_diffset)
		result.add(Elem("S_" + "".join(map(str, longest_diffset)), longest_diffset))

	return result



def minimum_k_and_d(T, E):
	S = generate_S(T, E)
	minimum_set = find_minimum_set_in_S_covering_E(E, S)
	return len(minimum_set)


def get_number_of_sites_from_partition_file(file):
	partition_lines = open(file, 'r').readlines()
	first_partition = partition_lines[0].split()
	return len(first_partition)


def print_split(split, k):
	"""Print the split in the format of a data distribution file."""
	# Already included "number of partitions" so the code can be extended more easily later (hopefully)
	number_of_partitions = 1

	print(k)
	for i in range(k):
		print('CPU' + str(i + 1), str(number_of_partitions))
		for j in range(number_of_partitions):
			print("partition_" + str(j), end=' ')
			for l in split[i]:
				print(str(l), end=' ')
			print()


def generate_E(V: set, H: set):
	E = set()
	for idx, v in enumerate(V):
		e_i = set()
		for h in H:
			if v in h:
				e_i.add(h)
		E.add(Elem("E" + str(idx + 1), e_i))

	return E



def main():
	args = parse_arguments()
	V = {1, 2, 3, 4, 5, 6}
	H = {
		Elem("H1", {1, 2, 3}),
		Elem("H2", {4, 5}),
		Elem("H3", {2, 6}),
		Elem("H4", {3, 5, 6})
	}
	E = generate_E(V, H)
	minimum_k_and_d(generate_T(H, 3), E)
	# number_of_sites = get_number_of_sites_from_partition_file(args.partition_file)
	# split = split_very_naive(number_of_sites, args.k)
	# print_split(split, args.k)


if __name__ == "__main__":
	main()
