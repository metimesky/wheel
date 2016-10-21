if exist build rmdir /s /q build
mkdir build
cd build
bash -c "PATH=../../crosstools/bin/:$PATH make -f ../Makefile" && "C:\Program Files\qemu\qemu-system-x86_64.exe" -no-kvm -smp 2 -m 64 -cdrom cd.iso
cd ..