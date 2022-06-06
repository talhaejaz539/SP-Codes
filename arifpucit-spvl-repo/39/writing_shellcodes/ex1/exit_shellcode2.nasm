;  Video Lecture: 39
;  Instructor: Arif Butt
;  Course: System Programming with Linux
;  exit_shellcode1.nasm: Assembly program making exit system call
;  Assemble: nasm -felf64 exit_shellcode1.nasm

global _start

SECTION .text
   _start:
     ; mov rax,60
      xor rax, rax
      mov al, 60

      ;mov rdi,0
      xor rdi, rdi
      syscall         ; exit(0)
