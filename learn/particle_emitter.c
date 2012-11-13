#include "particle_emitter.h"

#pragma mark - Protoypes

static VALUE particleEmitter_allocate(VALUE klass);
// static void particleEmitter_mark(ParticleEmitter *emitter);
static void particleEmitter_free(ParticleEmitter *emitter);

VALUE particleEmitter_init(VALUE self);
VALUE particleEmitter_active(VALUE self);
VALUE particleEmitter_get_sourcePosition(VALUE self);

VALUE particleEmitter_optimize(VALUE self);
VALUE particleEmitter_updateWithDelta(VALUE self,VALUE rbDelta);

#pragma mark - Internal Methods

void particleEmitter_parseParticleConfig(VALUE self);
void particleEmitter_addParticle(VALUE self);
void particleEmitter_stopParticleEmitter(VALUE self);
void particleEmitter_initParticle(VALUE self,Particle *particle);
VALUE particleEmitter_renderParticles(VALUE self);

#pragma mark - Init

void Init_ParticleEmitter(VALUE module) {

  VALUE cParticleEmitter = rb_define_class_under(module,"ParticleEmitter",rb_cObject);
  rb_define_alloc_func(cParticleEmitter, particleEmitter_allocate);
  rb_define_method(cParticleEmitter, "initialize", particleEmitter_init, 0);

  rb_define_method(cParticleEmitter, "active?", particleEmitter_active, 0);
  rb_define_method(cParticleEmitter, "source_position", particleEmitter_get_sourcePosition, 0);

  rb_define_method(cParticleEmitter, "optimize", particleEmitter_optimize, 0);
  rb_define_method(cParticleEmitter, "update", particleEmitter_updateWithDelta, 1);
  rb_define_method(cParticleEmitter, "render_particles", particleEmitter_renderParticles, 0);

}

#pragma mark - Allocation

static VALUE particleEmitter_allocate(VALUE klass) {
    ParticleEmitter *emitter = ALLOC(ParticleEmitter);
    memset(emitter, 0, sizeof(ParticleEmitter));

    return Data_Wrap_Struct(klass, 0, particleEmitter_free, emitter);
}

// static void particleEmitter_mark(ParticleEmitter *emitter) {
//     // if(!NIL_P(texture2D->rb_shader)) rb_gc_mark(texture2D->rb_shader);
//     // rb_gc_mark(texture2D->rb_image);
// }

// Deallocate data structure and its contents.
static void particleEmitter_free(ParticleEmitter *emitter) {
    // glDeleteBuffersARB(1, &texture2D->vbo_id);
    // xfree(texture2D->color_array);
    // xfree(texture2D->texture_coords_array);
    // xfree(texture2D->vertex_array);
    //
    // xfree(texture2D->particles);
    xfree(emitter);
}


#pragma mark - Initialization

VALUE particleEmitter_init(VALUE self) {
  return self;
}

#define GET_FLOAT(OBJECT,METHOD) (float)NUM2DBL( rb_funcall(OBJECT,rb_intern(#METHOD),0) );

#pragma mark -

//
// This is ParticleEmitter.m - setupArrays
//
VALUE particleEmitter_optimize(VALUE self) {
  EMITTER();

  particleEmitter_parseParticleConfig(self);

  int maxParticles = emitter->maxParticles;

  printf("\n MaxParticles, (MAXS,MAXT) %d, (%f,%f)",maxParticles, emitter->texture->maxS,emitter->texture->maxT);

  GLfloat maxS = emitter->texture->maxS;
  GLfloat maxT = emitter->texture->maxT;

  // Allocate the memory necessary for the particle emitter arrays
  emitter->particles = malloc( sizeof(Particle) * maxParticles );
  emitter->quads     = calloc( sizeof(ParticleQuad), maxParticles );
  emitter->indices   = calloc( sizeof(GLushort), maxParticles * 6);

  // Set up the indices for all particles. This provides an array of indices into the quads array that is used during
  // rendering. As we are rendering quads there are six indices for each particle as each particle is made of two triangles
  // that are each defined by three vertices.
  for( int i=0; i < maxParticles; i++) {
    emitter->indices[i*6+0] = i*4+0;
    emitter->indices[i*6+1] = i*4+1;
    emitter->indices[i*6+2] = i*4+2;

    emitter->indices[i*6+5] = i*4+3;
    emitter->indices[i*6+4] = i*4+2;
    emitter->indices[i*6+3] = i*4+1;
  }

  // Set up texture coordinates for all particles as these will not change.
  for(int i=0; i < maxParticles; i++) {
    emitter->quads[i].bl.texture.x = 0;
    emitter->quads[i].bl.texture.y = 0;

    emitter->quads[i].br.texture.x = maxS;
    emitter->quads[i].br.texture.y = 0;

    emitter->quads[i].tl.texture.x = 0;
    emitter->quads[i].tl.texture.y = maxT;

    emitter->quads[i].tr.texture.x = maxS;
    emitter->quads[i].tr.texture.y = maxT;
  }

  glGenBuffers(1, &emitter->verticesID);
  glBindBuffer(GL_ARRAY_BUFFER, emitter->verticesID);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleQuad) * maxParticles, emitter->quads, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  emitter->active = Qtrue;

  emitter->particleCount = 0;
  emitter->elapsedTime = 0;

  return self;
}

void particleEmitter_parseParticleConfig(VALUE self) {
  EMITTER();

  VALUE config = rb_funcall(self,rb_intern("ped"),0);

  // VALUE rbImageBlobData = rb_funcall(config,rb_intern("image_data"),0);
  // char *textureBlobData = StringValuePtr( rbImageBlobData );
  // emitter->texture = Texture2D_createWithBlob(textureBlobData);

  VALUE rbImageFileName = rb_funcall(config,rb_intern("image_path"),0);
    char *imageFileName = StringValueCStr( rbImageFileName );

  printf("Loading Texture For: %s",imageFileName);

  emitter->texture = Texture2D_createWithFile( imageFileName );

  emitter->emitterType = FIX2INT( rb_funcall(config,rb_intern("x"),0) );

  emitter->sourcePosition.x = GET_FLOAT(config,x)
  emitter->sourcePosition.y = GET_FLOAT(config,y)
  emitter->sourcePositionVariance.x = GET_FLOAT(config,x_var)
  emitter->sourcePositionVariance.y = GET_FLOAT(config,y_var)

  emitter->angle = GET_FLOAT(config,angle)
  emitter->angleVariance = GET_FLOAT(config,angle_var)

  emitter->speed = GET_FLOAT(config,speed)
  emitter->speedVariance = GET_FLOAT(config,speed_var)

  emitter->radialAcceleration = GET_FLOAT(config,radial_acceleration)
  emitter->radialAccelVariance = GET_FLOAT(config,radial_acceleration_var)

  emitter->tangentialAcceleration = GET_FLOAT(config,tangential_acceleration)
  emitter->tangentialAccelVariance = GET_FLOAT(config,tangential_acceleration_var)

  emitter->gravity.x = GET_FLOAT(config,gravity_x)
  emitter->gravity.y = GET_FLOAT(config,gravity_y)

  emitter->particleLifespan = GET_FLOAT(config,life_span)
  emitter->particleLifespanVariance = GET_FLOAT(config,life_span_var)

  VALUE startColor = rb_funcall(config,rb_intern("start_color"),0);

  emitter->startColor.red = GET_FLOAT(startColor,red)
  emitter->startColor.green = GET_FLOAT(startColor,green)
  emitter->startColor.blue = GET_FLOAT(startColor,blue)
  emitter->startColor.alpha = GET_FLOAT(startColor,alpha)

  VALUE startColorVar = rb_funcall(config,rb_intern("start_color_var"),0);

  emitter->startColorVariance.red = GET_FLOAT(startColorVar,red)
  emitter->startColorVariance.green = GET_FLOAT(startColorVar,green)
  emitter->startColorVariance.blue = GET_FLOAT(startColorVar,blue)
  emitter->startColorVariance.alpha = GET_FLOAT(startColorVar,alpha)

  VALUE finishColor = rb_funcall(config,rb_intern("finish_color"),0);

  emitter->finishColor.red = GET_FLOAT(finishColor,red)
  emitter->finishColor.green = GET_FLOAT(finishColor,green)
  emitter->finishColor.blue = GET_FLOAT(finishColor,blue)
  emitter->finishColor.alpha = GET_FLOAT(finishColor,alpha)

  VALUE finishColorVar = rb_funcall(config,rb_intern("finish_color_var"),0);

  emitter->finishColorVariance.red = GET_FLOAT(finishColorVar,red)
  emitter->finishColorVariance.green = GET_FLOAT(finishColorVar,green)
  emitter->finishColorVariance.blue = GET_FLOAT(finishColorVar,blue)
  emitter->finishColorVariance.alpha = GET_FLOAT(finishColorVar,alpha)

  emitter->maxParticles = GET_FLOAT(config,max_particles)
  emitter->startParticleSize = GET_FLOAT(config,start_particle_size)
  emitter->startParticleSizeVariance = GET_FLOAT(config,start_particle_size_var)
  emitter->finishParticleSize = GET_FLOAT(config,finish_particle_size)
  emitter->finishParticleSizeVariance = GET_FLOAT(config,finish_particle_size_var)
  emitter->duration = GET_FLOAT(config,duration)
  emitter->blendFuncSource = NUM2INT( rb_funcall(config,rb_intern("blend_func_source"),0) );
  emitter->blendFuncDestination = NUM2INT( rb_funcall(config,rb_intern("blend_func_destination"),0) );


  emitter->maxRadius = GET_FLOAT(config,max_radius)
  emitter->maxRadiusVariance = GET_FLOAT(config,max_radius_var)
  emitter->radiusSpeed = GET_FLOAT(config,radius_speed)
  emitter->minRadius = GET_FLOAT(config,min_radius)
  emitter->rotatePerSecond = GET_FLOAT(config,rotate_per_second)
  emitter->rotatePerSecondVariance = GET_FLOAT(config,rotate_per_second_var)

  emitter->rotationStart = GET_FLOAT(config,rotation_start)
  emitter->rotationStartVariance = GET_FLOAT(config,rotation_start_var)
  emitter->rotationEnd = GET_FLOAT(config,rotation_end)
  emitter->rotationEndVariance = GET_FLOAT(config,rotation_end_var)

  emitter->emissionRate = (emitter->maxParticles / emitter->particleLifespan);

}

VALUE particleEmitter_updateWithDelta(VALUE self,VALUE rbDelta) {
  EMITTER();

  float aDelta = (float)NUM2DBL(rbDelta);

  // printf("\nUpdating");

  if(emitter->active && emitter->emissionRate) {
    float rate = 1.0f/emitter->emissionRate;
    emitter->emitCounter += aDelta;

    while(emitter->particleCount < emitter->maxParticles && emitter->emitCounter > rate) {
      particleEmitter_addParticle(self);
      emitter->emitCounter -= rate;
    }

    emitter->elapsedTime += aDelta;

    if(emitter->duration != -1 && emitter->duration < emitter->elapsedTime) {
      particleEmitter_stopParticleEmitter(self);
    }
  }



  // Reset the particle index before updating the particles in this emitter
  emitter->particleIndex = 0;

  int particleIndex = 0;

    // Loop through all the particles updating their location and color
  while(particleIndex < emitter->particleCount) {
    // Get the particle for the current particle index
    Particle *currentParticle = &emitter->particles[particleIndex];

    // FIX 1
    // Reduce the life span of the particle
    currentParticle->timeToLive -= aDelta;

    // If the current particle is alive then update it
    if(currentParticle->timeToLive > 0) {

      // If maxRadius is greater than 0 then the particles are going to spin otherwise
      // they are effected by speed and gravity
      if (emitter->emitterType == kParticleTypeRadial) {

        // FIX 2
        // Update the angle of the particle from the sourcePosition and the radius.  This is only
        // done of the particles are rotating
        currentParticle->angle += currentParticle->degreesPerSecond * aDelta;
        currentParticle->radius -= currentParticle->radiusDelta;

        Vector2f tmp;
        tmp.x = emitter->sourcePosition.x - cosf(currentParticle->angle) * currentParticle->radius;
        tmp.y = emitter->sourcePosition.y - sinf(currentParticle->angle) * currentParticle->radius;
        currentParticle->position = tmp;

        if (currentParticle->radius < emitter->minRadius)
          currentParticle->timeToLive = 0;
      } else {

        Vector2f tmp, radial, tangential;

        radial = Vector2fZero;
        Vector2f diff = Vector2fSub(currentParticle->startPos, Vector2fZero);

        currentParticle->position = Vector2fSub(currentParticle->position, diff);

        if (currentParticle->position.x || currentParticle->position.y) {
          radial = Vector2fNormalize(currentParticle->position);
        }

        tangential.x = radial.x;
        tangential.y = radial.y;
        radial = Vector2fMultiply(radial, currentParticle->radialAcceleration);

        GLfloat newy = tangential.x;
        tangential.x = -tangential.y;
        tangential.y = newy;
        tangential = Vector2fMultiply(tangential, currentParticle->tangentialAcceleration);

        tmp = Vector2fAdd( Vector2fAdd(radial, tangential), emitter->gravity);
        tmp = Vector2fMultiply(tmp, aDelta);
        currentParticle->direction = Vector2fAdd(currentParticle->direction, tmp);
        tmp = Vector2fMultiply(currentParticle->direction, aDelta);
        currentParticle->position = Vector2fAdd(currentParticle->position, tmp);
        currentParticle->position = Vector2fAdd(currentParticle->position, diff);

      }

      // Update the particles color
      currentParticle->color.red += currentParticle->deltaColor.red;
      currentParticle->color.green += currentParticle->deltaColor.green;
      currentParticle->color.blue += currentParticle->deltaColor.blue;
      currentParticle->color.alpha += currentParticle->deltaColor.alpha;

      // Update the particle size
      currentParticle->particleSize += currentParticle->particleSizeDelta;

      // Update the rotation of the particle
      currentParticle->rotation += (currentParticle->rotationDelta * aDelta);

      // As we are rendering the particles as quads, we need to define 6 vertices for each particle
      GLfloat halfSize = currentParticle->particleSize * 0.5f;

      // If a rotation has been defined for this particle then apply the rotation to the vertices that define
      // the particle
      if (currentParticle->rotation) {
          float x1 = -halfSize;
          float y1 = -halfSize;
          float x2 = halfSize;
          float y2 = halfSize;
          float x = currentParticle->position.x;
          float y = currentParticle->position.y;
          float r = (float)DEGREES_TO_RADIANS(currentParticle->rotation);
          float cr = cosf(r);
          float sr = sinf(r);
          float ax = x1 * cr - y1 * sr + x;
          float ay = x1 * sr + y1 * cr + y;
          float bx = x2 * cr - y1 * sr + x;
          float by = x2 * sr + y1 * cr + y;
          float cx = x2 * cr - y2 * sr + x;
          float cy = x2 * sr + y2 * cr + y;
          float dx = x1 * cr - y2 * sr + x;
          float dy = x1 * sr + y2 * cr + y;

          emitter->quads[particleIndex].bl.vertex.x = ax;
          emitter->quads[particleIndex].bl.vertex.y = ay;
          emitter->quads[particleIndex].bl.color = currentParticle->color;

          emitter->quads[particleIndex].br.vertex.x = bx;
          emitter->quads[particleIndex].br.vertex.y = by;
          emitter->quads[particleIndex].br.color = currentParticle->color;

          emitter->quads[particleIndex].tl.vertex.x = dx;
          emitter->quads[particleIndex].tl.vertex.y = dy;
          emitter->quads[particleIndex].tl.color = currentParticle->color;

          emitter->quads[particleIndex].tr.vertex.x = cx;
          emitter->quads[particleIndex].tr.vertex.y = cy;
          emitter->quads[particleIndex].tr.color = currentParticle->color;
      } else {
          // Using the position of the particle, work out the four vertices for the quad that will hold the particle
          // and load those into the quads array.
          emitter->quads[particleIndex].bl.vertex.x = currentParticle->position.x - halfSize;
          emitter->quads[particleIndex].bl.vertex.y = currentParticle->position.y - halfSize;
          emitter->quads[particleIndex].bl.color = currentParticle->color;

          emitter->quads[particleIndex].br.vertex.x = currentParticle->position.x + halfSize;
          emitter->quads[particleIndex].br.vertex.y = currentParticle->position.y - halfSize;
          emitter->quads[particleIndex].br.color = currentParticle->color;

          emitter->quads[particleIndex].tl.vertex.x = currentParticle->position.x - halfSize;
          emitter->quads[particleIndex].tl.vertex.y = currentParticle->position.y + halfSize;
          emitter->quads[particleIndex].tl.color = currentParticle->color;

          emitter->quads[particleIndex].tr.vertex.x = currentParticle->position.x + halfSize;
          emitter->quads[particleIndex].tr.vertex.y = currentParticle->position.y + halfSize;
          emitter->quads[particleIndex].tr.color = currentParticle->color;
      }

      // Update the particle and vertex counters
      emitter->particleIndex = particleIndex++;
    } else {
      // As the particle is not alive anymore replace it with the last active particle
      // in the array and reduce the count of particles by one.  This causes all active particles
      // to be packed together at the start of the array so that a particle which has run out of
      // life will only drop into this clause once
      if(particleIndex != emitter->particleCount - 1) {
        emitter->particles[particleIndex] = emitter->particles[emitter->particleCount - 1];
      }
      emitter->particleIndex = particleIndex--;
    }
  }

  return Qnil;
}

void particleEmitter_addParticle(VALUE self) {
  EMITTER();

  if (emitter->particleCount == emitter->maxParticles) {
    return;
  }

  Particle *particle = &emitter->particles[emitter->particleCount];

  particleEmitter_initParticle(self,particle);

  emitter->particleCount++;
}

VALUE particleEmitter_renderParticles(VALUE self) {
  EMITTER();

  // glEnable(GL_BLEND);
  // glEnable(GL_TEXTURE_2D);

  // Bind to the verticesID VBO and popuate it with the necessary vertex, color and texture informaiton
  glBindBuffer(GL_ARRAY_BUFFER, emitter->verticesID);

  // Using glBufferSubData means that a copy is done from the quads array to the buffer rather than recreating the buffer which
  // would be an allocation and copy. The copy also only takes over the number of live particles. This provides a nice performance
  // boost.

  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ParticleQuad) * emitter->particleIndex, emitter->quads);

  // Configure the vertex pointer which will use the currently bound VBO for its data
  glVertexPointer(2, GL_FLOAT, sizeof(TexturedColoredVertex), 0);
  glColorPointer(4, GL_FLOAT, sizeof(TexturedColoredVertex), (GLvoid*) offsetof(TexturedColoredVertex, color));
  glTexCoordPointer(2, GL_FLOAT, sizeof(TexturedColoredVertex), (GLvoid*) offsetof(TexturedColoredVertex, texture));

  // Bind to the particles texture
  glBindTexture(GL_TEXTURE_2D, emitter->texture->name);

  // Set the blend function based on the configuration
  glBlendFunc(emitter->blendFuncSource, emitter->blendFuncDestination);

  // Now that all of the VBOs have been used to configure the vertices, pointer size and color
  // use glDrawArrays to draw the points
  glDrawElements(GL_TRIANGLES, emitter->particleIndex * 6, GL_UNSIGNED_SHORT, emitter->indices);

  // Unbind the current VBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // printf("\nRendering %d",emitter->verticesID);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // printf("\nTexture Name: %d",emitter->texture->name);

  return Qnil;
}

void particleEmitter_initParticle(VALUE self,Particle* particle) {
  EMITTER();

  // Init the position of the particle.  This is based on the source position of the particle emitter
  // plus a configured variance.  The RANDOM_MINUS_1_TO_1 macro allows the number to be both positive
  // and negative
  particle->position.x = emitter->sourcePosition.x + emitter->sourcePositionVariance.x * RANDOM_MINUS_1_TO_1();
  particle->position.y = emitter->sourcePosition.y + emitter->sourcePositionVariance.y * RANDOM_MINUS_1_TO_1();
  particle->startPos.x = emitter->sourcePosition.x;
  particle->startPos.y = emitter->sourcePosition.y;

  // Init the direction of the particle.  The newAngle is calculated using the angle passed in and the
  // angle variance.
  float newAngle = (GLfloat)DEGREES_TO_RADIANS(emitter->angle + emitter->angleVariance * RANDOM_MINUS_1_TO_1());

  // Create a new Vector2f using the newAngle
  Vector2f vector = Vector2fMake(cosf(newAngle), sinf(newAngle));

  // Calculate the vectorSpeed using the speed and speedVariance which has been passed in
  float vectorSpeed = emitter->speed + emitter->speedVariance * RANDOM_MINUS_1_TO_1();

  // The particles direction vector is calculated by taking the vector calculated above and
  // multiplying that by the speed
  particle->direction = Vector2fMultiply(vector, vectorSpeed);

  // Set the default diameter of the particle from the source position
  particle->radius = emitter->maxRadius + emitter->maxRadiusVariance * RANDOM_MINUS_1_TO_1();
  particle->radiusDelta = (emitter->maxRadius / emitter->particleLifespan) * (1.0 / MAXIMUM_UPDATE_RATE);
  particle->angle = DEGREES_TO_RADIANS(emitter->angle + emitter->angleVariance * RANDOM_MINUS_1_TO_1());
  particle->degreesPerSecond = DEGREES_TO_RADIANS(emitter->rotatePerSecond + emitter->rotatePerSecondVariance * RANDOM_MINUS_1_TO_1());

  particle->radialAcceleration = emitter->radialAcceleration;
  particle->tangentialAcceleration = emitter->tangentialAcceleration;

  // Calculate the particles life span using the life span and variance passed in
  particle->timeToLive = MAX(0, emitter->particleLifespan + emitter->particleLifespanVariance * RANDOM_MINUS_1_TO_1());

  // Calculate the particle size using the start and finish particle sizes
  GLfloat particleStartSize = emitter->startParticleSize + emitter->startParticleSizeVariance * RANDOM_MINUS_1_TO_1();
  GLfloat particleFinishSize = emitter->finishParticleSize + emitter->finishParticleSizeVariance * RANDOM_MINUS_1_TO_1();
  particle->particleSizeDelta = ((particleFinishSize -particleStartSize) / particle->timeToLive) * (1.0f / MAXIMUM_UPDATE_RATE);
  particle->particleSize = MAX(0, particleStartSize);

  // Calculate the color the particle should have when it starts its life.  All the elements
  // of the start color passed in along with the variance are used to calculate the star color
  Color4f start = {0, 0, 0, 0};
  start.red = emitter->startColor.red + emitter->startColorVariance.red * RANDOM_MINUS_1_TO_1();
  start.green = emitter->startColor.green + emitter->startColorVariance.green * RANDOM_MINUS_1_TO_1();
  start.blue = emitter->startColor.blue + emitter->startColorVariance.blue * RANDOM_MINUS_1_TO_1();
  start.alpha = emitter->startColor.alpha + emitter->startColorVariance.alpha * RANDOM_MINUS_1_TO_1();

  // Calculate the color the particle should be when its life is over.  This is done the same
  // way as the start color above
  Color4f end = {0, 0, 0, 0};
  end.red = emitter->finishColor.red + emitter->finishColorVariance.red * RANDOM_MINUS_1_TO_1();
  end.green = emitter->finishColor.green + emitter->finishColorVariance.green * RANDOM_MINUS_1_TO_1();
  end.blue = emitter->finishColor.blue + emitter->finishColorVariance.blue * RANDOM_MINUS_1_TO_1();
  end.alpha = emitter->finishColor.alpha + emitter->finishColorVariance.alpha * RANDOM_MINUS_1_TO_1();

  // Calculate the delta which is to be applied to the particles color during each cycle of its
  // life.  The delta calculation uses the life span of the particle to make sure that the
  // particles color will transition from the start to end color during its life time.  As the game
  // loop is using a fixed delta value we can calculate the delta color once saving cycles in the
  // update method
  particle->color = start;
  particle->deltaColor.red = ((end.red - start.red) / particle->timeToLive) * (1.0f / MAXIMUM_UPDATE_RATE);
  particle->deltaColor.green = ((end.green - start.green) / particle->timeToLive)  * (1.0f / MAXIMUM_UPDATE_RATE);
  particle->deltaColor.blue = ((end.blue - start.blue) / particle->timeToLive)  * (1.0f / MAXIMUM_UPDATE_RATE);
  particle->deltaColor.alpha = ((end.alpha - start.alpha) / particle->timeToLive)  * (1.0f / MAXIMUM_UPDATE_RATE);

  // printf("\n(%f,%f,%f)",particle->color.red,particle->color.green,particle->color.blue,particle->color.alpha);

  // Calculate the rotation
  GLfloat startA = emitter->rotationStart + emitter->rotationStartVariance * RANDOM_MINUS_1_TO_1();
  GLfloat endA = emitter->rotationEnd + emitter->rotationEndVariance * RANDOM_MINUS_1_TO_1();
  particle->rotation = startA;
  particle->rotationDelta = (endA - startA) / particle->timeToLive;

}

void particleEmitter_stopParticleEmitter(VALUE self) {
  EMITTER();

  emitter->active = Qfalse;
  emitter->elapsedTime = 0.0f;
  emitter->emitCounter = 0.0f;
}


#pragma mark - Getters

VALUE particleEmitter_active(VALUE self) {
  EMITTER();
  return emitter->active;
}

VALUE particleEmitter_get_sourcePosition(VALUE self) {
  EMITTER();

  return rb_float_new(emitter->sourcePosition.x);
}