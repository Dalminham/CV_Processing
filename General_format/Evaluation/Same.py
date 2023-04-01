import cv2 
import numpy as np 

file1 = "./out/out2b.bmp"
file2 = "./out/new_s2.bmp"

image1 = cv2.imread(file1)
image2 = cv2.imread(file2)
difference = cv2.subtract(image1, image2)
print(difference)
result = not np.any(difference)

if result is True:
    print("Same!!\n")
else:
    print("Not Same\n")