#include <Show.h>
#include "CTMeshLight.h"

// C J O I N T -----------------------------------------------------------------

// constructor
CJoint::CJoint(CVector<float>& aDirection, CVector<float>& aPoint, int aParent) {
  mDirection = aDirection;
  mPoint = aPoint;
  mMoment = aPoint/aDirection;
  mParent = aParent;
}

// operator =
CJoint& CJoint::operator=(CJoint& aCopyFrom) {
  mDirection = aCopyFrom.mDirection;
  mPoint = aCopyFrom.mPoint;
  mMoment = aCopyFrom.mMoment;
  mParent = aCopyFrom.mParent;
  return *this;
}

// C M E S H -------------------------------------------------------------------

// readModel
bool CMesh::readModel(const char* aFilename, bool smooth) {
  cout << "Read Model... ";

  std::ifstream aStream(aFilename);
  if(!aStream.is_open()) {
    cerr << "Could not open " << aFilename << endl;
    return false;
  }

  int aXSize, aYSize, size = 4;

  CJoint Joint;

  aStream >> mNumPoints; 
  cout << "mn" << mNumPoints << endl; 
  aStream >> mNumPatch; 
  aStream >> mJointNumber;
  if(smooth) {
    aStream >> mNumSmooth;
    if(mNumSmooth == 1)
      smooth = false;
    else
      size = 3 + mNumSmooth * 2;
  } else
    mNumSmooth = 1;
  
  cout << mNumPoints << " " << mNumPatch << " " << mJointNumber << " " << mNumSmooth << endl;

  CVector<bool> BoundJoints(mJointNumber+1,false);

  mJoint.setSize(mJointNumber+1);
  
  // Read mesh components
  mPoints.resize(mNumPoints); 
  mPatch.resize(mNumPatch);

  for(int i=0;i<mNumPoints;i++) {
    mPoints[i].setSize(size);
  } 

  for(int i=0;i<mNumPatch;i++) {
    mPatch[i].setSize(3);
  }
  
  for (int i = 0; i < mNumPoints; i++) {
    aStream >> mPoints[i][0]; 
    aStream >> mPoints[i][1];
    aStream >> mPoints[i][2]; 
    if(smooth) {
      for(int n = 0; n < mNumSmooth * 2; n++)
        aStream >> mPoints[i][3 + n];
    } else
      aStream >> mPoints[i][3]; 
    BoundJoints((int)mPoints[i][3]) = true;
  }
  
  for (int i = 0; i < mNumPatch; i++) {
    aStream >> mPatch[i][0];
    aStream >> mPatch[i][1];
    aStream >> mPatch[i][2];
  }
  
  // set bounds
  int count = 0;
  mJointMap.setSize(mJointNumber+1);
  mJointMap = -1;
  for(int j = 0; j<=mJointNumber; ++j)
    if(BoundJoints(j))
      mJointMap(j) = count++;      

  mBounds.setSize(count,9,3);
  mNeighbor.setSize(count, count); 
  CMatrix<float> minV(count,3,100000);
  CMatrix<float> maxV(count,3,-100000);
   
  for (int i = 0; i < mNumPoints; i++) {
    int index = mJointMap((int)mPoints[i][3]);
    
    if ( mPoints[i][0]<minV(index,0) ) minV(index,0) = mPoints[i][0];
    if ( mPoints[i][1]<minV(index,1) ) minV(index,1) = mPoints[i][1];
    if ( mPoints[i][2]<minV(index,2) ) minV(index,2) = mPoints[i][2];
    if ( mPoints[i][0]>maxV(index,0) ) maxV(index,0) = mPoints[i][0];
    if ( mPoints[i][1]>maxV(index,1) ) maxV(index,1) = mPoints[i][1];
    if ( mPoints[i][2]>maxV(index,2) ) maxV(index,2) = mPoints[i][2];
  }
  
  for(int i=0; i<count; ++i) {
    mBounds(i,0,0) = mBounds(i,1,0) = mBounds(i,2,0) = mBounds(i,3,0) = minV(i,0);
    mBounds(i,4,0) = mBounds(i,5,0) = mBounds(i,6,0) = mBounds(i,7,0) = maxV(i,0);
    mBounds(i,0,1) = mBounds(i,1,1) = mBounds(i,4,1) = mBounds(i,5,1) = minV(i,1);
    mBounds(i,2,1) = mBounds(i,3,1) = mBounds(i,6,1) = mBounds(i,7,1) = maxV(i,1);
    mBounds(i,0,2) = mBounds(i,2,2) = mBounds(i,4,2) = mBounds(i,6,2) = minV(i,2);
    mBounds(i,1,2) = mBounds(i,3,2) = mBounds(i,5,2) = mBounds(i,7,2) = maxV(i,2);
  }

  mCenter.setSize(4);
  mCenter[0] = (maxV(0,0) + minV(0,0))/2.0f;
  mCenter[1] = (maxV(0,1) + minV(0,1))/2.0f;
  mCenter[2] = (maxV(0,2) + minV(0,2))/2.0f;
  mCenter[3] = 1.0f;
  
  for(int j=0; j<mJointMap.size();++j)
    if(mJointMap(j)>=0)
      mBounds(mJointMap(j),8,0) = j;
  
  // Read joints
  int dummy;
  CVector<float> aDirection(3);
  CVector<float> aPoint(3);
  for (int aJointID = 1; aJointID <= mJointNumber; aJointID++) {
    aStream >> dummy; // ID
    aStream >> aDirection(0) >> aDirection(1) >> aDirection(2);
    aStream >> aPoint(0) >> aPoint(1) >> aPoint(2);
    mJoint(aJointID).set(aDirection,aPoint);
    aStream >> mJoint(aJointID).mParent;
  }
  // Determine which joint motion is influenced by parent joints
  mInfluencedBy.setSize(mJointNumber+1,mJointNumber+1);
  mInfluencedBy = false;
  for (int j = 0; j <= mJointNumber; j++)
    for (int i = 0; i <= mJointNumber; i++) {
      if (i == j) mInfluencedBy(i,j) = true;
      if (isParentOf(j,i)) mInfluencedBy(i,j) = true;
    }

  mNeighbor = false;
  for (int i = 0; i < mNeighbor.xSize(); i++) {
    mNeighbor(i,i) = true;
    int jID = (int)mBounds(i,8,0);
    for (int j = jID-1; j >= 0; --j) {
      if(mJointMap(j)>=0 && mInfluencedBy(jID,j)) {
	mNeighbor(i,mJointMap(j)) = true;
	mNeighbor(mJointMap(j),i) = true;
	break;
      }
    }
  }

  mEndJoint.setSize(mJointNumber+1);
  mEndJoint.fill(true);
  for(int i=1; i<=mJointNumber; ++i)
    mEndJoint[mJoint(i).mParent]=false;

  for(int i=1; i<=mJointNumber; ++i)
    if(mEndJoint[i]==true) {
      int j = i;
      while(mJointMap[--j]==-1) {
	mEndJoint[j]=true;
      }
    }

#if 0
  mCovered.setSize(mBounds.xSize());
  mExtremity.setSize(mBounds.xSize());
  for(int i=0; i<mExtremity.size(); ++i)
    aStream >> mExtremity[i];
  for(int i=0; i<mCovered.size(); ++i)
    aStream >> mCovered[i];
#endif
cout << endl << "Read Model ends... " << endl;
  return true;
}

void CMesh::centerModel() {

  CVector<float> trans(4);
  trans(0) = -mCenter(0); trans(1) = -mCenter(1); trans(2) = -mCenter(2); trans(3) = 0;

  for (int i = 0; i < mNumPoints; i++) {
    mPoints[i](0) += trans(0);
    mPoints[i](1) += trans(1);
    mPoints[i](2) += trans(2);
  }
  
  for (int i = 1; i <= mJointNumber; i++) {
    CVector<float> jPos(mJoint[i].getPoint());
    for(int j = 0; j < 3; ++j) 
      jPos(j) += trans(j);
    mJoint[i].setPoint(jPos);
  }

  for(int i = 0; i < mBounds.xSize(); ++i) 
    for(int j = 0; j < mBounds.ySize()-1; ++j) {
      mBounds(i,j,0) += trans(0);
      mBounds(i,j,1) += trans(1);
      mBounds(i,j,2) += trans(2);
    }
  mCenter += trans;
}

bool CMesh::adaptOFF(const char* aFilename, float lambda) {
  cout << "Read OFF... " << aFilename << endl;
  std::ifstream aStream(aFilename);
  if(aStream.is_open()) {
    char buffer[200];
    aStream.getline(buffer,200);
    cout << " 1 " << buffer << endl;
    aStream.getline(buffer,200);
    cout << " 2 " << buffer << endl;
    cout << " 3 " << mNumPoints << endl;
    for (int i = 0; i < mNumPoints; i++) {
	cout << endl;
	for(int j=0; j<3; ++j) {
	  float tmp;
	  aStream >> tmp;
	  mPoints[i][j] *= (1-lambda);
	  mPoints[i][j] += lambda*tmp;
	  cout << mPoints[i][j] << " " << endl;
	}
    }
    return true;
  } else return false;
}

// isParentOf
bool CMesh::isParentOf(int aParentJointID, int aJointID) {
  if (aJointID == 0) return false;
  if (mJoint(aJointID).mParent == aParentJointID) return true;
  return isParentOf(aParentJointID,mJoint(aJointID).mParent);
}

// operator=
void CMesh::operator=(const CMesh& aMesh) {
  
  mJointNumber = aMesh.mJointNumber;
  
  mNumPoints=aMesh.mNumPoints;
  mNumPatch=aMesh.mNumPatch;
  mNumSmooth=aMesh.mNumSmooth;
  
  mPoints = aMesh.mPoints;
  mPatch =  aMesh.mPatch;
  
  mBounds=aMesh.mBounds;
  mCenter=aMesh.mCenter;
  mJointMap=aMesh.mJointMap;
  mNeighbor=aMesh.mNeighbor;
  mEndJoint=aMesh.mEndJoint;

  mCovered=aMesh.mCovered;
  mExtremity=aMesh.mExtremity;

  mJoint = aMesh.mJoint;
  mInfluencedBy = aMesh.mInfluencedBy;
  
}

