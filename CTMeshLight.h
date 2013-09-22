#ifndef CMeshH
#define CMeshH

#include <fstream>
#include <CMatrix.h>
#include <CTensor.h>
#include <iostream>
//#include <iostream.h>
using namespace std;

class CJoint {
public:
  // constructor
  inline CJoint() {mParent = 0;mDirection.setSize(0);mPoint.setSize(0);mMoment.setSize(0);  };
  ~CJoint(){};
  inline CJoint(CJoint& aCopyFrom) {*this = aCopyFrom;};
  CJoint(CVector<float>& aDirection, CVector<float>& aPoint, int aParent);
  // Performs a rigid motion M of the joint
  //void rigidMotion(CMatrix<float>& M);
  // Constructs the motion matrix from the joint axis and the rotation angle
  //void angleToMatrix(float aAngle, CMatrix<float>& M);
  // Access to joint's position and axis
  inline void set(CVector<float>& aDirection, CVector<float>& aPoint) {mDirection = aDirection; mPoint = aPoint; mMoment = aPoint/aDirection;};
  inline void setDirection(CVector<float>& aDirection) {mDirection = aDirection; mMoment = mPoint/aDirection;};
  inline CVector<float>& getDirection() {return mDirection;};
  inline void setPoint(CVector<float>& aPoint) {mPoint = aPoint; mMoment = aPoint/mDirection;};
  inline CVector<float>& getPoint() {return mPoint;};
  inline CVector<float>& getMoment() {return mMoment;};
  // Copy operator
  CJoint& operator=(CJoint& aCopyFrom);
  // Parent joint
  int mParent;
protected:
  // Defines joint's position and axis
  CVector<float> mDirection;
  CVector<float> mPoint;
  CVector<float> mMoment;
};

class CMesh {
public:
  // constructor
  inline CMesh();
  inline CMesh(const CMesh& aMesh);

  // Reads the mesh from a file
  bool readModel(const char* aFilename, bool smooth = false);
  bool adaptOFF(const char* aFilename, float lambda);
  void centerModel();

  // Fast projection of a 3-D point to the image plane
  inline void projectPoint(CMatrix<float>& P, float X, float Y, float Z, int& x, int& y);
  inline void projectPoint(CMatrix<float>& P, float X, float Y, float Z, float& x, float& y);

  // Copies aMesh
  void operator=(const CMesh& aMesh);

  // Returns the number of joints
  int joints() {return mJointNumber;};
  // Returns a joint
  CJoint& joint(int aJointID) {return mJoint(aJointID);};
  
  // Returns whether a point is influenced by a certain joint
  bool influencedBy(int aJointIDOfPoint, int aJointID) { return mInfluencedBy(aJointIDOfPoint,aJointID);};
  bool isNeighbor(int i,int j) {return mNeighbor(mJointMap(i),mJointMap(j));};
  bool isEndJoint(int aJointID) {return mEndJoint[aJointID];}

  inline void GetPoint(int i,  float& x, float& y, float& z);
  inline void GetPoint(int i,  float& x, float& y, float& z, int& j);
  inline void GetPoint(int i,  float& x, float& y, float& z, float& j);
  inline void GetPatch(int i, int& x, int& y, int& z);
  inline void GetBounds(int J, int i, float& x, float& y, float& z);
  int GetBoundJID(int J) {return (int)mBounds(J,8,0);};
  float GetCenter(int i) {return mCenter[i];};
  int GetBodyPart(int jID) {return mJointMap[jID];};

  int GetMeshSize() {return mNumPatch;};
  int GetPointSize() {return mNumPoints;};
  int GetBoundSize() {return mBounds.xSize();};

  int GetJointID(int i) {return (int)mPoints[i](3);};

 protected:
  
  int mJointNumber;
  
  std::vector<CVector<float> >  mPoints;
  std::vector<CVector<int> >  mPatch;
  CTensor<float> mBounds;
  CVector<float> mCenter;
  CVector<int> mJointMap;
  CMatrix<bool> mNeighbor;
  CVector<bool> mEndJoint;

  CVector<bool> mCovered;
  CVector<bool> mExtremity;
  
  int mNumPoints;
  int mNumPatch;
  int mNumSmooth; // how many joints can influence any given point
  
  CMatrix<bool> mInfluencedBy;

  CVector<CJoint>  mJoint;
    
  // true if aParentJoint is an ancestor of aJoint
  bool isParentOf(int aParentJointID, int aJointID);
};

// constructor
inline CMesh::CMesh() {
}

inline CMesh::CMesh(const CMesh& aMesh) {
  *this = aMesh;
}

// projectPoint
inline void CMesh::projectPoint(CMatrix<float>& P, float X, float Y, float Z, int& x, int& y) {
  
  float hx = P.data()[0]*X + P.data()[1]*Y + P.data()[2]*Z + P.data()[3];
  float hy = P.data()[4]*X + P.data()[5]*Y + P.data()[6]*Z + P.data()[7];
  float hz = P.data()[8]*X + P.data()[9]*Y + P.data()[10]*Z + P.data()[11];
  
  
  float invhz = 1.0/hz;
  x = (int)(hx*invhz+0.5);
  y = (int)(hy*invhz+0.5);
}

inline void CMesh::projectPoint(CMatrix<float>& P, float X, float Y, float Z, float& x, float& y) {
  
  float hx = P.data()[0]*X + P.data()[1]*Y + P.data()[2]*Z + P.data()[3];
  float hy = P.data()[4]*X + P.data()[5]*Y + P.data()[6]*Z + P.data()[7];
  float hz = P.data()[8]*X + P.data()[9]*Y + P.data()[10]*Z + P.data()[11];
  
  float invhz = 1.0/hz;
  x = hx*invhz;
  y = hy*invhz;
}

inline void CMesh::GetPoint(int i,  float& x, float& y, float& z) {
  x=mPoints[i](0);
  y=mPoints[i](1);
  z=mPoints[i](2);
}

inline void CMesh::GetPatch(int i, int& x, int& y, int& z) {
  x=mPatch[i](0);
  y=mPatch[i](1);
  z=mPatch[i](2);   
}


inline void CMesh::GetBounds(int J, int i,  float& x, float& y, float& z) {
  x=mBounds(J,i,0);
  y=mBounds(J,i,1);
  z=mBounds(J,i,2);
}

inline void CMesh::GetPoint(int i,  float& x, float& y, float& z, int& j) {
  x=mPoints[i](0);
  y=mPoints[i](1);
  z=mPoints[i](2);
  j=int(mPoints[i](3));
}

inline void CMesh::GetPoint(int i,  float& x, float& y, float& z, float& j) {
  x=mPoints[i](0);
  y=mPoints[i](1);
  z=mPoints[i](2);
  j=mPoints[i](3);
}


#endif

