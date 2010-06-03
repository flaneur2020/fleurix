#!/usr/bin/ruby
# make a sign for boot
# \x55\xAA 
open(ARGV[0], 'r+') do |f|

  fsize = f.stat.size
  if fsize > 510 
    puts "boot binary too big! #{fsize}" 
    exit
  end

  buf = f.read
  buf += "\0" * (510-fsize*2)
  buf += "\x55\xAA"
  puts buf.size
  f.write(buf)
end
