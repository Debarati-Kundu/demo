Author:  Juergen Gall
Contact: gall@vision.ee.ethz.ch
Date: 10.07.2009
Computer Vision Laboratory, ETH Zurich

All data is only for research purposes. When using this data, please acknowledge the effort that went into data collection by referencing the corresponding paper:

Gall J., Stoll C., de Aguiar E., Theobalt C., Rosenhahn B., and Seidel H.-P., Motion Capture Using Joint Skeleton Tracking and Surface Estimation, IEEE Conference on Computer Vision and Pattern Recognition (CVPR'09), 2009.

##########################################################################

Description of calibration data:

The images and silhouettes are already undistorted. The files proj0?.dat give the calibration data for each of the 8 cameras as matrices:

P=K*M

i.e., a vertex X is projected by P*X.

matrixCalibration.dat is an alternative representation that gives the OpenGL modelview and projection matrix for each camera. The file starts with the number of cameras (=8) followed by the camera id (0-7) and the two OpenGL matrices. Note that the projection draws the images upside down on the graphics card. Since the (0,0)-point is at the bottom left in OpenGL and at the top left for standard images, the images are again in the right order when reading the framebuffer byte-by-byte. In addition, when you load the 3d meshes in a program for 3d visualization, you might flip the orientation of the triangles since the world coordinate system might be different. 

Please check the example source code that reads the meshes and the proj*.dat files to project it onto the images.

##########################################################################

Description of model data:

model.dat is a file that contains the triangle mesh (similar to .off file), the skeleton, and the skinning weights:

2501 5000 30 15 
// #vertices #triangles #joints(DoF) #body parts
// order of the file: vertices - triangles - skeleton
// vertices + weights:
-93.928 -54.142 0.556 3 0.3421 9 0.3405 0 0.3154 15 0.0009 4 0.0005 10 0.0004 18 0.0001 24 0.0001 6 0 12 0 19 0 21 0 25 0 27 0 30 0
// 3d position (3 values) + 15 * (limb/joint index + skinning weight) (#body parts * 2 = 30 values)
// Remark: the weights sum up to 1, the pairs (index + wieght) are ordered by the weight.
.....
// triangles:
2496 2499 27
// vertex indices of a triangle 
// remark: index starts with zero similar to .off file 
....
// skeleton:
1 -0.956965 -0.0873481 0.276745 -10.033 -25.431 -118.655 0
2 0.0505746 -0.989231 -0.137344 -10.033 -25.431 -118.655 1
3 0.285761 -0.117437 0.951078 -10.033 -25.431 -118.655 2
4 0.0725656 0.172207 -0.982384 9.058 -398.848 -170.5 3
// jointID (1 value) + rotation axis (3 values) + 3d position (3 values) + jointID of previous joint (1 value)
// Remark: the joints are modeled by Euler anngles, i.e., each joint corresponds to only one Euler angle.
// A 'real joint' can have 1-3 DoF, i.e., it consists of three joints. For example, joints with ID 1-3 are at position 
// (-10.033,-25.431,-118.655). They build one real joint with 3 DoF and rotation axes: 
// (-0.956965 -0.0873481 0.276745)  
// (0.0505746 -0.989231 -0.137344)
// (0.285761 -0.117437 0.951078)
// A vertex is only associated with the last DoF of a joint, i.e., the skinning weights pairs (index + wieght) contain the index '3' 
// but not '2' or '1' since the three DoF build one real joint.
....
// At the end of the file, there can be some unused dummy values:
0 0 1 1 0 1 1 0 0 1 1 0 1 1 1
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 

The model.dat file is a representation that we use, but we also provide the skeleton, mesh, and the skinning weights as separate files. Note that the scale and coordinate system might differ. The model.dat file uses the right scale and coordinate system.

skeleton: *.skel
mesh: *.off (we used the meshes with the lower number of triangles)
weights: *.out

The models were rigged with:
I. Baran and J. Popovic. Automatic rigging and animation of
3d characters. ACM Trans. Graph., 26(3):72, 2007.
http://www.mit.edu/~ibaran/autorig/
The skeleton and weight file format corresponds to the one that is used by the rigging software.

##########################################################################

Description of mesh files:

The file seq.bmf contains the names of the images. The estimated meshes are named output*.off. Note that for the first frame a mesh is not available. Output0001.off is the mesh for the second frame in the .bmf file. For testing you can run the demo application, it reads the calibration files, the meshes, and the images and projects the meshes onto the images.

Compiling: 
make all
Running: 
/* 
   Usage: ./ShowMesh <srcpath> <sequence.bmf> <input> <outputpath> <view>
   <srcpath> - path to calibration data  
   <sequence.bmf> - seq.bmf (seq.bmf needs to specify absolute path to images)
   <input> - path to meshes + prefix, 
             PATH/output reads PATH/output0001.off, PATH/output0002.off, ...  
   <outputpath> - path for storing projections ('Results' is added to the directory name)
   <view> - specify a camera view (0-7) or generate all views (-1)
*/ 

##########################################################################

Juergen Gall
