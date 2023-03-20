import os

#main
read_path = "./InImage"
write_path = "./OutImage"

files = os.listdir(read_path)
name_cnt = 1

file_num = 0
for file in files:
    file_num += 1
#gauss and pepper takes halve respectively
file_num /= 2
print(file_num)
for file in files:
    if name_cnt > file_num:
        break

    if not os.path.isdir(file):
        '''
        running main for each image
        '''
        gauss_input = f"{read_path}/gauss_{name_cnt}.bmp"
        gauss_output = f"{write_path}/gauss_d_{name_cnt}.bmp"
        gauss_command = f"./main {gauss_input} {gauss_output}"

        pepper_input = f"{read_path}/pepper_{name_cnt}.bmp"
        pepper_output = f"{write_path}/pepper_d_{name_cnt}.bmp"
        pepper_command = f"./main {pepper_input} {pepper_output}"
        
        os.system(gauss_command)
        os.system(pepper_command)
        name_cnt = name_cnt + 1
