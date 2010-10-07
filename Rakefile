CIncs   = '-Isrc/inc'
CFlags  = "-Wall -finline-functions -nostdinc -fno-builtin"

sh "mkdir bin" if not File.exists? 'bin'

task :default => :bochs

task :bochs => :build do
  sh "bochs -q -f .bochsrc"
end

task :nm => :build do 
  sh 'cat main.nmtab'
end

task :debug => :build do
  sh "bochs-dbg -q -f .bochsrc"
end
  
task :build => 'bin/kernel.img'

task :clean do
  sh "rm -rf bin/* src/kern/intv.S .bochsout"
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
file 'bin/boot.o' => ['src/boot/boot.S'] do
  sh "nasm -f elf -o bin/boot.o src/boot/boot.S"
end

file 'bin/boot.bin' => ['bin/boot.o', 'boot.ld'] do 
  sh "ld bin/boot.o -o bin/boot.bin -e c -T boot.ld"
end

#######################################################################
# mainly C part
# => main.bin
#######################################################################

OFiles = %w{ bin/entry.o bin/main.o bin/seg.o bin/sched.o bin/tty.o bin/syscall.o bin/idt.o bin/timer.o bin/page.o bin/intv.o }

file 'bin/main.bin' => 'bin/main.elf' do
  sh "objcopy -R .pdr -R .comment -R .note -S -O binary bin/main.elf bin/main.bin"
end

file 'bin/main.elf' => OFiles + ['main.ld'] do
  sh "ld #{OFiles * ' '} -o bin/main.elf -e c -T main.ld"
  sh "(nm bin/main.elf | sort) > main.nmtab"
end

file 'src/kern/intv.S' => 'src/kern/intv.S.rb' do 
  sh 'ruby src/kern/intv.S.rb > src/kern/intv.S'
end

[
  ['src/kern/intv.S'],
  ['src/kern/entry.S']
].each do |fn_s, *_|
  fn_o = 'bin/'+File.basename(fn_s).ext('o')
  file fn_o => [fn_s, *_] do
    sh "nasm -f elf -o #{fn_o} #{fn_s}"
  end
end

[
  ['src/kern/tty.c'],
  ['src/kern/syscall.c'],
  ['src/kern/sched.c'],
  ['src/kern/seg.c'],
  ['src/kern/idt.c'],
  ['src/kern/page.c'],
  ['src/kern/timer.c'],
  ['src/kern/main.c']
].each do |fn_c, *_|
  fn_o = 'bin/'+File.basename(fn_c).ext('o')
  file fn_o => [fn_c, *_] do
    sh "gcc #{CFlags} #{CIncs} -o #{fn_o} -c #{fn_c} 2>&1"
  end
end
