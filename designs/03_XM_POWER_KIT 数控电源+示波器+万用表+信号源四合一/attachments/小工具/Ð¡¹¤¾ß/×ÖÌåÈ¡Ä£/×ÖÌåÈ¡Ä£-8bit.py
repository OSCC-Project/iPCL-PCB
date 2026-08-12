import tkinter as tk
from tkinter import filedialog, messagebox, ttk
from PIL import Image, ImageFont, ImageDraw
import os

class FontToBinConverter:
    def __init__(self, root):
        self.root = root
        self.root.title("字体取模工具 (8位灰度模式)")
        self.root.geometry("1200x850")
        
        self.font_path = None
        self.font_name = "选择字体"
        self.width_var = tk.IntVar(value=16)
        self.height_var = tk.IntVar(value=16)
        self.font_size_var = tk.IntVar(value=16)
        self.font_name_var = tk.StringVar(value="font")
        
        # 偏移相关
        self.global_offset_x = 0
        self.global_offset_y = 0
        self.char_offsets = {}      # char -> (dx, dy)
        self.selected_char = None
        
        self.bitmap_text = None
        self.fontinfo_text = None
        
        self.create_widgets()
    
    def create_widgets(self):
        # 第一行：字体 + 尺寸
        top_frame = ttk.Frame(self.root)
        top_frame.pack(pady=10, padx=10, fill=tk.X)
        
        self.font_btn = ttk.Button(top_frame, text=self.font_name, command=self.select_font)
        self.font_btn.pack(side=tk.LEFT, padx=5)
        
        ttk.Label(top_frame, text="宽度:").pack(side=tk.LEFT, padx=5)
        ttk.Entry(top_frame, textvariable=self.width_var, width=6).pack(side=tk.LEFT)
        
        ttk.Label(top_frame, text="高度:").pack(side=tk.LEFT, padx=5)
        ttk.Entry(top_frame, textvariable=self.height_var, width=6).pack(side=tk.LEFT)
        
        ttk.Label(top_frame, text="字号:").pack(side=tk.LEFT, padx=5)
        ttk.Entry(top_frame, textvariable=self.font_size_var, width=6).pack(side=tk.LEFT)
        
        # 第二行：字符输入 + 全局偏移
        second_frame = ttk.Frame(self.root)
        second_frame.pack(pady=5, padx=10, fill=tk.X)
        
        ttk.Label(second_frame, text="取模字符:").pack(side=tk.LEFT, padx=5)
        self.chars_text = tk.Text(second_frame, height=4, width=50)
        self.chars_text.pack(side=tk.LEFT, padx=5, fill=tk.X, expand=True)
        
        # 全局偏移按钮
        dir_frame = ttk.Frame(second_frame)
        dir_frame.pack(side=tk.LEFT, padx=10)
        ttk.Button(dir_frame, text="上", command=lambda: self.move_global_offset(0, -1)).pack()
        ttk.Button(dir_frame, text="下", command=lambda: self.move_global_offset(0, 1)).pack()
        ttk.Button(dir_frame, text="左", command=lambda: self.move_global_offset(-1, 0)).pack(side=tk.LEFT)
        ttk.Button(dir_frame, text="右", command=lambda: self.move_global_offset(1, 0)).pack(side=tk.LEFT)
        
        # 第三行：单个字符调整
        third_frame = ttk.Frame(self.root)
        third_frame.pack(pady=5, padx=10, fill=tk.X)
        
        ttk.Label(third_frame, text="单个字符调整:").pack(side=tk.LEFT, padx=5)
        self.char_combo = ttk.Combobox(third_frame, width=10, state="readonly")
        self.char_combo.pack(side=tk.LEFT, padx=5)
        self.char_combo.bind("<<ComboboxSelected>>", self.on_char_selected)
        
        self.ind_offset_label = ttk.Label(third_frame, text="偏移: (0, 0)")
        self.ind_offset_label.pack(side=tk.LEFT, padx=10)
        
        # 单个字符方向按钮
        ind_dir_frame = ttk.Frame(third_frame)
        ind_dir_frame.pack(side=tk.LEFT, padx=10)
        
        ttk.Button(ind_dir_frame, text="左", command=lambda: self.move_individual(-1, 0)).grid(row=1, column=0, padx=3, pady=2)
        
        up_down_frame = ttk.Frame(ind_dir_frame)
        up_down_frame.grid(row=0, column=1, rowspan=3, padx=3, pady=2)
        ttk.Button(up_down_frame, text="上", command=lambda: self.move_individual(0, -1)).pack(pady=1)
        ttk.Button(up_down_frame, text="下", command=lambda: self.move_individual(0, 1)).pack(pady=1)
        
        ttk.Button(ind_dir_frame, text="右", command=lambda: self.move_individual(1, 0)).grid(row=1, column=2, padx=3, pady=2)
        
        ttk.Button(third_frame, text="重置选中", command=self.reset_selected_offset).pack(side=tk.LEFT, padx=8)
        
        # 分割线
        ttk.Separator(self.root, orient=tk.HORIZONTAL).pack(fill=tk.X, pady=10)
        
        ttk.Label(self.root, text="预览点阵 (灰度显示):").pack(anchor=tk.W, padx=10)
        
        self.preview_frame = ttk.Frame(self.root)
        self.preview_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)
        
        self.preview_scroll_y = tk.Scrollbar(self.preview_frame, orient=tk.VERTICAL)
        self.preview_scroll_y.pack(side=tk.RIGHT, fill=tk.Y)
        self.preview_scroll_x = tk.Scrollbar(self.preview_frame, orient=tk.HORIZONTAL)
        self.preview_scroll_x.pack(side=tk.BOTTOM, fill=tk.X)
        
        self.preview_canvas = tk.Canvas(self.preview_frame, height=400,
                                        xscrollcommand=self.preview_scroll_x.set,
                                        yscrollcommand=self.preview_scroll_y.set)
        self.preview_canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
        self.preview_scroll_x.config(command=self.preview_canvas.xview)
        self.preview_scroll_y.config(command=self.preview_canvas.yview)
        
        ttk.Button(self.root, text="刷新预览", command=self.update_preview).pack(pady=5)
        
        ttk.Separator(self.root, orient=tk.HORIZONTAL).pack(fill=tk.X, pady=10)
        
        # 生成区域
        gen_frame = ttk.Frame(self.root)
        gen_frame.pack(pady=10, fill=tk.X)
        ttk.Label(gen_frame, text="字体命名（用于数组/结构体名）:").pack(side=tk.LEFT, padx=5)
        ttk.Entry(gen_frame, textvariable=self.font_name_var, width=15).pack(side=tk.LEFT, padx=5)
        ttk.Button(gen_frame, text="生成 .bin + bitmap + FontInfo", command=self.generate).pack(side=tk.LEFT, padx=20)
        
        # 结果区域
        result_frame = ttk.Frame(self.root)
        result_frame.pack(fill=tk.BOTH, expand=True, pady=10, padx=10)
        
        # 左侧：合并输出
        left_frame = ttk.Frame(result_frame)
        left_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=(0, 15))
        
        header_left = ttk.Frame(left_frame)
        header_left.pack(fill=tk.X)
        ttk.Label(header_left, text="bitmap数组 + FontInfo结构体（可复制）:").pack(side=tk.LEFT)
        ttk.Button(header_left, text="复制 合并输出", command=lambda: self.copy_text(self.bitmap_text)).pack(side=tk.RIGHT, padx=5)
        
        self.bitmap_text = tk.Text(left_frame, height=12, font=("Consolas", 10))
        self.bitmap_text.pack(fill=tk.BOTH, expand=True)
        
        # 右侧：extern声明
        right_frame = ttk.Frame(result_frame)
        right_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
        header_right = ttk.Frame(right_frame)
        header_right.pack(fill=tk.X)
        ttk.Label(header_right, text="FontInfo extern声明（可复制）:").pack(side=tk.LEFT)
        ttk.Button(header_right, text="复制 extern", command=lambda: self.copy_text(self.fontinfo_text)).pack(side=tk.RIGHT, padx=5)
        
        self.fontinfo_text = tk.Text(right_frame, height=12, font=("Consolas", 10))
        self.fontinfo_text.pack(fill=tk.BOTH, expand=True)
    
    def on_char_selected(self, event):
        self.selected_char = self.char_combo.get()
        self.update_ind_label()
    
    def update_ind_label(self):
        if self.selected_char and self.selected_char in self.char_offsets:
            dx, dy = self.char_offsets[self.selected_char]
            self.ind_offset_label.config(text=f"偏移: ({dx}, {dy})")
        else:
            self.ind_offset_label.config(text="偏移: (0, 0)")
    
    def move_global_offset(self, dx, dy):
        self.global_offset_x += dx
        self.global_offset_y += dy
        self.update_preview()
    
    def move_individual(self, dx, dy):
        if not self.selected_char:
            messagebox.showwarning("提示", "请先从下拉列表选择一个字符")
            return
        ox, oy = self.char_offsets.get(self.selected_char, (0, 0))
        self.char_offsets[self.selected_char] = (ox + dx, oy + dy)
        self.update_ind_label()
        self.update_preview()
    
    def reset_selected_offset(self):
        if not self.selected_char:
            return
        if self.selected_char in self.char_offsets:
            self.char_offsets[self.selected_char] = (0, 0)
        self.update_ind_label()
        self.update_preview()
    
    def select_font(self):
        self.font_path = filedialog.askopenfilename(filetypes=[("字体文件", "*.ttf *.otf")])
        if self.font_path:
            self.font_name = os.path.splitext(os.path.basename(self.font_path))[0]
            self.font_btn.config(text=self.font_name)
            font_base_name = self.font_name.replace("-", "_")
            self.font_name_var.set(font_base_name)
    
    def update_preview(self):
        if not self.font_path:
            return
        
        self.preview_canvas.delete("all")
        
        chars_input = self.chars_text.get("1.0", tk.END).strip()
        chars = [c for c in chars_input if c.strip()]
        
        for c in chars:
            if c not in self.char_offsets:
                self.char_offsets[c] = (0, 0)
        
        unique_chars = []
        seen = set()
        for c in chars:
            if c not in seen:
                unique_chars.append(c)
                seen.add(c)
        self.char_combo['values'] = unique_chars
        
        if unique_chars:
            if self.selected_char not in unique_chars:
                self.selected_char = unique_chars[0]
                self.char_combo.set(self.selected_char)
            self.update_ind_label()
        
        width = self.width_var.get()
        height = self.height_var.get()
        size = self.font_size_var.get()
        
        try:
            font = ImageFont.truetype(self.font_path, size)
            
            max_visible_height = 400
            grid_size = max(1, max_visible_height // height) if height > 50 else 8
            char_spacing = 10
            
            total_w = (width * grid_size + char_spacing) * len(chars)
            total_h = height * grid_size + 20
            
            self.preview_canvas.config(scrollregion=(0, 0, total_w, total_h))
            
            cur_x = 0
            for char in chars:
                dx, dy = self.char_offsets.get(char, (0, 0))
                total_x = self.global_offset_x + dx
                total_y = self.global_offset_y + dy
                
                img = Image.new('L', (width, height), 0)
                draw = ImageDraw.Draw(img)
                draw.text((total_x, total_y), char, font=font, fill=255)
                
                for y in range(height):
                    for x in range(width):
                        px = img.getpixel((x, y))
                        gray_hex = f'#{px:02x}{px:02x}{px:02x}'
                        self.preview_canvas.create_rectangle(
                            cur_x + x * grid_size, y * grid_size,
                            cur_x + (x+1) * grid_size, (y+1) * grid_size,
                            fill=gray_hex, outline='gray'
                        )
                cur_x += width * grid_size + char_spacing
        except Exception as e:
            messagebox.showerror("预览失败", str(e))
    
    def copy_text(self, text_widget):
        content = text_widget.get("1.0", tk.END).strip()
        if content:
            self.root.clipboard_clear()
            self.root.clipboard_append(content)
            messagebox.showinfo("复制成功", "已复制到剪贴板")
    
    def generate(self):
        if not self.font_path:
            return
        
        chars_input = self.chars_text.get("1.0", tk.END).strip()
        if not chars_input:
            messagebox.showerror("错误", "请输入字符")
            return
        
        chars = []
        seen = set()
        for c in chars_input:
            if c not in seen and c.strip():
                chars.append(c)
                seen.add(c)
        
        char_count = len(chars)
        width = self.width_var.get()
        height = self.height_var.get()
        size = self.font_size_var.get()
        custom_name = self.font_name_var.get().strip() or "font"
        font_label = f"{custom_name}{width}x{height}"
        
        for c in chars:
            if c not in self.char_offsets:
                self.char_offsets[c] = (0, 0)
        
        try:
            font = ImageFont.truetype(self.font_path, size)
            bytes_per_char = width * height
            
            bin_data = bytearray()
            font_map_lines = [f"const uint8_t font_map_{font_label}[][4] = {{"]
            
            for char in chars:
                dx, dy = self.char_offsets.get(char, (0, 0))
                total_x = self.global_offset_x + dx
                total_y = self.global_offset_y + dy
                
                img = Image.new('L', (width, height), 0)
                draw = ImageDraw.Draw(img)
                draw.text((total_x, total_y), char, font=font, fill=255)
                
                char_data = bytearray(bytes_per_char)
                data_index = 0
                for row in range(height):
                    for col in range(width):
                        px = img.getpixel((col, row))
                        char_data[data_index] = px
                        data_index += 1
                
                bin_data.extend(char_data)
                
                utf8 = char.encode('utf-8')
                utf8_padded = list(utf8) + [0] * (4 - len(utf8))
                hex_bytes = ", ".join(f"0x{b:02X}" for b in utf8_padded)
                comment_char = char.replace('\\', '\\\\').replace('"', '\\"')
                font_map_lines.append(f"    {{ {hex_bytes} }},  /* {comment_char} */")
            
            font_map_lines[-1] = font_map_lines[-1].rstrip(',')
            font_map_lines.append("};")
            
            # ==================== 先保存 .bin ====================
            bin_filename = f"{font_label}.bin"
            bin_path = filedialog.asksaveasfilename(
                defaultextension=".bin",
                initialfile=bin_filename,
                filetypes=[("BIN files", "*.bin")]
            )
            if not bin_path:
                return
            
            with open(bin_path, 'wb') as f:
                f.write(bin_data)
            
            # ==================== 构造合并输出（左侧） ====================
            header_comment = f"/*========== {self.font_name}字体{size}号 ==========*/"
            real_path = f"0:/font/{os.path.basename(bin_path)}"
            
            fontinfo_struct = f"""const FontInfo {font_label} = {{
    .font_width  = {width},
    .font_height = {height},
    .font_num    = {char_count},
    .path        = "{real_path}",
    .font_map    = font_map_{font_label}
}};"""
            
            combined_text = header_comment + "\n" + "\n".join(font_map_lines) + "\n\n" + fontinfo_struct
            
            # ==================== extern 声明（右侧） ====================
            extern_code = f"extern const FontInfo {font_label};"
            
            # 填充文本框
            self.bitmap_text.delete("1.0", tk.END)
            self.bitmap_text.insert(tk.END, combined_text)
            
            self.fontinfo_text.delete("1.0", tk.END)
            self.fontinfo_text.insert(tk.END, extern_code)
            
            messagebox.showinfo("完成", f"已生成 {os.path.basename(bin_path)}！\n左侧 = bitmap数组 + FontInfo\n右侧 = extern声明")
            
        except Exception as e:
            messagebox.showerror("生成失败", str(e))


if __name__ == "__main__":
    root = tk.Tk()
    app = FontToBinConverter(root)
    root.mainloop()
