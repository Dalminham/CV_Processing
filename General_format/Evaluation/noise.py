import numpy as np 
import cv2 
import os.path

def gaussian_noise(image, mean, var):

    image = np.array(image / 255, dtype=float)
    noise = np.random.normal(mean, var ** 0.5, image.shape)
    out   = image + noise 

    if out.min() < 0:
        low_clip = -1.
    else:
        low_clip = 0.

    out = np.clip(out, low_clip, 1.0)
    out = np.uint8(out * 255)

    return out 
path = "./origin/Pic_out.bmp"
print(os.path.exists(path))
image = cv2.imread(path)
noisy1 = gaussian_noise(image, mean=0, var = 0.01)
noisy2 = gaussian_noise(image, mean=0.1, var = 0.01)
noisy3 = gaussian_noise(image, mean=0, var = 0.2)

cv2.imwrite('./in/1b.bmp',noisy1)
cv2.imwrite('./in/2b.bmp',noisy2)
cv2.imwrite('./in/3b.bmp',noisy3)