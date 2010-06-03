task :default => :bochs

task :bochs => :build do
  sh "bochs -q -f .bochsrc"
end
  
task :build => 'boot.img'

file 'boot.o' => ['boot.S'] do
  sh "nasm -f elf boot.S"
end

file 'main.o' => ['main.c'] do 
  sh "gcc -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -c main.c"
end

file 'boot.bin' => ['boot.o', 'main.o', 'img.ld'] do 
  sh "ld boot.o main.o -o boot.bin -e c -T img.ld"
  sh "ruby ./sign.rb boot.bin"
end

#file 'boot.bin' => ['boot.elf'] do
#  sh "objcopy -R .pdr -R .comment -R .note -S -O binary boot.elf boot.bin"
#end

file 'boot.img' => ['boot.bin'] do
  sh "dd if=boot.bin of=boot.img bs=512 count=1"
  sh "dd if=/dev/zero of=boot.img skip=1 seek=1 bs=512 count=2879"
end

task :clean do
  sh "rm -rf boot.o boot.elf boot.bin boot.img"
end
