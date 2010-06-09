CIncs   = '-Isrc'
CFlags  = "-Wall -finline-functions -nostdinc -fno-builtin"

task :default => :bochs

task :bochs => :build do
  sh "bochs -q -f .bochsrc"
end

task :debug => :build do
  sh "bochsd -q -f .bochsrc"
end
  
task :build => 'kernel.img'

task :clean do
  sh "rm -rf *.o *.bin *.img .bochsout"
end

#######################################################################
# => kernel.img
#######################################################################
file 'kernel.img' => ['boot.bin', 'main.bin'] do
  sh "cat boot.bin main.bin > kernel.img"
end

#######################################################################
# => boot.bin
#######################################################################
file 'boot.o' => ['src/boot.S'] do
  sh "nasm -f elf -o boot.o src/boot.S"
end

file 'boot.bin' => ['boot.o', 'boot.ld'] do 
  sh "ld boot.o -o boot.bin -e c -T boot.ld"
end

#######################################################################
# mainly C part
# => main.bin
#######################################################################

OFiles = %w{ entry.o main.o print.o idt.o }

file 'main.bin' => OFiles + ['main.ld'] do
  sh "ld #{OFiles * ' '} -o main.bin -e c -T main.ld"
end

file 'entry.o' => ['src/entry.S'] do
  sh "nasm -f elf -o entry.o src/entry.S"
end

[
  ['src/main.c'],
  ['src/print.c'],
  ['src/idt.c']
].each do |fn_c, *_|
  fn_o = File.basename(fn_c).ext('o')
  file fn_o => [fn_c, *_] do
    sh "gcc #{CFlags} #{CIncs} -c #{fn_c}"
  end
end
