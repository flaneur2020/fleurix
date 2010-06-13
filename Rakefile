CIncs   = '-Isrc'
CFlags  = "-Wall -finline-functions -nostdinc -fno-builtin"

task :default => :bochs

task :bochs => :build do
  sh "bochs -q -f .bochsrc"
end

task :debug => :build do
  sh "bochsd -q -f .bochsrc"
end
  
task :build => 'bin/kernel.img'

task :clean do
  sh "rm -rf bin/* .bochsout"
end

#######################################################################
# => kernel.img
#######################################################################
file 'bin/kernel.img' => ['bin/boot.bin', 'bin/main.bin'] do
  sh "cat bin/boot.bin bin/main.bin > bin/kernel.img"
end

#######################################################################
# => boot.bin
#######################################################################
file 'bin/boot.o' => ['src/boot.S'] do
  sh "nasm -f elf -o bin/boot.o src/boot.S"
end

file 'bin/boot.bin' => ['bin/boot.o', 'boot.ld'] do 
  sh "ld bin/boot.o -o bin/boot.bin -e c -T boot.ld"
end

#######################################################################
# mainly C part
# => main.bin
#######################################################################

OFiles = %w{ bin/entry.o bin/main.o bin/print.o bin/idt.o }

file 'bin/main.bin' => OFiles + ['main.ld'] do
  sh "ld #{OFiles * ' '} -o bin/main.bin -e c -T main.ld"
end

file 'bin/entry.o' => ['src/entry.S'] do
  sh "nasm -f elf -o bin/entry.o src/entry.S"
end

[
  ['src/main.c'],
  ['src/print.c'],
  ['src/idt.c']
].each do |fn_c, *_|
  fn_o = 'bin/'+File.basename(fn_c).ext('o')
  file fn_o => [fn_c, *_] do
    sh "gcc #{CFlags} #{CIncs} -o #{fn_o} -c #{fn_c}"
  end
end
