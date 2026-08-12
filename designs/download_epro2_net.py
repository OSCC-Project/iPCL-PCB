#!/usr/bin/env python3
"""
Download .epro2 and .net files for all projects in the typical circuit list.

Requires authentication with LCEDA Pro (pro.lceda.cn).
Set the LCEDA_COOKIE environment variable with your session cookie.

To obtain the cookie:
1. Open https://pro.lceda.cn/editor in Chrome/Firefox
2. Log in with your JLC/LCEDA account
3. Open DevTools (F12) > Application > Cookies > pro.lceda.cn
4. Copy the full cookie string (all name=value pairs joined by '; ')
   At minimum you need: lceda_pro_session=...
5. Export: export LCEDA_COOKIE='lceda_pro_session=...; ...'

Usage:
    export LCEDA_COOKIE='lceda_pro_session=eyJ...'
    python3 designs/download_epro2_net.py [--project N] [--dry-run]
"""

import argparse
import json
import os
import sys
import time
import zipfile
import io
import urllib.request
import urllib.error
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parent
API_BASE = "https://pro.lceda.cn"
OSHWHUB_API = "https://oshwhub.com"

HEADERS_TEMPLATE = {
    "User-Agent": "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36",
    "Accept": "application/json",
    "Origin": "https://pro.lceda.cn",
    "Referer": "https://pro.lceda.cn/editor",
}


def get_cookie():
    cookie = os.environ.get("LCEDA_COOKIE", "")
    if not cookie:
        print("ERROR: LCEDA_COOKIE environment variable not set.", file=sys.stderr)
        print(__doc__, file=sys.stderr)
        sys.exit(1)
    return cookie


def api_request(url, cookie, method="GET", data=None):
    headers = dict(HEADERS_TEMPLATE)
    headers["Cookie"] = cookie
    if data:
        headers["Content-Type"] = "application/json"
        data = json.dumps(data).encode()

    req = urllib.request.Request(url, headers=headers, data=data, method=method)
    try:
        with urllib.request.urlopen(req, timeout=60) as resp:
            return json.loads(resp.read())
    except urllib.error.HTTPError as e:
        body = e.read().decode(errors="replace")
        try:
            return json.loads(body)
        except json.JSONDecodeError:
            return {"success": False, "code": e.code, "message": body[:200]}
    except Exception as e:
        return {"success": False, "code": -1, "message": str(e)}


def verify_auth(cookie):
    resp = api_request(f"{API_BASE}/api/user", cookie)
    if resp.get("success"):
        user = resp.get("result", {})
        print(f"Authenticated as: {user.get('nickname', user.get('username', 'unknown'))}")
        return True
    else:
        print(f"Authentication failed: {resp.get('message', 'unknown error')}", file=sys.stderr)
        return False


def get_project_info(project_uuid, cookie):
    resp = api_request(f"{API_BASE}/api/projects/{project_uuid}", cookie)
    if not resp.get("success"):
        return None
    return resp["result"]


def get_document_source(doc_uuid, cookie):
    resp = api_request(f"{API_BASE}/api/documents/{doc_uuid}", cookie)
    if not resp.get("success"):
        return None
    return resp.get("result")


def get_proImages(project_uuid):
    url = f"{OSHWHUB_API}/api/project/{project_uuid}/proImages"
    headers = {
        "User-Agent": "Mozilla/5.0",
        "Accept": "application/json",
        "Referer": "https://oshwhub.com/",
    }
    req = urllib.request.Request(url, headers=headers)
    try:
        with urllib.request.urlopen(req, timeout=30) as resp:
            data = json.loads(resp.read())
            if data.get("success"):
                return data["result"]
    except Exception:
        pass
    return None


def build_epro2(project_uuid, project_info, doc_sources, output_path):
    """Build an .epro2 file (ZIP archive of document sources)."""
    with zipfile.ZipFile(output_path, "w", zipfile.ZIP_DEFLATED) as zf:
        project_meta = {
            "uuid": project_uuid,
            "name": project_info.get("name", ""),
            "editorVersion": project_info.get("editorVersion", ""),
            "boards": project_info.get("boards", []),
        }
        zf.writestr("project.json", json.dumps(project_meta, ensure_ascii=False, indent=2))

        for doc_uuid, doc_data in doc_sources.items():
            if doc_data and isinstance(doc_data, dict):
                source = doc_data.get("source", "")
                doc_type = doc_data.get("docType", 0)
                display_name = doc_data.get("display_title", doc_uuid)

                ext = ".esch" if doc_type == 1 else ".epcb" if doc_type == 4 else ".json"
                filename = f"{display_name}_{doc_uuid}{ext}"
                if isinstance(source, str):
                    zf.writestr(filename, source)
                elif isinstance(source, dict):
                    zf.writestr(filename, json.dumps(source, ensure_ascii=False))

    print(f"  Created: {output_path.name} ({output_path.stat().st_size // 1024} KB)")


def extract_netlist_from_source(sch_sources):
    """Extract netlist connectivity from schematic source data."""
    nets = {}
    components = []

    for doc_uuid, doc_data in sch_sources.items():
        if not doc_data or doc_data.get("docType") != 1:
            continue

        source = doc_data.get("source", "")
        if isinstance(source, str):
            try:
                source = json.loads(source)
            except json.JSONDecodeError:
                continue

        if not isinstance(source, dict):
            continue

        # EasyEDA Pro schematic format stores nets and components
        # Parse the source to extract connectivity
        for key, val in source.items():
            if isinstance(val, dict):
                obj_type = val.get("type", "")
                if obj_type == "wire" or obj_type == "net_label":
                    net_name = val.get("net", val.get("name", ""))
                    if net_name:
                        nets.setdefault(net_name, [])
                elif obj_type == "component" or obj_type == "symbol":
                    designator = val.get("designator", val.get("name", ""))
                    if designator:
                        comp_info = {
                            "designator": designator,
                            "value": val.get("value", ""),
                            "footprint": val.get("footprint", ""),
                            "pins": {},
                        }
                        # Extract pin connections
                        pins = val.get("pins", {})
                        if isinstance(pins, dict):
                            for pin_id, pin_data in pins.items():
                                if isinstance(pin_data, dict):
                                    net = pin_data.get("net", "")
                                    pin_name = pin_data.get("name", pin_id)
                                    if net:
                                        comp_info["pins"][pin_name] = net
                                        nets.setdefault(net, []).append(
                                            f"{designator}.{pin_name}"
                                        )
                        components.append(comp_info)

    return nets, components


def write_netlist(nets, components, output_path, project_name=""):
    """Write netlist in a standard .net format (Telesis-like)."""
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(f"* Netlist exported from EasyEDA Pro project: {project_name}\n")
        f.write(f"* Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write(f"* Components: {len(components)}, Nets: {len(nets)}\n")
        f.write("*\n")

        # Component section
        f.write("\n$COMPONENTS\n")
        for comp in sorted(components, key=lambda c: c["designator"]):
            f.write(f"  {comp['designator']}")
            if comp["value"]:
                f.write(f" {comp['value']}")
            if comp["footprint"]:
                f.write(f" {comp['footprint']}")
            f.write("\n")
        f.write("$END_COMPONENTS\n")

        # Nets section
        f.write("\n$NETS\n")
        for net_name in sorted(nets.keys()):
            pins = nets[net_name]
            if pins:
                f.write(f"  {net_name}\n")
                for pin in sorted(pins):
                    f.write(f"    {pin}\n")
        f.write("$END_NETS\n")

    if output_path.exists():
        print(f"  Created: {output_path.name} ({output_path.stat().st_size // 1024} KB)")
    else:
        print(f"  WARNING: Failed to create {output_path.name}")


def save_raw_source_as_epro2(doc_sources, project_info, output_path):
    """Save document sources as .epro2 (ZIP format matching EasyEDA Pro export)."""
    with zipfile.ZipFile(output_path, "w", zipfile.ZIP_DEFLATED) as zf:
        for doc_uuid, doc_data in doc_sources.items():
            if not doc_data:
                continue
            source = doc_data.get("source")
            doc_type = doc_data.get("docType", 0)
            title = doc_data.get("display_title", doc_uuid)

            if source is None:
                continue

            if doc_type == 4:  # PCB
                filename = f"{title}.epcb"
            elif doc_type == 1:  # Schematic
                filename = f"{title}.esch"
            else:
                filename = f"{title}.ejson"

            content = source if isinstance(source, str) else json.dumps(source, ensure_ascii=False)
            zf.writestr(f"{doc_uuid}/{filename}", content)

        # Write project metadata
        meta = {
            "uuid": project_info.get("uuid", ""),
            "name": project_info.get("name", ""),
            "boards": project_info.get("boards", []),
            "editorVersion": project_info.get("editorVersion", ""),
        }
        zf.writestr("project.json", json.dumps(meta, ensure_ascii=False, indent=2))


def load_project_list():
    """Load the project list from metadata.json files in design directories."""
    projects = []
    for entry in sorted(BASE_DIR.iterdir()):
        meta_path = entry / "metadata.json"
        if not meta_path.is_file():
            continue
        try:
            with open(meta_path, encoding="utf-8") as f:
                data = json.load(f)
            projects.append({
                "index": data.get("index", 0),
                "name": data.get("name", entry.name),
                "uuid": data.get("uuid"),
                "folder": entry,
                "url": data.get("url", ""),
            })
        except (OSError, ValueError):
            continue
    return projects


def process_project(project, cookie, dry_run=False):
    """Download .epro2 and .net for a single project."""
    folder = project["folder"]
    uuid = project["uuid"]
    name = project["name"]

    print(f"\n{'='*60}")
    print(f"Project {project['index']:02d}: {name}")
    print(f"  UUID: {uuid}")
    print(f"  Folder: {folder.name}")

    # Check if files already exist
    existing_net = list(folder.glob("*.net"))
    existing_epro2 = list(folder.glob("*.epro2"))
    if existing_net and existing_epro2:
        print(f"  SKIP: Already has .net ({len(existing_net)}) and .epro2 ({len(existing_epro2)})")
        return True

    if dry_run:
        print("  DRY RUN: Would download files")
        return True

    # Get project info from Pro API
    info = get_project_info(uuid, cookie)
    if not info:
        print(f"  ERROR: Could not get project info from Pro API")
        return False

    boards = info.get("boards", [])
    if not boards:
        print(f"  ERROR: No boards found in project")
        return False

    print(f"  Boards: {len(boards)}")
    for b in boards:
        print(f"    {b.get('name', 'unnamed')}: SCH={b['sch'][:8]}... PCB={b['pcb'][:8]}...")

    # Collect all document UUIDs
    doc_uuids = set()
    for board in boards:
        doc_uuids.add(board["sch"])
        doc_uuids.add(board["pcb"])

    # Also get sub-documents (schematic pages) from proImages
    pro_images = get_proImages(uuid)
    if pro_images:
        for board_info in pro_images.get("boards", []):
            sch_info = board_info.get("schematic", {})
            for doc in sch_info.get("documents", []):
                doc_uuids.add(doc["uuid"])

    print(f"  Documents to fetch: {len(doc_uuids)}")

    # Download each document source
    doc_sources = {}
    sch_sources = {}
    for doc_uuid in doc_uuids:
        time.sleep(0.5)  # Rate limiting
        doc_data = get_document_source(doc_uuid, cookie)
        if doc_data:
            doc_sources[doc_uuid] = doc_data
            if doc_data.get("docType") == 1:
                sch_sources[doc_uuid] = doc_data
            print(f"    OK: {doc_data.get('display_title', doc_uuid[:8])} "
                  f"(type={doc_data.get('docType')}, "
                  f"source={'yes' if doc_data.get('source') else 'no'})")
        else:
            print(f"    FAIL: {doc_uuid[:8]}...")

    if not doc_sources:
        print(f"  ERROR: Could not download any document sources")
        return False

    # Save .epro2 file
    epro2_path = folder / f"{name}.epro2"
    save_raw_source_as_epro2(doc_sources, info, epro2_path)

    # Generate .net file from schematic sources
    if sch_sources:
        nets, components = extract_netlist_from_source(sch_sources)
        if nets or components:
            net_path = folder / f"{name}.net"
            write_netlist(nets, components, net_path, project_name=name)
        else:
            print("  WARNING: Could not extract netlist from schematic sources")
            # Save a placeholder that indicates source was obtained but netlist parsing failed
            net_path = folder / f"{name}.net"
            with open(net_path, "w") as f:
                f.write(f"* Netlist placeholder for: {name}\n")
                f.write(f"* Source documents downloaded but netlist extraction needs manual export\n")
                f.write(f"* Use EasyEDA Pro editor: File > Export > Netlist\n")
            print(f"  Created placeholder: {net_path.name}")
    else:
        print("  WARNING: No schematic sources available for netlist generation")

    return True


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--project", "-p", type=int,
                        help="Process only project N (1-based index)")
    parser.add_argument("--dry-run", "-n", action="store_true",
                        help="Show what would be done without downloading")
    parser.add_argument("--force", "-f", action="store_true",
                        help="Re-download even if files exist")
    args = parser.parse_args()

    cookie = get_cookie()

    print("Verifying authentication...")
    if not verify_auth(cookie):
        print("\nPlease set LCEDA_COOKIE with a valid session cookie.")
        print("See script docstring for instructions.")
        sys.exit(1)

    projects = load_project_list()
    print(f"\nFound {len(projects)} projects")

    if args.project:
        projects = [p for p in projects if p["index"] == args.project]
        if not projects:
            print(f"ERROR: Project {args.project} not found")
            sys.exit(1)

    success = 0
    failed = 0
    skipped = 0

    for project in projects:
        if not project["uuid"]:
            print(f"\nSKIP: {project['name']} (no UUID)")
            skipped += 1
            continue

        if not args.force:
            folder = project["folder"]
            if list(folder.glob("*.net")) and list(folder.glob("*.epro2")):
                skipped += 1
                continue

        if process_project(project, cookie, dry_run=args.dry_run):
            success += 1
        else:
            failed += 1

        time.sleep(1)  # Rate limiting between projects

    print(f"\n{'='*60}")
    print(f"SUMMARY: {success} success, {failed} failed, {skipped} skipped")
    print(f"Total: {len(projects)} projects")


if __name__ == "__main__":
    main()
