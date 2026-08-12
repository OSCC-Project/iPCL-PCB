#!/usr/bin/env bash
# Report .net/.epro2 coverage for the Excel typical-circuit list.

set -Eeuo pipefail
BASE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
XLSX="$BASE_DIR/典型电路样例 lxq.xlsx"

python3 - "$BASE_DIR" "$XLSX" <<'PY'
import json
import os
import sys
import zipfile
import xml.etree.ElementTree as ET

base, xlsx = sys.argv[1:]
ns = {"x": "http://schemas.openxmlformats.org/spreadsheetml/2006/main"}

with zipfile.ZipFile(xlsx) as zf:
    shared = []
    root = ET.fromstring(zf.read("xl/sharedStrings.xml"))
    for si in root.findall("x:si", ns):
        shared.append("".join(t.text or "" for t in si.iterfind(".//x:t", ns)))
    sheet = ET.fromstring(zf.read("xl/worksheets/sheet1.xml"))
    names = []
    for row in sheet.findall(".//x:row", ns):
        if int(row.attrib.get("r", "0")) < 2:
            continue
        cell = row.find("x:c[@r='D%s']" % row.attrib["r"], ns)
        if cell is None:
            continue
        value = cell.find("x:v", ns)
        if value is not None and cell.attrib.get("t") == "s":
            names.append(shared[int(value.text)])

metadata = {}
for entry in os.listdir(base):
    path = os.path.join(base, entry, "metadata.json")
    if not os.path.isfile(path):
        continue
    try:
        with open(path, encoding="utf-8") as fh:
            data = json.load(fh)
        metadata[data.get("name", entry)] = entry
    except (OSError, ValueError):
        pass

print("name\tstatus\tnet_count\tepro2_count\tdirectory")
missing_projects = 0
missing_files = 0
for name in names:
    folder_name = metadata.get(name)
    if not folder_name:
        missing_projects += 1
        print(f"{name}\tMISSING_DIRECTORY\t0\t0\t-")
        continue
    folder = os.path.join(base, folder_name)
    net = []
    epro2 = []
    for root, _, files in os.walk(folder):
        for filename in files:
            if filename.lower().endswith(".net"):
                net.append(os.path.join(root, filename))
            elif filename.lower().endswith(".epro2"):
                epro2.append(os.path.join(root, filename))
    status = "OK" if net and epro2 else "MISSING_FILES"
    if status != "OK":
        missing_files += 1
    print(f"{name}\t{status}\t{len(net)}\t{len(epro2)}\t{folder_name}")

print(f"SUMMARY\tprojects={len(names)}\tmissing_directories={missing_projects}\tprojects_missing_net_or_epro2={missing_files}")
PY
