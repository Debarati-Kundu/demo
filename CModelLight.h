// Author: Juergen Gall 

#ifndef CModelH
#define CModelH

#include <vector>

#include <CTMeshLight.h>

#include <CMatrix.h>
#include <CVector.h>
#include <CTensor.h>

#include <pbuffer.h>

/* various modes for drawing an object:
 * TEXTURE - textured object
 * COLORINDEX - each triangle is drawn in an unique color (RGBA)
 * COLORINDEXPOINTS - each point is drawn in an unique color (RGBA)
 * COLORCOMPONENTS - each triangle is drawn in the color of its component 
 * POINTS - draws visible mesh points
 * SILHOUETTE - draws silhouette
 * MESH - draws visible mesh 
 */
enum drawmode {TEXTURE, COLORINDEX, COLORINDEXPOINTS, COLORCOMPONENTS, SETCOLORCOMPONENTS, POINTS, SILHOUETTE, MESH};

/* structure for triangles that stores the indices of the (three) mesh vertices
 * used for COLORINDEX
 */
struct STriangle {
  CMatrix<int> mIndices;
};

/* structure for points that stores the indices of the mesh vertex
 * used for COLORINDEXPOINTS
 */
struct SPoint {
  int x;
  int y;
};

struct SubImageRGB {
  int minX;
  int minY;
  CTensor<float> mImg;
};

struct SubImage {
  int minX;
  int minY;
  CMatrix<float> mImg;
};

struct SubMap {
  int minX;
  int minY;
  CMatrix<GLuint> mImg;
};

struct SOcclusion {
  int material;
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat velocity;
};

/* structure containing the projection data from geocast */
struct GeoCastData
{
  /******* GPU projection *******/
  // GL_MODELVIEW matrix
  float MVmatrixInverse[16];
  // parameters for gluPerspective
  float Fovy;
  float Aspect;
  float ClipNear;
  float ClipFar;
  // parameters for glFrustum
  float left;
  float right;
  float top;
  float bottom;
  float near;
  float far;

  // position
  float PosX, PosY, PosZ;

  /******* CPU projection *******/
  // 3x4 projection matrix
  CMatrix<float> Projection;
};

class CModel {
public:
  // constructor
  CModel(int aWidth, int aHeight, int aViews);
  // destructor
  ~CModel();

  void resetColor() {mR = 1.0; mG = 1.0; mB = 1.0;};

  // generate mesh, silhouette or visible points from given pose
  void generateMesh(CMesh* aMesh, std::vector<CTensor<float> >& aImage, GLubyte aR, GLubyte aG, GLubyte aB, bool aOverlay = false) const {drawPBufferBW(aMesh, MESH, aImage, aR, aG, aB, aOverlay);};

  // Fast projection of a 3-D point to the image plane
  inline void projectPoint(int aView, const CVector<float>& v3D, int& x, int& y) const;
  const CMatrix<float>& getProjection(int aView) const {return mCamera[aView].Projection;};
  
private:
  int mWidth, mHeight, mViews;
  GLuint mNumVertices, mNumPatches;
  int mNumTex;
  GLfloat mR, mG, mB; 

  // Cameras
  std::vector<GeoCastData> mCamera;

  // P-Buffer
  PBuffer* mPBufferTex;

  // Textures
  GLuint* tex_id;
  CMatrix<float> mTexCoord;

  // init functions
  void initCamera(const char* file, int view);
  void initPBuffer();

  // draws colored object for all views. 
  // aMode:  POINTS, SILHOUETTE or  MESH. 
  // overlay: true - draws object over given images aImage
  void drawPBufferBW(CMesh* aMesh, drawmode aMode, std::vector<CTensor<float> >& aImage, GLubyte aR, GLubyte aG, GLubyte aB, bool aOverlay) const;

  // for COLORINDEXPOINTS, POINTS function draw_object_points() is better!
  void draw_object_tex(CMesh* aMesh, drawmode aMode, GLubyte aR = 255, GLubyte aG = 0, GLubyte aB = 0, GLubyte aA = 0, std::vector<GLfloat>* aGray = NULL) const;
  // draws object where aMode is COLORINDEXPOINTS, POINTS
  void draw_object_points(CMesh* aMesh, drawmode aMode, GLubyte aR = 255, GLubyte aG = 0, GLubyte aB = 0, GLubyte aA = 0) const;

  // convert images GPU (zero: bottom left) <-> CPU (zero: top left) 
  void readGlImage(GLubyte* aGlImage, CTensor<float>& aImg) const;
  void readGlImage(GLubyte* aGlImage, CMatrix<float>& aImg) const;
  void readGlImage(GLuint* aGlImage, CMatrix<GLuint>& aImage) const;
  void drawGlImage(CTensor<float>& aImage, GLubyte* aGlImage) const;
};

// projectPoint
void CModel::projectPoint(int aView, const CVector<float>& v3D, int& x, int& y) const {
  float hx = mCamera[aView].Projection.data()[0]*v3D[0] + mCamera[aView].Projection.data()[1]*v3D[1] + mCamera[aView].Projection.data()[2]*v3D[2] + mCamera[aView].Projection.data()[3];
  float hy = mCamera[aView].Projection.data()[4]*v3D[0] + mCamera[aView].Projection.data()[5]*v3D[1] + mCamera[aView].Projection.data()[6]*v3D[2] + mCamera[aView].Projection.data()[7];
  float hz = mCamera[aView].Projection.data()[8]*v3D[0] + mCamera[aView].Projection.data()[9]*v3D[1] + mCamera[aView].Projection.data()[10]*v3D[2] + mCamera[aView].Projection.data()[11];
  
  float invhz = 1.0/hz;
  x = (int)(hx*invhz+0.5);
  y = (int)(hy*invhz+0.5);
}


 
#endif
