puts ("---" * 30)

require 'gosu'
require 'happymapper'
require 'rmagick'
require './learn/learn'

class ParticleColor
  include HappyMapper

  attribute :red, Float
  attribute :green, Float
  attribute :blue, Float
  attribute :alpha, Float

  def color
    Gosu::Color.new alpha, red, green, blue
  end

end

Vector2f = Struct.new(:x,:y)

class Texture2D

  def initialize(magick_image)
    @magick_image = magick_image
  end

end

class ParticleFile
  include HappyMapper

  attr_accessor :window

  tag 'particleEmitterConfig'

  has_one :image_path, String, xpath: "texture/@name"

  has_one :image_data, String, xpath: "texture/@data"

  def image
    @image ||= Gosu::Image.new(window,image_path,false)
  end

  def image_data
    Magick::Image.read(image_path).first.to_blob
  end

  def texture
    @texture ||= Texture2D.new(Magick::Image.read(image_path).first)
  end

  def source_position
    Vector2f.new x, y
  end

  def source_position_var
    Vector2f.new x_var, y_var
  end

  has_one :x, Float, xpath: "sourcePosition/@x"
  has_one :y, Float, xpath: "sourcePosition/@y"
  has_one :x_var, Float, xpath: "sourcePositionVariance/@x"
  has_one :y_var, Float, xpath: "sourcePositionVariance/@y"

  has_one :speed, Float, xpath: "speed/@value"
  has_one :speed_var, Float, xpath: "speedVariance/@value"

  has_one :life_span, Float, xpath: "particleLifeSpan/@value"
  has_one :life_span_var, Float, xpath: "particleLifespanVariance/@value"

  has_one :angle, Float, xpath: "angle/@value"
  has_one :angle_var, Float, xpath: "angleVariance/@value"

  def gravity
    Vector2f.new gravity_x, gravity_y
  end

  has_one :gravity_x, Float, xpath: "gravity/@x"
  has_one :gravity_y, Float, xpath: "gravity/@y"

  has_one :radial_acceleration, Float, xpath: "radialAcceleration/@value"
  has_one :radial_acceleration_var, Float, xpath: "radialAccelVariance/@value"

  has_one :tangential_acceleration, Float, xpath: "tangentialAcceleration/@value"
  has_one :tangential_acceleration_var, Float, xpath: "tangentialAccelVariance/@value"

  has_one :start_color, ParticleColor, tag: "startColor"
  has_one :start_color_var, ParticleColor, tag: "startColorVariance"
  has_one :finish_color, ParticleColor, tag: "finishColor"
  has_one :finish_color_var, ParticleColor, tag: "finishColorVariance"

  has_one :max_particles, Float, xpath: "maxParticles/@value"

  has_one :start_particle_size, Float, xpath: "startParticleSize/@value"
  has_one :start_particle_size_var, Float, xpath: "startParticleSizeVariance/@value"


  has_one :finish_particle_size, Float, xpath: "finishParticleSize/@value"
  has_one :finish_particle_size_var, Float, xpath: "FinishParticleSizeVariance/@value"
  has_one :duration, Float, xpath: "duration/@value"

  has_one :emitter_type, String, xpath: "emitterType/@value"

  has_one :max_radius, Float, xpath: "maxRadius/@value"
  has_one :max_radius_var, Float, xpath: "maxRadiusVariance/@value"
  has_one :min_radius, Float, xpath: "minRadius/@value"
  has_one :radius_speed, Float, xpath: "radiusSpeed/@value"

  def radius_speed
    # TODO: See why radius speed is missing from the xml input file (.pex file)
    @radius_speed.to_f
  end

  has_one :rotate_per_second, Float, xpath: "rotatePerSecond/@value"
  has_one :rotate_per_second_var, Float, xpath: "rotatePerSecondVariance/@value"

  has_one :blend_func_source, Float, xpath: "blendFuncSource/@value"
  has_one :blend_func_destination, Float, xpath: "blendFuncDestination/@value"

  has_one :rotation_start, Float, xpath: "rotationStart/@value"
  has_one :rotation_start_var, Float, xpath: "rotationStartVariance/@value"

  has_one :rotation_end, Float, xpath: "rotationEnd/@value"
  has_one :rotation_end_var, Float, xpath: "rotationEndVariance/@value"

  def emission_rate
    max_particles / life_span
  end

end



window = Gosu::Window.new(200,200,false)

class Learn::ParticleEmitter

  attr_reader :ped

  def initialize(window,particle_file)
    contents = File.read(particle_file)
    @ped = ParticleFile.parse(contents, single: true)
    @ped.window = window
    @particle_count = 0
    @emit_counter = 0
    @elapsed_time = 0
    @duration = ped.duration
    @active = false
  end

  def max_particles ; ped.max_particles ; end

end

pe = Learn::ParticleEmitter.new(window,"purple.pex")
puts "Emitter Ready: #{pe.active?}"
pe.optimize
puts "Emitter Ready: #{pe.active?}"
puts "Source Position: #{pe.source_position}"


puts ("---" * 30)
puts "Learning Is Great!"