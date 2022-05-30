#!/usr/bin/env python3

from itertools import product
import argparse

parser = argparse.ArgumentParser(description='Make combinations')
for i in range(1,10):
    parser.add_argument(f'-{i}', f'--list{i}', nargs='+',
                                help=f'List {i}')
args = parser.parse_args()
args=vars(args)


lists=[args[f"list{i}"] for i in range(1,10) if args[f"list{i}"]]
for x in product(*lists):
    print(" ".join(x))

