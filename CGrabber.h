// Author: Juergen Gall 

#ifndef CGrabberH
#define CGrabberH

#include <string>
#include <CTensor.h>

class CGrabber {
 public:
  CGrabber(std::string s);
  bool nextFrame(int inc = 1);
  std::vector<CTensor<float> > Frame;
  std::vector<CMatrix<float> > FrameSilh;
  std::vector<std::string> FrameFeature;
  int getFrame() {return framecounter;};
 private:
  CMatrix<std::string> Image;
  CMatrix<std::string> Silhouette;
  CMatrix<std::string> ImFeature;
  int framecounter;
};

#endif
