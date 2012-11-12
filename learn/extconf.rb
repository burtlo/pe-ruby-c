require 'mkmf'

extension_name = "learn/learn"
dir_config(extension_name)

case RUBY_PLATFORM
  when /darwin/
    # Everyone on OSX has plenty of OpenGL to go around.
    $LDFLAGS <<  " -framework OpenGL -framework CoreGraphics"

  when /win32|mingw/
    gl_path = File.expand_path("../vendor/gl", __FILE__)
    $LDFLAGS << %{ -L"#{gl_path}/lib"}
    $CFLAGS  << %{ -I"#{gl_path}/include"}
    exit unless have_library('opengl32.lib', 'glVertex3d') || have_library('opengl32')

    exit unless have_header 'GL/gl.h'

  else
    $LDFLAGS << " -lGL"

    # You are on Linux, so everything is hunky dory!
    exit unless have_library 'GL'
end

# 1.9 compatibility
$CFLAGS << ' -DRUBY_19' if RUBY_VERSION =~ /^1.9/

# let's use a nicer C (rather than C90)
$CFLAGS << " -std=gnu99"

# Make it possible to use a debugger.
#$CFLAGS << " -g -O0"

# Stop getting annoying warnings for valid C99 code.
$warnflags.gsub!('-Wdeclaration-after-statement', '') if $warnflags
$warnflags << ' -Wall' # Let's be good boys and girls!


create_header
create_makefile extension_name
