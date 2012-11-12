// Structure used to hold particle specific information
typedef struct {
  Vector2f position;
  Vector2f direction;
  Vector2f startPos;
  Color4f color;
  Color4f deltaColor;
  GLfloat rotation;
  GLfloat rotationDelta;
  GLfloat radialAcceleration;
  GLfloat tangentialAcceleration;
  GLfloat radius;
  GLfloat radiusDelta;
  GLfloat angle;
  GLfloat degreesPerSecond;
  GLfloat particleSize;
  GLfloat particleSizeDelta;
  GLfloat timeToLive;
} Particle;
