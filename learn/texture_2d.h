#import "global.h"

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

void Init_Texture2D(VALUE module);
Texture2D Texture2D_createWithBlob(char *textureBlob);
Texture2D Texture2D_createWithFile(char *textureFileName);


VALUE Texture2D_initialize(VALUE self, VALUE image);

VALUE Texture2D_get_name(VALUE self);
VALUE Texture2D_get_contentSize(VALUE self);
VALUE Texture2D_get_width(VALUE self);
VALUE Texture2D_get_height(VALUE self);
VALUE Texture2D_get_maxS(VALUE self);
VALUE Texture2D_get_maxT(VALUE self);
VALUE Texture2D_get_textureRatio(VALUE self);
VALUE Texture2D_get_pixelFormat(VALUE self);


#define TEXTURE2D() \
    Texture2D* texture2D; \
    Data_Get_Struct(self, Texture2D, texture2D);


#define GET_TEXTURE2D_DATA(ATTRIBUTE_NAME, ATTRIBUTE, CAST) \
    VALUE Texture2D_get_##ATTRIBUTE_NAME(VALUE self) \
    { \
      TEXTURE2D(); \
      return CAST(texture2D->ATTRIBUTE); \
    }


