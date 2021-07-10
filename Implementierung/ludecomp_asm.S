.intel_syntax noprefix
.global ludecomp_asm_simd
.global ludecomp_asm

# Assembler Implementation without SIMD Intructions (Scroll down for vectorized version)
ludecomp_asm:
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
jbe .L_loop1_end_


# Copy A in U


mov rax,rdi
imul rax,rdi


# Copying A in U
mov rbx, rsi
mov r12, rcx

.L_copyA_:
        cmp rax,0
        jle .L_copyA_end_
        movss   xmm0, dword ptr [rbx]
        movss dword ptr [r12], xmm0
        add rbx,4
        add r12,4
        dec rax
        jmp .L_copyA_

.L_copyA_end_:





# Writing Identity matrices in P and L


mov rax,rdi
imul rax,rdi


pxor xmm0,xmm0
movss xmm1, dword ptr [rip+.L_V1]

# rbx := P
# rax := L
mov rbx,r8
mov r12,rdx


.L_ident_:
mov r13,rdi
movss dword ptr [r12],xmm1
movss dword ptr [rbx],xmm1
add r12,4
add rbx,4
dec rax

cmp rax,0
je .L_ident_end_

.L_write_nulls_:
cmp r13,0
je .L_ident_
movss dword ptr [r12],xmm0
movss dword ptr [rbx],xmm0
add r12,4
add rbx,4
dec r13
dec rax
jmp .L_write_nulls_


.L_ident_end_:




# Starting Decomposition

# For Loops
mov rax,rdi
mov r9,rdi
dec r9


# r10 : handle for L
lea r10,[rdx+4*rdi]



# r15 : handle for U
lea r15,[rcx+4*rdi]


mov r11,rcx

# r14 : handle for r8
mov r14,r8


# Pivoting 
.L_pivotize_:

mov rax,r9
inc rax

# r11 : handle for U during Pivoting 
mov r11,rcx
mov r15,rcx

# r14 :  counts offset for swapping
mov r14,0
# r10 : handle for L during Pivoting
mov r10,rdx
mov rbx,0

# Finding max absolute value in given column
.L_find_max_: 
dec rax
cmp rax,0
jle .L_handle_address_

lea r11,[r11+4*rdi]


# Comparision of column entries

movss xmm0,dword ptr [r15]

# setting to absolute value
xorps xmm3,xmm3
subps xmm3,xmm0
maxps xmm0,xmm3


movss xmm1,dword ptr [r11]

# setting to absolute value
xorps xmm3,xmm3
subps xmm3,xmm1
maxps xmm1,xmm3


comiss xmm1,xmm0

jbe .L_find_max_

# Saving handle to the row with which has to be swapped in U
mov r15,r11

# Saving offset for swapping 
mov rbx,r9
inc rbx
sub rbx,rax

# Saving for later use to swap columns in P
mov r14,rbx

jmp .L_find_max_

# Setting handle to the row with which has to be swapped in L
.L_handle_address_:

cmp rbx,0
jle .L_swap_
lea r10,[r10+4*rdi]
dec rbx
jmp .L_handle_address_


# Swapping Rows in U
# swap([r11],[r15])
.L_swap_:
cmp r15,rcx
je .L_swap_end_

mov r11,rcx
mov rax,r9
inc rax

.L_swap_scalar_U_:
cmp rax,0
jle .L_swap_L_


movss xmm1 , dword ptr [r11]
movss xmm0 , dword ptr [r15] 
movss dword ptr [r11] , xmm0
movss dword ptr [r15] , xmm1

dec rax
add r11,4
add r15,4

jmp .L_swap_scalar_U_


# Swapping Rows in L 
# Swap([r12],[r10])
.L_swap_L_:
mov r12,rdx
mov rax,rdi
sub rax,r9
cmp rax,1 
jle .L_swap_P_

.L_swap_scalar_L_:
cmp rax,0
jle .L_swap_P_

sub r12,4
sub r10,4

movss xmm1 , dword ptr [r12]
movss xmm0 , dword ptr [r10] 
movss dword ptr [r12] , xmm0
movss dword ptr [r10] , xmm1

dec rax

jmp .L_swap_scalar_L_


# Swapping Columns in P
# r14 : Offset for swapping columns
# r11 : Offset for swapping columns
.L_swap_P_:
cmp r14,0
jle .L_swap_end_
mov rbx,r8
mov rax,rdi
mov r11,rdi
sub r11,r9
dec r11
add r14,r11

.L_swap_scalar_P_:
cmp rax,0
jle .L_swap_end_

# Swapping in P
movss xmm1 , dword ptr [rbx+4*r11]
movss xmm0 , dword ptr [rbx+4*r14] 
movss dword ptr [rbx+4*r14] , xmm1
movss dword ptr [rbx+4*r11] , xmm0

lea rbx,[rbx+4*rdi]
dec rax
jmp .L_swap_scalar_P_

.L_swap_end_:

lea r15,[rcx+4*rdi]
mov r13,r15

mov r11,rcx

lea r10,[rdx+4*rdi]

mov rax,r9
inc rax

.L_loop1_:
dec rax
mov r15,r13
mov r11,rcx
cmp rax,0
jle .L_loop2_

jmp .L_factor_and_subtract_

.L_loop2_:
dec r9
cmp r9,0
jle .L_loop1_end_

# Updating Addresses for Matrix U and L

lea rcx,[rcx+4*rdi+4]

lea rdx,[rdx+4*rdi+4]


jmp .L_pivotize_



.L_factor_and_subtract_:
xorps xmm3,xmm3
# Calculating Factor and Writing in L
movss xmm0,dword ptr [rcx]

# Check if the Matrix can be decomposed
comiss xmm0,xmm3
je .L_not_decomposed_

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

# Scalar

.L_subtract_scalar_:
cmp rbx,0
jle .L_loop1_

 movss  xmm0,dword ptr[r11]
 movss xmm2,dword ptr [r15]
 mulss xmm0,xmm1
 subss xmm2,xmm0
 movss dword ptr [r15],xmm2

add r15,4
add r11,4
dec rbx
jmp .L_subtract_scalar_


.L_not_decomposed_:
mov rax,0
pop r15
pop r14
pop r13
pop r12
pop rbx
ret
.L_loop1_end_:
# If the last entry in the U is 0 -> can not be decomposed
lea rcx,[rcx+4*rdi+4]
xorps xmm3,xmm3
movss xmm0,dword ptr [rcx]
comiss xmm0,xmm3
je .L_not_decomposed_
mov rax,1
pop r15
pop r14
pop r13
pop r12
pop rbx

ret




# SIMD vectorized Assembler version of LU-Decomposition
ludecomp_asm_simd:
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
jbe .L_loop1_end


# Copy A in U


mov rax,rdi
imul rax,rdi


# Copying Pointers
mov rbx, rsi
mov r12, rcx

# Vectorized
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


# Handle to L
lea r10,[rdx+4*rdi]



# Handle to U
lea r15,[rcx+4*rdi]


mov r11,rcx

# Handle to P
mov r14,r8

# Pivoting 
.L_pivotize:

mov rax,r9
inc rax
# Handle to U during  Pivoting
mov r11,rcx
mov r15,rcx
# r14 :  counts offset for swapping
mov r14,0
# Handle to L during Pivoting
mov r10,rdx
mov rbx,0

# Finding max abolute value in given column
.L_find_max: 
dec rax
cmp rax,0
jle .L_handle_address

lea r11,[r11+4*rdi]

# Comparision of column entries

movss xmm0,dword ptr [r15]

# setting to absolute value
xorps xmm3,xmm3
subps xmm3,xmm0
maxps xmm0,xmm3


movss xmm1,dword ptr [r11]

# setting to absolute value
xorps xmm3,xmm3
subps xmm3,xmm1
maxps xmm1,xmm3


comiss xmm1,xmm0

jbe .L_find_max

mov r15,r11

# Saving offset for swapping
mov rbx,r9
inc rbx
sub rbx,rax

# Saving for later use to swap columns in P
mov r14,rbx

jmp .L_find_max

.L_handle_address:

cmp rbx,0
jle .L_swap
lea r10,[r10+4*rdi]
dec rbx
jmp .L_handle_address


# Swapping Rows in U
# swap([r11],[r15])
.L_swap:
cmp r15,rcx
je .L_swap_end

mov r11,rcx
mov rax,r9
inc rax


# Vectorized
.L_swap_vect_U:
cmp rax,4
jl .L_swap_scalar_U


movups xmm1 , xmmword ptr [r11]
movups xmm0 , xmmword ptr [r15] 
movups xmmword ptr [r11] , xmm0
movups xmmword ptr [r15] , xmm1


sub rax,4
add r11,16
add r15,16

jmp .L_swap_vect_U

# Scalar
.L_swap_scalar_U:
cmp rax,0
jle .L_swap_L


movss xmm1 , dword ptr [r11]
movss xmm0 , dword ptr [r15] 
movss dword ptr [r11] , xmm0
movss dword ptr [r15] , xmm1

dec rax
add r11,4
add r15,4

jmp .L_swap_scalar_U


# Swapping Rows in L 
# Swap([r12],[r10])
.L_swap_L:
mov r12,rdx
mov rax,rdi
sub rax,r9
cmp rax,1 
jle .L_swap_P

# Vectorized
.L_swap_vect_L:
cmp rax,4
jl .L_swap_scalar_L

sub r12,16
sub r10,16

movups xmm1 , xmmword ptr [r12]
movups xmm0 , xmmword ptr [r10] 
movups xmmword ptr [r12] , xmm0
movups xmmword ptr [r10] , xmm1

sub rax,4

jmp .L_swap_vect_L

# Scalar
.L_swap_scalar_L:
cmp rax,0
jle .L_swap_P

sub r12,4
sub r10,4

movss xmm1 , dword ptr [r12]
movss xmm0 , dword ptr [r10] 
movss dword ptr [r12] , xmm0
movss dword ptr [r10] , xmm1

dec rax

jmp .L_swap_scalar_L


# Swapping Columns in P
# r14 : Offset for swapping columns
# r11 : Offset for swapping columns
.L_swap_P:
cmp r14,0
jle .L_swap_end
mov rbx,r8
mov rax,rdi
mov r11,rdi
sub r11,r9
dec r11
add r14,r11

.L_swap_scalar_P:
cmp rax,0
jle .L_swap_end

# Swapping in P
movss xmm1 , dword ptr [rbx+4*r11]
movss xmm0 , dword ptr [rbx+4*r14] 
movss dword ptr [rbx+4*r14] , xmm1
movss dword ptr [rbx+4*r11] , xmm0

lea rbx,[rbx+4*rdi]
dec rax
jmp .L_swap_scalar_P

.L_swap_end:

lea r15,[rcx+4*rdi]
mov r13,r15

mov r11,rcx

lea r10,[rdx+4*rdi]

mov rax,r9
inc rax

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

# Updating Addresses for Matrix U and P and L

lea rcx,[rcx+4*rdi+4]

lea rdx,[rdx+4*rdi+4]


jmp .L_pivotize



.L_factor_and_subtract:

xorps xmm3,xmm3
# Calculating Factor and Writing in L
movss xmm0,dword ptr [rcx]

# Check if the Matrix can be decomposed
comiss xmm0,xmm3
je .L_not_decomposed

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

.L_not_decomposed:
mov rax,0
pop r15
pop r14
pop r13
pop r12
pop rbx

ret

.L_loop1_end:
# If the last entry in the U is 0 -> can not be decomposed
lea rcx,[rcx+4*rdi+4]
xorps xmm3,xmm3
movss xmm0,dword ptr [rcx]
comiss xmm0,xmm3
je .L_not_decomposed
mov rax,1
pop r15
pop r14
pop r13
pop r12
pop rbx

ret



.L_V1:
    .long 1065353216

