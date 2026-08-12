# KiCad PCB 布局布线报告 — 音视频KVM切换器 光纤、模拟音频、HDMI

**日期**: 2026-08-12
**Case**: kicad_pcb_case11
**工具**: KiCad 10.99.0 (`kicad-cli`) + 自定义 EasyEDA Pro `.epcb` → KiCad PCB 转换器
**状态**: 完成

## 来源

| 项目 | 内容 |
|------|------|
| 设计目录 | `/home/lxq/AiEDA/iPCL-PCB/designs/07_音视频KVM切换器 光纤、模拟音频、HDMI` |
| 源归档 | `音视频KVM切换器 光纤、模拟音频、HDMI.epro2` |
| PCB 文档 | `PCB1.epcb` |
| PCB UUID | `3731a25e04264d4fa46338222c0f7fde` |
| 板名 | `Board1` |


## 转换流程

1. 从 `.epro2` ZIP 中抽取目标 `.epcb`。
2. 解析 PCB JSON 行命令并生成 `.kicad_pcb` / `.kicad_pro`。
3. 用 `kicad-cli pcb export` 导出 SVG、Gerber、钻孔和 DRC 产物。

## PCB 统计

| 指标 | 数值 |
|------|------|
| 元件数 | 398 |
| 网络数 | 328 |
| 走线段数 | 5421 |
| 过孔数 | 1496 |
| 铺铜区数 | 1897 |
| 铜层数 | 4 |
| 铜层 | F.Cu, In1.Cu, In2.Cu, B.Cu |

## 输出文件

- `音视频KVM切换器 光纤、模拟音频、HDMI.kicad_pcb`：KiCad PCB 文件
- `layout_front.svg` / `layout_back.svg`
- `layer_f_cu.svg` / `layer_b_cu.svg` / `routing_all.svg`
- `pcb_front.svg` / `pcb_back.svg` / `pcb_all_layers.svg`
- `drc_report.json`
- `gerbers/`
