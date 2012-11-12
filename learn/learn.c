#include "learn.h"

void Init_learn() {
  VALUE cLearn = rb_define_module("Learn");

  Init_Texture2D(cLearn);
  Init_ParticleEmitter(cLearn);
}