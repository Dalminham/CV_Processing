from PIL import Image
import os
import codecs

include_extension = ['bmp']

def P2A(image, name):
    x,y = image.size
    print(f"x={x},y={y}\n")
    file = codecs.open(name, 'w', 'utf-8')
    for i in range(x):
        for j in range(y):
            r,g,b = image.getpixel((i,j))
            file.write(f"{r},{g},{b},")
        file.write('\r\n')
    file.close


read_path = "./in"
filedir = os.listdir(read_path)
filenames = [fn for fn in filedir if any(fn.endswith(ext) for ext in include_extension)  ]

file_num = 0
for file in filenames:
    file_num += 1
    print(file)
    im = Image.open(f"./in/{file}")
    im_rgb = im.convert('RGB')
    P2A(im_rgb, f"./Array/{file}_array.txt")

print(f"file_num={file_num}\n")




