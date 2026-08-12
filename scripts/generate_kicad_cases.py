#!/usr/bin/env python3
"""Generate KiCad PCB report case folders from EasyEDA Pro .epro2 archives.

The design archives in designs/* are compact .epro2 ZIP files containing
project.json plus one or more .epcb PCB documents. KiCad's EasyEDA Pro importer
expects the older .epro project metadata shape, so this script rebuilds a small
.epro wrapper per PCB document, imports it with kicad-cli, and exports the same
style of visual/manufacturing artifacts used by reports/kicad_pcb_case2.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import time
import uuid
import zipfile
from dataclasses import dataclass
from decimal import Decimal, ROUND_HALF_UP
from math import ceil, cos, pi, sin
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
DESIGNS_DIR = ROOT / "designs"
REPORTS_DIR = ROOT / "reports"
KICAD_INSTALL = ROOT / "third-party" / "kicad-install"
KICAD_CLI = KICAD_INSTALL / "bin" / "kicad-cli"
KICAD_LIB = KICAD_INSTALL / "lib"


@dataclass(frozen=True)
class PcbSource:
    design_index: int
    design_name: str
    design_dir: Path
    epro2_path: Path
    epcb_name: str
    epcb_index: int
    pcb_uuid: str
    board_name: str


@dataclass
class CaseResult:
    case_no: int
    source: PcbSource | None
    status: str
    message: str = ""
    output_dir: Path | None = None
    stats: dict[str, Any] | None = None


@dataclass(frozen=True)
class EpcbBlock:
    doc_type: str
    head: dict[str, Any]
    rows: list[list[Any]]


@dataclass(frozen=True)
class ParsedEpcb:
    blocks: list[EpcbBlock]
    unit: str
    canvas_origin: tuple[float, float]


@dataclass(frozen=True)
class PcbObject:
    kind: str
    uuid: str
    net: str
    layer: int
    data: dict[str, Any]


def sanitize_stem(value: str, fallback: str = "PCB") -> str:
    value = re.sub(r"[\\/:*?\"<>|\s]+", "_", value).strip("._")
    value = re.sub(r"_+", "_", value)
    return value[:80] or fallback


def run(cmd: list[str], cwd: Path, env: dict[str, str], timeout: int = 240) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        cmd,
        cwd=cwd,
        env=env,
        timeout=timeout,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )


def load_design_metadata(design_dir: Path) -> dict[str, Any]:
    meta_path = design_dir / "metadata.json"
    if not meta_path.exists():
        return {}

    try:
        return json.loads(meta_path.read_text(encoding="utf-8"))
    except Exception:
        return {}


def project_board_entries(project_json: dict[str, Any]) -> list[dict[str, str]]:
    boards = project_json.get("boards")
    if isinstance(boards, list):
        out = []
        for entry in boards:
            if isinstance(entry, dict):
                out.append(
                    {
                        "name": str(entry.get("name") or entry.get("title") or ""),
                        "pcb": str(entry.get("pcb") or ""),
                        "sch": str(entry.get("sch") or entry.get("schematic") or ""),
                    }
                )
        return out

    pcbs = project_json.get("pcbs")
    if isinstance(pcbs, dict):
        out = []
        for pcb_uuid, value in pcbs.items():
            title = value.get("title") if isinstance(value, dict) else value
            out.append({"name": str(title or pcb_uuid), "pcb": str(pcb_uuid), "sch": ""})
        return out

    return []


def discover_sources() -> tuple[list[PcbSource], list[Path]]:
    sources: list[PcbSource] = []
    missing: list[Path] = []

    for design_dir in sorted(DESIGNS_DIR.glob("[0-9][0-9]_*")):
        meta = load_design_metadata(design_dir)
        design_index = int(meta.get("index") or design_dir.name[:2])
        design_name = str(meta.get("name") or design_dir.name[3:])
        epro2_files = sorted(design_dir.glob("*.epro2"))

        if not epro2_files:
            missing.append(design_dir)
            continue

        found = False

        for epro2_path in epro2_files:
            try:
                with zipfile.ZipFile(epro2_path) as zf:
                    names = zf.namelist()
                    project_json = {}
                    if "project.json" in names:
                        project_json = json.loads(zf.read("project.json").decode("utf-8"))

                    board_entries = project_board_entries(project_json)
                    epcb_names = [n for n in names if n.lower().endswith(".epcb")]
            except Exception:
                continue

            if not epcb_names:
                continue

            found = True

            for epcb_index, epcb_name in enumerate(epcb_names, start=1):
                entry = board_entries[epcb_index - 1] if epcb_index <= len(board_entries) else {}
                pcb_uuid = entry.get("pcb") or uuid.uuid5(
                    uuid.NAMESPACE_URL, f"{epro2_path.resolve()}#{epcb_name}"
                ).hex
                board_name = entry.get("name") or Path(epcb_name).stem

                sources.append(
                    PcbSource(
                        design_index=design_index,
                        design_name=design_name,
                        design_dir=design_dir,
                        epro2_path=epro2_path,
                        epcb_name=epcb_name,
                        epcb_index=epcb_index,
                        pcb_uuid=pcb_uuid,
                        board_name=board_name,
                    )
                )

        if not found:
            missing.append(design_dir)

    sources.sort(key=lambda s: (s.design_index, s.epcb_index, s.epcb_name))
    return sources, missing


def pick_case_sources(sources: list[PcbSource], start_case: int, end_case: int) -> dict[int, PcbSource]:
    """Map case3-case30 to source PCBs, skipping the existing case2 source.

    case2 already covers designs/12_STM32F407.../PCB1_2.epcb. The remaining
    case range has 28 slots; use the first 28 source PCB documents after
    excluding that one.
    """

    available = [
        s
        for s in sources
        if not (s.design_index == 12 and Path(s.epcb_name).name == "PCB1_2.epcb")
    ]
    mapping: dict[int, PcbSource] = {}
    for case_no in range(start_case, end_case + 1):
        source_idx = case_no - 3
        if 0 <= source_idx < len(available):
            mapping[case_no] = available[source_idx]
    return mapping


def make_epro(source: PcbSource, out_dir: Path, project_stem: str) -> Path:
    epro_path = out_dir / f"{project_stem}.epro"
    sch_uuid = uuid.uuid5(uuid.NAMESPACE_URL, f"{source.epro2_path.resolve()}#{source.epcb_name}#sch").hex

    project = {
        "title": source.design_name,
        "pcbs": {source.pcb_uuid: source.board_name},
        "schematics": {sch_uuid: source.board_name},
        "boards": {source.board_name: {"schematic": sch_uuid, "pcb": source.pcb_uuid}},
        "footprints": {},
        "symbols": {},
        "devices": {},
    }

    with zipfile.ZipFile(source.epro2_path) as src, zipfile.ZipFile(
        epro_path, "w", zipfile.ZIP_DEFLATED
    ) as dst:
        dst.writestr("project.json", json.dumps(project, ensure_ascii=False))
        dst.writestr(f"{source.pcb_uuid}.epcb", src.read(source.epcb_name))

    return epro_path


def q(v: float) -> str:
    return format(Decimal(str(v)).quantize(Decimal('0.0001'), rounding=ROUND_HALF_UP), 'f')


EASYEDA_COORDINATE_SCALE_MM = 0.0254


def to_mm(value: Any, unit: str | None = None) -> float:
    """Match KiCad's EasyEDA Pro importer coordinate conversion.

    EasyEDA Pro .epcb files frequently label the canvas as ``mm`` while the
    stored PCB coordinates are still the 1/1000-inch internal grid.  KiCad's
    native importer applies this fixed scale to both ``mm`` and ``mil`` files.
    """
    try:
        num = float(value)
    except Exception:
        num = 0.0
    return num * EASYEDA_COORDINATE_SCALE_MM


def as_float(value: Any, default: float = 0.0) -> float:
    try:
        if value is None:
            return default
        return float(value)
    except Exception:
        return default


def as_int(value: Any, default: int = 0) -> int:
    try:
        if value is None:
            return default
        return int(value)
    except Exception:
        return default


def parse_epcb_blocks(epcb_text: str) -> ParsedEpcb:
    blocks: list[EpcbBlock] = []
    unit = 'mm'
    canvas_origin = (0.0, 0.0)

    for raw_block in epcb_text.split('\n\n'):
        rows: list[list[Any]] = []
        for line in raw_block.splitlines():
            line = line.strip()
            if not line:
                continue
            try:
                row = json.loads(line)
            except Exception:
                continue
            if isinstance(row, list) and row:
                rows.append(row)

        if not rows:
            continue

        doc_type = ''
        head: dict[str, Any] = {}
        for row in rows:
            if row[0] == 'DOCTYPE' and len(row) > 1:
                doc_type = str(row[1])
            elif row[0] == 'HEAD' and len(row) > 1 and isinstance(row[1], dict):
                head = row[1]
            elif row[0] == 'CANVAS' and len(row) > 3:
                unit = str(row[3])
                canvas_origin = (float(row[1]), float(row[2]))

        blocks.append(EpcbBlock(doc_type=doc_type, head=head, rows=rows))

    return ParsedEpcb(blocks=blocks, unit=unit, canvas_origin=canvas_origin)


def iter_epcb_objects(block: EpcbBlock) -> list[PcbObject]:
    objects: list[PcbObject] = []

    for row in block.rows:
        if not row:
            continue
        typ = row[0]
        if typ == 'COMPONENT':
            objects.append(PcbObject(kind='COMPONENT', uuid=str(row[1]), net='', layer=int(row[3]), data={
                'x': float(row[4]),
                'y': float(row[5]),
                'angle': float(row[6]),
                'meta': row[7] if len(row) > 7 and isinstance(row[7], dict) else {},
            }))
        elif typ == 'ATTR' and len(row) > 8:
            key = str(row[7])
            value = row[8]
            objects.append(PcbObject(kind='ATTR', uuid=str(row[3]), net=key, layer=as_int(row[4]), data={
                'x': as_float(row[5]),
                'y': as_float(row[6]),
                'key': key,
                'value': value,
                'font': str(row[11]) if len(row) > 11 else 'default',
                'height': as_float(row[8]) if isinstance(row[8], (int, float)) else 0.0,
                'stroke': as_float(row[9]) if len(row) > 9 and isinstance(row[9], (int, float)) else 0.0,
                'key_visible': bool(row[10]) if len(row) > 10 else False,
                'val_visible': bool(row[15]) if len(row) > 15 else False,
                'align': as_int(row[16], 3) if len(row) > 16 and isinstance(row[16], (int, float)) else 3,
                'angle': as_float(row[17]) if len(row) > 17 and isinstance(row[17], (int, float)) else 0.0,
                'inverted': bool(row[18]) if len(row) > 18 else False,
                'mirror': bool(row[20]) if len(row) > 20 else False,
                'raw': row,
            }))
        elif typ == 'PAD_NET':
            objects.append(PcbObject(kind='PAD_NET', uuid=str(row[1]), net=str(row[3]), layer=0, data={'pad': str(row[2])}))
        elif typ in {'LINE', 'ARC', 'VIA', 'POUR', 'REGION', 'POLY', 'FILL', 'TEARDROP', 'STRING', 'IMAGE'}:
            objects.append(PcbObject(kind=typ, uuid=str(row[1]), net=str(row[3]) if len(row) > 3 else '', layer=int(row[4]) if len(row) > 4 and isinstance(row[4], (int, float)) else 0, data={'row': row}))

    return objects


def parse_epcb_text(epcb_text: str) -> ParsedEpcb:
    return parse_epcb_blocks(epcb_text)


def extract_epcb_block(parsed: ParsedEpcb, doc_type: str) -> EpcbBlock | None:
    for block in parsed.blocks:
        if block.doc_type == doc_type:
            return block
    return None


def get_head_title(block: EpcbBlock, default: str) -> str:
    title = block.head.get('title') if isinstance(block.head, dict) else None
    if isinstance(title, str) and title:
        return title
    return default


def get_block_unit(block: EpcbBlock, fallback: str) -> str:
    for row in block.rows:
        if row and row[0] == 'CANVAS' and len(row) > 3:
            return str(row[3])
    return fallback


def format_mm(value: Any, unit: str) -> str:
    return q(to_mm(value, unit))


def angle_degrees(value: Any) -> float:
    angle = as_float(value)
    if abs(angle) > 360:
        angle = angle / 10.0
    return angle % 360.0


def point_mm(x: Any, y: Any, unit: str, origin: tuple[float, float]) -> tuple[str, str]:
    # The native KiCad importer does not subtract CANVAS coordinates.
    return q(to_mm(x, unit)), q(-to_mm(y, unit))


def layer_name(num: int) -> str:
    mapping = {
        1: 'F.Cu', 2: 'B.Cu', 3: 'F.SilkS', 4: 'B.SilkS', 5: 'F.Mask', 6: 'B.Mask',
        7: 'F.Paste', 8: 'B.Paste', 9: 'F.Fab', 10: 'B.Fab', 11: 'Edge.Cuts', 12: 'Edge.Cuts',
        13: 'Dwgs.User', 14: 'Eco2.User', 15: 'In1.Cu', 16: 'In2.Cu', 17: 'In3.Cu', 18: 'In4.Cu',
        19: 'In5.Cu', 20: 'In6.Cu', 21: 'In7.Cu', 22: 'In8.Cu', 23: 'In9.Cu', 24: 'In10.Cu',
        25: 'In11.Cu', 26: 'In12.Cu', 27: 'In13.Cu', 28: 'In14.Cu', 29: 'In15.Cu', 30: 'In16.Cu',
        31: 'In17.Cu', 32: 'In18.Cu', 33: 'In19.Cu', 34: 'In20.Cu', 35: 'In21.Cu', 36: 'In22.Cu',
        37: 'In23.Cu', 38: 'In24.Cu', 39: 'In25.Cu', 40: 'In26.Cu', 41: 'In27.Cu', 42: 'In28.Cu',
        43: 'In29.Cu', 44: 'In30.Cu', 48: 'F.Fab', 49: 'F.Fab', 53: 'User.4', 54: 'User.5',
        55: 'User.6', 56: 'User.7',
    }
    return mapping.get(num, 'User.1')


def pad_attr_from_shape(shape: list[Any], default_layer: str) -> tuple[str, str, str, str]:
    kind = str(shape[0]) if shape else 'RECT'
    if kind == 'RECT':
        x = float(shape[1])
        y = float(shape[2])
        rot = float(shape[3]) if len(shape) > 3 else 0.0
        return 'rect', q(x), q(y), q(rot)
    if kind == 'ELLIPSE':
        x = float(shape[1])
        y = float(shape[2])
        return 'circle', q(x), q(y), '0'
    if kind == 'OVAL':
        x = float(shape[1])
        y = float(shape[2])
        return 'oval', q(x), q(y), '0'
    return 'rect', '0', '0', '0'


def parse_outline_poly(poly_data: list[Any], unit: str, origin: tuple[float, float], *, in_fill: bool = False, max_error: float = 0.05) -> list[tuple[float, float]]:
    points: list[tuple[float, float]] = []
    prev: tuple[float, float] | None = None
    i = 0
    while i < len(poly_data):
        val = poly_data[i]
        if isinstance(val, str):
            tag = val
            if tag == 'L':
                if prev is not None:
                    points.append(prev)
                i += 1
                while i + 1 < len(poly_data) and isinstance(poly_data[i], (int, float)) and isinstance(poly_data[i+1], (int, float)):
                    prev = (float(poly_data[i]), float(poly_data[i+1]))
                    points.append(prev)
                    i += 2
                continue
            if tag == 'R':
                if i + 5 < len(poly_data):
                    sx = float(poly_data[i+1]); sy = float(poly_data[i+2]); wx = float(poly_data[i+3]); wy = float(poly_data[i+4]); angle = float(poly_data[i+5]); cr = float(poly_data[i+6]) if i + 6 < len(poly_data) and isinstance(poly_data[i+6], (int, float)) else 0.0
                    # EasyEDA rectangle height extends opposite the displayed Y axis.
                    rect_pts = [(sx, sy), (sx+wx, sy), (sx+wx, sy-wy), (sx, sy-wy), (sx, sy)]
                    if angle:
                        theta = angle * pi / 180.0
                        ct, st = cos(theta), sin(theta)
                        rotated = []
                        for px, py in rect_pts:
                            dx, dy = px - sx, py - sy
                            rotated.append((sx + dx * ct - dy * st, sy + dx * st + dy * ct))
                        rect_pts = rotated
                    points.extend(rect_pts)
                    prev = rect_pts[-1]
                    i += 7 if cr else 6
                    continue
            if tag in {'ARC', 'CARC'}:
                if prev is not None and i + 3 < len(poly_data):
                    angle = float(poly_data[i+1]) / (10.0 if in_fill else 1.0)
                    ex = float(poly_data[i+2]); ey = float(poly_data[i+3])
                    # Approximate arc with 8 segments using midpoint curvature.
                    sx, sy = prev
                    cx = (sx + ex) / 2.0
                    cy = (sy + ey) / 2.0
                    for t in range(1, 9):
                        frac = t / 8.0
                        px = sx + (ex - sx) * frac
                        py = sy + (ey - sy) * frac + ((1 - (2*frac - 1) ** 2) * (angle / 90.0) * 0.05)
                        points.append((px, py))
                    prev = (ex, ey)
                    i += 4
                    continue
            if tag == 'C' and prev is not None and i + 6 < len(poly_data):
                # cubic bezier approximation
                p1 = (float(poly_data[i+1]), float(poly_data[i+2]))
                p2 = (float(poly_data[i+3]), float(poly_data[i+4]))
                p3 = (float(poly_data[i+5]), float(poly_data[i+6]))
                sx, sy = prev
                for t_i in range(1, 9):
                    t = t_i / 8.0
                    mt = 1 - t
                    px = mt**3 * sx + 3 * mt**2 * t * p1[0] + 3 * mt * t**2 * p2[0] + t**3 * p3[0]
                    py = mt**3 * sy + 3 * mt**2 * t * p1[1] + 3 * mt * t**2 * p2[1] + t**3 * p3[1]
                    points.append((px, py))
                prev = p3
                i += 7
                continue
            if tag == 'CIRCLE' and i + 3 < len(poly_data):
                cx = float(poly_data[i+1]); cy = float(poly_data[i+2]); r = float(poly_data[i+3])
                for t_i in range(0, 17):
                    a = 2 * 3.141592653589793 * (t_i / 16.0)
                    points.append((cx + r * __import__('math').cos(a), cy + r * __import__('math').sin(a)))
                prev = (cx + r, cy)
                i += 4
                continue
        elif isinstance(val, (int, float)):
            if i + 1 < len(poly_data) and isinstance(poly_data[i+1], (int, float)):
                prev = (float(val), float(poly_data[i+1]))
                points.append(prev)
                i += 2
                continue
        i += 1

    if not points:
        return []

    # convert to board coordinates
    converted: list[tuple[float, float]] = []
    for x, y in points:
        converted.append((to_mm(x, unit), -to_mm(y, unit)))
    return converted


def _net_index(nets: dict[str, int], name: str) -> int:
    return nets.get(name, 0) if name else 0


def kstr(value: Any) -> str:
    return str(value).replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n')


def _shape_to_ki(shape: list[Any], unit: str) -> tuple[str, str, str, str]:
    kind = str(shape[0]) if shape else 'RECT'
    if kind == 'RECT':
        return 'rect', q(to_mm(shape[1], unit)), q(to_mm(shape[2], unit)), q(float(shape[3]) if len(shape) > 3 else 0.0)
    if kind == 'ELLIPSE':
        return 'circle', q(to_mm(shape[1], unit)), q(to_mm(shape[2], unit)), '0'
    if kind == 'OVAL':
        return 'oval', q(to_mm(shape[1], unit)), q(to_mm(shape[2], unit)), '0'
    return 'rect', '0', '0', '0'


@dataclass(frozen=True)
class PadSpec:
    number: str
    kind: str
    at_x: float
    at_y: float
    angle: float
    size_x: float
    size_y: float
    layers: str
    attrib: str
    drill: tuple[float, float] | None = None
    net: str = ""


def pad_shape_to_kicad(shape: Any, unit: str) -> tuple[str, float, float, float]:
    if not isinstance(shape, list) or not shape:
        return 'rect', 1.0, 1.0, 0.0

    kind = str(shape[0])
    if kind == 'RECT':
        return 'rect', max(to_mm(shape[1], unit), 0.05), max(to_mm(shape[2], unit), 0.05), as_float(shape[3]) if len(shape) > 3 else 0.0
    if kind == 'ELLIPSE':
        sx = max(to_mm(shape[1], unit), 0.05)
        sy = max(to_mm(shape[2], unit), 0.05)
        return ('circle' if abs(sx - sy) < 0.001 else 'oval'), sx, sy, 0.0
    if kind == 'OVAL':
        return 'oval', max(to_mm(shape[1], unit), 0.05), max(to_mm(shape[2], unit), 0.05), 0.0

    return 'rect', 1.0, 1.0, 0.0


def pad_layers(layer_num: int, has_hole: bool) -> tuple[str, str]:
    if has_hole:
        return '"*.Cu" "*.Mask"', 'thru_hole'
    if layer_num == 2:
        return '"B.Cu" "B.Paste" "B.Mask"', 'smd'
    return '"F.Cu" "F.Paste" "F.Mask"', 'smd'


def pad_spec_from_row(row: list[Any], unit: str, *, relative_to: tuple[float, float] = (0.0, 0.0)) -> PadSpec | None:
    if len(row) <= 10:
        return None

    number = str(row[5])
    x = to_mm(row[6], unit) - relative_to[0]
    y = -to_mm(row[7], unit) - relative_to[1]
    angle = angle_degrees(row[8])
    hole = row[9]
    shape = row[10]
    kind, sx, sy, shape_angle = pad_shape_to_kicad(shape, unit)
    angle = (angle + shape_angle) % 360.0

    drill: tuple[float, float] | None = None
    if isinstance(hole, list) and len(hole) >= 3 and str(hole[0]) in {'ROUND', 'SLOT'}:
        dx = to_mm(hole[1], unit)
        dy = to_mm(hole[2], unit)
        if dx > 0 or dy > 0:
            drill = (max(dx, 0.05), max(dy if dy > 0 else dx, 0.05))

    layers, attrib = pad_layers(as_int(row[4], 1), drill is not None)
    return PadSpec(number=number, kind=kind, at_x=x, at_y=y, angle=angle, size_x=sx, size_y=sy, layers=layers, attrib=attrib, drill=drill, net=str(row[3]) if len(row) > 3 else "")


def estimate_pad_specs(pads: list[tuple[str, str]], layer_num: int) -> list[PadSpec]:
    if not pads:
        return []

    layers, attrib = pad_layers(layer_num, False)
    count = len(pads)
    specs: list[PadSpec] = []

    if count == 1:
        pad_no, net = pads[0]
        return [PadSpec(pad_no, 'circle', 0.0, 0.0, 0.0, 1.2, 1.2, layers, attrib, None, net)]

    if count <= 2:
        pitch = 1.4
        for idx, (pad_no, net) in enumerate(pads):
            x = (idx - (count - 1) / 2.0) * pitch
            specs.append(PadSpec(pad_no, 'roundrect', x, 0.0, 0.0, 0.9, 1.0, layers, attrib, None, net))
        return specs

    if count <= 8:
        radius = max(1.0, count * 0.22)
        for idx, (pad_no, net) in enumerate(pads):
            a = 2.0 * pi * idx / count
            specs.append(PadSpec(pad_no, 'circle', radius * cos(a), radius * sin(a), 0.0, 0.75, 0.75, layers, attrib, None, net))
        return specs

    pins_per_side = ceil(count / 4)
    pitch = 0.65 if count > 40 else 0.8
    half = (pins_per_side - 1) * pitch / 2.0
    body = max(2.4, pins_per_side * pitch + 0.8)

    for idx, (pad_no, net) in enumerate(pads):
        side = min(idx // pins_per_side, 3)
        pos = idx % pins_per_side
        offset = -half + pos * pitch
        if side == 0:
            x, y, ang = offset, -body / 2.0, 90.0
        elif side == 1:
            x, y, ang = body / 2.0, offset, 0.0
        elif side == 2:
            x, y, ang = -offset, body / 2.0, 90.0
        else:
            x, y, ang = -body / 2.0, -offset, 0.0
        specs.append(PadSpec(pad_no, 'roundrect', x, y, ang, 0.42, 0.9, layers, attrib, None, net))

    return specs


def write_pad_line(pcb_lines: list[str], pad: PadSpec, nets: dict[str, int], *, net_override: str | None = None) -> None:
    net_name = pad.net if net_override is None else net_override
    drill = ""
    if pad.drill is not None:
        dx, dy = pad.drill
        if abs(dx - dy) < 0.001:
            drill = f' (drill {q(dx)})'
        else:
            drill = f' (drill oval {q(dx)} {q(dy)})'
    rr = ' (roundrect_rratio 0.25)' if pad.kind == 'roundrect' else ''
    pcb_lines.append(
        f'    (pad "{kstr(pad.number)}" {pad.attrib} {pad.kind} '
        f'(at {q(pad.at_x)} {q(pad.at_y)} {q(pad.angle)}) '
        f'(size {q(pad.size_x)} {q(pad.size_y)}){drill} '
        f'(layers {pad.layers}){rr} (net {_net_index(nets, net_name)} "{kstr(net_name)}"))'
    )


def build_board_from_epcb(epcb_text: str, project_name: str, pcb_uuid: str, out_stem: str, source_epcb_name: str) -> str:
    parsed = parse_epcb_text(epcb_text)
    pcb_block = extract_epcb_block(parsed, 'PCB')
    if pcb_block is None:
        raise ValueError('no PCB block found')

    unit = get_block_unit(pcb_block, parsed.unit)
    origin = parsed.canvas_origin
    head_title = get_head_title(pcb_block, project_name)

    rows = pcb_block.rows
    footprint_templates: dict[str, list[PadSpec]] = {}
    footprint_template_titles: dict[str, str] = {}
    for fp_block in parsed.blocks:
        if fp_block.doc_type != 'FOOTPRINT':
            continue
        fp_unit = get_block_unit(fp_block, unit)
        pad_specs = [spec for row in fp_block.rows if row and row[0] == 'PAD' for spec in [pad_spec_from_row(row, fp_unit)] if spec is not None]
        if not pad_specs:
            continue
        keys = [
            str(fp_block.head.get('uuid') or ''),
            str(fp_block.head.get('source') or ''),
            str(fp_block.head.get('title') or ''),
        ]
        for key in keys:
            if key:
                footprint_templates[key] = pad_specs
                footprint_template_titles[key] = str(fp_block.head.get('title') or key)

    nets: dict[str, int] = {'': 0}
    for row in rows:
        if row and row[0] == 'NET':
            name = str(row[1]) if len(row) > 1 else ''
            if name not in nets:
                nets[name] = len(nets)

    netnames = [name for name in nets if name]

    objects: list[PcbObject] = []
    for row in rows:
        if not row:
            continue
        typ = row[0]
        if typ == 'COMPONENT':
            objects.append(PcbObject(kind='COMPONENT', uuid=str(row[1]), net='', layer=as_int(row[3]), data={
                'x': as_float(row[4]), 'y': as_float(row[5]), 'angle': as_float(row[6]), 'meta': row[7] if len(row) > 7 and isinstance(row[7], dict) else {}
            }))
        elif typ == 'ATTR' and len(row) > 8:
            objects.append(PcbObject(kind='ATTR', uuid=str(row[3]), net=str(row[7]), layer=as_int(row[4]), data={
                'x': as_float(row[5]), 'y': as_float(row[6]), 'key': str(row[7]), 'value': row[8], 'font': str(row[11]) if len(row) > 11 else 'default',
                'height': as_float(row[8]) if isinstance(row[8], (int, float)) else 0.0, 'stroke': as_float(row[9]) if len(row) > 9 and isinstance(row[9], (int, float)) else 0.0,
                'align': as_int(row[16], 3) if len(row) > 16 and isinstance(row[16], (int, float)) else 3, 'angle': as_float(row[17]) if len(row) > 17 and isinstance(row[17], (int, float)) else 0.0,
                'inverted': bool(row[18]) if len(row) > 18 else False, 'mirror': bool(row[20]) if len(row) > 20 else False,
            }))
        elif typ == 'PAD_NET' and len(row) > 3:
            objects.append(PcbObject(kind='PAD_NET', uuid=str(row[1]), net=str(row[3]), layer=0, data={'pad': str(row[2])}))
        elif typ in {'LINE', 'ARC', 'VIA', 'POUR', 'REGION', 'POLY', 'FILL', 'TEARDROP', 'STRING', 'IMAGE', 'DIMENSION'}:
            objects.append(PcbObject(kind=typ, uuid=str(row[1]), net=str(row[3]) if len(row) > 3 else '', layer=int(row[4]) if len(row) > 4 and isinstance(row[4], (int, float)) else 0, data={'row': row}))

    # group by component id
    comp_map: dict[str, dict[str, Any]] = {}
    for obj in objects:
        if obj.kind == 'COMPONENT':
            comp_map[obj.uuid] = {'component': obj, 'attrs': [], 'padnets': []}
        elif obj.kind == 'ATTR' and obj.uuid in comp_map:
            comp_map[obj.uuid]['attrs'].append(obj)
        elif obj.kind == 'PAD_NET' and obj.uuid in comp_map:
            comp_map[obj.uuid]['padnets'].append(obj)

    # footprint library name from source tokens
    lib_nick = sanitize_stem(out_stem[:10] + '-easyedapro', fallback='easyedapro')
    pcb_lines: list[str] = []
    pcb_lines.append('(kicad_pcb (version 20240108) (generator "epcb_converter")')
    pcb_lines.append('  (general (thickness 1.6) (legacy_teardrops no))')
    pcb_lines.append('  (layers')
    pcb_lines.append('    (0 "F.Cu" signal) (1 "In1.Cu" signal) (2 "In2.Cu" signal) (31 "B.Cu" signal)')
    pcb_lines.append('    (32 "B.Adhes" user) (33 "F.Adhes" user) (34 "B.Paste" user) (35 "F.Paste" user)')
    pcb_lines.append('    (36 "B.SilkS" user) (37 "F.SilkS" user) (38 "B.Mask" user) (39 "F.Mask" user)')
    pcb_lines.append('    (40 "Dwgs.User" user) (44 "Edge.Cuts" user)')
    pcb_lines.append('    (46 "B.CrtYd" user) (47 "F.CrtYd" user) (48 "B.Fab" user) (49 "F.Fab" user))')
    pcb_lines.append('  (setup')
    pcb_lines.append('    (pad_to_mask_clearance 0.05)')
    pcb_lines.append('    (grid_origin 0 0)')
    pcb_lines.append('    (aux_axis_origin 0 0))')
    for name in netnames:
        pcb_lines.append(f'  (net {nets[name]} "{kstr(name)}")')

    # board outline / drawings / tracks / vias / zones
    comp_positions: dict[str, tuple[float, float, float, int]] = {}
    comp_nets: dict[str, list[tuple[str, str]]] = {}
    for comp_id, bundle in comp_map.items():
        comp = bundle['component'].data
        comp_positions[comp_id] = (to_mm(comp['x'], unit), -to_mm(comp['y'], unit), float(comp['angle']), int(bundle['component'].layer))
        for pn in bundle['padnets']:
            comp_nets.setdefault(comp_id, []).append((pn.data['pad'], pn.net))

    # footprint definitions assembled inline as simple footprints.
    for comp_id, bundle in comp_map.items():
        comp = bundle['component'].data
        x, y, angle, layer = comp_positions[comp_id]
        attrs = {a.data['key']: a.data['value'] for a in bundle['attrs'] if a.kind == 'ATTR'}
        fp_name = str(attrs.get('Footprint') or bundle['component'].data['meta'].get('footprint') or comp_id)
        ref = str(attrs.get('Designator') or attrs.get('Reference') or comp_id)
        template = footprint_templates.get(fp_name)
        if template:
            fp_display = footprint_template_titles.get(fp_name, fp_name)
        else:
            fp_display = fp_name
        fp_id = f'{lib_nick}:{sanitize_stem(fp_display, fp_display)}'
        pcb_lines.append(f'  (footprint "{kstr(fp_id)}" (layer "{layer_name(layer)}")')
        pcb_lines.append(f'    (at {x} {y} {q(angle)})')
        pcb_lines.append(f'    (property "Reference" "{kstr(ref)}" (at 0 -2 {q(angle)}) (layer "{layer_name(3 if layer == 1 else 4)}") (effects (font (size 0.8 0.8) (thickness 0.12))))')
        pcb_lines.append('    (property "Value" "" (at 0 2 {0}) (layer "{1}") (effects (font (size 0.8 0.8) (thickness 0.12))))'.format(q(angle), 'B.Fab' if layer == 2 else 'F.Fab'))
        pads_for_comp = comp_nets.get(comp_id, [])
        pad_net_map = {pad_no: pad_net for pad_no, pad_net in pads_for_comp}
        pad_specs = template if template else estimate_pad_specs(pads_for_comp, layer)
        if pad_specs:
            min_x = min(p.at_x - p.size_x / 2.0 for p in pad_specs)
            max_x = max(p.at_x + p.size_x / 2.0 for p in pad_specs)
            min_y = min(p.at_y - p.size_y / 2.0 for p in pad_specs)
            max_y = max(p.at_y + p.size_y / 2.0 for p in pad_specs)
            margin = 0.35
            start_x, end_x = min_x - margin, max_x + margin
            start_y, end_y = min_y - margin, max_y + margin
        else:
            start_x, end_x, start_y, end_y = -1.25, 1.25, -0.8, 0.8
        pcb_lines.append('    (fp_rect (start {0} {1}) (end {2} {3}) (layer "{4}") (width 0.05) (fill none))'.format(q(start_x), q(start_y), q(end_x), q(end_y), 'B.CrtYd' if layer == 2 else 'F.CrtYd'))
        pcb_lines.append('    (fp_rect (start {0} {1}) (end {2} {3}) (layer "{4}") (width 0.1) (fill none))'.format(q(start_x), q(start_y), q(end_x), q(end_y), 'B.Fab' if layer == 2 else 'F.Fab'))
        for pad in pad_specs:
            write_pad_line(pcb_lines, pad, nets, net_override=pad_net_map.get(pad.number, pad.net))
        pcb_lines.append('  )')

    for row in rows:
        if not row:
            continue
        typ = row[0]
        if typ == 'VIA':
            net = str(row[3]) if len(row) > 3 else ''
            x = to_mm(float(row[5]), unit)
            y = -to_mm(float(row[6]), unit)
            drill = to_mm(float(row[7]), unit)
            dia = to_mm(float(row[8]), unit)
            pcb_lines.append(f'  (via (at {q(x)} {q(y)}) (size {q(dia)}) (drill {q(drill)}) (layers "F.Cu" "B.Cu") (net {_net_index(nets, net)}))')
        elif typ == 'LINE':
            net = str(row[3]) if len(row) > 3 else ''
            layer = int(row[4]) if len(row) > 4 else 1
            if layer in {11, 12}:
                continue
            x1, y1 = point_mm(row[5], row[6], unit, origin)
            x2, y2 = point_mm(row[7], row[8], unit, origin)
            width = format_mm(row[9], unit) if len(row) > 9 else '0.1'
            pcb_lines.append(f'  (segment (start {x1} {y1}) (end {x2} {y2}) (width {width}) (layer "{layer_name(layer)}") (net {_net_index(nets, net)}))')
        elif typ == 'ARC':
            net = str(row[3]) if len(row) > 3 else ''
            layer = int(row[4]) if len(row) > 4 else 1
            if layer in {11, 12}:
                continue
            sx, sy = float(row[5]), float(row[6])
            ex, ey = float(row[7]), float(row[8])
            angle = float(row[9]) if len(row) > 9 else 0.0
            width = format_mm(row[10], unit) if len(row) > 10 else '0.1'
            # approximate with 8 segments
            pts = []
            for i in range(9):
                t = i / 8.0
                x = sx + (ex - sx) * t
                y = sy + (ey - sy) * t + (1 - (2 * t - 1) ** 2) * angle * 0.01
                pts.append(point_mm(x, y, unit, origin))
            for (x1, y1), (x2, y2) in zip(pts, pts[1:]):
                pcb_lines.append(f'  (segment (start {x1} {y1}) (end {x2} {y2}) (width {width}) (layer "{layer_name(layer)}") (net {_net_index(nets, net)}))')
        elif typ == 'POLY':
            net = str(row[3]) if len(row) > 3 else ''
            layer = int(row[4]) if len(row) > 4 else 1
            if layer in {11, 12}:
                continue
            width = format_mm(row[5], unit) if len(row) > 5 else '0.1'
            poly = row[6] if len(row) > 6 else []
            if isinstance(poly, list):
                pts = parse_outline_poly(poly, unit, origin, in_fill=False)
                if len(pts) >= 2:
                    for (x1, y1), (x2, y2) in zip(pts, pts[1:]):
                        pcb_lines.append(f'  (segment (start {q(x1)} {q(y1)}) (end {q(x2)} {q(y2)}) (width {width}) (layer "{layer_name(layer)}") (net {_net_index(nets, net)}))')
        elif typ in {'POUR', 'FILL', 'REGION', 'TEARDROP'}:
            net = str(row[3]) if len(row) > 3 else ''
            layer = int(row[4]) if len(row) > 4 and isinstance(row[4], (int, float)) else 1
            if layer in {11, 12}:
                continue
            poly_data = row[8] if typ == 'POUR' and len(row) > 8 else row[7] if typ in {'FILL', 'REGION'} and len(row) > 7 else row[4] if typ == 'TEARDROP' and len(row) > 4 else []
            if typ == 'REGION' and len(row) > 6:
                poly_data = row[6]
            elif typ == 'FILL' and len(row) > 7:
                poly_data = row[7]
            elif typ == 'TEARDROP' and len(row) > 4:
                poly_data = row[4]
            if isinstance(poly_data, list):
                if typ == 'POUR' and poly_data and isinstance(poly_data[0], list):
                    contours = [parse_outline_poly(pd, unit, origin, in_fill=False) for pd in poly_data if isinstance(pd, list)]
                else:
                    contours = [parse_outline_poly(poly_data, unit, origin, in_fill=False)]
                for pts in contours:
                    if len(pts) < 3:
                        continue
                    pcb_lines.append('  (zone (net {0}) (net_name "{1}") (layer "{2}") (hatch edge 0.508) (priority 500)'.format(_net_index(nets, net), kstr(net), layer_name(layer)))
                    pcb_lines.append('    (connect_pads (clearance 0.2))')
                    pcb_lines.append('    (min_thickness 0.1)')
                    pcb_lines.append('    (filled_areas_thickness no)')
                    pcb_lines.append('    (polygon (pts')
                    for x, y in pts:
                        pcb_lines.append(f'      (xy {q(x)} {q(y)})')
                    pcb_lines.append('    ))')
                    pcb_lines.append('  )')
        elif typ == 'STRING':
            layer = int(row[3]) if len(row) > 3 else 3
            x, y = point_mm(row[4], row[5], unit, origin)
            txt = kstr(row[6])
            font = str(row[7]) if len(row) > 7 else 'default'
            size = format_mm(row[8], unit) if len(row) > 8 else '1.0'
            stroke = format_mm(row[9], unit) if len(row) > 9 else '0.15'
            angle = q(float(row[13]) if len(row) > 13 and isinstance(row[13], (int, float)) else 0.0)
            mirror = bool(row[16]) if len(row) > 16 else False
            pcb_lines.append(f'  (gr_text "{txt}" (at {x} {y} {angle}) (layer "{layer_name(layer)}") (effects (font (size {size} {size}) (thickness {stroke}))))')
        elif typ == 'DIMENSION':
            pts = row[8] if len(row) > 8 else []
            if isinstance(pts, list) and len(pts) >= 8:
                x1, y1 = point_mm(pts[0], pts[1], unit, origin)
                x2, y2 = point_mm(pts[2], pts[3], unit, origin)
                x3, y3 = point_mm(pts[4], pts[5], unit, origin)
                x4, y4 = point_mm(pts[6], pts[7], unit, origin)
                pcb_lines.append(f'  (gr_line (start {x1} {y1}) (end {x2} {y2}) (layer "Dwgs.User") (width 0.15))')
                pcb_lines.append(f'  (gr_line (start {x3} {y3}) (end {x4} {y4}) (layer "Dwgs.User") (width 0.15))')
        elif typ == 'IMAGE':
            pass

    # board edge from board outline layer and drawing data
    for row in rows:
        if not row:
            continue
        typ = row[0]
        if typ == 'STRING':
            continue
        if typ == 'LINE' and len(row) > 4 and int(row[4]) in {11, 12}:
            x1, y1 = point_mm(row[5], row[6], unit, origin)
            x2, y2 = point_mm(row[7], row[8], unit, origin)
            width = format_mm(row[9], unit) if len(row) > 9 else '0.1'
            pcb_lines.append(f'  (gr_line (start {x1} {y1}) (end {x2} {y2}) (layer "Edge.Cuts") (width {width}))')
        elif typ == 'ARC' and len(row) > 4 and int(row[4]) in {11, 12}:
            sx, sy = float(row[5]), float(row[6])
            ex, ey = float(row[7]), float(row[8])
            angle = float(row[9]) if len(row) > 9 else 0.0
            width = format_mm(row[10], unit) if len(row) > 10 else '0.1'
            pts = []
            for i in range(9):
                t = i / 8.0
                x = sx + (ex - sx) * t
                y = sy + (ey - sy) * t + (1 - (2 * t - 1) ** 2) * angle * 0.01
                pts.append(point_mm(x, y, unit, origin))
            for (x1, y1), (x2, y2) in zip(pts, pts[1:]):
                pcb_lines.append(f'  (gr_line (start {x1} {y1}) (end {x2} {y2}) (layer "Edge.Cuts") (width {width}))')
        elif typ == 'POLY' and len(row) > 6 and int(row[4]) in {11, 12} and isinstance(row[6], list):
            pts = parse_outline_poly(row[6], unit, origin, in_fill=False)
            width = format_mm(row[5], unit) if len(row) > 5 else '0.1'
            for (x1, y1), (x2, y2) in zip(pts, pts[1:]):
                pcb_lines.append(f'  (gr_line (start {q(x1)} {q(y1)}) (end {q(x2)} {q(y2)}) (layer "Edge.Cuts") (width {width}))')
        elif typ in {'FILL', 'REGION'} and len(row) > 7 and int(row[4]) in {11, 12}:
            poly_data = row[7] if typ == 'FILL' else row[6]
            contours = poly_data if isinstance(poly_data, list) and poly_data and isinstance(poly_data[0], list) else [poly_data]
            for contour in contours:
                if not isinstance(contour, list):
                    continue
                pts = parse_outline_poly(contour, unit, origin, in_fill=False)
                for (x1, y1), (x2, y2) in zip(pts, pts[1:]):
                    pcb_lines.append(f'  (gr_line (start {q(x1)} {q(y1)}) (end {q(x2)} {q(y2)}) (layer "Edge.Cuts") (width 0.1))')

    pcb_lines.append(')')
    return '\n'.join(pcb_lines) + '\n'


def build_kicad_pcb_from_epcb(source: PcbSource, out_dir: Path, project_stem: str) -> Path:
    with zipfile.ZipFile(source.epro2_path) as zf:
        epcb_text = zf.read(source.epcb_name).decode('utf-8', errors='ignore')
    pcb_text = build_board_from_epcb(epcb_text, source.design_name, source.pcb_uuid, project_stem, source.epcb_name)
    pcb_path = out_dir / f'{project_stem}.kicad_pcb'
    pcb_path.write_text(pcb_text, encoding='utf-8')
    return pcb_path


def build_kicad_project_file(source: PcbSource, out_dir: Path, project_stem: str) -> Path:
    pro_path = out_dir / f'{project_stem}.kicad_pro'
    project = {
        'board': {'active_layer': 0, 'active_layer_preset': '', 'auto_track_width': True, 'hidden_netclasses': [], 'hidden_nets': [], 'high_contrast_mode': 0, 'net_color_mode': 1, 'opacity': {'images': 0.6, 'pads': 1.0, 'shapes': 1.0, 'tracks': 1.0, 'vias': 1.0, 'zones': 0.6}, 'prototype_zone_fills': False, 'selection_filter': {'dimensions': True, 'footprints': True, 'graphics': True, 'gridItems': True, 'keepouts': True, 'lockedItems': False, 'otherItems': True, 'pads': True, 'text': True, 'tracks': True, 'vias': True, 'zones': True}, 'visible_items': ['vias', 'footprint_text', 'footprint_anchors', 'ratsnest', 'grid', 'footprints_front', 'footprints_back', 'footprint_values', 'footprint_references', 'tracks', 'drc_errors', 'drawing_sheet', 'bitmaps', 'pads', 'zones', 'drc_warnings', 'drc_exclusions', 'locked_item_shadows', 'conflict_shadows', 'shapes', 'board_outline_area', 'ly_points', 'constraint_shadows', 'grid_items'], 'visible_layers': 'ffffffff_ffffffff_ffffffff_ffffffff', 'zone_display_mode': 0},
        'git': {'integration_disabled': False, 'repo_type': '', 'repo_username': '', 'ssh_key': ''},
        'meta': {'filename': pro_path.name, 'version': 6},
        'net_inspector_panel': {'col_hidden': [], 'col_order': [], 'col_widths': [], 'custom_group_rules': [], 'expanded_rows': [], 'filter_by_net_name': True, 'filter_by_netclass': True, 'filter_text': '', 'group_by_constraint': False, 'group_by_net_chain': False, 'group_by_netclass': False, 'show_time_domain_details': False, 'show_unconnected_nets': False, 'show_zero_pad_nets': False, 'sort_ascending': True, 'sorting_column': -1},
        'open_jobsets': [],
        'project': {'files': []},
        'schematic': {'hierarchy_collapsed': [], 'selection_filter': {'graphics': True, 'images': True, 'labels': True, 'lockedItems': False, 'otherItems': True, 'pins': True, 'ruleAreas': True, 'symbols': True, 'text': True, 'wires': True}},
    }
    pro_path.write_text(json.dumps(project, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    return pro_path


def count_s_expr_token(pcb_path: Path, token: str) -> int:
    text = pcb_path.read_text(encoding='utf-8', errors='ignore') if pcb_path.exists() else ''
    return len(re.findall(rf'\({re.escape(token)}\b', text))

def collect_stats(pcb_path: Path) -> dict[str, Any]:
    text = pcb_path.read_text(encoding="utf-8", errors="ignore") if pcb_path.exists() else ""
    layers = re.findall(r'\(\d+\s+"([^"]+)"\s+signal\)', text)
    copper = [name for name in layers if name.endswith(".Cu")]
    nets = len(re.findall(r'^\s*\(net\s+\d+\s+"', text, re.MULTILINE))
    return {
        "size_bytes": pcb_path.stat().st_size if pcb_path.exists() else 0,
        "components": count_s_expr_token(pcb_path, "footprint"),
        "nets": nets,
        "tracks": count_s_expr_token(pcb_path, "segment"),
        "vias": count_s_expr_token(pcb_path, "via"),
        "zones": count_s_expr_token(pcb_path, "zone"),
        "layer_count": max(len(copper), 0),
        "copper_layers": copper,
    }


def write_stats(path: Path, source: PcbSource, stats: dict[str, Any]) -> None:
    lines = [
        f"project: {source.design_name}",
        f"design_index: {source.design_index}",
        f"board: {source.board_name}",
        f"source_epro2: {source.epro2_path}",
        f"source_epcb: {source.epcb_name}",
        f"components: {stats['components']}",
        f"nets: {stats['nets']}",
        f"tracks: {stats['tracks']}",
        f"vias: {stats['vias']}",
        f"zones: {stats['zones']}",
        f"layer_count: {stats['layer_count']}",
        f"copper_layers: {', '.join(stats['copper_layers'])}",
        f"kicad_pcb_bytes: {stats['size_bytes']}",
    ]
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def write_flow_report(path: Path, case_no: int, source: PcbSource, stats: dict[str, Any], status: str) -> None:
    source_note = ""
    if stats["components"] == 0:
        source_note = "\n> 注：该 PCB 文档未包含 `COMPONENT` / `PAD_NET` 元件焊盘记录，报告中保留其原始屏蔽层/边框/铺铜数据。\n"

    content = f"""# KiCad PCB 布局布线报告 — {source.design_name}

**日期**: {time.strftime('%Y-%m-%d')}
**Case**: kicad_pcb_case{case_no}
**工具**: KiCad 10.99.0 (`kicad-cli`) + 自定义 EasyEDA Pro `.epcb` → KiCad PCB 转换器
**状态**: {status}

## 来源

| 项目 | 内容 |
|------|------|
| 设计目录 | `{source.design_dir}` |
| 源归档 | `{source.epro2_path.name}` |
| PCB 文档 | `{source.epcb_name}` |
| PCB UUID | `{source.pcb_uuid}` |
| 板名 | `{source.board_name}` |
{source_note}

## 转换流程

1. 从 `.epro2` ZIP 中抽取目标 `.epcb`。
2. 解析 PCB JSON 行命令并生成 `.kicad_pcb` / `.kicad_pro`。
3. 用 `kicad-cli pcb export` 导出 SVG、Gerber、钻孔和 DRC 产物。

## PCB 统计

| 指标 | 数值 |
|------|------|
| 元件数 | {stats['components']} |
| 网络数 | {stats['nets']} |
| 走线段数 | {stats['tracks']} |
| 过孔数 | {stats['vias']} |
| 铺铜区数 | {stats['zones']} |
| 铜层数 | {stats['layer_count']} |
| 铜层 | {', '.join(stats['copper_layers']) or '未识别'} |

## 输出文件

- `{source.design_name}.kicad_pcb`：KiCad PCB 文件
- `layout_front.svg` / `layout_back.svg`
- `layer_f_cu.svg` / `layer_b_cu.svg` / `routing_all.svg`
- `pcb_front.svg` / `pcb_back.svg` / `pcb_all_layers.svg`
- `drc_report.json`
- `gerbers/`
"""
    path.write_text(content, encoding="utf-8")


def write_index(path: Path, case_no: int, source: PcbSource, stats: dict[str, Any]) -> None:
    html = f"""<!doctype html>
<html lang="zh-CN">
<head>
<meta charset="utf-8">
<title>KiCad PCB Case {case_no} - {source.design_name}</title>
<style>
body {{ margin:0; font-family:-apple-system,BlinkMacSystemFont,"Noto Sans SC",Arial,sans-serif; background:#101218; color:#e8eaf0; }}
header {{ padding:24px 36px; background:#182033; border-bottom:1px solid #30384f; }}
h1 {{ margin:0 0 6px; font-size:24px; }}
.sub {{ color:#aab4cc; font-size:13px; }}
.stats {{ display:flex; flex-wrap:wrap; gap:12px; padding:18px 36px; background:#151925; }}
.stat {{ min-width:120px; padding:12px 14px; background:#202638; border:1px solid #343d58; border-radius:6px; }}
.value {{ font-size:22px; font-weight:700; color:#6fd2ff; }}
.label {{ margin-top:3px; color:#aab4cc; font-size:11px; text-transform:uppercase; }}
main {{ padding:24px 36px 40px; }}
h2 {{ margin:26px 0 12px; font-size:18px; color:#6fd2ff; }}
.grid {{ display:grid; grid-template-columns:repeat(2,minmax(0,1fr)); gap:16px; }}
.card {{ background:#181d2b; border:1px solid #30384f; border-radius:6px; overflow:hidden; }}
.card h3 {{ margin:0; padding:10px 12px; font-size:13px; background:#202638; color:#dbe5ff; }}
object {{ display:block; width:100%; min-height:320px; background:white; }}
a {{ color:#8edbff; }}
@media (max-width:900px) {{ .grid {{ grid-template-columns:1fr; }} }}
</style>
</head>
<body>
<header>
<h1>KiCad PCB 布局布线可视化报告</h1>
<div class="sub">Case {case_no} | {source.design_name} | {source.board_name}</div>
</header>
<section class="stats">
<div class="stat"><div class="value">{stats['components']}</div><div class="label">Components</div></div>
<div class="stat"><div class="value">{stats['nets']}</div><div class="label">Nets</div></div>
<div class="stat"><div class="value">{stats['tracks']}</div><div class="label">Tracks</div></div>
<div class="stat"><div class="value">{stats['vias']}</div><div class="label">Vias</div></div>
<div class="stat"><div class="value">{stats['zones']}</div><div class="label">Zones</div></div>
<div class="stat"><div class="value">{stats['layer_count']}</div><div class="label">Copper Layers</div></div>
</section>
<main>
<p>源文件：<code>{source.epro2_path}</code> / <code>{source.epcb_name}</code></p>
<p><a href="flow_report.md">查看流程报告</a> · <a href="pcb_stats.txt">查看统计文本</a></p>
<h2>布局</h2>
<div class="grid">
<div class="card"><h3>Front Layout</h3><object data="layout_front.svg" type="image/svg+xml"></object></div>
<div class="card"><h3>Back Layout</h3><object data="layout_back.svg" type="image/svg+xml"></object></div>
</div>
<h2>布线</h2>
<div class="grid">
<div class="card"><h3>F.Cu</h3><object data="layer_f_cu.svg" type="image/svg+xml"></object></div>
<div class="card"><h3>B.Cu</h3><object data="layer_b_cu.svg" type="image/svg+xml"></object></div>
<div class="card"><h3>All Routing</h3><object data="routing_all.svg" type="image/svg+xml"></object></div>
<div class="card"><h3>All Layers</h3><object data="pcb_all_layers.svg" type="image/svg+xml"></object></div>
</div>
<h2>3D</h2>
<div class="grid">
<div class="card"><h3>3D Viewer</h3><object data="3d_viewer.html" type="text/html"></object></div>
<div class="card"><h3>GLB</h3><p style="padding:12px 14px;">{('生成成功' if (path.parent / 'pcb_3d_model.glb').exists() else '未生成')}</p></div>
</div>
</main>
</body>
</html>
"""
    path.write_text(html, encoding="utf-8")


def write_3d_viewer(path: Path, case_no: int, source: PcbSource, stats: dict[str, Any]) -> None:
    html = f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="utf-8">
<title>KiCad PCB 3D Viewer - {source.design_name}</title>
<style>
body {{ margin:0; overflow:hidden; background:#0f1320; color:#e8ecf5; font-family:-apple-system,BlinkMacSystemFont,'Noto Sans SC',sans-serif; }}
#info {{ position:absolute; top:10px; left:10px; z-index:10; background:rgba(16,20,32,.82); padding:12px 14px; border:1px solid #2f3a56; border-radius:6px; }}
#info h2 {{ margin:0 0 6px; font-size:15px; }}
#info .stat {{ font-size:12px; color:#b5c0da; }}
#loading {{ position:absolute; inset:0; display:flex; align-items:center; justify-content:center; font-size:16px; color:#8ecfff; }}
canvas {{ width:100vw; height:100vh; display:block; }}
</style>
</head>
<body>
<div id="info">
  <h2>{source.design_name}</h2>
  <div class="stat">Case {case_no} | Components {stats['components']} | Nets {stats['nets']} | Vias {stats['vias']}</div>
</div>
<div id="loading">Loading 3D model...</div>
<canvas id="canvas"></canvas>
<script type="importmap">
{{
  "imports": {{
    "three": "https://cdn.jsdelivr.net/npm/three@0.160.0/build/three.module.js",
    "three/addons/": "https://cdn.jsdelivr.net/npm/three@0.160.0/examples/jsm/"
  }}
}}
</script>
<script type="module">
import * as THREE from 'three';
import {{ OrbitControls }} from 'three/addons/controls/OrbitControls.js';
import {{ GLTFLoader }} from 'three/addons/loaders/GLTFLoader.js';
const canvas = document.getElementById('canvas');
const renderer = new THREE.WebGLRenderer({{ canvas, antialias: true, alpha: false }});
renderer.setSize(window.innerWidth, window.innerHeight);
renderer.setPixelRatio(window.devicePixelRatio);
renderer.setClearColor(0x0f1320);
renderer.toneMapping = THREE.ACESFilmicToneMapping;
renderer.toneMappingExposure = 1.15;
const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.1, 10000);
camera.position.set(0, 100, 140);
const controls = new OrbitControls(camera, canvas);
controls.enableDamping = true;
controls.dampingFactor = 0.06;
scene.add(new THREE.AmbientLight(0xffffff, 0.55));
const key = new THREE.DirectionalLight(0xffffff, 0.85); key.position.set(60, 120, 80); scene.add(key);
const fill = new THREE.DirectionalLight(0x8ecfff, 0.25); fill.position.set(-70, 60, -40); scene.add(fill);
scene.add(new THREE.GridHelper(240, 48, 0x2a3551, 0x1f2740));
new GLTFLoader().load('pcb_3d_model.glb', (gltf) => {{
  const model = gltf.scene;
  const box = new THREE.Box3().setFromObject(model);
  const size = box.getSize(new THREE.Vector3());
  const center = box.getCenter(new THREE.Vector3());
  model.position.sub(center);
  scene.add(model);
  const maxDim = Math.max(size.x, size.y, size.z);
  camera.position.set(0, Math.max(80, maxDim * 0.9), Math.max(120, maxDim * 1.3));
  controls.target.set(0, 0, 0);
  controls.update();
  document.getElementById('loading').style.display = 'none';
}}, undefined, () => {{
  document.getElementById('loading').textContent = '3D model unavailable';
}});
window.addEventListener('resize', () => {{
  camera.aspect = window.innerWidth / window.innerHeight;
  camera.updateProjectionMatrix();
  renderer.setSize(window.innerWidth, window.innerHeight);
}});
(function animate(){{ requestAnimationFrame(animate); controls.update(); renderer.render(scene, camera); }})();
</script>
</body>
</html>
"""
    path.write_text(html, encoding="utf-8")


def write_skip_report(case_dir: Path, case_no: int, source: PcbSource | None, message: str) -> None:
    name = source.design_name if source else "未分配"
    text = f"""# KiCad PCB Case {case_no} 未生成

**项目**: {name}
**原因**: {message}

此目录由 `scripts/generate_kicad_cases.py` 创建，用于记录批处理无法完成的原因。
"""
    case_dir.mkdir(parents=True, exist_ok=True)
    (case_dir / "flow_report.md").write_text(text, encoding="utf-8")
    (case_dir / "index.html").write_text(
        f"<!doctype html><meta charset='utf-8'><title>Case {case_no} skipped</title>"
        f"<body><h1>Case {case_no} 未生成</h1><p>{message}</p></body>",
        encoding="utf-8",
    )


def export_artifacts(case_dir: Path, pcb_path: Path, kicad_env: dict[str, str], glb_timeout: int) -> list[str]:
    warnings: list[str] = []
    gerbers_dir = case_dir / "gerbers"
    gerbers_dir.mkdir(exist_ok=True)

    exports = [
        ("layout_front.svg", ["pcb", "export", "svg", "--layers", "F.Fab,F.CrtYd", "-o"]),
        ("layout_back.svg", ["pcb", "export", "svg", "--layers", "B.Fab,B.CrtYd", "-o"]),
        ("layer_f_cu.svg", ["pcb", "export", "svg", "--layers", "F.Cu", "-o"]),
        ("layer_b_cu.svg", ["pcb", "export", "svg", "--layers", "B.Cu", "-o"]),
        ("routing_all.svg", ["pcb", "export", "svg", "--layers", "F.Cu,B.Cu,Edge.Cuts", "-o"]),
        ("pcb_front.svg", ["pcb", "export", "svg", "--layers", "F.Cu,F.SilkS,F.Mask,Edge.Cuts", "-o"]),
        ("pcb_back.svg", ["pcb", "export", "svg", "--layers", "B.Cu,B.SilkS,B.Mask,Edge.Cuts", "-o"]),
        ("pcb_all_layers.svg", ["pcb", "export", "svg", "--layers", "F.Cu,B.Cu,F.SilkS,B.SilkS,Edge.Cuts", "-o"]),
    ]

    for out_name, args in exports:
        out_path = case_dir / out_name
        cmd = [str(KICAD_CLI), *args, str(out_path), "--page-size-mode", "2", "--exclude-drawing-sheet", str(pcb_path)]
        proc = run(cmd, case_dir, kicad_env)
        if proc.returncode != 0:
            warnings.append(f"SVG export failed for {out_name}: {proc.stdout.strip()[:500]}")

    drc = run(
        [str(KICAD_CLI), "pcb", "drc", "--format", "json", "-o", str(case_dir / "drc_report.json"), str(pcb_path)],
        case_dir,
        kicad_env,
        timeout=360,
    )
    if drc.returncode != 0:
        warnings.append(f"DRC failed: {drc.stdout.strip()[:500]}")
        drc_path = case_dir / "drc_report.json"
        if not drc_path.exists():
            drc_path.write_text(
                json.dumps(
                    {"status": "failed", "returncode": drc.returncode, "message": drc.stdout.strip()},
                    ensure_ascii=False,
                    indent=2,
                )
                + "\n",
                encoding="utf-8",
            )

    gerbers = run([str(KICAD_CLI), "pcb", "export", "gerbers", "-o", str(gerbers_dir), str(pcb_path)], case_dir, kicad_env)
    if gerbers.returncode != 0:
        warnings.append(f"Gerber export failed: {gerbers.stdout.strip()[:500]}")

    drill = run([str(KICAD_CLI), "pcb", "export", "drill", "-o", str(gerbers_dir), str(pcb_path)], case_dir, kicad_env)
    if drill.returncode != 0:
        warnings.append(f"Drill export failed: {drill.stdout.strip()[:500]}")

    try:
        glb = run(
            [str(KICAD_CLI), "pcb", "export", "glb", "--include-tracks", "--include-zones", "--include-pads", "-o", str(case_dir / "pcb_3d_model.glb"), str(pcb_path)],
            case_dir,
            kicad_env,
            timeout=glb_timeout,
        )
        if glb.returncode != 0:
            warnings.append(f"GLB export failed: {glb.stdout.strip()[:500]}")
    except subprocess.TimeoutExpired:
        warnings.append(f"GLB export timed out after {glb_timeout} seconds")

    return warnings


def generate_case(case_no: int, source: PcbSource, overwrite: bool, kicad_env: dict[str, str], skip_exports: bool, glb_timeout: int) -> CaseResult:
    case_dir = REPORTS_DIR / f"kicad_pcb_case{case_no}"

    if case_dir.exists() and any(case_dir.iterdir()):
        if not overwrite:
            return CaseResult(case_no, source, "skipped", "output directory already exists", case_dir)
        shutil.rmtree(case_dir)

    case_dir.mkdir(parents=True, exist_ok=True)
    project_stem = sanitize_stem(source.design_name)

    try:
        pcb_path = build_kicad_pcb_from_epcb(source, case_dir, project_stem)
        build_kicad_project_file(source, case_dir, project_stem)
    except Exception as exc:
        write_skip_report(case_dir, case_no, source, f"failed to build .kicad_pcb: {exc}")
        return CaseResult(case_no, source, "failed", str(exc), case_dir)

    stats = collect_stats(pcb_path)
    warnings: list[str] = []

    if not skip_exports:
        warnings = export_artifacts(case_dir, pcb_path, kicad_env, glb_timeout)

    write_stats(case_dir / "pcb_stats.txt", source, stats)
    write_flow_report(case_dir / "flow_report.md", case_no, source, stats, "完成" if not warnings else "完成，部分导出警告")
    write_3d_viewer(case_dir / "3d_viewer.html", case_no, source, stats)
    write_index(case_dir / "index.html", case_no, source, stats)

    if warnings:
        (case_dir / "export_warnings.log").write_text("\n\n".join(warnings), encoding="utf-8")

    for lock_file in case_dir.glob("~*.lck"):
        lock_file.unlink(missing_ok=True)

    return CaseResult(case_no, source, "ok", "; ".join(warnings), case_dir, stats)


def write_summary(path: Path, results: list[CaseResult], missing_designs: list[Path], unassigned: list[PcbSource]) -> None:
    lines = [
        "# KiCad PCB 批量生成摘要",
        "",
        f"生成时间: {time.strftime('%Y-%m-%d %H:%M:%S')}",
        "",
        "## Case 结果",
        "",
        "| Case | 状态 | 设计 | PCB 文档 | 说明 |",
        "|------|------|------|----------|------|",
    ]

    for result in results:
        source = result.source
        design = source.design_name if source else ""
        epcb = source.epcb_name if source else ""
        lines.append(f"| {result.case_no} | {result.status} | {design} | {epcb} | {result.message} |")

    lines.extend(["", "## 缺少顶层 .epro2/.epcb 的设计", ""])
    if missing_designs:
        for path_item in missing_designs:
            lines.append(f"- `{path_item}`")
    else:
        lines.append("- 无")

    lines.extend(["", "## 未分配到 case3-case30 的 PCB 文档", ""])
    if unassigned:
        for src in unassigned:
            lines.append(f"- `{src.epro2_path}` / `{src.epcb_name}`")
    else:
        lines.append("- 无")

    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--start-case", type=int, default=3)
    parser.add_argument("--end-case", type=int, default=30)
    parser.add_argument("--overwrite", action="store_true")
    parser.add_argument("--skip-exports", action="store_true", help="Only import .kicad_pcb, do not export SVG/Gerber/DRC")
    parser.add_argument("--glb-timeout", type=int, default=300, help="Timeout in seconds for GLB export")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    if not KICAD_CLI.exists():
        print(f"ERROR: kicad-cli not found: {KICAD_CLI}", file=sys.stderr)
        return 2

    sources, missing_designs = discover_sources()
    mapping = pick_case_sources(sources, args.start_case, args.end_case)
    assigned = set(mapping.values())
    unassigned = [
        s
        for s in sources
        if s not in assigned and not (s.design_index == 12 and Path(s.epcb_name).name == "PCB1_2.epcb")
    ]

    print(f"Discovered {len(sources)} PCB documents from .epro2 archives")
    print(f"Mapping {len(mapping)} documents to kicad_pcb_case{args.start_case}-kicad_pcb_case{args.end_case}")

    for case_no in range(args.start_case, args.end_case + 1):
        src = mapping.get(case_no)
        if not src:
            print(f"case{case_no}: no source assigned")
            continue
        print(f"case{case_no}: design {src.design_index:02d} {src.design_name} / {src.epcb_name}")

    if args.dry_run:
        return 0

    REPORTS_DIR.mkdir(exist_ok=True)
    kicad_env = os.environ.copy()
    kicad_env["LD_LIBRARY_PATH"] = f"{KICAD_LIB}:{kicad_env.get('LD_LIBRARY_PATH', '')}".rstrip(":")

    results: list[CaseResult] = []

    for case_no in range(args.start_case, args.end_case + 1):
        source = mapping.get(case_no)
        if not source:
            case_dir = REPORTS_DIR / f"kicad_pcb_case{case_no}"
            write_skip_report(case_dir, case_no, None, "no source PCB document assigned")
            results.append(CaseResult(case_no, None, "skipped", "no source PCB document assigned", case_dir))
            continue

        print(f"[case{case_no}] importing {source.epro2_path.name}::{source.epcb_name}", flush=True)
        result = generate_case(case_no, source, args.overwrite, kicad_env, args.skip_exports, args.glb_timeout)
        results.append(result)
        print(f"[case{case_no}] {result.status}: {result.message}", flush=True)

    write_summary(REPORTS_DIR / "kicad_pcb_case3_case30_summary.md", results, missing_designs, unassigned)

    ok = sum(1 for r in results if r.status == "ok")
    failed = sum(1 for r in results if r.status == "failed")
    skipped = sum(1 for r in results if r.status == "skipped")
    print(f"DONE: {ok} ok, {failed} failed, {skipped} skipped")
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
