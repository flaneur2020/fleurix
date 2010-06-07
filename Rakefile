Incs    = '-Isrc'
Flags   = "-Wall -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin"

task :default => :bochs

task :bochs => :build do
  sh "bochs -q -f .bochsrc"
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
file 'main.bin' => ['entry.o', 'main.o', 'video.o', 'idt.o', 'main.ld'] do
  sh "ld entry.o main.o video.o idt.o -o main.bin -e c -T main.ld"
end

file 'entry.o' => ['src/entry.S'] do
  sh "nasm -f elf -o entry.o src/entry.S"
end

file 'main.o' => ['src/main.c'] do 
  sh "gcc #{Flags} #{Incs} -o main.o -c src/main.c"
end

file 'video.o' => ['src/video.c'] do
  sh "gcc #{Flags} #{Incs} -o video.o -c src/video.c"
end

file 'idt.o' => ['src/idt.c'] do 
  sh "gcc #{Flags} #{Incs} -o idt.o -c src/idt.c"
end
