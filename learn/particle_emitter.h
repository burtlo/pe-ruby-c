#import "global.h"

#import "texture_2d.h"
#import "point_sprite.h"
#import "textured_colored_vertex.h"
#import "particle_quad.h"
#import "particle.h"

// Particle type
enum kParticleTypes {
  kParticleTypeGravity,
  kParticleTypeRadial
};

typedef struct _particle_emitter {

  /////////////////// Particle iVars
  int emitterType;
  Texture2D *texture;
  Vector2f sourcePosition, sourcePositionVariance;
  GLfloat angle, angleVariance;
  GLfloat speed, speedVariance;
  GLfloat radialAcceleration, tangentialAcceleration;
  GLfloat radialAccelVariance, tangentialAccelVariance;
  Vector2f gravity;
  GLfloat particleLifespan, particleLifespanVariance;
  Color4f startColor, startColorVariance;
  Color4f finishColor, finishColorVariance;
  GLfloat startParticleSize, startParticleSizeVariance;
  GLfloat finishParticleSize, finishParticleSizeVariance;
  GLuint maxParticles;
  GLint particleCount;
  GLfloat emissionRate;
  GLfloat emitCounter;
  GLfloat elapsedTime;
  GLfloat duration;
  GLfloat rotationStart, rotationStartVariance;
  GLfloat rotationEnd, rotationEndVariance;

  int blendFuncSource, blendFuncDestination;

  //////////////////// Particle ivars only used when a maxRadius value is provided.  These values are used for
  //////////////////// the special purpose of creating the spinning portal emitter
  GLfloat maxRadius;      // Max radius at which particles are drawn when rotating
  GLfloat maxRadiusVariance;    // Variance of the maxRadius
  GLfloat radiusSpeed;     // The speed at which a particle moves from maxRadius to minRadius
  GLfloat minRadius;      // Radius from source below which a particle dies
  GLfloat rotatePerSecond;    // Numeber of degress to rotate a particle around the source pos per second
  GLfloat rotatePerSecondVariance;  // Variance in degrees for rotatePerSecond

  //////////////////// Particle Emitter iVars
  BOOL active;
  BOOL useTexture;
  GLint particleIndex;  // Stores the number of particles that are going to be rendered
  GLint vertexIndex;         // Stores the index of the vertices being used for each particle

  ///////////////////// Render
  GLuint verticesID;   // Holds the buffer name of the VBO that stores the color and vertices info for the particles
  GLuint vertexObjectID;
  Particle *particles;  // Array of particles that hold the particle emitters particle details
  ParticleQuad *quads;        // Array holding quad information for each particle;
  GLushort *indices;          // Array holding an index reference into an array of quads for rendering
}  ParticleEmitter;

#define MAXIMUM_UPDATE_RATE 90.0f	// The maximum number of updates that occur per frame

void Init_ParticleEmitter(VALUE module);

#define EMITTER() \
    ParticleEmitter* emitter; \
    Data_Get_Struct(self, ParticleEmitter, emitter);


#define GET_EMITTER_DATA(ATTRIBUTE_NAME, ATTRIBUTE, CAST) \
    VALUE particleEmitter_get_##ATTRIBUTE_NAME(VALUE self) \
    { \
      EMITTER(); \
      return CAST(emitter->ATTRIBUTE); \
    }
