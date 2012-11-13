#include "learn.h"

void Init_learn() {
  VALUE cLearn = rb_define_module("Learn");
  Init_ParticleEmitter(cLearn);
}