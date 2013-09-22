// Author: Juergen Gall 

#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glx.h>

#define REQUIRED_EXTENSIONS "GLX_SGIX_pbuffer " \
                            "GLX_SGIX_fbconfig "

// Convenience stuff for pbuffer object.
#define GLH_EXT_SINGLE_FILE
#include <glh/glh_glut.h>
#include <glh/glh_extensions.h>
#include <glh/glh_obs.h>

#include <CModelLight.h>
#include <Show.h>


CModel::CModel(int aWidth, int aHeight, int aViews) : mWidth(aWidth), mHeight(aHeight), mViews(aViews) {

  resetColor();
  
  mPBufferTex = new PBuffer("rgba texture2D depth");

  // Read Geocast
  mCamera = vector<GeoCastData>(aViews);
  for (int i = 0; i < aViews; i++) {
    char buffer[100];
    sprintf(buffer,"%sproj%02d.dat",NShow::mInputDir.c_str(),i); 
    cout << buffer << endl;
    initCamera(buffer, i);
  }

  initPBuffer();

  cout << "...MODELmodule loaded" << endl << endl;
}

CModel::~CModel() {
  delete mPBufferTex;
}

// initCamera
void CModel::initCamera(const char* file, int view) {
  GeoCastData* cam = &mCamera[view];
  FILE *infile = fopen(file, "r");
  if (infile == NULL)
    {
      fprintf(stderr, "Can't open '%s'! Exiting...\n", file);
      exit(-1);
    }
  cout << file << endl;
     
  // not used entries in geocast format
  char dummy[255];

  fscanf(infile, "P:\n");
  fscanf(infile, "%f %f %f %f\n", dummy, dummy, dummy, dummy);
  fscanf(infile, "%f %f %f %f\n", dummy, dummy, dummy, dummy);
  fscanf(infile, "%f %f %f %f\n", dummy, dummy, dummy, dummy);

  cam->Projection = CMatrix<float>(4,3,0);
  fscanf(infile, "\nK:\n");
  fscanf(infile, "%f %f %f\n", &cam->Projection.data()[0], &cam->Projection.data()[1], &cam->Projection.data()[2]);
  fscanf(infile, "%f %f %f\n", &cam->Projection.data()[4], &cam->Projection.data()[5], &cam->Projection.data()[6]);
  fscanf(infile, "%f %f %f\n", &cam->Projection.data()[8], &cam->Projection.data()[9], &cam->Projection.data()[10]);

  fscanf(infile, "\nM:\n");
  fscanf(infile, "%f %f %f %f\n", &cam->MVmatrixInverse[0], &cam->MVmatrixInverse[4], &cam->MVmatrixInverse[8], &cam->MVmatrixInverse[12]);
  fscanf(infile, "%f %f %f %f\n", &cam->MVmatrixInverse[1], &cam->MVmatrixInverse[5], &cam->MVmatrixInverse[9], &cam->MVmatrixInverse[13]);
  fscanf(infile, "%f %f %f %f\n", &cam->MVmatrixInverse[2], &cam->MVmatrixInverse[6], &cam->MVmatrixInverse[10], &cam->MVmatrixInverse[14]);
  fscanf(infile, "%f %f %f %f\n", &cam->MVmatrixInverse[3], &cam->MVmatrixInverse[7], &cam->MVmatrixInverse[11], &cam->MVmatrixInverse[15]);
  
  fclose(infile);

  // -z
  for(int i = 0; i<4; ++i)
    cam->MVmatrixInverse[i*4+2] *= -1; 

  float f = cam->Projection(1,1)*2.0/(float)mHeight;
  cam->Aspect = f/cam->Projection(0,0)*(float)mWidth/2.0;
  cam->ClipNear = 1000.0;
  float Cx = cam->Projection(2,0); 
  float Cy = cam->Projection(2,1);

  cam->Projection = CMatrix<float>(4,3,0);
  cam->Projection(0,0) = f/cam->Aspect*(float)mWidth/2.0;
  cam->Projection(1,1) = f*(float)mHeight/2.0;
  cam->Projection(2,0) = Cx;
  cam->Projection(2,1) = Cy;
  cam->Projection(2,2) = 1.0;
  
  cam->left = cam->Aspect*cam->ClipNear/f*2.0*(-Cx/(float)mWidth);
  cam->right = cam->Aspect*cam->ClipNear/f*2.0*(1.0-Cx/(float)mWidth);
  cam->top = cam->ClipNear/f*2.0*(1.0-Cy/(float)mHeight);
  cam->bottom = cam->ClipNear/f*2.0*(-Cy/(float)mHeight);
  cam->near = cam->ClipNear;
  cam->far = 10000.0;
  cout << "Frustum: " << cam->left << " " << cam->right << " " << cam->bottom << " " << cam->top << endl; 

  CMatrix<float> modelM(4,4);
  for(int i = 0; i<4; ++i)
    for(int j = 0; j<4; ++j) {
      if(j==2) 
	modelM.data()[i + j*4] = -cam->MVmatrixInverse[j + i*4];
      else
	modelM.data()[i + j*4] = cam->MVmatrixInverse[j + i*4];
    }
 
  cout << cam->Projection;
  cam->Projection *= modelM;
  cout << "Projection matrix for view " << view << ":\n" << cam->Projection << endl;
  cout << modelM;
  
}

// initPBuffer
void CModel::initPBuffer() {
  cout << "Init PBuffer... " << endl;

  // Get the entry points for the extension.
  if( !glh_init_extensions(REQUIRED_EXTENSIONS) ) {
    cerr << "Necessary extensions were not supported:" << endl
	 << glh_get_unsupported_extensions() << endl;
    exit( -1 );
  }
  
  // Initialize the PBuffer now that we have a valid context
  // that we can use during the p-buffer creation process.
  mPBufferTex->Initialize(mWidth, mHeight, false, false);

  // Initialize some graphics state for the PBuffer's rendering context.
  mPBufferTex->Activate();

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glShadeModel(GL_FLAT);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glPolygonOffset(1.0, 1.0); 
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);

  mPBufferTex->Deactivate();

  cout << "ok" << endl;
}

//drawPBufferBW
void CModel::drawPBufferBW(CMesh* aMesh, drawmode aMode, vector<CTensor<float> >& aImage, GLubyte aR, GLubyte aG, GLubyte aB, bool aOverlay) const {
  // Activate PBuffer
  mPBufferTex->Activate();

  // Clear color
  glClearColor( 0.0, 0.0, 0.0, 0.0 );
  if(aMode == POINTS || aMode == MESH)
    glEnable(GL_POLYGON_OFFSET_FILL);     

  // Reserve Memory
  GLubyte glImg[mWidth*mHeight*3];

  for(int i=0; i<mViews; ++i) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const GeoCastData* cam = &mCamera[i];

    // set up OpenGL projection from GeoCast
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(cam->left, cam->right, cam->bottom, cam->top, cam->near, cam->far);

    glMatrixMode ( GL_MODELVIEW ); 
    glLoadIdentity ( );
    glMultMatrixf(cam->MVmatrixInverse);

    // Simulate depth test for polygon modes LINES and POINTS 
    if(aMode == POINTS || aMode == MESH) {
      draw_object_tex(aMesh, SILHOUETTE, 0, 0, 0);
    }

    if(aOverlay) {
      glDisable(GL_DEPTH_TEST);
      glDepthMask(GL_FALSE);

      // draw image into buffer
      drawGlImage(aImage[i], glImg);
      
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluOrtho2D(0.0,(GLdouble) mWidth, 0.0, (GLdouble) mHeight); 
      
      glMatrixMode ( GL_MODELVIEW ); 
      glLoadIdentity ( );

      glRasterPos2i(0, 0);
      glDrawPixels( mWidth, mHeight, GL_RGB, GL_UNSIGNED_BYTE, glImg );
      
      // reset settings
      glDepthMask(GL_TRUE);
      glEnable(GL_DEPTH_TEST);

      // set up OpenGL projection from GeoCast
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glFrustum(cam->left, cam->right, cam->bottom, cam->top, cam->near, cam->far);

      glMatrixMode ( GL_MODELVIEW ); 
      glLoadIdentity ( );
      glMultMatrixf(cam->MVmatrixInverse);
    }
    
    // draw object
    if(aMode == POINTS)
      draw_object_points(aMesh, aMode, aR, aG, aB);
    else
      draw_object_tex(aMesh, aMode, aR, aG, aB);

    // Read the data from the PBuffer.
    glReadPixels( 0, 0, mWidth, mHeight, GL_RGB, GL_UNSIGNED_BYTE, glImg );

    readGlImage(glImg, aImage[i]);

   }

  if(aMode == POINTS || aMode == MESH) 
    glDisable(GL_POLYGON_OFFSET_FILL);     

  mPBufferTex->Deactivate();

}


/*******************************************************************
 * Drawing
 ******************************************************************/

// draw_object_tex
void CModel::draw_object_tex(CMesh* aMesh, drawmode aMode, GLubyte aR, GLubyte aG, GLubyte aB, GLubyte aA, vector<GLfloat>* aGray) const {

  float a[3],b[3],c[3];
  int vIndex[3];
  int texIndex = -1;

  GLuint cIndex = 0;
  GLubyte R, G, B, A;
  GLuint size = aMesh->GetMeshSize();

  // settings according to selected mode
  switch(aMode) {
  case TEXTURE:
    glEnable(GL_TEXTURE_2D);
    break;
  case SILHOUETTE: 
    glColor4ub(aR,aG,aB, aA);
    break;
  case MESH: 
    glColor4ub(aR,aG,aB, aA);
    glPolygonMode(GL_FRONT, GL_LINE);
    break;
  case POINTS: 
    glColor4ub(aR,aG,aB, aA);
    glPolygonMode(GL_FRONT, GL_POINT);
    break;
  case COLORINDEXPOINTS:
    glShadeModel(GL_SMOOTH);
    glPolygonMode(GL_FRONT, GL_POINT);
    break;
  }
  
  for(int i=0;i<size;++i) {	
    aMesh->GetPatch(i,vIndex[0],vIndex[1],vIndex[2]);
    aMesh->GetPoint(vIndex[0],a[0],a[1],a[2]);
    aMesh->GetPoint(vIndex[1],b[0],b[1],b[2]);
    aMesh->GetPoint(vIndex[2],c[0],c[1],c[2]);
    
    switch(aMode) {
    case TEXTURE:
      if((int)mTexCoord(i,0) != texIndex) {
	texIndex = (int)mTexCoord(i,0);
	glBindTexture(GL_TEXTURE_2D, tex_id[texIndex]);
      }
      break;
    case COLORINDEX:
      ++cIndex;
      A = cIndex & 0xff; 
      B = (cIndex >> 8) & 0xff; 
      G = (cIndex >> 16) & 0xff; 
      R = (cIndex >> 24) & 0xff;
      glColor4ub(R, G, B, A);
      break;
    case COLORCOMPONENTS:
      glColor4ub(aMesh->GetBodyPart( aMesh->GetJointID(vIndex[0]) ) + 1, 0, 0, 0);
      break;
    case SETCOLORCOMPONENTS:
      glColor4f((*aGray)[aMesh->GetBodyPart( aMesh->GetJointID(vIndex[0]) ) + 1], 0, 0, 0);
      break;
    }
   
	
    glBegin (GL_TRIANGLES);
	
    if(aMode==COLORINDEXPOINTS) {
      cIndex = vIndex[0]+1;
      A = cIndex & 0xff; 
      B = (cIndex >> 8) & 0xff; 
      G = (cIndex >> 16) & 0xff; 
      R = (cIndex >> 24) & 0xff;
      glColor4ub(R, G, B, A);
    } else if(aMode == TEXTURE)
      glTexCoord2f( mTexCoord(i,1), mTexCoord(i,2) ); 
   

    glVertex3f (a[0],a[1],a[2]);

    if(aMode==COLORINDEXPOINTS) {
      cIndex = vIndex[1]+1;
      A = cIndex & 0xff; 
      B = (cIndex >> 8) & 0xff; 
      G = (cIndex >> 16) & 0xff; 
      R = (cIndex >> 24) & 0xff;
      glColor4ub(R, G, B, A);
    } else if(aMode == TEXTURE)
      glTexCoord2f( mTexCoord(i,3), mTexCoord(i,4) ); 
   

    glVertex3f (b[0],b[1],b[2]);

    if(aMode==COLORINDEXPOINTS) {
      cIndex = vIndex[2]+1;
      A = cIndex & 0xff; 
      B = (cIndex >> 8) & 0xff; 
      G = (cIndex >> 16) & 0xff; 
      R = (cIndex >> 24) & 0xff;
      glColor4ub(R, G, B, A);
    } else if(aMode == TEXTURE)
      glTexCoord2f( mTexCoord(i,5), mTexCoord(i,6) );
    

    glVertex3f (c[0],c[1],c[2]);

    glEnd();
   
	
  } // end for: i
    
  // reset settings
  switch(aMode) {
  case TEXTURE:
    glDisable(GL_TEXTURE_2D);
    break;
  case MESH: 
    glPolygonMode(GL_FRONT, GL_FILL);
    break;
  case POINTS: 
    glPolygonMode(GL_FRONT, GL_FILL);
    break;
  case COLORINDEXPOINTS:
    glShadeModel(GL_FLAT);
    glPolygonMode(GL_FRONT, GL_FILL);
    break;
  }

}

// draw_object_tex
void CModel::draw_object_points(CMesh* aMesh, drawmode aMode, GLubyte aR, GLubyte aG, GLubyte aB, GLubyte aA) const {

  float a[3];
  GLuint cIndex = 0;
  GLubyte R, G, B, A;

  // settings according to selected mode
  if(aMode == POINTS) 
    glColor4ub(aR,aG,aB, aA);

  glBegin (GL_POINTS);

  for(int i=0;i<aMesh->GetPointSize();++i) {	
    aMesh->GetPoint(i,a[0],a[1],a[2]);
    
    if(aMode==COLORINDEXPOINTS) {
      cIndex = i+1;
      A = cIndex & 0xff; 
      B = (cIndex >> 8) & 0xff; 
      G = (cIndex >> 16) & 0xff; 
      R = (cIndex >> 24) & 0xff;
      glColor4ub(R, G, B, A);
    }    

    glVertex3f (a[0],a[1],a[2]);
	
  } // end for: i

  glEnd();
}


/*******************************************************************
 * Conversion
 ******************************************************************/

void CModel::readGlImage(GLubyte* aGlImage, CTensor<float>& aImg) const {

  GLubyte* p;
  p = aGlImage;
  // reverse order
  aImg(0,0,0) = *p;
  aImg(0,0,1) = *(++p);
  aImg(0,0,2) = *(++p);
  for (int j = 1; j < aImg.xSize(); j++) {  
    aImg(j,0,0) = *(++p);
    aImg(j,0,1) = *(++p);
    aImg(j,0,2) = *(++p);
  }
  for (int i = 1; i < aImg.ySize(); i++) {
    for (int j = 0; j < aImg.xSize(); j++) {  
      aImg(j,i,0) = *(++p);
      aImg(j,i,1) = *(++p);
      aImg(j,i,2) = *(++p);
    }
  }
    
}

void CModel::readGlImage(GLubyte* aGlImage, CMatrix<float>& aImg) const {
  for(int i=0; i<aImg.size(); ++i)
    aImg.data()[i] = aGlImage[i];
}

void CModel::readGlImage(GLuint* aGlImage, CMatrix<GLuint>& aImage) const {
  memcpy(aImage.data(),aGlImage,mHeight*mWidth*sizeof(GLuint));
}

void CModel::drawGlImage(CTensor<float>& aImage, GLubyte* aGlImage) const {
  GLubyte* p;
  p = aGlImage;
  *p = (GLubyte) aImage(0,0,0);
  *(++p) = (GLubyte) aImage(0,0,1);
  *(++p) = (GLubyte) aImage(0,0,2);
  for (int j = 1; j < aImage.xSize(); j++) {    
    *(++p) = (GLubyte) aImage(j,0,0);
    *(++p) = (GLubyte) aImage(j,0,1);
    *(++p) = (GLubyte) aImage(j,0,2);
  }
  for (int i = 1; i < aImage.ySize(); i++) {
    for (int j = 0; j < aImage.xSize(); j++) {    
      *(++p) = (GLubyte) aImage(j,i,0);
      *(++p) = (GLubyte) aImage(j,i,1);
      *(++p) = (GLubyte) aImage(j,i,2);
    }
  } 
}
