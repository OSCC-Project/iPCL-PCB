import tkinter as tk
from tkinter import filedialog, messagebox, ttk
from PIL import Image, ImageTk
import os
import re
import struct

class ImageToRGB565Converter:
    def __init__(self, root):
        self.root = root
        self.root.title("图片转RGB565 .bin + ImageInfo 生成器（批量版）")
        self.root.geometry("1000x700")
        self.root.resizable(True, True)
        
        self.style = ttk.Style()
        self.style.configure(".", font=("SimHei", 10))
        
        # 改为列表存储多个图片路径
        self.image_paths = []
        self.images = {}  # 存储图片对象 {路径: (image, width, height)}
        self.tk_image = None
        self.max_display_size = (400, 300)
        
        self.file_name_var = tk.StringVar()
        # 存储按钮引用（替代原来粗暴的children获取）
        self.convert_btn = None
        self.copy_btn = None
        
        self.create_widgets()
    
    def create_widgets(self):
        # 选择图片按钮
        ttk.Button(self.root, text="选择图片（支持多选）", command=self.select_image).pack(pady=10)
        
        # 图片预览区域
        self.image_frame = ttk.Frame(self.root)
        self.image_frame.pack(pady=10)
        self.image_label = ttk.Label(self.image_frame, text="未选择图片")
        self.image_label.pack()
        
        # 图片信息标签
        self.info_label = ttk.Label(self.root, text="选中图片: 0张 | 尺寸: -- x --")
        self.info_label.pack(pady=5)
        
        # 文件名输入区域
        name_frame = ttk.Frame(self.root)
        name_frame.pack(pady=10, fill=tk.X, padx=20)
        
        self.name_label = ttk.Label(name_frame, text="文件名（建议带分辨率）：")
        self.name_label.pack(side=tk.LEFT, padx=5)
        
        vcmd = (self.root.register(self.validate_name), '%P')
        self.name_entry = ttk.Entry(
            name_frame, 
            textvariable=self.file_name_var,
            validate="key",
            validatecommand=vcmd,
            width=40
        )
        self.name_entry.pack(side=tk.LEFT, padx=5, fill=tk.X, expand=True)
        self.name_hint = ttk.Label(name_frame, text=".bin 会自动添加", foreground="gray")
        self.name_hint.pack(side=tk.LEFT, padx=5)
        
        # 生成按钮（存储引用）
        self.convert_btn = ttk.Button(self.root, text="生成 .bin 并输出 ImageInfo", command=self.generate, state=tk.DISABLED)
        self.convert_btn.pack(pady=15)
        
        # 结果展示区域
        ttk.Label(self.root, text="生成的 ImageInfo 代码（直接复制）:").pack(anchor=tk.W, padx=20)
        self.result_text = tk.Text(self.root, height=15, width=80, font=("Consolas", 11))
        self.result_text.pack(padx=20, pady=5, fill=tk.BOTH, expand=True)
        
        # 复制按钮（存储引用）
        self.copy_btn = ttk.Button(self.root, text="复制到剪贴板", command=self.copy_result, state=tk.DISABLED)
        self.copy_btn.pack(pady=10)
    
    def validate_name(self, value):
        if not value:
            return True
        # 允许字母、数字、下划线、连字符
        if re.match(r'^[a-zA-Z_][a-zA-Z0-9_-]*$', value):
            return True
        return False
    
    def select_image(self):
        # 修改为支持多选文件
        file_paths = filedialog.askopenfilenames(
            filetypes=[("图片文件", "*.png *.jpg *.jpeg *.bmp *.gif *.tiff"), ("所有文件", "*.*")]
        )
        if not file_paths:
            return
            
        self.image_paths = list(file_paths)
        self.images.clear()
        valid_count = 0
        preview_image = None
        
        # 处理每个选中的图片
        for file_path in self.image_paths:
            try:
                image = Image.open(file_path)
                width, height = image.size
                self.images[file_path] = (image, width, height)
                valid_count += 1
                # 取第一张图片作为预览
                if preview_image is None:
                    preview_image = image
            except Exception as e:
                messagebox.showwarning("警告", f"无法打开图片 {os.path.basename(file_path)}:\n{str(e)}")
        
        if valid_count == 0:
            self.image_paths = []
            self.info_label.config(text="选中图片: 0张 | 尺寸: -- x --")
            self.image_label.config(text="未选择图片", image="")
            self.convert_btn.config(state=tk.DISABLED)
            return
        
        # 更新预览和信息
        if preview_image:
            resized = self.resize_image(preview_image, self.max_display_size)
            self.tk_image = ImageTk.PhotoImage(resized)
            self.image_label.config(image=self.tk_image, text="")
        
        # 更新信息标签
        if valid_count == 1:
            # 单选模式 - 启用文件名输入
            file_path = self.image_paths[0]
            image, width, height = self.images[file_path]
            self.info_label.config(text=f"选中图片: 1张 | 尺寸: {width} x {height}")
            
            # 自动建议文件名
            base = os.path.splitext(os.path.basename(file_path))[0]
            base = re.sub(r'[^a-zA-Z0-9_-]', '_', base).strip('_')
            if not base or base[0].isdigit():
                base = "img_" + base
            suggested = f"{base}_{width}x{height}"
            self.file_name_var.set(suggested)
            
            # 启用文件名输入相关控件
            self.name_label.config(state=tk.NORMAL)
            self.name_entry.config(state=tk.NORMAL)
            self.name_hint.config(state=tk.NORMAL)
        else:
            # 多选模式 - 禁用文件名输入
            self.info_label.config(text=f"选中图片: {valid_count}张 | 批量转换模式")
            self.file_name_var.set("")
            
            # 禁用文件名输入相关控件
            self.name_label.config(state=tk.DISABLED)
            self.name_entry.config(state=tk.DISABLED)
            self.name_hint.config(state=tk.DISABLED)
        
        # 启用生成按钮
        self.convert_btn.config(state=tk.NORMAL)
    
    def resize_image(self, img, max_size):
        w, h = img.size
        ratio = min(max_size[0]/w, max_size[1]/h)
        if ratio >= 1:
            return img
        return img.resize((int(w*ratio), int(h*ratio)), Image.LANCZOS)
    
    def rgb_to_rgb565(self, r, g, b):
        r5 = (r >> 3) & 0x1F
        g6 = (g >> 2) & 0x3F
        b5 = (b >> 3) & 0x1F
        return (r5 << 11) | (g6 << 5) | b5
    
    def generate(self):
        if not self.image_paths or len(self.images) == 0:
            return
        
        # 存储所有生成的C代码
        all_c_codes = []
        # 存储生成的文件路径
        generated_files = []
        
        try:
            # 遍历所有选中的图片
            for file_path in self.image_paths:
                if file_path not in self.images:
                    continue
                    
                image, width, height = self.images[file_path]
                
                # 确定文件名
                if len(self.image_paths) == 1:
                    # 单选模式 - 使用自定义文件名
                    name_base = self.file_name_var.get().strip()
                    if not name_base:
                        messagebox.showerror("错误", "请输入文件名")
                        return
                    if not self.validate_name(name_base):
                        messagebox.showerror("错误", "文件名只能包含字母、数字、下划线、连字符，且不能以数字开头")
                        return
                else:
                    # 多选模式 - 使用原文件名（处理非法字符）
                    base = os.path.splitext(os.path.basename(file_path))[0]
                    base = re.sub(r'[^a-zA-Z0-9_-]', '_', base).strip('_')
                    if not base or base[0].isdigit():
                        base = "img_" + base
                    name_base = f"{base}_{width}x{height}"
                
                bin_filename = name_base + ".bin"
                full_path_in_c = f"0:/images/{bin_filename}"
                
                # 转换为RGB565
                rgb_img = image.convert('RGB')
                pixel_data = bytearray()
                
                for y in range(height):
                    for x in range(width):
                        r, g, b = rgb_img.getpixel((x, y))
                        val = self.rgb_to_rgb565(r, g, b)
                        # 小端序（STM32 默认）
                        pixel_data.extend([val & 0xFF, (val >> 8) & 0xFF])
                
                # 多选时默认使用原文件名保存，只在第一次询问保存目录
                if len(generated_files) == 0:
                    # 选择保存目录（而非具体文件）
                    save_dir = filedialog.askdirectory(title="选择批量保存 .bin 文件的目录")
                    if not save_dir:
                        return
                
                # 拼接保存路径
                save_path = os.path.join(save_dir, bin_filename)
                
                # 写入文件
                with open(save_path, 'wb') as f:
                    f.write(pixel_data)
                
                generated_files.append(save_path)
                
                # 生成C代码行
                c_code = (
                    f'const ImageInfo img_{name_base} = {{ '
                    f'.width = {width}, '
                    f'.height = {height}, '
                    f'.path = "{full_path_in_c}" '
                    f'}};'
                )
                all_c_codes.append(c_code)
            
            # 清空结果框并写入所有代码
            self.result_text.delete(1.0, tk.END)
            self.result_text.insert(tk.END, '\n\n'.join(all_c_codes))
            
            # 启用复制按钮
            self.copy_btn.config(state=tk.NORMAL)
            
            # 提示完成
            file_list = "\n".join([f"- {os.path.basename(f)}" for f in generated_files])
            messagebox.showinfo(
                "完成", 
                f"成功生成 {len(generated_files)} 个.bin 文件：\n{file_list}\n\n"
                f"总大小：{sum(os.path.getsize(f) for f in generated_files):,} 字节\n\n"
                f"ImageInfo 代码已显示在下方，可直接复制"
            )
            
        except Exception as e:
            messagebox.showerror("生成失败", str(e))
    
    def copy_result(self):
        text = self.result_text.get(1.0, tk.END).strip()
        if text:
            self.root.clipboard_clear()
            self.root.clipboard_append(text)
            messagebox.showinfo("已复制", "所有ImageInfo代码已复制到剪贴板")

if __name__ == "__main__":
    root = tk.Tk()
    app = ImageToRGB565Converter(root)
    root.mainloop()