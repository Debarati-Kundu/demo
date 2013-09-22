//#define WINDOWS

#include "Show.h"
#ifdef WINDOWS
#include "Main.h"
#endif


namespace NShow {

  int mViewNo = 0;
  int mImageNo = 0;
  std::vector<CTensor<float> >* mInputImage = NULL;
  std::string mResultDir = "";
  std::string mInputDir = "";
  std::string mFileName = "";
  std::ostringstream mOut;
  std::ofstream mLogFile;
  CMatrix<std::string> mInput;

  bool mForceContourColor = false;
  int mContourColorR = 255;
  int mContourColorG = 255;
  int mContourColorB = 255;
  int mMeshColorR = 255;
  int mMeshColorG = 255;
  int mMeshColorB = 255;
  int mMeshPointColorR = 128;
  int mMeshPointColorG = 128;
  int mMeshPointColorB = 128;


  #ifdef WINDOWS

  // idle
  void idle() {
    Application->ProcessMessages();
  }

  // message
  void message(bool aClear) {
    mOut << '\0';
    MainWindow->mStatusBar->SimpleText = NShow::mOut.str().c_str();
    mLogFile << NShow::mOut.str().c_str();
    if (aClear) {
      mOut.seekp(0);
      mLogFile << std::endl;
    }
    else mOut.seekp(-1,ios_base::cur);
    Application->ProcessMessages();
  }

  // show
  void show(int aNo, CMatrix<float>& aImage, const char* aText) {
    if (MainWindow->mPopup[aNo] == 0) Application->CreateForm(__classid(TPopupForm), &MainWindow->mPopup[aNo]);
    MainWindow->mPopup[aNo]->showImage(aImage,aText);
    Application->ProcessMessages();
  }

  void show(int aNo, CTensor<float>& aImage, const char* aText) {
    if (MainWindow->mPopup[aNo] == 0) Application->CreateForm(__classid(TPopupForm), &MainWindow->mPopup[aNo]);
    MainWindow->mPopup[aNo]->showImage(aImage,aText);
    Application->ProcessMessages();
  }

  #else

  // message
  void message(bool aClear) {
    if (aClear) std::cout << std::endl;
    else std::cout << std::flush; 
  }

  // show
  void show(int aNo, CMatrix<float>& aImage, const char* aText) {
    char buffer[200];
    sprintf(buffer,"%s%d.png",aText,aNo);
#if SIGCALIB
    aImage.writeToPNGflip(buffer);
#else
    aImage.writeToPNG(buffer);
#endif
  }

  void show(int aNo, CTensor<float>& aImage, const char* aText) {
    char buffer[200];
    sprintf(buffer,"%s%d.png",aText,aNo);
#if SIGCALIB
    aImage.writeToPNGflip(buffer);
#else
    aImage.writeToPNG(buffer);
#endif
  }

  void idle() {
  }

  #endif

  // writeToFile
  void writeToFile(CMatrix<float>& aImage, const char* aFileName, const char* type) {
    char buffer[200];
    sprintf(buffer,"%s%04dView%d.%s",(mResultDir+aFileName).c_str(),mImageNo,mViewNo,type);
    //std::cout << buffer << std::endl;
    if(type == "PNG" || type == "png")
#if SIGCALIB
      aImage.writeToPNGflip(buffer);
#else
      aImage.writeToPNG(buffer);
#endif
    else if(type == "PGM" || type == "pgm")
      aImage.writeToPGM(buffer);
    else
      std::cerr << "Unknown Filetype: " << type << std::endl; 
  }
  
  void writeToFile(CTensor<float>& aImage, const char* aFileName, const char* type) {
    char buffer[200];
    sprintf(buffer,"%s%04dView%d.%s",(mResultDir+aFileName).c_str(),mImageNo,mViewNo,type);
    //std::cout << buffer << std::endl;
    if(type == "PNG" || type == "png")
#if SIGCALIB
      aImage.writeToPNGflip(buffer);
#else
      aImage.writeToPNG(buffer);
#endif
    else if(type == "PPM" || type == "ppm")
      aImage.writeToPPM(buffer);
    else
      std::cerr << "Unknown Filetype: " << type << std::endl; 
  }

  void writeToFile(CMatrix<float>& aImage, const char* aFileName, int i, const char* type) {
    char buffer[200];
    sprintf(buffer,"%s%04d%03dView%d.%s",(mResultDir+aFileName).c_str(),mImageNo,i,mViewNo,type);
    std::cout << buffer << std::endl;
    if(type == "PNG" || type == "png")
#if SIGCALIB
      aImage.writeToPNGflip(buffer);
#else
      aImage.writeToPNG(buffer);
#endif
    else if(type == "PGM" || type == "pgm")
      aImage.writeToPGM(buffer);
    else
      std::cerr << "Unknown Filetype: " << type << std::endl; 
  }
  
  void writeToFile(CTensor<float>& aImage, const char* aFileName, int i, const char* type) {
    char buffer[200];
    sprintf(buffer,"%s%04d%03dView%d.%s",(mResultDir+aFileName).c_str(),mImageNo,i,mViewNo,type);
    std::cout << buffer << std::endl;
    if(type == "PNG" || type == "png")
#if SIGCALIB
      aImage.writeToPNGflip(buffer);
#else
      aImage.writeToPNG(buffer);
#endif
    else if(type == "PPM" || type == "ppm")
      aImage.writeToPPM(buffer);
    else
      std::cerr << "Unknown Filetype: " << type << std::endl; 
  }

  void writeToFile(std::vector<CMatrix<float> >& aImage, const char* aFileName, const char* type) {
    for(mViewNo = 0; mViewNo<aImage.size(); ++mViewNo)
      writeToFile(aImage[mViewNo], aFileName, type);
  }

  void writeToFile(std::vector<CTensor<float> >& aImage, const char* aFileName, const char* type) {
    for(mViewNo = 0; mViewNo<aImage.size(); ++mViewNo)
      writeToFile(aImage[mViewNo], aFileName, type);
  }

  void writeToFile(std::vector<CMatrix<float> >& aImage, const char* aFileName, int i, const char* type) {
    for(mViewNo = 0; mViewNo<aImage.size(); ++mViewNo)
      writeToFile(aImage[mViewNo], aFileName, i, type);
    }
  
  void writeToFile(std::vector<CTensor<float> >& aImage, const char* aFileName, int i, const char* type) {
    for(mViewNo = 0; mViewNo<aImage.size(); ++mViewNo)
      writeToFile(aImage[mViewNo], aFileName, i, type);
  }

  
}


// operator <<
std::ostream& operator<<(std::ostream& aStream, const std::vector<double>& vec) {
  for(std::vector<double>::const_iterator it = vec.begin(); it!=vec.end(); ++it)
    aStream << (*it) << ' ';
  aStream << std::endl;
  return aStream;
}

// operator <<
std::ostream& operator<<(std::ostream& aStream, const std::vector<float>& vec) {
  for(std::vector<float>::const_iterator it = vec.begin(); it!=vec.end(); ++it)
    aStream << (*it) << ' ';
  aStream << std::endl;
  return aStream;
}

// operator <<
std::ostream& operator<<(std::ostream& aStream, const std::vector<int>& vec) {
  for(std::vector<int>::const_iterator it = vec.begin(); it!=vec.end(); ++it)
    aStream << (*it) << ' ';
  aStream << std::endl;
  return aStream;
}

std::ostream& operator<<(std::ostream& aStream, const std::vector<bool>& vec) {
  for(std::vector<bool>::const_iterator it = vec.begin(); it!=vec.end(); ++it)
    aStream << (*it) << ' ';
  aStream << std::endl;
  return aStream;
}

std::ostream& operator<<(std::ostream& aStream, const std::vector<GLubyte>& vec) {
  for(std::vector<GLubyte>::const_iterator it = vec.begin(); it!=vec.end(); ++it)
    aStream << int(*it) << ' ';
  aStream << std::endl;
  return aStream;
}
