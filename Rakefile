Options = "-Wall -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin"

task :default => :bochs

task :bochs => :build do
  sh "bochs -q -f .bochsrc"
end
  
task :build => 'kernel.img'

file 'boot.o' => ['boot.S'] do
  sh "nasm -f elf boot.S"
end

file 'main.o' => ['main.c'] do 
  sh "gcc #{Options} -c main.c"
end

file 'boot.bin' => ['boot.o', 'main.o', 'img.ld'] do 
  sh "ld boot.o -o boot.bin -e c -T boot.ld"
  sh "ld main.o -o main.bin -e c -T main.ld"
end

file 'kernel.img' => ['boot.bin', 'main.bin'] do
  sh "cat boot.bin main.bin > kernel.img"
end

task :clean do
  sh "rm -rf boot.o main.o boot.bin main.bin boot.img"
end
