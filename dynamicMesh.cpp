// Author: Juergen Gall 

/* 
   Usage: ./ShowMesh <srcpath> <sequence.bmf> <input> <outputpath> <view>
   <srcpath> - path to calibration data  
   <sequence.bmf> - seq.bmf (seq.bmf needs to specify absolute path to images)
   <input> - path to meshes + prefix, 
             PATH/output reads PATH/output0001.off, PATH/output0002.off, ...  
   <outputpath> - path for storing projections 
   <view> - specify a camera view (0-7) or generate all views (-1)
*/


#include <vector>

#include "CMatrix.h"
#include "Show.h"

#include "CGrabber.h"
#include "CTMeshLight.h"

#include "CModelLight.h"

#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glx.h>


using namespace std;


/***********************************************/
/*                TYPE OF MODEL                */

#define SMOOTHMODEL true
/***********************************************/


// ************ Data for Results ************* // 
CMesh Mesh;
int showview = -1;
string inputpath;

// Set up directories for in- and output
void parseArg(int argc, char** argv) {

  if (argc < 6) { 
    cout << "Usage: ShowMesh <srcpath> <sequence.bmf> <input> <outputpath> <view>" << endl;
    exit(-1);
  }

  string s = argv[1];
  if( (char)(*--s.end()) != '/' )
    s.append("/");
  NShow::mInputDir = s;

  // Determine image/sequence name
  inputpath = argv[3];
  cout << inputpath << endl;

  // Directory for output files
  s = argv[4];
  NShow::mResultDir = s +"/";
  cout << NShow::mResultDir << endl;

  NShow::mResultDir = "mkdir " + s + "Results";
  system(NShow::mResultDir.c_str());
  NShow::mResultDir = s +"Results/";
  cout << NShow::mResultDir << endl;

  s = argv[5];
  istringstream(s) >> showview;
  cout << "View: " << showview << endl;
  
}

// Init GLUT for GPU
void initGLUT() {
  // init GLUT
  int dummy = 0;
  glutInit (&dummy, NULL);

  // Generate a hidden dummy window
  glutInitWindowSize (1, 1); 
  glutCreateWindow (""); 
  glutHideWindow();

}

/////////////////////////////////////////////////////////////////////
// Main                                                            //
/////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
  
  // ************** Read Input ************* //

  /*************** For some dummy testing to read off data */
//  cout << "Calling from main " << Mesh.readModel("../skelsurf_data/dance_mesh/output0001.off", 1) << endl;
  //cout << "Calling from main " << Mesh.adaptOFF("../skelsurf_data/dance_mesh/output0001.off", 1) << endl;
  /*******************************/

  cout << endl << "*** ParseArg ***" << endl;
  parseArg(argc, argv);
  CGrabber Grab( NShow::mInputDir+argv[2]);

  // ************** Init GLUT ************** //
  cout << endl << "*** InitGlut ***" << endl;
  initGLUT();
  CModel model(Grab.Frame[0].xSize(), Grab.Frame[0].ySize(), Grab.Frame.size());
  
  // ************** Load Mesh ************** //
  cout << endl << "*** InitMesh ***" << endl;
  if( !Mesh.readModel( (NShow::mInputDir + "model.dat").c_str(), SMOOTHMODEL) ) exit(-1);
  Mesh.centerModel();

  // *************************************** //
  //                                         //
  //              TRACKING                   //
  //                                         //
  // *************************************** //

  while(Grab.nextFrame()) {
    
    cout << endl << "*** Get Mesh ***" << endl;    
    char buffer[200];
    sprintf(buffer,"%s%04d.off",inputpath.c_str(),NShow::mImageNo);
    Mesh.adaptOFF(buffer, 1);
    
    cout << endl << "*** Write Image ***" << endl;
    vector<CTensor<float> > aShow = *NShow::mInputImage;
    model.generateMesh(&Mesh,aShow,255,255,80,true);
    if(showview==-1)
      NShow::writeToFile(aShow,"Pose");
    else {
      NShow::mViewNo = showview;
      NShow::writeToFile(aShow[showview],"Pose");
    }
   

  }

  return 0;
}

