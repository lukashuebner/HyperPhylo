#!/usr/bin/env python3

import itertools
import argparse

import converter
from Elem import Elem


def parse_arguments():
    parser = argparse.ArgumentParser(description="Split one partition of a MSA to k CPUs using judicious partitioning.")

    parser.add_argument('partition_file', help="the partitions file you want to split")
    parser.add_argument('k', type=int, help="the number of blocks you want to split the partition into")

    args = parser.parse_args()
    return args


def generate_T(H, cm_plus_d):
    return set(itertools.combinations(H, cm_plus_d))


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
    return minimum_set, len(minimum_set)


def generate_E(V: set, H: set):
    E = set()
    for idx, v in enumerate(V):
        e_i = set()
        for h in H:
            if v in h:
                e_i.add(h)
        E.add(Elem("E" + str(idx + 1), e_i, {v}))

    return E


def partitioner(numCPUs, V, H):
    E = generate_E(V, H)
    cm = len(max(E, key=len))  # maximum hyperdegree
    m = len(H)
    min_max_L = cm
    for d in range(m - cm):
        S_star, k = minimum_k_and_d(generate_T(H, cm + d), E)
        if k > numCPUs:
            # create new E
            E = set()
            used_hypernodes = set()
            for idx, s in enumerate(S_star):
                E_i = set()
                E_i_hypernodes = set()
                for e in s:
                    E_i.update(e)
                    # add only hypernodes to the element E_i that aren't already used in another element E_j
                    diff = e.contained_hypernodes - used_hypernodes
                    used_hypernodes.update(diff)
                    E_i_hypernodes.update(diff)
                E.add(Elem("E" + str(idx + 1), E_i, E_i_hypernodes))

        else:
            min_max_L = cm + d
            # extract partitions
            V = set()
            for idx, s in enumerate(S_star):
                V_i = set()
                for e in s:
                    V_i.update(e.contained_hypernodes)
                V.add(Elem("V" + str(idx + 1), V_i))

            return min_max_L, V


def partition_file_to_hypergraph(file_path):
    V_raw, H_raw = converter.partition_file_to_hypergraph(file_path)
    V = set(V_raw)
    H = set()
    for idx, h in enumerate(H_raw):
        H.add(Elem("H" + str(idx + 1), set(h)))

    return V, H


def print_ddf(k, blocks):
    """Print the split in the format of a data distribution file."""
    print(k)
    for i, block in enumerate(sorted(blocks, key=str)):
        print('CPU' + str(i + 1), str(1))
        print("partition_0", len(block), end=' ')
        for l in block:
            print(str(l), end=' ')
        print()


def main():
    args = parse_arguments()

    # V = {1, 2, 3, 4, 5, 6}
    # H = {
    # 	Elem("H1", {1, 2, 3}),
    # 	Elem("H2", {4, 5}),
    # 	Elem("H3", {2, 6}),
    # 	Elem("H4", {3, 5, 6})
    # }

    V, H = partition_file_to_hypergraph(args.partition_file)
    l, partitions = partitioner(args.k, V, H)

    print_ddf(args.k, partitions)


if __name__ == "__main__":
    main()
