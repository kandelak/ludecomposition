.intel_syntax noprefix
.global ludecomp_asm2
 


ludecomp_asm2:
    push rbx
    push r12
    push r13
    push r14
    push r15
  
# rdi = n      (unsigned long int)
# rsi = A      (pointer)
# rdx = L      (pointer)
# rcx = U      (pointer)
# r8  = P      (pointer)


cmp rdi,0
jbe .L_end


# Copy A in U

# Vectorized
mov rax,rdi
imul rax,rdi


# Copying Pointers
mov rbx, rsi
mov r12, rcx

.L_copyA:
        cmp rax,4
        jl .L_copyA_scalar
        movups   xmm0, xmmword ptr [rbx]
        movups   xmmword ptr [r12], xmm0
        add rbx,16
        add r12,16
        sub rax,4
        jmp .L_copyA

# Scalar
.L_copyA_scalar:
        cmp rax,0
        jle .L_copyA_end
        movss   xmm0, dword ptr [rbx]
        movss dword ptr [r12], xmm0
        add rbx,4
        add r12,4
        dec rax
        jmp .L_copyA_scalar

.L_copyA_end:





# Writing Identity matrices in P and L


mov rax,rdi
imul rax,rdi


pxor xmm0,xmm0
movss xmm1, dword ptr [rip+.L_V1]

# Copying Registers (Addresses)
mov rbx,r8
mov r12,rdx

# Vectorized
.L_ident:
mov r13,rdi
movss dword ptr [r12],xmm1
movss dword ptr [rbx],xmm1
add r12,4
add rbx,4
dec rax

cmp rax,0
je .L_ident_end

.L_write_nulls:
cmp r13,4
jl .L_write_nulls_scalar
movups xmmword ptr [r12],xmm0
movups xmmword ptr [rbx],xmm0
add r12,16
add rbx,16
sub r13,4
sub rax,4
jmp .L_write_nulls

# Scalar
.L_write_nulls_scalar:
cmp r13,0
je .L_ident
movss dword ptr [r12],xmm0
movss dword ptr [rbx],xmm0
add r12,4
add rbx,4
dec r13
dec rax
jmp .L_write_nulls_scalar


.L_ident_end:


# Starting Decomposition

# For Loops
mov rax,rdi
mov r9,rdi
dec r9

# Copying Registers (Addresses)
# L
lea r10,[rdx+4*rdi]


# Address for all Entries in each Column (Under each Leading Entry)
# U
lea r15,[rcx+4*rdi]
mov r11,rcx
mov r13,r15

# P
mov r14,r8



.L_loop1:
dec rax
mov r15,r13
mov r11,rcx
cmp rax,0
jle .L_loop2

jmp .L_factor_and_subtract

.L_loop2:
dec r9
cmp r9,0
jle .L_loop1_end
mov rax,r9



lea rcx,[rcx+4*rdi+4]


# Updating Addresses for Matrix U and L
lea r15,[rcx+4*rdi]
mov r11,rcx

lea rdx,[rdx+4*rdi+4]
lea r10,[rdx+4*rdi]


# Pivoting 
# Swap with Row addressed with r15 if needed
mov rbx,r9
.L_find_max:
dec rbx
cmp rbx,0
jle .L_swap
movss xmm0,dword ptr [r15]



.L_swap




.L_factor_and_subtract:

# Calculating Factor and Writing in L
movss xmm0,dword ptr [rcx]
movss xmm1,dword ptr [r15]
divss xmm1,xmm0
movss [r10],xmm1

lea r10,[r10+4*rdi]
lea r13,[r15+4*rdi]

# Factors
shufps xmm1,xmm1,0x0

# Updating Row of the U

# Counter
mov rbx,r9
inc rbx

# Vectorized

 .L_subtract:
  
  cmp rbx,4
  jl .L_subtract_scalar
  
  movups xmm0,xmmword ptr [r11]
  movups xmm2,xmmword ptr [r15]
  mulps xmm0,xmm1
  subps xmm2,xmm0
  
  movups xmmword ptr [r15],xmm2
  
  add r15,16
  add r11,16
  sub rbx,4
  jmp .L_subtract
 
# Scalar

.L_subtract_scalar:
cmp rbx,0
jle .L_loop1

 movss  xmm0,dword ptr[r11]
 movss xmm2,dword ptr [r15]
 mulss xmm0,xmm1
 subss xmm2,xmm0
 movss dword ptr [r15],xmm2

add r15,4
add r11,4
dec rbx
jmp .L_subtract_scalar


.L_loop1_end:


.L_end:

pop r15
pop r14
pop r13
pop r12
pop rbx

 
ret

.L_V1:
    .long 1065353216

