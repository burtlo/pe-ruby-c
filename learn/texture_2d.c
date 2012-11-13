#include "texture_2d.h"

Texture2D* Texture2D_createWithFile(char *textureFileName) {

  printf("Texture Creation For: %s",textureFileName);
  CGDataProviderRef dataProvider = CGDataProviderCreateWithFilename(textureFileName);
  CGImageRef image = CGImageCreateWithPNGDataProvider(dataProvider, NULL, 0, kCGRenderingIntentDefault);

  Texture2D *texture = malloc(sizeof(Texture2D));

  // printf("Image Data: %s",image);

  CGImageAlphaInfo info = CGImageGetAlphaInfo(image);
  BOOL hasAlpha = ((info == kCGImageAlphaPremultipliedLast) ||
              (info == kCGImageAlphaPremultipliedFirst) ||
              (info == kCGImageAlphaLast) ||
              (info == kCGImageAlphaFirst) ? 1 : 0);


  if(CGImageGetColorSpace(image)) {
    if(hasAlpha) {
      texture->pixelFormat = kTexture2DPixelFormat_RGBA8888;
    } else {
      texture->pixelFormat = kTexture2DPixelFormat_RGB565;
    }
  } else  {//NOTE: No colorspace means a mask image
    texture->pixelFormat = kTexture2DPixelFormat_A8;
  }

  texture->contentSize = CGSizeMake(CGImageGetWidth(image), CGImageGetHeight(image));

  printf("%f,%f",texture->contentSize.width,texture->contentSize.height);

  GLuint pot = 0;

  texture->width = texture->contentSize.width;

  if((texture->width != 1) && (texture->width & (texture->width - 1))) {
      pot = 1;
      while( pot < texture->width)
          pot *= 2;
      texture->width = pot;
  }

  texture->height = texture->contentSize.height;
  if((texture->height != 1) && (texture->height & (texture->height - 1))) {
      pot = 1;
      while(pot < texture->height)
          pot *= 2;
      texture->height = pot;
  }

  CGAffineTransform transform = CGAffineTransformIdentity;

  // Now that we have created a width and height which is power of 2 and will contain our image
  // we need to make sure that the texture is now not bigger than 1024 x 1024 which is the largest
  // single texture size the iPhone can handle.  If it is too big then the image is scaled down by
  // 50%
  // while((width > kMaxTextureSize) || (height > kMaxTextureSize)) {
  //     width /= 2;
  //     height /= 2;
  //     transform = CGAffineTransformScale(transform, 0.5, 0.5);
  //     contentSize.width *= 0.5;
  //     contentSize.height *= 0.5;
  // }

  CGColorSpaceRef colorSpace;
  CGContextRef context = NULL;
  GLvoid* data = NULL;

  printf("%i %i",texture->width,texture->height);

  switch(texture->pixelFormat) {
    case kTexture2DPixelFormat_RGBA8888:
      colorSpace = CGColorSpaceCreateDeviceRGB();
      data = malloc(texture->height * texture->width * 4);
      printf("kTexture2DPixelFormat_RGBA8888");

      context = CGBitmapContextCreate(data, texture->width, texture->height, 8, 4 * texture->width, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
      CGColorSpaceRelease(colorSpace);
      break;

    case kTexture2DPixelFormat_RGB565:
      printf("kTexture2DPixelFormat_RGB565");

      colorSpace = CGColorSpaceCreateDeviceRGB();
      data = malloc(texture->height * texture->width * 4);
      context = CGBitmapContextCreate(data, texture->width, texture->height, 8, 4 * texture->width, colorSpace, kCGImageAlphaNoneSkipLast | kCGBitmapByteOrder32Big);
      CGColorSpaceRelease(colorSpace);
      break;

    case kTexture2DPixelFormat_A8:
      printf("kTexture2DPixelFormat_A8");

      data = malloc(texture->height * texture->width);

      context = CGBitmapContextCreate(data, texture->width, texture->height, 8, texture->width, NULL, kCGImageAlphaOnly);
      break;
    }

    CGContextClearRect(context, CGRectMake(0, 0, texture->width, texture->height));
    CGContextTranslateCTM(context, 0, texture->height - texture->contentSize.height);

    if(!CGAffineTransformIsIdentity(transform))
        CGContextConcatCTM(context, transform);

    CGContextDrawImage(context, CGRectMake(0, 0, CGImageGetWidth(image), CGImageGetHeight(image)), image);

    if(texture->pixelFormat == kTexture2DPixelFormat_RGB565) {
      void* tempData = malloc(texture->height * texture->width * 2);
      unsigned int *inPixel32 = (unsigned int*)data;
      unsigned short *outPixel16 = (unsigned short*)tempData;
      for(int i = 0; i < texture->width * texture->height; ++i, ++inPixel32)
          *outPixel16++ = ((((*inPixel32 >> 0) & 0xFF) >> 3) << 11) | ((((*inPixel32 >> 8) & 0xFF) >> 2) << 5) |  ((((*inPixel32 >> 16) & 0xFF) >> 3) << 0);
      free(data);
      data = tempData;
    }

    glGenTextures(1, &texture->name);

    glBindTexture(GL_TEXTURE_2D, texture->name);

    GLenum aFilter = GL_LINEAR;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, aFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, aFilter);

    printf("\nTexture (Width,Height) (%d,%d)",texture->width,texture->height);

    switch(texture->pixelFormat) {
      case kTexture2DPixelFormat_RGBA8888:
        printf("\nkTexture2DPixelFormat_RGBA8888");
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        break;
      case kTexture2DPixelFormat_RGB565:
        printf("\nkTexture2DPixelFormat_RGB565");
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data);
        break;
      case kTexture2DPixelFormat_A8:
        printf("\nkTexture2DPixelFormat_A8");
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, texture->width, texture->height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
        break;
      default:
        printf("\nCould not generate the texture");
    }

    texture->maxS = texture->contentSize.width / (float)texture->width;
    texture->maxT = texture->contentSize.height / (float)texture->height;

    printf("The MaxS,MaxT Size is (%f,%f)",texture->maxS,texture->maxT);

    texture->textureRatio.width = 1.0f / (float)texture->width;
    texture->textureRatio.height = 1.0f / (float)texture->height;

    CGContextRelease(context);
    free(data);

    return texture;

}