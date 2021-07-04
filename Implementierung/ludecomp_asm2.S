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


# Saving Pointers
mov rbx, rsi
mov r12, rcx

.L_copyA:
        cmp rax,4
        jl .L_copyA_scalar
        movups   xmm0, [rbx]
        movups   [r12], xmm0
        add rbx,16
        add r12,16
        sub rax,4
        jmp .L_copyA

# Scalar
.L_copyA_scalar:
        cmp rax,0
        jle .L_copyA_end
        movss   xmm0,[rbx]
        movss [r12], xmm0
        add rbx,4
        add r12,4
        dec rax
        jmp .L_copyA_scalar

.L_copyA_end:





# Writing Identity matrices in P and L


mov rax,rdi
imul rax,rdi


pxor xmm0,xmm0
movss xmm1,[rip+.L_V1]

# Copying Registers (Addresses)
mov rbx,r8
mov r12,rdx

# Vectorized
.L_ident:
mov r13,rdi
movss [r12],xmm1
movss [rbx],xmm1
add r12,4
add rbx,4
dec rax

cmp rax,0
je .L_ident_end

.L_write_nulls:
cmp r13,4
jl .L_write_nulls_scalar
movups [r12],xmm0
movups [rbx],xmm0
add r12,16
add rbx,16
sub r13,4
sub rax,4
jmp .L_write_nulls

# Scalar
.L_write_nulls_scalar:
cmp r13,0
je .L_ident
movss [r12],xmm0
movss [rbx],xmm0
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



.L_decomp:
dec rax
mov r15,r13
mov r11,rcx
cmp rax,0
jle .L_cond

jmp .L_factor_and_subtract

.L_cond:
dec r9
cmp r9,0
jle .L_decomp_end
mov rax,r9

# Updating Addresses for Matrix U and L

lea rcx,[rcx+4*rdi+4]
lea r15,[rcx+4*rdi]
mov r11,rcx

lea rdx,[rdx+4*rdi+4]
lea r10,[rdx+4*rdi]



.L_factor_and_subtract:

# Calculating Factor and Writing in L
movss xmm0,[rcx]
movss xmm1,[r15]
divss xmm1,xmm0
movss [r10],xmm1

lea r10,[r10+4*rdi]
lea r13,[r15+4*rdi]

# Factors
shufps xmm1,xmm1,0x0

# Updating Row of the U

# Counter
mov rbx,rdi

# Vectorized



.L_subtract:

cmp rbx,4
jl .L_subtract_scalar

movups xmm0,[r11]
movups xmm2,[r15]
mulps xmm0,xmm1
subps xmm2,xmm0

movups [r15],xmm2

add r15,16
add r11,16
sub rbx,4
jmp .L_subtract

# Scalar

.L_subtract_scalar:
cmp rbx,0
jle .L_decomp

movss  xmm0,[r11]
movss xmm2,[r15]
mulss xmm0,xmm1
subss xmm2,xmm0
movss [r15],xmm2

add r15,4
add r11,4
dec rbx
jmp .L_subtract_scalar


.L_decomp_end:


.L_end:

pop r15
pop r14
pop r13
pop r12
pop rbx

 
ret

.L_V1:
    .long 1065353216

