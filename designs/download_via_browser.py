#!/usr/bin/env python3
"""
Download .epro2 and .net files using Selenium browser automation.

This script opens the EasyEDA Pro editor in a browser, logs in (interactively
or via saved cookies), and exports project files for all 28 typical circuits.

Prerequisites:
    pip install selenium
    # Firefox + geckodriver must be installed

Usage:
    # Interactive login (browser window pops up for you to log in):
    python3 designs/download_via_browser.py --login

    # Using saved cookies from a previous session:
    python3 designs/download_via_browser.py

    # Process a single project:
    python3 designs/download_via_browser.py --project 1
"""

import argparse
import json
import os
import sys
import time
import zipfile
from pathlib import Path

try:
    from selenium import webdriver
    from selenium.webdriver.firefox.options import Options
    from selenium.webdriver.firefox.service import Service
    from selenium.webdriver.common.by import By
    from selenium.webdriver.support.ui import WebDriverWait
    from selenium.webdriver.support import expected_conditions as EC
except ImportError:
    print("ERROR: selenium not installed. Run: pip install selenium")
    sys.exit(1)

BASE_DIR = Path(__file__).resolve().parent
COOKIE_FILE = BASE_DIR / ".lceda_cookies.json"
PRO_EDITOR_URL = "https://pro.lceda.cn/editor"


def save_cookies(driver, path):
    cookies = driver.get_cookies()
    with open(path, "w") as f:
        json.dump(cookies, f)
    print(f"Cookies saved to {path}")


def load_cookies(driver, path):
    if not path.exists():
        return False
    with open(path) as f:
        cookies = json.load(f)
    driver.get("https://pro.lceda.cn/editor")
    time.sleep(3)
    for cookie in cookies:
        cookie.pop("sameSite", None)
        cookie.pop("expiry", None)
        try:
            driver.add_cookie(cookie)
        except Exception:
            pass
    return True


def create_driver(headless=True):
    options = Options()
    if headless:
        options.add_argument("--headless")
    options.add_argument("--no-sandbox")
    options.add_argument("--width=1920")
    options.add_argument("--height=1080")

    download_dir = str(BASE_DIR / ".downloads")
    os.makedirs(download_dir, exist_ok=True)
    options.set_preference("browser.download.dir", download_dir)
    options.set_preference("browser.download.folderList", 2)
    options.set_preference("browser.download.useDownloadDir", True)
    options.set_preference(
        "browser.helperApps.neverAsk.saveToDisk",
        "application/octet-stream,application/zip,application/json,text/plain",
    )

    service = Service("/snap/bin/geckodriver")
    return webdriver.Firefox(options=options, service=service)


def interactive_login(driver):
    """Open browser for user to log in manually."""
    driver.get("https://pro.lceda.cn/editor")
    print("\n" + "=" * 60)
    print("Please log in to your LCEDA/JLC account in the browser window.")
    print("After logging in, press Enter here to continue...")
    print("=" * 60)
    input()

    # Verify login
    result = driver.execute_script("""
        return new Promise(async (resolve) => {
            let r = await fetch('/api/user');
            let data = await r.json();
            resolve(JSON.stringify(data));
        });
    """)
    data = json.loads(result)
    if data.get("success"):
        user = data.get("result", {})
        print(f"Logged in as: {user.get('nickname', user.get('username', 'unknown'))}")
        save_cookies(driver, COOKIE_FILE)
        return True
    else:
        print("Login verification failed. Please try again.")
        return False


def check_auth(driver):
    """Check if current session is authenticated."""
    result = driver.execute_script("""
        return new Promise(async (resolve) => {
            try {
                let r = await fetch('/api/user');
                let data = await r.json();
                resolve(JSON.stringify(data));
            } catch(e) {
                resolve(JSON.stringify({success: false, message: e.message}));
            }
        });
    """)
    data = json.loads(result)
    return data.get("success", False)


def fetch_document_source(driver, doc_uuid):
    """Fetch document source via browser's authenticated session."""
    result = driver.execute_script(f"""
        return new Promise(async (resolve) => {{
            try {{
                let r = await fetch('/api/documents/{doc_uuid}');
                let data = await r.json();
                if (data.success && data.result) {{
                    resolve(JSON.stringify({{
                        success: true,
                        uuid: data.result.uuid,
                        docType: data.result.docType,
                        display_title: data.result.display_title,
                        source: data.result.source
                    }}));
                }} else {{
                    resolve(JSON.stringify({{success: false, message: data.message}}));
                }}
            }} catch(e) {{
                resolve(JSON.stringify({{success: false, message: e.message}}));
            }}
        }});
    """)
    return json.loads(result)


def get_project_documents(driver, project_uuid):
    """Get all document UUIDs for a project."""
    result = driver.execute_script(f"""
        return new Promise(async (resolve) => {{
            try {{
                let r = await fetch('/api/projects/{project_uuid}');
                let data = await r.json();
                if (data.success) {{
                    resolve(JSON.stringify({{
                        success: true,
                        name: data.result.name,
                        boards: data.result.boards,
                        editorVersion: data.result.editorVersion
                    }}));
                }} else {{
                    resolve(JSON.stringify({{success: false, message: data.message}}));
                }}
            }} catch(e) {{
                resolve(JSON.stringify({{success: false, message: e.message}}));
            }}
        }});
    """)
    return json.loads(result)


def get_oshwhub_proImages(project_uuid):
    """Get document list from oshwhub public API (no auth needed)."""
    import urllib.request

    url = f"https://oshwhub.com/api/project/{project_uuid}/proImages"
    req = urllib.request.Request(url, headers={
        "User-Agent": "Mozilla/5.0",
        "Accept": "application/json",
    })
    try:
        with urllib.request.urlopen(req, timeout=30) as resp:
            data = json.loads(resp.read())
            if data.get("success"):
                return data["result"]
    except Exception:
        pass
    return None


def process_project(driver, project):
    """Download .epro2 and .net for a single project."""
    folder = project["folder"]
    uuid = project["uuid"]
    name = project["name"]

    print(f"\n{'─'*50}")
    print(f"Project {project['index']:02d}: {name}")

    # Check existing files
    if list(folder.glob("*.net")) and list(folder.glob("*.epro2")):
        print("  SKIP: Files already exist")
        return True

    # Get project info
    proj_data = get_project_documents(driver, uuid)
    if not proj_data.get("success"):
        print(f"  ERROR: {proj_data.get('message', 'unknown')}")
        return False

    boards = proj_data.get("boards", [])
    print(f"  Boards: {len(boards)}")

    # Collect document UUIDs
    doc_uuids = set()
    for board in boards:
        doc_uuids.add(board["sch"])
        doc_uuids.add(board["pcb"])

    # Get sub-documents from proImages
    pro_images = get_oshwhub_proImages(uuid)
    if pro_images:
        for board_info in pro_images.get("boards", []):
            sch_info = board_info.get("schematic", {})
            for doc in sch_info.get("documents", []):
                doc_uuids.add(doc["uuid"])

    print(f"  Documents: {len(doc_uuids)}")

    # Fetch all document sources
    doc_sources = {}
    for doc_uuid in doc_uuids:
        time.sleep(0.3)
        doc_data = fetch_document_source(driver, doc_uuid)
        if doc_data.get("success"):
            doc_sources[doc_uuid] = doc_data
            print(f"    OK: {doc_data.get('display_title', doc_uuid[:8])}")
        else:
            print(f"    FAIL: {doc_uuid[:8]} - {doc_data.get('message', '')}")

    if not doc_sources:
        print("  ERROR: No documents downloaded")
        return False

    # Save .epro2
    epro2_path = folder / f"{name}.epro2"
    with zipfile.ZipFile(epro2_path, "w", zipfile.ZIP_DEFLATED) as zf:
        for doc_uuid, doc_data in doc_sources.items():
            source = doc_data.get("source")
            if source is None:
                continue
            doc_type = doc_data.get("docType", 0)
            title = doc_data.get("display_title", doc_uuid)
            ext = ".esch" if doc_type == 1 else ".epcb" if doc_type == 4 else ".ejson"
            content = source if isinstance(source, str) else json.dumps(source, ensure_ascii=False)
            zf.writestr(f"{doc_uuid}/{title}{ext}", content)

        zf.writestr("project.json", json.dumps({
            "uuid": uuid,
            "name": name,
            "boards": boards,
        }, ensure_ascii=False, indent=2))

    print(f"  Saved: {epro2_path.name} ({epro2_path.stat().st_size // 1024} KB)")

    # Generate .net (netlist) - try to extract from schematic sources
    sch_sources = {k: v for k, v in doc_sources.items() if v.get("docType") == 1}
    net_path = folder / f"{name}.net"

    if sch_sources:
        nets, components = extract_netlist(sch_sources)
        write_netlist_file(nets, components, net_path, name)
        print(f"  Saved: {net_path.name} ({net_path.stat().st_size // 1024} KB)")
    else:
        with open(net_path, "w") as f:
            f.write(f"* Netlist placeholder for: {name}\n")
            f.write("* Schematic sources saved in .epro2 but netlist needs manual export\n")
        print(f"  Saved: {net_path.name} (placeholder)")

    return True


def extract_netlist(sch_sources):
    """Parse EasyEDA Pro schematic source to extract netlist."""
    nets = {}
    components = []

    for doc_uuid, doc_data in sch_sources.items():
        source = doc_data.get("source", "")
        if isinstance(source, str):
            try:
                source = json.loads(source)
            except (json.JSONDecodeError, TypeError):
                continue

        if not isinstance(source, dict):
            continue

        for obj_id, obj in source.items():
            if not isinstance(obj, dict):
                continue
            obj_type = obj.get("type", "")
            if obj_type in ("component", "symbol", "schlib"):
                designator = obj.get("designator", obj.get("name", ""))
                if not designator:
                    continue
                comp = {"designator": designator, "pins": {}}
                pins = obj.get("pins", {})
                if isinstance(pins, dict):
                    for pin_id, pin in pins.items():
                        if isinstance(pin, dict):
                            net = pin.get("net", "")
                            pin_name = pin.get("name", pin_id)
                            if net:
                                comp["pins"][pin_name] = net
                                nets.setdefault(net, []).append(f"{designator}.{pin_name}")
                components.append(comp)

    return nets, components


def write_netlist_file(nets, components, path, project_name):
    """Write netlist in standard format."""
    with open(path, "w", encoding="utf-8") as f:
        f.write(f"* Netlist: {project_name}\n")
        f.write(f"* Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write(f"* Components: {len(components)}, Nets: {len(nets)}\n\n")

        f.write("$COMPONENTS\n")
        for comp in sorted(components, key=lambda c: c["designator"]):
            f.write(f"  {comp['designator']}\n")
        f.write("$END_COMPONENTS\n\n")

        f.write("$NETS\n")
        for net_name in sorted(nets.keys()):
            pins = nets[net_name]
            f.write(f"  {net_name}\n")
            for pin in sorted(pins):
                f.write(f"    {pin}\n")
        f.write("$END_NETS\n")


def load_projects():
    """Load project list from metadata files."""
    projects = []
    for entry in sorted(BASE_DIR.iterdir()):
        meta_path = entry / "metadata.json"
        if not meta_path.is_file():
            continue
        try:
            with open(meta_path) as f:
                data = json.load(f)
            projects.append({
                "index": data.get("index", 0),
                "name": data.get("name", entry.name),
                "uuid": data.get("uuid"),
                "folder": entry,
            })
        except (OSError, ValueError):
            continue
    return projects


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--login", action="store_true",
                        help="Open browser for interactive login")
    parser.add_argument("--project", "-p", type=int,
                        help="Process only project N")
    parser.add_argument("--headless", action="store_true", default=True,
                        help="Run browser in headless mode (default)")
    parser.add_argument("--no-headless", action="store_true",
                        help="Show browser window")
    args = parser.parse_args()

    headless = not args.no_headless and not args.login

    print("Starting browser...")
    driver = create_driver(headless=headless)

    try:
        if args.login:
            if not interactive_login(driver):
                sys.exit(1)
        else:
            # Try to load saved cookies
            if COOKIE_FILE.exists():
                load_cookies(driver, COOKIE_FILE)
                driver.get(PRO_EDITOR_URL)
                time.sleep(5)
                if not check_auth(driver):
                    print("Saved cookies expired. Please run with --login")
                    sys.exit(1)
                print("Authenticated with saved cookies")
            else:
                print("No saved cookies. Please run with --login first")
                sys.exit(1)

        # Load project list
        projects = load_projects()
        print(f"\nFound {len(projects)} projects")

        if args.project:
            projects = [p for p in projects if p["index"] == args.project]

        success = failed = skipped = 0
        for project in projects:
            if not project["uuid"]:
                skipped += 1
                continue
            if process_project(driver, project):
                success += 1
            else:
                failed += 1
            time.sleep(1)

        print(f"\n{'='*50}")
        print(f"Done: {success} success, {failed} failed, {skipped} skipped")

    finally:
        driver.quit()


if __name__ == "__main__":
    main()
