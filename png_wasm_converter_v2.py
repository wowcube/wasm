from PIL import Image
import os
import textwrap


def png_2_rgb565(png_file_name):
    img = Image.open(png_file_name)
    if img.mode != 'RGB' and img.mode != 'RGBA':
        img = img.convert('RGB')
    pixels = img.load()
    width, height = img.size
    out565 = []
    for y in range(height):
        for x in range(width):
            rgb = pixels[x, y]
            if isinstance(rgb, int):
                print("Unsupported format: ")
                return [], 0, 0
            # alpha channel avaialable
            if len(rgb) == 4 and rgb[3] == 0:
                # WOWCube now assumes black as transparent
                out565.append(0)
            else:
                #  convert RGB888 to RGB565
                out565.append((((rgb[0] >> 3) & 0x1f) << 11) | (((rgb[1] >> 2) & 0x3f) << 5) | ((rgb[2] >> 3) & 0x1f))
    return out565, width, height


def rle(rgb565_buffer):
    if not rgb565_buffer:
        return []

    rle_out = []
    rle_sym = rgb565_buffer[0]
    rle_cnt = 1

    for idx in range(1, len(rgb565_buffer)):
        if rgb565_buffer[idx] != rle_sym or rle_cnt == 0x8000:
            rle_out.append(rle_cnt)
            rle_out.append(rle_sym)
            rle_cnt = 1
            rle_sym = rgb565_buffer[idx]
        else:
            rle_cnt += 1

    if rle_cnt >= 1:
        rle_out.append(rle_cnt)
        rle_out.append(rle_sym)

    rle2_out = []
    rle2_norepeat = []
    rle2_idx = -1

    i = 0
    while i < len(rle_out):
        if rle_out[i] == 1:
            rle2_norepeat.append(rle_out[i + 1])
        else:
            if len(rle2_norepeat) > 0:
                rle2_idx += 1
                rle2_out.insert(rle2_idx, (0xFFFF - len(rle2_norepeat)))

                for j in range(len(rle2_norepeat)):
                    rle2_idx += 1
                    rle2_out.insert(rle2_idx, rle2_norepeat[j])

                rle2_norepeat = []

            rle2_idx += 1
            rle2_out.insert(rle2_idx, rle_out[i])
            rle2_idx += 1
            rle2_out.insert(rle2_idx, rle_out[i + 1])
        i += 2

    if len(rle2_norepeat) > 0:
        rle2_idx += 1
        rle2_out.insert(rle2_idx, 0xFFFF - len(rle2_norepeat))

        for k in range(len(rle2_norepeat)):
            rle2_idx += 1
            rle2_out.insert(rle2_idx, rle2_norepeat[k])
        rle2_norepeat = []
    return rle2_out


def png_2_cubios_picture(png_file_name):
    rgb565, width, height = png_2_rgb565(png_file_name)
    compressed = rle(rgb565)
    compressed.insert(0, height)
    compressed.insert(0, width)
    return rgb565, compressed


def estimate_size(path):
    fl = open('size_measure.log', 'w')
    uncompressed = 0
    compressed = 0
    for root, dir, files in os.walk(path):
        for file in fnmatch.filter(files, '*.png'):
            rgb, rgb_rle = png_2_cubios_picture(os.path.join(root,file))
            uncompressed += len(rgb)
            compressed += len(rgb_rle)
            s = "%s: origin: %.2fKB compressed: %.2fKB\n" % (file, len(rgb) / 1024.0, len(rgb_rle) / 1024.0)
            print(s)
            fl.write(s)

    s = ''
    s += "======================\n"
    s += "Uncompressed size: %.3f KB\n" % (uncompressed / 1024.0)
    s += "Compressed size: %.3f KB\n" % (compressed / 1024.0)
    print(s)
    fl.write(s)
    fl.close()


getRes = """

typedef struct {
    const void* ptr;
    uint16_t size;
} resource_t;

enum EResource{
%s,
er_img_Size
};

resource_t get_resource(int idx){
    static resource_t resources[] = {
%s
    };
    return resources[idx];
}
"""


def convert_dir(path, rle):
    names = []
    enum = []
    with open(path+'.h', "w") as out:
        for root, dir, files in os.walk(path):  
            for file in sorted(fnmatch.filter(files, '*.png')):
                print(file)
                string, name, _len = convert_picture(os.path.join(root,file), rle)
                names.append("{%s, %d}" % (name, _len))
                enum.append("er%s" % name.capitalize())
                out.write(string)
                out.write('\n')

        out.write(getRes % (',\n'.join(enum), ',\n'.join(names)))




def convert_picture(pic_path, rle):
    rgb, rgb_rle = png_2_cubios_picture(pic_path)
    arr_name = os.path.basename(pic_path).split('.')[0]
    #c_file_name = os.path.splitext(pic_path)[0] + '.c'

    #fl = open(c_file_name, 'w')
    final_arr = rgb_rle if rle else rgb
    text = ""
    arr_name = "_img_" + arr_name
    text += 'const uint16_t {}[{}] = {{\n'.format(arr_name, len(final_arr))
    for s in textwrap.wrap(', '.join(format(x, '#06x') for x in final_arr), 80):
        text += '    {}\n'.format(s)
    text += '};\n'
    return text, arr_name, len(final_arr)


if __name__ == '__main__':
    import os
    import fnmatch
    import argparse

    parser = argparse.ArgumentParser(description='Parse map file from resources build')
    parser.add_argument('-d', dest='dir', action='store', default='',
                        help='Directory with source png')
    parser.add_argument('-p', dest='picture', action='store', default='',
                        help='Specify picture to convert')
    parser.add_argument('--rle', dest='rle_compress', action='store_true', default=False,
                        help='Enables RLE compression')
    args = parser.parse_args()

    if args.dir:
        convert_dir(args.dir, args.rle_compress)

    if args.picture:
        convert_picture(args.picture, args.rle_compress)