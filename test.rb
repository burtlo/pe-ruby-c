require 'happymapper'
require 'gosu'
require 'opengl'


class Gosu::Image

  def maxS
    1
  end

  def maxT
    1
  end

  # // We need to calculate the maximum texture coordinates for our image within the texture.
  #   // As the texture size is power of 2 and could be therefore larger than the actual image
  #   // which it contains, we need to calculate the maximum s, t values using the size of the
  #   // content and the size of the texture
  #   maxS = contentSize.width / (float)width;
  #   maxT = contentSize.height / (float)height;
  #

end

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

class ParticleFile
  include HappyMapper

  attr_accessor :window

  tag 'particleEmitterConfig'

  has_one :image_path, String, xpath: "texture/@name"

  def image
    @image ||= Gosu::Image.new(window,image_path,false)
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
  has_one :tangential_accel_var, Float, xpath: "tangentialAccelVariance/@value"

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




class Particle
  attr_accessor :position, :direction, :start_position, :color, :delta_color, :rotation,
    :rotation_delta, :radial_acceleration, :tangential_acceleration, :radius, :radius_delta,
    :angle, :degrees_per_second, :particle_size, :particle_size_delta, :time_to_live

  attr_accessor :window

  def initialize
    @position = Vector2f.new
    @start_position = Vector2f.new
    @start_position = Vector2f.new
    @color = Gosu::Color.new
    @delta_color = Gosu::Color.new
    @rotation = 0
    @roatation_delta = 0
    @radial_acceleration = 0
    @tangential_accelation = 0
    @radius = 0
    @radius_delta = 0
    @angle = 0
    @degrees_per_second = 0
    @particle_size = 0
    @particle_size_delta = 0
    @time_to_live = 0
  end
  
  attr_accessor :image
  
  def render

    window.gl do
      glBindBuffer(GL_ARRAY_BUFFER, verticesID);
      
    end

    # window.gl do
    #   info = image.gl_tex_info
    #   glEnable GL_TEXTURE_2D
    #   glBindTexture GL_TEXTURE_2D, info.tex_name
    #   glEnable GL_BLEND
    #   glBlendFunc GL_ONE, GL_ZERO
    # 
    #   glBegin GL_QUADS do
    #     glTexCoord2d info.left, info.top
    #     glVertex2d 0, height # BL
    # 
    #     glTexCoord2d info.left, info.bottom
    #     glVertex2d 0, 0 # TL
    # 
    #     glTexCoord2d info.right, info.bottom
    #     glVertex2d width, 0 # TR
    # 
    #     glTexCoord2d info.right, info.top
    #     glVertex2d width, height # BR
    #   end
    # end

  end

end

Vector2f = Struct.new(:x,:y)

class TexturedColoredVertex

  attr_accessor :vertex, :texture, :color

  def initialize
    @vertex = Vector2f.new
    @texture = Vector2f.new
    @color = Gosu::Color
  end
end

class ParticleQuad

  attr_accessor :bl, :br, :tl, :tr

  def initialize
    @bl = TexturedColoredVertex.new
    @br = TexturedColoredVertex.new
    @tl = TexturedColoredVertex.new
    @tr = TexturedColoredVertex.new
  end

end

class ParticleEmitter

  attr_accessor :particle_count
  attr_accessor :emit_counter

  attr_accessor :active
  attr_accessor :source_position

  attr_accessor :duration
  attr_accessor :elapsed_time


  attr_reader :ped

  def initialize(window,particle_file)
    contents = File.read(particle_file)
    @ped = ParticleFile.parse(contents, single: true)
    @ped.window = window
    @particle_count = 0
    @emit_counter = 0
    @elapsed_time = 0
    @duration = ped.duration
    @active = true
    setup_arrays

  end

  def indices
    @indices ||= []
  end

  def quads
    @quads ||= []
  end

  def texture
    ped.image
  end

  def particles
    @particles ||= []
  end

  def setup_arrays
    puts "Creating #{ped.max_particles} particles"
    @particles = ped.max_particles.to_i.times.map do
      particle = Particle.new
      particle.window = ped.window
      particle.image = texture
      particle
    end

    @quads = particles.count.times.map { ParticleQuad.new }
    @indices = particles.count.times.map { 0 }

    particles.count.times do |i|
      indices[i*6+0] = i*4+0;
      indices[i*6+1] = i*4+1;
      indices[i*6+2] = i*4+2;
      indices[i*6+5] = i*4+3;
      indices[i*6+4] = i*4+2;
      indices[i*6+3] = i*4+1;
    end

    particles.count.times do |i|
      quads[i].bl.texture.x = 0;
      quads[i].bl.texture.y = 0;

      quads[i].br.texture.x = texture.maxS;
      quads[i].br.texture.y = 0;

      quads[i].tl.texture.x = 0;
      quads[i].tl.texture.y = texture.maxT;

      quads[i].tr.texture.x = texture.maxS;
      quads[i].tr.texture.y = texture.maxT;
    end
    
    
    
    ped.window.gl do
      puts "-------------"
      self.verticesID = glGenBuffers(1)
      
      # glBindBuffer(GL_ARRAY_BUFFER, self.verticesID);
      # 
      # # glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleQuad) * maxParticles, quads, GL_DYNAMIC_DRAW);
      # glBufferData(GL_ARRAY_BUFFER, sizeof(quads), quads, GL_DYNAMIC_DRAW);
      # glBindBuffer(GL_ARRAY_BUFFER, 0);
      # 
      puts "-------------"
    end
    

  end


  attr_accessor :verticesID

  def cosf(value)
    Math.cos(value)
  end

  def sinf(value)
    Math.sin(value)
  end

  def Vector2fMultiply(vector,acceleration)
    vector.x = vector.x * acceleration
    vector.y = vector.y * acceleration
    vector
  end

  def Vector2fAdd(vect1,vect2)
    Vector2f.new (vect1.x + vect2.x), (vect1.y + vect2.y)
  end
  
  def Vector2fSub(vect1,vect2)
    Vector2f.new (vect1.x - vect2.x), (vect1.y - vect2.y)
  end

  def DEGREES_TO_RADIANS(value)
    value * Math::PI / 180
  end

  def update(a_delta=0.08)

    if active and ped.emission_rate
      rate = 1.0/ ped.emission_rate
      self.emit_counter += a_delta

      while particle_count < ped.max_particles and emit_counter > rate
        add_particle
        self.emit_counter -= rate
      end

      self.elapsed_time += a_delta
      if (duration != -1 and duration < elapsed_time)
        stop_particle_emitter
      end

    end

    particle_index = 0

    while particle_index < particle_count

      cp = particles[particle_index]
      cp.time_to_live -= a_delta

      if cp.time_to_live > 0
        if ped.emitter_type == "kParticleTypeRadial"
          cp.angle += cp.degrees_per_second * a_delta
          cp.radius -= cp.radius_delta

          tmp_position = Vector2f.new
          tmp.x = source_position.x - cosf(cp.angle) * cp.radius
          tmp.y = source_position.y - cosf(cp.angle) * cp.radius
          cp.position = tmp_position

          if cp.radius < min_radius
            cp.time_to_live = 0
          end

        else

          tmp = Vector2f.new(0,0)
          radial = Vector2f.new(0,0)
          tangential = Vector2f.new(0,0)
          diff = Vector2fSub(cp.start_position,Vector2f.new(0,0))

          cp.position = Vector2fMultiply(cp.position,cp.radial_acceleration)

          newy = tangential.x
          tangential.x = -tangential.y
          tangential.y = newy
          tangential = Vector2fMultiply(tangential,cp.tangential_acceleration)

          tmp = Vector2fAdd( Vector2fAdd(radial,tangential), ped.gravity )
          tmp = Vector2fMultiply(tmp,a_delta)
          cp.direction = Vector2fAdd(cp.direction,tmp)
          tmp = Vector2fMultiply(cp.direction,a_delta)

          cp.position = Vector2fAdd(cp.position, tmp)
          cp.position = Vector2fAdd(cp.position, diff)
        end

        cp.color.red += cp.delta_color.red
        cp.color.green += cp.delta_color.green
        cp.color.blue += cp.delta_color.blue
        cp.color.alpha += cp.delta_color.alpha

        cp.particle_size += cp.particle_size_delta
        cp.rotation += cp.rotation_delta * a_delta

        half_size = cp.particle_size * 0.5

        if cp.rotation
          x1 = -half_size
          y1 = -half_size
          x2 = half_size
          y2 = half_size

          x = cp.position.x
          y = cp.position.y

          r = DEGREES_TO_RADIANS(cp.rotation)
          cr = cosf(r)
          sr = sinf(r)

          ax = x1 * cr - y1 * sr + x;
          ay = x1 * sr + y1 * cr + y;
          bx = x2 * cr - y1 * sr + x;
          by = x2 * sr + y1 * cr + y;
          cx = x2 * cr - y2 * sr + x;
          cy = x2 * sr + y2 * cr + y;
          dx = x1 * cr - y2 * sr + x;
          dy = x1 * sr + y2 * cr + y;

          quads[particle_index].bl.vertex.x = ax
          quads[particle_index].bl.vertex.y = ay
          quads[particle_index].bl.color = cp.color

          quads[particle_index].br.vertex.x = bx
          quads[particle_index].br.vertex.y = by
          quads[particle_index].br.color = cp.color

          quads[particle_index].tl.vertex.x = dx
          quads[particle_index].tl.vertex.y = dy
          quads[particle_index].tl.color = cp.color

          quads[particle_index].tr.vertex.x = cx
          quads[particle_index].tr.vertex.y = cy
          quads[particle_index].tr.color = cp.color

        else

          quads[particle_index].bl.vertex.x = currentParticle.position.x - halfSize
          quads[particle_index].bl.vertex.y = currentParticle.position.y - halfSize
          quads[particle_index].bl.color = currentParticle.color

          quads[particle_index].br.vertex.x = currentParticle.position.x + halfSize
          quads[particle_index].br.vertex.y = currentParticle.position.y - halfSize;
          quads[particle_index].br.color = currentParticle.color;

          quads[particle_index].tl.vertex.x = currentParticle.position.x - halfSize
          quads[particle_index].tl.vertex.y = currentParticle.position.y + halfSize
          quads[particle_index].tl.color = currentParticle.color

          quads[particle_index].tr.vertex.x = currentParticle.position.x + halfSize
          quads[particle_index].tr.vertex.y = currentParticle.position.y + halfSize
          quads[particle_index].tr.color = currentParticle.color

        end

        particle_index += 1
      else

        if particle_index != (particle_count - 1)
          particles[particle_index] = particles[particle_count - 1]
        end

        self.particle_count -= 1

      end


    end

  end

  def stop_particle_emitter
    self.active = false
    self.elapsed_time = 0
    self.emit_counter = 0
  end

  def add_particle
    # puts "Adding Particle"

    return false if particle_count == ped.max_particles

    particle = particles[particle_count]

    init_particle(particle)

    self.particle_count += 1

    true
  end

  def rand1
    [ -1, 1 ].sample
  end

  def maximum_update_rate
    90.0
  end

  def source_position
    ped.source_position
  end

  def source_position_var
    ped.source_position_var
  end

  def init_particle(particle)

    particle.position.x = source_position.x + source_position_var.x * rand1
    particle.position.y = source_position.y + source_position_var.y * rand1
    particle.start_position.x = source_position.x
    particle.start_position.y = source_position.y

    new_angle = DEGREES_TO_RADIANS(ped.angle + ped.angle_var * rand1)
    vector = Vector2f.new cosf(new_angle), sinf(new_angle)
    vector_speed = ped.speed + ped.speed_var * rand1

    particle.direction = Vector2fMultiply(vector,vector_speed)

    particle.radius = ped.max_radius + ped.max_radius_var * rand1
    particle.radius_delta = (ped.max_radius / ped.life_span) * (1.0 / maximum_update_rate)
    particle.angle = DEGREES_TO_RADIANS(ped.angle + ped.angle_var * rand1)
    particle.degrees_per_second = DEGREES_TO_RADIANS(ped.rotate_per_second + ped.rotate_per_second_var * rand1)

    particle.radial_acceleration = ped.radial_acceleration
    particle.tangential_acceleration = ped.tangential_acceleration

    particle.time_to_live = max(0,ped.life_span + ped.life_span_var)

    particle_start_size = ped.start_particle_size + ped.start_particle_size_var * rand1
    particle_finish_size = ped.finish_particle_size + ped.finish_particle_size_var * rand1

    particle.particle_size_delta = ((particle_finish_size - particle_start_size) / particle.time_to_live) * (1.0 / maximum_update_rate)
    particle.particle_size = max(0,particle_start_size)

    start = ParticleColor.new
    start.red = ped.start_color.red + ped.start_color_var.red * rand1
    start.green = ped.start_color.green + ped.start_color_var.green * rand1
    start.blue = ped.start_color.blue + ped.start_color_var.blue * rand1
    start.alpha = ped.start_color.alpha + ped.start_color_var.alpha * rand1

    color_end = ParticleColor.new
    color_end.red = ped.finish_color.red + ped.finish_color_var.red * rand1
    color_end.green = ped.finish_color.green + ped.finish_color_var.green * rand1
    color_end.blue = ped.finish_color.blue + ped.finish_color_var.blue * rand1
    color_end.alpha = ped.finish_color.alpha + ped.finish_color_var.alpha * rand1

    particle.color = start
    particle.delta_color.red = ((color_end.red - start.red) / particle.time_to_live) * (1.0 / maximum_update_rate);
    particle.delta_color.green = ((color_end.green - start.green) / particle.time_to_live) * (1.0 / maximum_update_rate);
    particle.delta_color.blue = ((color_end.blue - start.blue) / particle.time_to_live) * (1.0 / maximum_update_rate);
    particle.delta_color.alpha = ((color_end.alpha - start.alpha) / particle.time_to_live) * (1.0 / maximum_update_rate);


    start_a = ped.rotation_start + ped.rotation_start_var * rand1
    end_a = ped.rotation_end + ped.rotation_end_var * rand1
    particle.rotation = start_a
    particle.rotation_delta = (end_a - start_a) / particle.time_to_live

  end

  def maximum_update_rate
    90.0
  end

  def max(a,b)
    [ a, b ].max
  end

  def render_particles
    # puts "rendering particles"
    particles.each do |particle|
      particle.render
    end
    
  #
  # // Bind to the verticesID VBO and popuate it with the necessary vertex, color and texture informaiton
  # glBindBuffer(GL_ARRAY_BUFFER, verticesID);
  #
  # // Using glBufferSubData means that a copy is done from the quads array to the buffer rather than recreating the buffer which
  # // would be an allocation and copy. The copy also only takes over the number of live particles. This provides a nice performance
  # // boost.
  # glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ParticleQuad) * particleIndex, quads);
  #
  # // Configure the vertex pointer which will use the currently bound VBO for its data
  # glVertexPointer(2, GL_FLOAT, sizeof(TexturedColoredVertex), 0);
  # glColorPointer(4, GL_FLOAT, sizeof(TexturedColoredVertex), (GLvoid*) offsetof(TexturedColoredVertex, color));
  # glTexCoordPointer(2, GL_FLOAT, sizeof(TexturedColoredVertex), (GLvoid*) offsetof(TexturedColoredVertex, texture));
  #
  # // Bind to the particles texture
  # glBindTexture(GL_TEXTURE_2D, texture.name);
  #
  # // Set the blend function based on the configuration
  # glBlendFunc(blendFuncSource, blendFuncDestination);
  #
  # // Now that all of the VBOs have been used to configure the vertices, pointer size and color
  # // use glDrawArrays to draw the points
  # glDrawElements(GL_TRIANGLES, particleIndex * 6, GL_UNSIGNED_SHORT, indices);
  #
  # // Unbind the current VBO
  # glBindBuffer(GL_ARRAY_BUFFER, 0);
  #
  # glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  #
  #
  end

end


class MyWindow < Gosu::Window

  def initialize(width,height,fullscreen)
    super(width,height,fullscreen)

    @pe = ParticleEmitter.new(self,"purple.pex")
  end

  def update
    @pe.update
  end

  def draw
    @pe.render_particles
  end


end


$window = MyWindow.new(200,200,false)
$window.show
