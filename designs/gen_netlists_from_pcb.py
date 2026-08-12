#!/usr/bin/env python3
"""Generate .net files from PCB data in .epro2 archives."""

import json
import re
import time
import zipfile
from pathlib import Path

BASE_DIR = Path("/home/lxq/AiEDA/iPCL-PCB/designs")


def extract_netlist_from_pcb(pcb_content):
    """Extract netlist from EasyEDA Pro PCB format."""
    lines = pcb_content.strip().split('\n')
    
    components = {}  # comp_id -> {designator, value, footprint}
    pad_nets = []    # (comp_id, pad_num, net_name)
    
    for line in lines:
        try:
            row = json.loads(line)
        except:
            continue
        if not isinstance(row, list) or not row:
            continue
        
        cmd = row[0]
        
        if cmd == "COMPONENT" and len(row) >= 8:
            comp_id = row[1]
            comp_info = {"id": comp_id}
            # The dict at index 7 contains Name, Value, etc.
            if isinstance(row[7], dict):
                comp_info["value"] = row[7].get("Value", "")
                comp_info["name"] = row[7].get("Name", "")
            components[comp_id] = comp_info
            
        elif cmd == "ATTR" and len(row) >= 9:
            # row[3] = parent comp id, row[7] = attr name, row[8] = attr value
            parent_id = row[3]
            attr_name = row[7] if isinstance(row[7], str) else ""
            attr_value = row[8] if len(row) > 8 else ""
            
            if parent_id in components and attr_name:
                if attr_name == "Designator":
                    components[parent_id]["designator"] = attr_value
                elif attr_name == "Footprint":
                    components[parent_id]["footprint"] = attr_value
                elif attr_name == "Value" and not components[parent_id].get("value"):
                    components[parent_id]["value"] = attr_value
                    
        elif cmd == "PAD_NET" and len(row) >= 4:
            # ["PAD_NET", comp_id, pad_num, net_name, net_id]
            comp_id = row[1]
            pad_num = str(row[2])
            net_name = row[3] if row[3] else ""
            if net_name:
                pad_nets.append((comp_id, pad_num, net_name))
    
    # Build netlist
    nets = {}
    comp_list = []
    seen_des = set()
    
    for comp_id, comp in components.items():
        des = comp.get("designator", "")
        if des and des not in seen_des:
            seen_des.add(des)
            comp_list.append({
                "designator": des,
                "value": comp.get("value", ""),
                "footprint": comp.get("footprint", ""),
            })
    
    for comp_id, pad_num, net_name in pad_nets:
        comp = components.get(comp_id, {})
        des = comp.get("designator", "")
        if des and net_name:
            nets.setdefault(net_name, []).append(f"{des}.{pad_num}")
    
    return nets, comp_list


def write_netlist(nets, components, output_path, project_name):
    """Write netlist file."""
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(f"* Netlist: {project_name}\n")
        f.write(f"* Extracted from PCB layout data\n")
        f.write(f"* Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write(f"* Components: {len(components)}, Nets: {len(nets)}\n")
        f.write("*\n\n")
        
        f.write("$COMPONENTS\n")
        for comp in sorted(components, key=lambda c: c["designator"]):
            line = f"  {comp['designator']}"
            if comp.get("value"):
                line += f"  {comp['value']}"
            if comp.get("footprint"):
                line += f"  {{{comp['footprint']}}}"
            f.write(line + "\n")
        f.write("$END_COMPONENTS\n\n")
        
        f.write("$NETS\n")
        for net_name in sorted(nets.keys()):
            pins = nets[net_name]
            if pins:
                f.write(f"  {net_name}\n")
                for pin in sorted(set(pins)):
                    f.write(f"    {pin}\n")
        f.write("$END_NETS\n")


def main():
    print("Generating .net files from PCB data in .epro2 archives")
    print("=" * 60)
    
    ok = fail = skip = 0
    
    for entry in sorted(BASE_DIR.iterdir()):
        meta_path = entry / "metadata.json"
        if not meta_path.is_file():
            continue
        
        with open(meta_path) as f:
            meta = json.load(f)
        
        name = meta.get("name", entry.name)
        idx = meta.get("index", 0)
        
        epro2_files = list(entry.glob("*.epro2"))
        if not epro2_files:
            print(f"[{idx:02d}] {name}: SKIP (no .epro2)")
            skip += 1
            continue
        
        # Read PCB from .epro2
        pcb_content = None
        epro2_path = epro2_files[0]
        with zipfile.ZipFile(epro2_path) as zf:
            for zname in zf.namelist():
                if zname.endswith(".epcb"):
                    with zf.open(zname) as f:
                        pcb_content = f.read().decode("utf-8")
                    break
        
        if not pcb_content:
            print(f"[{idx:02d}] {name}: SKIP (no .epcb in archive)")
            skip += 1
            continue
        
        nets, components = extract_netlist_from_pcb(pcb_content)
        
        if not components:
            print(f"[{idx:02d}] {name}: FAIL (no components parsed)")
            fail += 1
            continue
        
        safe_name = re.sub(r'[/\\:*?"<>|]', '_', name)
        net_path = entry / f"{safe_name}.net"
        write_netlist(nets, components, net_path, name)
        
        size_kb = net_path.stat().st_size // 1024
        print(f"[{idx:02d}] {name}: OK ({len(components)} comps, {len(nets)} nets, {size_kb} KB)")
        ok += 1
    
    print(f"\n{'=' * 60}")
    print(f"DONE: {ok} OK, {fail} FAIL, {skip} SKIP")


if __name__ == "__main__":
    main()
