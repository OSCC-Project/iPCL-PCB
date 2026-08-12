"""
Modern Hex Viewer - 高性能十六进制查看器
Features:
- 内存映射大文件支持（只加载可见区域）
- 深色主题 UI（Modern Dark Theme）
- 虚拟化渲染（处理 GB 级文件）
- 键盘导航与选中功能
- 标准滚动条（Word/网页体验）
- 固定表头（偏移、列号、ASCII标识）
"""

import sys
import mmap
import os
from typing import Optional, Tuple, List
from dataclasses import dataclass

from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QLabel, QScrollArea, QLineEdit, QPushButton, QFileDialog,
    QFrame, QSizePolicy, QGraphicsDropShadowEffect, QStatusBar
)
from PyQt6.QtCore import Qt, QSize, QRect, pyqtSignal, QTimer, QPoint
from PyQt6.QtGui import (
    QFont, QFontDatabase, QColor, QPalette, QPainter, 
    QKeyEvent, QMouseEvent, QWheelEvent, QFontMetrics,
    QPen, QPaintEvent  
)


# ============ 配色方案 ============
class ColorScheme:
    """现代深色主题配色"""
    BG_PRIMARY = "#121212"
    BG_SECONDARY = "#1E1E1E"
    BG_TERTIARY = "#252525"
    TEXT_PRIMARY = "#E0E0E0"
    TEXT_SECONDARY = "#B0B0B0"
    TEXT_MUTED = "#666666"
    ACCENT_PRIMARY = "#00D4AA"
    ACCENT_SECONDARY = "#4A9EFF"
    ACCENT_WARNING = "#FFB74D"
    SELECTION_BG = "#1E3A5F"
    SELECTION_BORDER = "#00D4AA"
    ADDRESS_COLOR = "#4A9EFF"
    HEX_COLOR = "#E0E0E0"
    ASCII_COLOR = "#B0B0B0"
    ZERO_BYTE = "#666666"
    NON_ASCII = "#444444"
    HEADER_BG = "#181818"
    HEADER_TEXT = "#00D4AA"


@dataclass
class ViewState:
    """视图状态"""
    offset: int = 0
    bytes_per_row: int = 16
    row_height: int = 24
    visible_rows: int = 0
    file_size: int = 0
    selection_start: int = -1
    selection_end: int = -1
    cursor_pos: int = 0


class HexHeader(QWidget):
    """十六进制查看器固定表头"""
    def __init__(self, parent=None, viewport=None):
        super().__init__(parent)
        self.viewport = viewport  # 关联数据视口，同步布局参数
        self.colors = ColorScheme()
        
        # 字体与布局参数（与HexViewport保持一致）
        self.font_mono = None
        self.char_width = 0
        self.line_height = 24
        self.addr_width = 0
        self.hex_start = 0
        self.ascii_start = 0
        self.total_width = 0
        
        # 初始化样式
        self.setFixedHeight(30)
        self.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        self.setMinimumWidth(800)
        self._setup_font()
    
    def _setup_font(self):
        """同步视口字体"""
        if self.viewport:
            self.font_mono = self.viewport.font_mono
        else:
            # 备用字体设置（与HexViewport一致）
            available_families = QFontDatabase.families()
            preferred_fonts = ["JetBrains Mono", "Consolas", "Courier New", "Monaco", "DejaVu Sans Mono", "Courier"]
            selected_font = None
            for font_name in preferred_fonts:
                for family in available_families:
                    if font_name.lower() in family.lower():
                        selected_font = family
                        break
                if selected_font:
                    break
            
            if selected_font:
                self.font_mono = QFont(selected_font, 11)
            else:
                self.font_mono = QFontDatabase.systemFont(QFontDatabase.SystemFont.FixedFont)
                self.font_mono.setPointSize(11)
        
        self.setFont(self.font_mono)
        fm = QFontMetrics(self.font_mono)
        self.char_width = fm.horizontalAdvance('0')
        self.line_height = fm.height() + 4
        
        # 同步布局参数
        if self.viewport:
            self.addr_width = self.viewport.addr_width
            self.hex_start = self.viewport.hex_start
            self.ascii_start = self.viewport.ascii_start
            self.total_width = self.viewport.total_width
        else:
            fm = QFontMetrics(self.font_mono)
            self.addr_width = fm.horizontalAdvance("00000000  ")
            self.hex_start = self.addr_width
            self.hex_width = fm.horizontalAdvance("00 " * 16)
            self.ascii_start = self.hex_start + self.hex_width + fm.horizontalAdvance("  ")
            self.total_width = self.ascii_start + fm.horizontalAdvance("................")
        
        self.setMinimumWidth(int(self.total_width) + 40)
    
    def paintEvent(self, event: QPaintEvent):
        """绘制表头"""
        painter = QPainter(self)
        painter.setRenderHint(QPainter.RenderHint.TextAntialiasing)
        painter.setFont(self.font_mono)
        
        # 绘制表头背景
        painter.fillRect(self.rect(), QColor(self.colors.HEADER_BG))
        
        # 1. 绘制「偏移」标签
        painter.setPen(QColor(self.colors.HEADER_TEXT))
        painter.drawText(10, self.height() // 2 + self.line_height // 4, "偏移")
        
        # 2. 绘制十六进制列号（00-0F）
        hex_x = self.addr_width + 10
        painter.setPen(QColor(self.colors.HEADER_TEXT))
        for i in range(16):
            x = hex_x + i * (self.char_width * 3)
            col_text = f"{i:02X}"
            painter.drawText(int(x), self.height() // 2 + self.line_height // 4, col_text)
        
        # 3. 绘制「ASCII」标签
        ascii_x = self.ascii_start + 10
        painter.setPen(QColor(self.colors.HEADER_TEXT))
        painter.drawText(int(ascii_x), self.height() // 2 + self.line_height // 4, "ASCII")
        
        # 绘制分隔线
        painter.setPen(QPen(QColor(self.colors.BG_TERTIARY), 1))
        painter.drawLine(0, self.height() - 1, self.width(), self.height() - 1)
        
        painter.end()
    
    def sync_with_viewport(self):
        """同步视口布局参数（用于视口布局变化后更新）"""
        if self.viewport:
            self.addr_width = self.viewport.addr_width
            self.hex_start = self.viewport.hex_start
            self.ascii_start = self.viewport.ascii_start
            self.total_width = self.viewport.total_width
            self.setMinimumWidth(int(self.total_width) + 40)
            self.update()


class HexViewport(QWidget):
    """十六进制视口组件"""
    selection_changed = pyqtSignal(int, int)
    offset_changed = pyqtSignal(int)
    
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)
        self.setMouseTracking(True)
        
        # 字体设置
        available_families = QFontDatabase.families()
        preferred_fonts = ["JetBrains Mono", "Consolas", "Courier New", "Monaco", "DejaVu Sans Mono", "Courier"]
        selected_font = None
        for font_name in preferred_fonts:
            for family in available_families:
                if font_name.lower() in family.lower():
                    selected_font = family
                    break
            if selected_font:
                break
        
        if selected_font:
            self.font_mono = QFont(selected_font, 11)
        else:
            self.font_mono = QFontDatabase.systemFont(QFontDatabase.SystemFont.FixedFont)
            self.font_mono.setPointSize(11)
        
        self.setFont(self.font_mono)
        
        # 计算字体 metrics
        fm = QFontMetrics(self.font_mono)
        self.char_width = fm.horizontalAdvance('0')
        self.line_height = fm.height() + 4
        
        # 状态
        self.state = ViewState()
        self.state.row_height = self.line_height
        self.mm: Optional[mmap.mmap] = None
        self.file_path: Optional[str] = None
        
        # 配色
        self.colors = ColorScheme()
        
        # 设置大小策略
        self.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        self.setMinimumWidth(800)
        
        # 预计算布局
        self._calculate_layout()
    
    def _calculate_layout(self):
        """计算布局参数"""
        fm = QFontMetrics(self.font_mono)
        self.addr_width = fm.horizontalAdvance("00000000  ")
        self.hex_start = self.addr_width
        self.hex_width = fm.horizontalAdvance("00 " * 16)
        self.ascii_start = self.hex_start + self.hex_width + fm.horizontalAdvance("  ")
        self.total_width = self.ascii_start + fm.horizontalAdvance("................")
        self.setMinimumWidth(int(self.total_width) + 40)
        self._update_height()
    
    def _update_height(self):
        """【核心】根据文件大小更新视口总高度"""
        if self.state.file_size == 0:
            self.setFixedHeight(600)
            return
        
        total_rows = (self.state.file_size + self.state.bytes_per_row - 1) // self.state.bytes_per_row
        total_height = total_rows * self.state.row_height + 20  # 上下边距
        self.setFixedHeight(total_height)
    
    def sizeHint(self) -> QSize:
        """返回推荐大小"""
        return QSize(int(self.total_width) + 40, self.height())
    
    def set_file(self, file_path: str):
        """设置文件并创建内存映射"""
        self.close_file()
        self.file_path = file_path
        self.state.file_size = os.path.getsize(file_path)
        
        f = open(file_path, 'rb')
        self.mm = mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ)
        
        self.state.offset = 0
        self.state.cursor_pos = 0
        self.state.selection_start = -1
        self.state.selection_end = -1
        
        self._update_height()  # 更新总高度
        self.update()
    
    def close_file(self):
        """关闭文件"""
        if self.mm:
            self.mm.close()
            self.mm = None
        self.file_path = None
        self.state.file_size = 0
        self._update_height()
    
    def paintEvent(self, event: QPaintEvent):
        """绘制事件 - 只绘制可见区域"""
        if not self.mm or self.state.file_size == 0:
            return
        
        painter = QPainter(self)
        painter.setRenderHint(QPainter.RenderHint.TextAntialiasing)
        painter.setFont(self.font_mono)
        
        # 背景
        painter.fillRect(self.rect(), QColor(self.colors.BG_PRIMARY))
        
        # 获取可见区域
        visible_rect = event.rect()
        start_y = visible_rect.top()
        end_y = visible_rect.bottom()
        
        # 计算可见行范围
        start_row = max(0, (start_y - 10) // self.state.row_height)
        end_row = min(
            (self.state.file_size + self.state.bytes_per_row - 1) // self.state.bytes_per_row,
            (end_y - 10) // self.state.row_height + 1
        )
        
        # 绘制行背景（先画所有背景）
        for row in range(start_row, end_row):
            y = row * self.state.row_height + 10
            if row % 2 == 0:
                bg_rect = QRect(0, y - 2, self.width(), self.state.row_height)
                painter.fillRect(bg_rect, QColor(self.colors.BG_SECONDARY))
        
        # 绘制选中区域（在文字下面）
        if self.state.selection_start != -1 and self.state.selection_end != -1:
            self._draw_selection(painter, start_row, end_row)
        
        # 绘制文字内容
        for row in range(start_row, end_row):
            y = row * self.state.row_height + 10
            file_offset = row * self.state.bytes_per_row
            self._draw_row_text(painter, row, file_offset, y)
        
        # 绘制光标（在最上层）
        self._draw_cursor(painter, start_row, end_row)
        
        painter.end()
        
        # 更新当前偏移（基于滚动位置）
        new_offset = start_row * self.state.bytes_per_row
        if new_offset != self.state.offset:
            self.state.offset = new_offset
            self.offset_changed.emit(new_offset)
    
    def _draw_row_text(self, painter: QPainter, row: int, offset: int, y: int):
        """绘制单行文字内容"""
        # 1. 绘制地址
        addr_text = f"{offset:08X}"
        painter.setPen(QColor(self.colors.ADDRESS_COLOR))
        painter.drawText(10, y + self.line_height - 8, addr_text)
        
        # 2. 读取该行数据
        row_end = min(offset + self.state.bytes_per_row, self.state.file_size)
        row_bytes = self.mm[offset:row_end]
        
        # 3. 绘制十六进制
        hex_x = self.addr_width + 10
        for i, byte in enumerate(row_bytes):
            x = hex_x + i * (self.char_width * 3)
            hex_str = f"{byte:02X}"
            
            if byte == 0:
                painter.setPen(QColor(self.colors.ZERO_BYTE))
            else:
                painter.setPen(QColor(self.colors.HEX_COLOR))
            
            painter.drawText(int(x), y + self.line_height - 8, hex_str)
        
        # 4. 绘制 ASCII
        ascii_x = self.ascii_start + 10
        ascii_str = ""
        for byte in row_bytes:
            if 32 <= byte <= 126:
                ascii_str += chr(byte)
            else:
                ascii_str += "·"
        
        painter.setPen(QColor(self.colors.ASCII_COLOR))
        painter.drawText(int(ascii_x), y + self.line_height - 8, ascii_str)
    
    def _draw_selection(self, painter: QPainter, start_row: int, end_row: int):
        """绘制选中区域背景"""
        sel_start = min(self.state.selection_start, self.state.selection_end)
        sel_end = max(self.state.selection_start, self.state.selection_end)
        
        painter.setPen(Qt.PenStyle.NoPen)
        painter.setBrush(QColor(self.colors.SELECTION_BG))
        
        for pos in range(sel_start, sel_end + 1):
            row = pos // self.state.bytes_per_row
            if start_row <= row <= end_row:
                self._highlight_byte(painter, pos)
    
    def _highlight_byte(self, painter: QPainter, pos: int):
        """高亮单个字节"""
        row = pos // self.state.bytes_per_row
        col = pos % self.state.bytes_per_row
        
        y = row * self.state.row_height + 10 - 2
        hex_x = self.addr_width + 10 + col * (self.char_width * 3)
        
        rect = QRect(int(hex_x) - 2, y, int(self.char_width * 2) + 4, self.state.row_height - 4)
        painter.drawRoundedRect(rect, 3, 3)
    
    def _draw_cursor(self, painter: QPainter, start_row: int, end_row: int):
        """绘制光标"""
        cursor_row = self.state.cursor_pos // self.state.bytes_per_row
        if cursor_row < start_row or cursor_row > end_row:
            return
        
        cursor_col = self.state.cursor_pos % self.state.bytes_per_row
        y = cursor_row * self.state.row_height + 10 - 2
        hex_x = self.addr_width + 10 + cursor_col * (self.char_width * 3)
        
        painter.setPen(QPen(QColor(self.colors.ACCENT_PRIMARY), 2))
        painter.setBrush(Qt.BrushStyle.NoBrush)
        rect = QRect(int(hex_x) - 2, y, int(self.char_width * 2) + 4, self.state.row_height - 4)
        painter.drawRoundedRect(rect, 3, 3)
    
    def mousePressEvent(self, event: QMouseEvent):
        """鼠标点击 - 设置光标位置"""
        if not self.mm:
            return
        
        pos = self._hit_test(event.position().x(), event.position().y())
        if pos is not None and pos < self.state.file_size:
            self.state.cursor_pos = pos
            self.state.selection_start = pos
            self.state.selection_end = pos
            self.setFocus()
            self.update()
    
    def mouseMoveEvent(self, event: QMouseEvent):
        """鼠标拖动 - 扩展选中区域"""
        if not self.mm or event.buttons() != Qt.MouseButton.LeftButton:
            return
        
        pos = self._hit_test(event.position().x(), event.position().y())
        if pos is not None and pos < self.state.file_size:
            self.state.selection_end = pos
            self.selection_changed.emit(self.state.selection_start, self.state.selection_end)
            self.update()
    
    def _hit_test(self, x: float, y: float) -> Optional[int]:
        """命中测试 - 将坐标转换为文件偏移"""
        if x < self.addr_width + 10 or x > self.ascii_start:
            return None
        
        row = int((y - 10) // self.state.row_height)
        hex_x = x - (self.addr_width + 10)
        col = int(hex_x / (self.char_width * 3))
        col = max(0, min(col, self.state.bytes_per_row - 1))
        
        byte_pos = row * self.state.bytes_per_row + col
        if byte_pos >= self.state.file_size:
            return None
        
        return byte_pos
    
    def keyPressEvent(self, event: QKeyEvent):
        """键盘导航"""
        if not self.mm:
            return
        
        key = event.key()
        modifiers = event.modifiers()
        
        if key == Qt.Key.Key_Right:
            self._move_cursor(1, modifiers)
        elif key == Qt.Key.Key_Left:
            self._move_cursor(-1, modifiers)
        elif key == Qt.Key.Key_Down:
            self._move_cursor(self.state.bytes_per_row, modifiers)
        elif key == Qt.Key.Key_Up:
            self._move_cursor(-self.state.bytes_per_row, modifiers)
        elif key == Qt.Key.Key_PageDown:
            visible_rows = self.parent().parent().height() // self.state.row_height
            self._move_cursor(visible_rows * self.state.bytes_per_row, modifiers)
        elif key == Qt.Key.Key_PageUp:
            visible_rows = self.parent().parent().height() // self.state.row_height
            self._move_cursor(-visible_rows * self.state.bytes_per_row, modifiers)
        elif key == Qt.Key.Key_Home:
            if modifiers & Qt.KeyboardModifier.ControlModifier:
                self._move_cursor(-self.state.cursor_pos, modifiers)
            else:
                self._move_cursor(-(self.state.cursor_pos % self.state.bytes_per_row), modifiers)
        elif key == Qt.Key.Key_End:
            if modifiers & Qt.KeyboardModifier.ControlModifier:
                self._move_cursor(self.state.file_size - self.state.cursor_pos - 1, modifiers)
            else:
                self._move_cursor(
                    self.state.bytes_per_row - (self.state.cursor_pos % self.state.bytes_per_row) - 1, 
                    modifiers
                )
        else:
            super().keyPressEvent(event)
            return
        
        self._ensure_cursor_visible()
        self.update()
    
    def _move_cursor(self, delta: int, modifiers):
        """移动光标"""
        new_pos = max(0, min(self.state.file_size - 1, self.state.cursor_pos + delta))
        
        if modifiers & Qt.KeyboardModifier.ShiftModifier:
            self.state.selection_end = new_pos
            self.selection_changed.emit(self.state.selection_start, self.state.selection_end)
        else:
            self.state.selection_start = new_pos
            self.state.selection_end = new_pos
        
        self.state.cursor_pos = new_pos
    
    def _ensure_cursor_visible(self):
        """确保光标在可视区域内（滚动到可见位置）"""
        cursor_row = self.state.cursor_pos // self.state.bytes_per_row
        cursor_y = cursor_row * self.state.row_height
        
        # 获取滚动区域
        scroll_area = self.parent().parent()
        if isinstance(scroll_area, QScrollArea):
            viewport_rect = scroll_area.viewport().rect()
            scroll_pos = scroll_area.verticalScrollBar().value()
            
            # 检查光标是否在可见区域内
            if cursor_y < scroll_pos:
                # 光标在上方，滚动到光标位置
                scroll_area.verticalScrollBar().setValue(cursor_y)
            elif cursor_y + self.state.row_height > scroll_pos + viewport_rect.height():
                # 光标在下方，滚动到光标位置
                scroll_area.verticalScrollBar().setValue(cursor_y - viewport_rect.height() + self.state.row_height)
    
    def goto_offset(self, offset: int):
        """跳转到指定偏移"""
        if not self.mm:
            return
        
        offset = max(0, min(self.state.file_size - 1, offset))
        self.state.cursor_pos = offset
        self.state.selection_start = offset
        self.state.selection_end = offset
        
        # 滚动到指定位置
        target_row = offset // self.state.bytes_per_row
        target_y = target_row * self.state.row_height
        
        scroll_area = self.parent().parent()
        if isinstance(scroll_area, QScrollArea):
            scroll_area.verticalScrollBar().setValue(target_y)
        
        self.update()
    
    def get_selection_info(self) -> Tuple[int, int, bytes]:
        """获取选中区域信息"""
        if self.state.selection_start == -1:
            return 0, 0, b""
        
        start = min(self.state.selection_start, self.state.selection_end)
        end = max(self.state.selection_start, self.state.selection_end)
        length = end - start + 1
        
        if self.mm:
            data = self.mm[start:end+1]
        else:
            data = b""
        
        return start, length, data


class ModernHexViewer(QMainWindow):
    """主窗口"""
    
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Hex 文件查看器")
        self.setMinimumSize(1200, 800)
        
        self.colors = ColorScheme()
        self._setup_theme()
        
        self.current_file: Optional[str] = None
        
        central = QWidget()
        self.setCentralWidget(central)
        layout = QVBoxLayout(central)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        
        toolbar = self._create_toolbar()
        layout.addWidget(toolbar)
        
        content = QWidget()
        content_layout = QHBoxLayout(content)
        content_layout.setContentsMargins(10, 10, 10, 10)
        content_layout.setSpacing(10)
        
        # 【核心】滚动区域容器（包含表头+数据区）
        scroll_container = QWidget()
        scroll_container_layout = QVBoxLayout(scroll_container)
        scroll_container_layout.setContentsMargins(0, 0, 0, 0)
        scroll_container_layout.setSpacing(0)
        
        # 【新增】添加固定表头
        self.viewport = HexViewport()
        self.header = HexHeader(viewport=self.viewport)
        scroll_container_layout.addWidget(self.header)
        
        # 【核心】使用标准 QScrollArea，启用原生滚动条
        self.scroll_area = QScrollArea()
        self.scroll_area.setWidgetResizable(False)  # 视口自己控制大小
        self.scroll_area.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAsNeeded)
        self.scroll_area.setVerticalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAsNeeded)
        self.scroll_area.setFrameShape(QFrame.Shape.NoFrame)
        self.scroll_area.setStyleSheet(f"""
            QScrollArea {{
                background-color: {self.colors.BG_PRIMARY};
                border: none;
            }}
            QScrollBar:vertical {{
                background-color: {self.colors.BG_TERTIARY};
                width: 12px;
                border-radius: 6px;
                margin: 0px;
            }}
            QScrollBar::handle:vertical {{
                background-color: {self.colors.ACCENT_PRIMARY};
                min-height: 30px;
                border-radius: 6px;
                margin: 2px;
            }}
            QScrollBar::handle:vertical:hover {{
                background-color: {self.colors.ACCENT_SECONDARY};
            }}
            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {{
                height: 0px;
            }}
            QScrollBar:horizontal {{
                background-color: {self.colors.BG_TERTIARY};
                height: 12px;
                border-radius: 6px;
                margin: 0px;
            }}
            QScrollBar::handle:horizontal {{
                background-color: {self.colors.ACCENT_PRIMARY};
                min-width: 30px;
                border-radius: 6px;
                margin: 2px;
            }}
            QScrollBar::handle:horizontal:hover {{
                background-color: {self.colors.ACCENT_SECONDARY};
            }}
            QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {{
                width: 0px;
            }}
        """)
        
        self.viewport.offset_changed.connect(self._on_offset_changed)
        self.viewport.selection_changed.connect(self._on_selection_changed)
        self.scroll_area.setWidget(self.viewport)
        scroll_container_layout.addWidget(self.scroll_area)
        
        # 将滚动容器添加到主内容布局
        content_layout.addWidget(scroll_container)
        
        info_panel = self._create_info_panel()
        content_layout.addWidget(info_panel)
        
        layout.addWidget(content)
        
        self.status_bar = QStatusBar()
        self.status_bar.setStyleSheet(f"""
            QStatusBar {{
                background-color: {self.colors.BG_SECONDARY};
                color: {self.colors.TEXT_SECONDARY};
                padding: 5px;
                border-top: 1px solid {self.colors.BG_TERTIARY};
            }}
        """)
        self.setStatusBar(self.status_bar)
        self._update_status_bar()
        
        # 同步表头布局（初始化后）
        self.header.sync_with_viewport()
    
    def _setup_theme(self):
        """设置应用主题"""
        self.setStyleSheet(f"""
            QMainWindow {{
                background-color: {self.colors.BG_PRIMARY};
            }}
            QWidget {{
                background-color: {self.colors.BG_PRIMARY};
                color: {self.colors.TEXT_PRIMARY};
                font-family: \"Segoe UI\", \"Microsoft YaHei\", sans-serif;
            }}
            QLineEdit {{
                background-color: {self.colors.BG_SECONDARY};
                border: 1px solid {self.colors.BG_TERTIARY};
                border-radius: 6px;
                padding: 8px 12px;
                color: {self.colors.TEXT_PRIMARY};
                selection-background-color: {self.colors.ACCENT_PRIMARY};
            }}
            QLineEdit:focus {{
                border: 1px solid {self.colors.ACCENT_PRIMARY};
            }}
            QPushButton {{
                background-color: {self.colors.BG_SECONDARY};
                border: 1px solid {self.colors.BG_TERTIARY};
                border-radius: 6px;
                padding: 8px 16px;
                color: {self.colors.TEXT_PRIMARY};
                font-weight: 500;
            }}
            QPushButton:hover {{
                background-color: {self.colors.BG_TERTIARY};
                border: 1px solid {self.colors.ACCENT_PRIMARY};
            }}
            QPushButton:pressed {{
                background-color: {self.colors.ACCENT_PRIMARY};
                color: {self.colors.BG_PRIMARY};
            }}
            QLabel {{
                color: {self.colors.TEXT_SECONDARY};
            }}
        """)
    
    def _create_toolbar(self) -> QWidget:
        """创建顶部工具栏"""
        toolbar = QFrame()
        toolbar.setStyleSheet(f"""
            QFrame {{
                background-color: {self.colors.BG_SECONDARY};
                border-bottom: 1px solid {self.colors.BG_TERTIARY};
            }}
        """)
        toolbar.setFixedHeight(60)
        
        layout = QHBoxLayout(toolbar)
        layout.setContentsMargins(20, 10, 20, 10)
        layout.setSpacing(15)
        
        title = QLabel("Modern Hex Viewer")
        title.setStyleSheet(f"""
            color: {self.colors.ACCENT_PRIMARY};
            font-size: 18px;
            font-weight: bold;
        """)
        layout.addWidget(title)
        
        layout.addSpacing(30)
        
        btn_open = QPushButton("📂 打开文件")
        btn_open.setCursor(Qt.CursorShape.PointingHandCursor)
        btn_open.clicked.connect(self._open_file)
        layout.addWidget(btn_open)
        
        layout.addSpacing(20)
        
        lbl_goto = QLabel("跳转:")
        layout.addWidget(lbl_goto)
        
        self.input_goto = QLineEdit()
        self.input_goto.setFixedWidth(120)
        self.input_goto.setPlaceholderText("0x...")
        self.input_goto.returnPressed.connect(self._goto_offset)
        layout.addWidget(self.input_goto)
        
        btn_goto = QPushButton("→")
        btn_goto.setFixedWidth(40)
        btn_goto.setCursor(Qt.CursorShape.PointingHandCursor)
        btn_goto.clicked.connect(self._goto_offset)
        layout.addWidget(btn_goto)
        
        layout.addSpacing(20)
        
        self.lbl_offset = QLabel("Offset: 0x00000000")
        self.lbl_offset.setStyleSheet(f"color: {self.colors.ACCENT_SECONDARY}; font-family: monospace;")
        layout.addWidget(self.lbl_offset)
        
        layout.addStretch()
        
        return toolbar
    
    def _create_info_panel(self) -> QWidget:
        """创建右侧信息面板"""
        panel = QFrame()
        panel.setFixedWidth(250)
        panel.setStyleSheet(f"""
            QFrame {{
                background-color: {self.colors.BG_SECONDARY};
                border-radius: 8px;
                border: 1px solid {self.colors.BG_TERTIARY};
            }}
        """)
        
        shadow = QGraphicsDropShadowEffect()
        shadow.setBlurRadius(20)
        shadow.setColor(QColor(0, 0, 0, 80))
        shadow.setOffset(0, 4)
        panel.setGraphicsEffect(shadow)
        
        layout = QVBoxLayout(panel)
        layout.setContentsMargins(15, 15, 15, 15)
        layout.setSpacing(15)
        
        lbl_file = QLabel("📄 文件信息")
        lbl_file.setStyleSheet(f"color: {self.colors.ACCENT_PRIMARY}; font-weight: bold; font-size: 14px;")
        layout.addWidget(lbl_file)
        
        self.lbl_filename = QLabel("未打开文件")
        self.lbl_filename.setWordWrap(True)
        self.lbl_filename.setStyleSheet(f"color: {self.colors.TEXT_SECONDARY}; font-size: 12px;")
        layout.addWidget(self.lbl_filename)
        
        self.lbl_filesize = QLabel("Size: -")
        self.lbl_filesize.setStyleSheet("font-family: monospace;")
        layout.addWidget(self.lbl_filesize)
        
        layout.addSpacing(10)
        separator = QFrame()
        separator.setStyleSheet(f"background-color: {self.colors.BG_TERTIARY};")
        separator.setFixedHeight(1)
        layout.addWidget(separator)
        
        lbl_sel = QLabel("🎯 选中区域")
        lbl_sel.setStyleSheet(f"color: {self.colors.ACCENT_PRIMARY}; font-weight: bold; font-size: 14px;")
        layout.addWidget(lbl_sel)
        
        self.lbl_sel_range = QLabel("Range: -")
        self.lbl_sel_range.setStyleSheet("font-family: monospace;")
        layout.addWidget(self.lbl_sel_range)
        
        self.lbl_sel_size = QLabel("Size: -")
        self.lbl_sel_size.setStyleSheet("font-family: monospace;")
        layout.addWidget(self.lbl_sel_size)
        
        layout.addSpacing(10)
        separator2 = QFrame()
        separator2.setStyleSheet(f"background-color: {self.colors.BG_TERTIARY};")
        separator2.setFixedHeight(1)
        layout.addWidget(separator2)
        
        lbl_data = QLabel("📊 数据解释")
        lbl_data.setStyleSheet(f"color: {self.colors.ACCENT_PRIMARY}; font-weight: bold; font-size: 14px;")
        layout.addWidget(lbl_data)
        
        self.lbl_hex = QLabel("Hex: -")
        self.lbl_hex.setStyleSheet("font-family: monospace;")
        layout.addWidget(self.lbl_hex)
        
        self.lbl_dec = QLabel("Dec: -")
        self.lbl_dec.setStyleSheet("font-family: monospace;")
        layout.addWidget(self.lbl_dec)
        
        self.lbl_ascii = QLabel("ASCII: -")
        self.lbl_ascii.setStyleSheet("font-family: monospace;")
        layout.addWidget(self.lbl_ascii)
        
        layout.addStretch()
        
        return panel
    
    def _open_file(self):
        """打开文件对话框"""
        file_path, _ = QFileDialog.getOpenFileName(
            self, "选择文件", "", 
            "所有文件 (*);;可执行文件 (*.exe *.dll *.so);;文本文件 (*.txt *.md)"
        )
        
        if file_path:
            self._load_file(file_path)
    
    def _load_file(self, file_path: str):
        """加载文件"""
        self.current_file = file_path
        self.viewport.set_file(file_path)
        
        filename = os.path.basename(file_path)
        size = os.path.getsize(file_path)
        size_str = self._format_size(size)
        
        self.lbl_filename.setText(filename)
        self.lbl_filesize.setText(f"Size: {size_str} ({size:,} bytes)")
        
        # 同步表头布局（文件加载后可能触发视口布局变化）
        self.header.sync_with_viewport()
        
        self._update_status_bar()
    
    def _format_size(self, size: int) -> str:
        """格式化文件大小"""
        for unit in ['B', 'KB', 'MB', 'GB', 'TB']:
            if size < 1024.0:
                return f"{size:.2f} {unit}"
            size /= 1024.0
        return f"{size:.2f} PB"
    
    def _goto_offset(self):
        """跳转到指定偏移"""
        text = self.input_goto.text().strip()
        if not text:
            return
        
        try:
            if text.startswith("0x") or text.startswith("0X"):
                offset = int(text, 16)
            else:
                offset = int(text)
            self.viewport.goto_offset(offset)
        except ValueError:
            self.status_bar.showMessage("无效的偏移地址", 3000)
    
    def _on_offset_changed(self, offset: int):
        """偏移变化回调"""
        self.lbl_offset.setText(f"Offset: 0x{offset:08X}")
        self._update_status_bar()
    
    def _on_selection_changed(self, start: int, end: int):
        """选中变化回调"""
        actual_start = min(start, end)
        actual_end = max(start, end)
        length = actual_end - actual_start + 1
        
        self.lbl_sel_range.setText(f"Range: 0x{actual_start:08X} - 0x{actual_end:08X}")
        self.lbl_sel_size.setText(f"Size: {length:,} bytes")
        
        _, _, data = self.viewport.get_selection_info()
        if data:
            if len(data) == 1:
                byte = data[0]
                self.lbl_hex.setText(f"Hex: 0x{byte:02X}")
                self.lbl_dec.setText(f"Dec: {byte} / Int8: {byte if byte < 128 else byte - 256}")
                self.lbl_ascii.setText(f"ASCII: {chr(byte) if 32 <= byte <= 126 else '·'}")
            elif len(data) <= 8:
                if len(data) <= 4:
                    val = int.from_bytes(data, 'little', signed=False)
                    self.lbl_dec.setText(f"LE: {val} / BE: {int.from_bytes(data, 'big', signed=False)}")
                else:
                    self.lbl_dec.setText(f"LE: {int.from_bytes(data, 'little', signed=False):,}")
                self.lbl_hex.setText(f"Hex: {data.hex()}")
                self.lbl_ascii.setText(f"ASCII: {data[:4].decode('ascii', errors='replace')}")
        
        self._update_status_bar()
    
    def _update_status_bar(self):
        """更新状态栏"""
        if not self.current_file:
            self.status_bar.showMessage("就绪 - 请打开文件")
            return
        
        state = self.viewport.state
        percentage = (state.offset / state.file_size * 100) if state.file_size > 0 else 0
        self.status_bar.showMessage(
            f"文件: {os.path.basename(self.current_file)} | "
            f"位置: 0x{state.offset:08X} ({percentage:.1f}%) | "
            f"光标: 0x{state.cursor_pos:08X}"
        )
    
    def closeEvent(self, event):
        """关闭事件"""
        self.viewport.close_file()
        super().closeEvent(event)


def main():
    """主入口"""
    app = QApplication(sys.argv)
    app.setApplicationName("Modern Hex Viewer (标准滚动条版)")
    app.setStyle("Fusion")
    
    window = ModernHexViewer()
    window.show()
    
    if len(sys.argv) > 1 and os.path.exists(sys.argv[1]):
        window._load_file(sys.argv[1])
    
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
