#ifndef __PBUFFERS_H__
#define __PBUFFERS_H__

#  include <GL/glx.h>
#  include <GL/glxext.h>

#include <string>
#include <vector>

// The pixel format for the pbuffer is controlled by the mode string passed
// into the PBuffer constructor. This string can have the following attributes:
//
// r			- r pixel format (for float buffer).
// rg			- rg pixel format (for float buffer).
// rgb          - rgb pixel format. 8 bit or 16/32 bit in float buffer mode
// rgba         - same as "rgb alpha" string
// alpha        - must have alpha channel
// depth        - must have a depth buffer
// depth=n      - must have n-bit depth buffer
// stencil      - must have a stencil buffer
// double       - must support double buffered rendering
// samples=n    - must support n-sample antialiasing (n can be 2 or 4)
// float=n      - must support n-bit per channel floating point
// 
// texture2D
// textureRECT
// textureCUBE  - must support binding pbuffer as texture to specified target
//              - binding the depth buffer is also supporting by specifying
//                '=depth' like so: texture2D=depth or textureRECT=depth
//              - the internal format of the texture will be rgba by default or
//                float if pbuffer is floating point
//      
class PBuffer
{
    public:
        // see above for documentation on strMode format
        // set managed to true if you want the class to cleanup OpenGL objects in destructor
        PBuffer(const char *strMode, bool managed = false);
        ~PBuffer();

        bool Initialize(int iWidth, int iHeight, bool bShareContexts, bool bShareObjects);
        void Destroy();

        void Activate(PBuffer *current = NULL); // to switch between pbuffers, pass active pbuffer as argument
        void Deactivate();

        unsigned int GetSizeInBytes();
        unsigned int CopyToBuffer(void *ptr, int w=-1, int h=-1);

        inline int GetNumComponents()
        { return m_iNComponents; }

        inline int GetBitsPerComponent()
        { return m_iBitsPerComponent; }

        inline int GetWidth()
        { return m_iWidth; }

        inline int GetHeight()
        { return m_iHeight; }

        inline bool IsSharedContext()
        { return m_bSharedContext; }

    protected:

        Display    *m_pDisplay;
        GLXPbuffer  m_glxPbuffer;
        GLXContext  m_glxContext;

        Display    *m_pOldDisplay;
        GLXPbuffer  m_glxOldDrawable;
        GLXContext  m_glxOldContext;
        
        std::vector<int> m_pfAttribList;
        std::vector<int> m_pbAttribList;

        int m_iWidth;
        int m_iHeight;
        int m_iNComponents;
        int m_iBitsPerComponent;
    
        const char *m_strMode;
        bool m_bSharedContext;
        bool m_bShareObjects;

    private:
        std::string getStringValue(std::string token);
        int getIntegerValue(std::string token);

        void parseModeString(const char *modeString, std::vector<int> *pfAttribList, std::vector<int> *pbAttribList);

        bool m_bIsBound;
        bool m_bIsActive;
        bool m_bManaged;

};

#endif
