import sys
from PIL import Image
MAX_W = 480
MAX_H = 480

def main():
    if len(sys.argv) < 3: sys.exit(1)
    src_path = sys.argv[1]
    out_path = sys.argv[2]
    src = Image.open(src_path)
    if src.mode in ("RGBA", "LA") or (src.mode == "P" and "transparency" in src.info):
        src = src.convert("RGBA")
        background = Image.new("RGB", src.size, (0, 0, 0))
        background.paste(src, mask=src.split()[3])
        src = background
    else: src = src.convert("RGB")
    scale = min(MAX_W / src.width, MAX_H / src.height)
    new_w = max(1, round(src.width * scale))
    new_h = max(1, round(src.height * scale))
    resized = src.resize((new_w, new_h), Image.LANCZOS)
    pixels = list(resized.getdata())
    with open(out_path, "w") as f:
        f.write("#ifndef LOGO_FB_H\n")
        f.write("#define LOGO_FB_H\n")
        f.write("#include <stdint.h>\n")
        f.write("#define LOGO_FB_WIDTH %d\n" % new_w)
        f.write("#define LOGO_FB_HEIGHT %d\n" % new_h)
        f.write("static const uint8_t logo_fb_pixels[LOGO_FB_WIDTH * LOGO_FB_HEIGHT][3] = {")
        f.write(", ".join("{%d, %d, %d}" % p for p in pixels))
        f.write("};\n")
        f.write("#endif")
if __name__ == "__main__": main()