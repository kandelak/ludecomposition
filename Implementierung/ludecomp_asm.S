.intel_syntax noprefix

.global ludecomp_asm


# void ludecomp_asm(size_t n, const float *A, float *L, float *U, float *P)
ludecomp_asm:
    push rbx
    push r12
    push r13
    push r14
    push r15
    # rdi = n      (int)
    # (rax = n*n)  (int)
    # (rbx = n*4)  Offset pro zeile (n*4)
    # rsi = A      (pointer)
    # rdx = L      (pointer)
    # rcx = U      (pointer)
    # r8  = P      (pointer)

    # ggf fehler erkennung wenn n=0
    cmp rdi, 0
    jbe .L_end
    
    mov rax, rdi        # rax = n*n
    imul rax, rax
    lea rbx, [0+rdi*4]  # Offset pro zeile (n*4)



# ################### Initializing L & U & P ######################

    # Copying A in U
    xor r9, r9
    .L_copyA:
        movss   xmm0, dword ptr [rsi+r9*4]
        movss   dword ptr [rcx+r9*4], xmm0
        inc     r9
        cmp     rax, r9
        jne     .L_copyA
  

    # Writing Identity matrices in P and L
    xor r9, r9          # Zeile  i   
    xor r10, r10        # Spalte j
    xor r12, r12        # Offset für x Zeilen
    pxor    xmm0, xmm0                  # 0.0 in xmm0
    movss   xmm1, dword ptr .L_V1[rip]  # 1.0 in xmm1

    .L_createPL:
        # in xmm4 danach 0.0 oder 1.0 wenn i==j
        cmp     r9, r10
        jne     .L_set0          
        movaps  xmm4, xmm1   # wenn i == j -> 1.0
        jmp     .L_fillPL
        .L_set0: movaps xmm4, xmm0 # wenn i != j -> 0.0

        .L_fillPL:
            # in L & P an [i][j] danach xmm4
            lea r11, [r12+r10*4] # temp für L + i*offset
            movss   dword ptr [r8 + r11], xmm4 
            movss   dword ptr [rdx + r11], xmm4

            # erhöht i/j je nachdem ob i/j schon == n 
            inc r10
            cmp r10, rdi 
            jne .L_createPL # wenn j == n: für nächste Spalte wiederholen
            inc r9          # wenn j == n: nächste Zeile (i++) 
            add r12, rbx    # eine weitere Zeile im Offset
            xor r10, r10    # j = 0
            cmp r9, rdi
            jne .L_createPL # wenn nicht für nächste Zeile weiter wiederholen
                            # wenn i == n feritg



# ################# filling L & U #######################
push r8
# rdi = n      (int)
# (rax = n*n)  (int)
# (rbx = n*4)  Offset pro zeile (n*4)
# rsi = A      (pointer)
# rdx = L      (pointer)
# rcx = U      (pointer)

# r9  = i (in 4er Schritten bis n*4)
# r15 = i (in Ner Schritten bis n*n) 
# r10 = j (in 4er SChritten ab r9+4)
# r8  = j (in Ner SChritten ab r10+n)
# r11 = x (Zelle in Zeile in 4er Schritten)


    # for-Schleife für jede Stufe
    xor r9, r9      # i = 0 (bis n*4)
    xor r15,r15
    .L_forSteps:    
        lea r12, [r9+r15*4] # = (i*4) + (i*n*4)
        movss xmm1, dword ptr [rcx+r12] # = führender Eintrag der Aktuellen Stufe

        # for-Schleife für jede Spalte ab i (exklusiv)
        mov r10, r9 # j = i + 4
        add r10, 4
        mov r8, r15
        add r8, rdi 
        .L_forRows:
            lea r13, [r9+r8*4] # = (i*4) + (j*n*4)
            movss xmm0, dword ptr [rcx+r13]
            divss xmm0, xmm1 # xmm0 = faktor
            movss dword ptr [rdx+r12], xmm0 # faktor in L

            # for-Schleife für jede Zelle in der Zeile
            xor r11, r11  # x = 0 (bis n*4)
            .L_forCells:
                lea r14, [r11+r15*4] # = (x*4) + (i*n*4)
                movss xmm2, dword ptr [rcx+r14]
                mulss xmm2, xmm0
                lea r14, [r11+r8*4] # = (x*4) + (i*n*4)
                movss xmm3, dword ptr [rcx+r14]
                subss xmm3, xmm2 # zelle -= faktor*zelle der i.Zeile
                movss dword ptr [rcx+r14], xmm3


                add r11, 4
                cmp rbx, r11
                jne .L_forCells # (bis n*4)
            # ende forCells

            add r8, rdi 
            add r10, 4
            cmp r10, rbx
            jl .L_forRows # (bis n*4)
        # ende forRows

        add r15, rdi
        add r9, 4
        cmp rbx, r9
        jne .L_forSteps # (bis n*4)
    # ende forSteps

pop r8




# for (i = 0; i < n; i++)
#    for (j = i+1; j < n; j++) {
#       faktor = RCX+[(j*n*4) + (i*4)] / RCX+[(i*n*4) + (i*4)];
#       RDX+[(i*n*4) + (i*4)] = faktor;
#       for (x = 0; x < n; x++) {
#           temp = factor * RCX+[(i*n*4) + (x*4)];
#           RCX+[(j*n*4) + (x*4)] -= temp;
#       }
#    }
# }

# for (i = 0; i < n; i++)
#    for (j = i+1; j < n; j++) {
#       faktor = RCX+[(j*n*4) + (i*4)] / RCX+[(i*n*4) + (i*4)];
#       RDX+[(i*n*4) + (i*4)] = faktor;
#       for (x = 0; x < n; x++) {
#           temp = factor * RCX+[(i*n*4) + (x*4)];
#           RCX+[(j*n*4) + (x*4)] -= temp;
#       }
#    }
# }



# i*4 = r9
# j*4 = r10
# x*4 = r11
# n*4 = rbx
# n = rdi
#
# i = 0
# for (i<rbx) {}
#
#     add = (r9*n) + (r9)
#     R16 = RCX+[add]
#     j = i+4
#     for (j < rbx) {
#         faktor = RCX+[(r10*n) + (r9)] / R16;
#         RDX+[add] = faktor;
# 
#         x = 0
#         for (x < rbx) {
#          # for (i = 0; i < n; i++)
#    for (j = i+1; j < n; j++) {
#       faktor = RCX+[(j*n*4) + (i*4)] / RCX+[(i*n*4) + (i*4)];
#       RDX+[(i*n*4) + (i*4)] = faktor;
#       for (x = 0; x < n; x++) {
#           temp = factor * RCX+[(i*n*4) + (x*4)];
#           RCX+[(j*n*4) + (x*4)] -= temp;
#       }
#    }
# }

# for (i = 0; i < n; i++)
#    for (j = i+1; j < n; j++) {
#       faktor = RCX+[(j*n*4) + (i*4)] / RCX+[(i*n*4) + (i*4)];
#       RDX+[(i*n*4) + (i*4)] = faktor;
#       for (x = 0; x < n; x++) {
#           temp = factor * RCX+[(i*n*4) + (x*4)];
#           RCX+[(j*n*4) + (x*4)] -= temp;
#       }
#    }
# }
# 
# } i+=4





.L_end:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    ret


.L_V1:
    .long 1065353216

