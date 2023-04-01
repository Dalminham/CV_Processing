import os

#main
read_path = "./Array/Gau1"
write_path = "./Array/Gau1_out"

include_extension = ['bin']
filedir = os.listdir(read_path)
filenames = [fn for fn in filedir if any(fn.endswith(ext) for ext in include_extension)  ]

for file in filenames:
    fcommand = f"./main 1 ./Array/Gau1/{file} ./Array/Gau1_out/{file} 1632"
    os.system(fcommand)


