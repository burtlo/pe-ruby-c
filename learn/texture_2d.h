#import "global.h"
#include "Glee.h"

typedef enum {
  kTexture2DPixelFormat_Automatic = 0,
  kTexture2DPixelFormat_RGBA8888,
  kTexture2DPixelFormat_RGB565,
  kTexture2DPixelFormat_A8,
} Texture2DPixelFormat;

typedef struct texture_2d {

  GLuint name;   // Holds the OpenGL name generated for this texture
  CGSize contentSize;  // Stores the actual size of the image being loaded as a texture
  GLuint width;  // The actual width and height of the texture once it has been adjusted to be
  GLuint height;  // power of 2 compliant
  GLfloat maxS;   // Maximum texture coordinates for the image that has been placed inside our texture.
  GLfloat maxT;
  CGSize textureRatio; // The ratio between pixels and texels (texture coordinates)
  Texture2DPixelFormat pixelFormat; // The pixel format of the image that has been loaded

} Texture2D;

Texture2D* Texture2D_createWithFile(char *textureFileName);
