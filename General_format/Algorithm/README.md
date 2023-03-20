# Algorithm for Computer Vision Process

___
## File Introduction:
1. bf.c : Scalar version bilateralFilter developed by embedded business uint.
2. bilateralFilter_simd.c : SIMD Vector version bilateralFilter developed by us. 
This file is responsible for the upper encapsulation of the algorithm and provides the call interface.
3. bilateralFilter_simd.h : IMD Vector version bilateralFilter developed by us.
   This file is responsible for algorithm implementation.
4. main.c: top of program;
5. run.py: Simple script for batch processing pictures;

Dir **InImage** stores the input images.

Dir **OutImage** is to store the output images.

## Algorithms:

1. bilateralFilter: 
The __vectorized__ bilateral image filtering function developed based on LSX intrinsic;
The algorithm supports **rgb** images and **gray-scale** images.
At present, floating point algorithm and __fixed-point__ algorithm have been implemented.

## Operation

1. Run SIMD Vector Optimization algorithm:
```shell
#Input and output images are provided by command line parameters
#1 is a choosing tag
#loop_n is used for performace evaluation. Default Value is 1
./main "input-img" "output-img" 1 loop_num

```

2. Run the scalar version developed by embedded business unit:
```shell
#Input and output images are provided by command line parameters
#0 is a choosing tag
#loop_n is used for performace evaluation. Default Value is 1
./main "input-img" "output-img" 0 loop_num

```

## Reference:
OpenCV 4.x
