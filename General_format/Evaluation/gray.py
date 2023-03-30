import cv2 as cv
import os 
import numpy as np


img = cv.imread("origin/1.bmp",1)
gray_img = cv.cvtColor(img, cv.COLOR_RGB2GRAY)
cv.imwrite("1.bmp",gray_img)
