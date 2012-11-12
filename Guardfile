# Add files and commands to this file, like the example:
#   watch(%r{file/path}) { `command(s)` }
#
guard 'shell' do
  watch(/learn_test\.rb|learn\/(.*\.(?:rb|h|c))/) do |m|
    file = m[0]
    
    puts "#{file} has changed"
    if file !~ /^(extconf\.h|Makefile|mkmf\.log)$/
      `./build.sh`
    end
  end

end
