#include "texture_2d.h"
#include "Glee.h"

// #pragma mark - Prototypes

static VALUE texture2d_allocate(VALUE klass);
static void texture2D_mark(Texture2D* texture2D);
static void texture2D_free(Texture2D* texture2D);

// #pragma mark - Initialization

void Init_Texture2D(VALUE module) {

  VALUE cTexture2D = rb_define_class_under(module,"Texture2D",rb_cObject);

  rb_define_alloc_func(cTexture2D, texture2d_allocate);
  rb_define_method(cTexture2D, "initialize", Texture2D_initialize, 1);
  rb_define_method(cTexture2D, "name", Texture2D_get_name, 0);
  rb_define_method(cTexture2D, "size", Texture2D_get_contentSize, 0);
  rb_define_method(cTexture2D, "width", Texture2D_get_width, 0);
  rb_define_method(cTexture2D, "height", Texture2D_get_height, 0);
  rb_define_method(cTexture2D, "max_s", Texture2D_get_maxS, 0);
  rb_define_method(cTexture2D, "max_t", Texture2D_get_maxT, 0);
  rb_define_method(cTexture2D, "ratio", Texture2D_get_textureRatio, 0);
  rb_define_method(cTexture2D, "format", Texture2D_get_pixelFormat, 0);
}

// #pragma - Initialize

VALUE Texture2D_initialize(VALUE self, VALUE image) {
  TEXTURE2D();

  if(!GL_EXT_framebuffer_object)
  {
     rb_raise(rb_eRuntimeError, "Ashton::Texture requires GL_EXT_framebuffer_object, which is not supported by OpenGL");
  }

  glGenTextures(1, &texture2D->name);
  glBindTexture(GL_TEXTURE_2D, texture2D->name);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // MAG_FILTER set on each draw
  // GL_LINEAR is usually set in the ParticleEmitter codebase
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  char* data;
  if(NIL_P(image))
  {
     data = NULL; // Create an empty texture, that might be filled with junk.
  }
  else
  {
     data = StringValuePtr(image); // Create from blob data.
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture2D->width,
                      texture2D->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  GLint created_width;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &created_width);
  if(created_width == 0) {
    rb_raise(rb_eArgError, "Unable to create a texture of size %dx%d",
             texture2D->width, texture2D->height);
  }



  return self;
}

// #pragma - Allocation

static VALUE texture2d_allocate(VALUE klass) {
    Texture2D* texture2D = ALLOC(Texture2D);
    memset(texture2D, 0, sizeof(Texture2D));

    // return Data_Wrap_Struct(klass, texture2D_mark, texture2D_free, texture2D);
    return Data_Wrap_Struct(klass, 0, texture2D_free, texture2D);
}

static void texture2D_mark(Texture2D* texture2D) {
    // if(!NIL_P(texture2D->rb_shader)) rb_gc_mark(texture2D->rb_shader);
    // rb_gc_mark(texture2D->rb_image);
}

// Deallocate data structure and its contents.
static void texture2D_free(Texture2D* texture2D) {
    // glDeleteBuffersARB(1, &texture2D->vbo_id);
    // xfree(texture2D->color_array);
    // xfree(texture2D->texture_coords_array);
    // xfree(texture2D->vertex_array);
    //
    // xfree(texture2D->particles);
    xfree(texture2D);
}

// #pragma mark - Getters

GET_TEXTURE2D_DATA(name, name, UINT2NUM);
GET_TEXTURE2D_DATA(width, width, UINT2NUM);
GET_TEXTURE2D_DATA(height, height, UINT2NUM);
GET_TEXTURE2D_DATA(maxS, maxS, rb_float_new);
GET_TEXTURE2D_DATA(maxT, maxT, rb_float_new);


VALUE Texture2D_get_textureRatio(VALUE self) {
  TEXTURE2D();

  VALUE ratioArray = rb_ary_new();

  rb_ary_push(ratioArray, INT2FIX(texture2D->textureRatio.width));
  rb_ary_push(ratioArray, INT2FIX(texture2D->textureRatio.height));

  return ratioArray;
}


VALUE Texture2D_get_contentSize(VALUE self) {
  TEXTURE2D();

  VALUE sizeArray = rb_ary_new();

  rb_ary_push(sizeArray, INT2FIX(texture2D->contentSize.width));
  rb_ary_push(sizeArray, INT2FIX(texture2D->contentSize.height));

  return sizeArray;
}


VALUE Texture2D_get_pixelFormat(VALUE self) {
  TEXTURE2D();

  char *formats [4] = { "automatic", "rgba888", "rgb565", "ab" };
  char *format = formats[texture2D->pixelFormat];

  return rb_str_new2(format);
}

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

  printf("%f %f",texture->width,texture->height);

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
      printf("%s",data);
      printf("%s",context);
      break;
    }

    // Generates an ERROR when used:
    // Nov 12 18:20:55 Pod.local ruby[78738] <Error>: CGContextSaveGState: invalid context 0x0
    // Nov 12 18:20:55 Pod.local ruby[78738] <Error>: CGContextSetCompositeOperation: invalid context 0x0
    // Nov 12 18:20:55 Pod.local ruby[78738] <Error>: CGContextFillRects: invalid context 0x0
    // Nov 12 18:20:55 Pod.local ruby[78738] <Error>: CGContextRestoreGState: invalid context 0x0
    // if (context == NULL) { return; }

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

    switch(texture->pixelFormat) {
      case kTexture2DPixelFormat_RGBA8888:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        break;
      case kTexture2DPixelFormat_RGB565:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data);
        break;
      case kTexture2DPixelFormat_A8:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, texture->width, texture->height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
        break;
    }

    printf("The content Size is (%f,%f)",texture->contentSize.width,texture->contentSize.height);
    printf("The content Size is (%f,%f)",texture->width,texture->height);

    texture->maxS = texture->contentSize.width / (float)texture->width;
    texture->maxT = texture->contentSize.height / (float)texture->height;

    printf("The MaxS,MaxT Size is (%f,%f)",texture->maxS,texture->maxT);

    texture->textureRatio.width = 1.0f / (float)texture->width;
    texture->textureRatio.height = 1.0f / (float)texture->height;

    CGContextRelease(context);
    free(data);

    return texture;

}

Texture2D* Texture2D_createWithBlob(char *textureBlob) {

}