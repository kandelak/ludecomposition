.intel_syntax noprefix
 .global ludecomp_asm2
 

 ludecomp_asm2:
  push rax
  
# rdi = n      (int)
    # (rax = n*n)  (int)
    # (rbx = n*4)  Offset pro zeile (n*4)
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

push rsi
push rcx

.L_copyA:
        cmp rax,4
        jl .L_copyA_scalar
        movups   xmm0, [rsi]
        movups   [rcx], xmm0
        add rsi,16
        add rcx,16
        sub rax,4
        jmp .L_copyA

# Scalar
.L_copyA_scalar:
        cmp rax,0
        jle .L_copyA_end
        movss   xmm0,[rsi]
        movss [rcx], xmm0
        add rsi,4
        add rcx,4
        dec rax
        jmp .L_copyA_scalar

.L_copyA_end:
pop rcx 
pop rsi


# Writing Identity matrices in P and L

mov rax,rdi
imul rax,rdi


pxor xmm0,xmm0
movss xmm1,[rip+.L_V1]
shufps xmm1,xmm1,0x0
push r8
push rdx

# Vectorized
.L_ident:
mov rbx,rdi
movss [rdx],xmm1
movss [r8],xmm1
add rdx,4
add r8,4
dec rax

cmp rax,0
je .L_ident_end

.L_write_nulls:
cmp rbx,4
jl .L_write_nulls_scalar
movups [rdx],xmm0
movups [r8],xmm0
add rdx,16
add r8,16

# Scalar
.L_write_nulls_scalar:
cmp rbx,0
je .L_ident
movss [rdx],xmm0
movss [r8],xmm0
add rdx,4
add r8,4
dec rbx
jmp .L_write_nulls_scalar

.L_ident_end:
pop rdx 
pop r8


.L_end:
   pop rax
 ret
.L_V1:
    .long 1065353216