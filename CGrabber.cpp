// Author: Juergen Gall 

#include "CGrabber.h"
#include <Show.h>
using namespace std;

#define PNGPATH "../skelsurf_data/dog_im/"

CGrabber::CGrabber(string s) {
  cout << s << endl;

  int aImageCount, viewCount;
  std::ifstream aStream(s.c_str());
  if(aStream.is_open()) {

    aStream >> aImageCount;
    aStream >> viewCount;
    cout << viewCount << " " << aImageCount << endl;
    Frame.resize(viewCount);
    Image.setSize(viewCount,aImageCount);

    for (int i = 0; i < viewCount*aImageCount; i++) {
      aStream >> Image.data()[i];
    }

  } else {

    cerr << "Error reading file: " << s << "\n";
    exit(-1);

  }
 
  framecounter = 0; 
  for (int v = 0; v<Frame.size(); ++v) {
	string a = PNGPATH +  Image(v,framecounter);
	Frame[v].readFromPNG(a.c_str());
//    Frame[v].readFromPNG(Image(v,framecounter).c_str());
  } 

  NShow::mInputImage = &Frame;
  NShow::mImageNo = 0;
  NShow::mViewNo = 0;
}

bool CGrabber::nextFrame(int inc) {
  bool fileExist = false;
  
  framecounter += inc;
  cout << endl << "****************************" << endl << "Frame: " << framecounter << " | " << Image.ySize() << endl; 

  if(framecounter < Image.ySize()) {

    for (int v = 0; v<Frame.size(); ++v) {
	string a = PNGPATH +  Image(v,framecounter);
	Frame[v].readFromPNG(a.c_str());
//      Frame[v].readFromPNG(Image(v,framecounter).c_str());
    } 

    ++NShow::mImageNo;
    fileExist = true;
  }

  return fileExist;
}
