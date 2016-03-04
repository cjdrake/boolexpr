#!/usr/bin/env python3

"""
Parse a coverage.info file,
and report success if all files have 100% test coverage.
"""

import sys

if len(sys.argv) != 2:
    sys.exit("Usage: covall.py <file>")

covinfo = sys.argv[1]

COV = dict()

with open(covinfo) as fin:
    for line in fin:
        if line.startswith("SF:"):
            fname = line[3:-1]
        elif line.startswith("FNF:"):
            fnf = int(line[4:-1])
        elif line.startswith("FNH:"):
            fnh = int(line[4:-1])
        elif line.startswith("LF:"):
            lf = int(line[3:-1])
        elif line.startswith("LH:"):
            lh = int(line[3:-1])
        elif line.startswith("BRF:"):
            brf = int(line[4:-1])
        elif line.startswith("BRH:"):
            brh = int(line[4:-1])
        elif line.startswith("end_of_record"):
            COV[fname] = (fnf, fnh, lf, lh, brf, brh)

status = 0

for fname, entry in COV.items():
    fnf, fnh, lf, lh, brf, brh = entry
    if fnf != fnh or lf != lh:
        print("Incomplete:", fname)
        status = 1

sys.exit(status)
