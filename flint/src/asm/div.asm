;******************************************************************************
;*                                                                            *
;* Functions for arithmetic and number theory with large integers in C        *
;* Software supplement to the book "Cryptography in C and C++"                *
;* by Michael Welschenbach                                                    *
;*                                                                            *
;* Module div.asm          Revision: 18.12.2000                               *
;*                                                                            *
;*  Copyright (C) 1998-2005 by Michael Welschenbach                           *
;*  Copyright (C) 2001-2005 by Springer-Verlag Berlin, Heidelberg             *
;*  Copyright (C) 2001-2005 by Apress L.P., Berkeley, CA                      *
;*  Copyright (C) 2002-2005 by Wydawnictwa MIKOM, Poland                      *
;*  Copyright (C) 2002-2005 by PHEI, P.R.China                                *
;*  Copyright (C) 2002-2005 by InfoBook, Korea                                *
;*  Copyright (C) 2002-2005 by Triumph Publishing, Russia                     *
;*                                                                            *
;*  All Rights Reserved                                                       *
;*                                                                            *
;*  The software may be used for noncommercial purposes and may be altered,   *
;*  as long as the following conditions are accepted without any              *
;*  qualification:                                                            *
;*                                                                            *
;*  (1) All changes to the sources must be identified in such a way that the  *
;*      changed software cannot be misinterpreted as the original software.   *
;*                                                                            *
;*  (2) The statements of copyright may not be removed or altered.            *
;*                                                                            *
;*  (3) The following DISCLAIMER is accepted:                                 *
;*                                                                            *
;*  DISCLAIMER:                                                               *
;*                                                                            *
;*  There is no warranty for the software contained in this distribution, to  *
;*  the extent permitted by applicable law. The copyright holders provide the *
;*  software `as is' without warranty of any kind, either expressed or        *
;*  implied, including, but not limited to, the implied warranty of fitness   *
;*  for a particular purpose. The entire risk as to the quality and           *
;*  performance of the program is with you.                                   *
;*                                                                            *
;*  In no event unless required by applicable law or agreed to in writing     *
;*  will the copyright holders, or any of the individual authors named in     *
;*  the source files, be liable to you for damages, including any general,    *
;*  special, incidental or consequential damages arising out of any use of    *
;*  the software or out of inability to use the software (including but not   *
;*  limited to any financial losses, loss of data or data being rendered      *
;*  inaccurate or losses sustained by you or by third parties as a result of  *
;*  a failure of the software to operate with any other programs), even if    *
;*  such holder or other party has been advised of the possibility of such    *
;*  damages.                                                                  *
;*                                                                            *
;******************************************************************************
;*                                                                            *
;*      Division, interface compatible with C-function div_l()                *
;*                                                                            *
;*      Quotient := Dividend div Divisor                                      *
;*      Remainder := Dividend mod Divisor                                     *
;*                                                                            *
;*      Stack on calling of div_l:        SP+16 ---> Offset remainder         *
;*                                        SP+12 ---> Offset quotient          *
;*                                        SP+ 8 ---> Offset divisor           *
;*                                        SP+ 4 ---> Offset dividend          *
;*                                        SP    ---> Return address           *
;*                                                                            *
;*      Return Value: ax = -1 if divisor = 0                                  *
;*                    ax =  0 else                                            *
;*                                                                            *
;******************************************************************************
PUBLIC div_l
.386
.MODEL FLAT
.CODE
div_l  proc near
;
a       equ     0                       ; Dividend (working copy)
b       equ     1030                    ; Divisor  (working copy)
q       equ     1546                    ; Quotient qhat (working memory)
q1      equ     2566                    ; High-order digit of divisor (normalized)
v1      equ     2570                    ; 2nd digit of divisor (normalized)
v2      equ     2574                    ; Exponent for normalization
d       equ     2578                    ; uj+1 (normalized)
uj1     equ     2582                    ; uj+2 (normalized)
uj2     equ     2586
;
_dvd    equ     [ebp+8]                 ; Dividend  (parameter)
_dvs    equ     [ebp+12]                ; Divisor   (parameter)
_qot    equ     [ebp+16]                ; Quotient  (parameter)
_rst    equ     [ebp+20]                ; Remainder (parameter)
;
WORKSP  equ     2600                    ; Working memory in stack area
;
divs:   push    ebp                     ; Store value for calling procedure
        mov     ebp,esp
        sub     esp,WORKSP              ; Local memory
        mov     eax,esp
        push    ebx
        push    edi
        push    esi
        mov     ebx,eax                 ; Store start address of local memory
;
        mov     esi,_dvd                ; Offset of variable a
        mov     edi,_dvs                ; Offset of variable b
        mov     ax,[esi]                ; length(a)
        mov     dx,[edi]                ; length(b)
        cmp     dx,0                    ; b = 0 ?
        jne     @F
        jmp     divbyz                  ; Division by zero!
;
;>>>>>> Load Operands
;
@@:     push    edi                     ; Store Seg:Offs b
        lea     edi,a[ebx]              ; destination offset is a[bx]
        xor     ecx,ecx
        mov     cx,ax                   ; l(a) in cx
        shr     cx,1
        jnc     @F
        inc     cx
@@:     cld
rep     movsd                           ; Load a as ULONG
        movsw                           ;  plus one USHORT
        mov     cx,ax
        shl     cx,1
        lea     edi,a[ebx+ecx+2]
        mov     word ptr [edi],0
;
        pop     esi                     ; Get Seg:Offs of b
        lea     edi,b[ebx]              ; Destination offset is b[bx]
        xor     ecx,ecx
        mov     cx,dx                   ; l(b) in cx
        shr     cx,1
        jnc     @F
        inc     cx
@@:     cld
rep     movsd                           ; Load b as ULONG
        movsw                           ;  plus one USHORT
        mov     cx,dx
        shl     cx,1
        lea     edi,b[ebx+ecx+2]
        mov     word ptr [edi],0
;
;>>>>>> Prepare access to local memory
;
        push    ebp                     ; Save bp
        mov     ebp,ebx                 ; Indexed addressing based on ss
;
;>>>>>> Remove leading zeros from operands
;
        xor     eax,eax
        xor     ecx,ecx
        mov     ax,a[ebp]               ; #USHORTs into ax
        cmp     ax,0
        je      next1
        shl     eax,1                   ; Index to low-order byte
                                        ; of high-order digit
        mov     esi,eax
@@:     cmp     word ptr a[ebp][esi],0  ; Digit = 0 ?
        jne     @F                      ; No, then we're done
        sub     esi,2                   ; Else: step back one digit
        cmp     esi,0                   ; Index = 0?
        je      mazer1                  ; Then argument = 0
        jmp     @B                      ; Next comparison
@@:                                     ; Length found
mazer1: shr     esi,1                   ; #Digits
        mov     eax,esi	                ; Store number of digits 
        mov     word ptr a[ebp],ax      

;Operand2
next1:  mov     ax,b[ebp]               ; #USHORTs in ax
        cmp     ax,0
        je      end1
        shl     eax,1                   ; #Bytes, index to low-order byte
                                        ; of high-order digit
        mov     esi,eax
@@:     cmp     word ptr b[ebp][esi],0  ; Digit = 0 ?
        jne     @F                      ; If not, we're done
        sub     esi,2                   ; Else:	step back one digit
        cmp     esi,0                   ; Index = 0?
        je      mazer2                  ; Then argument = 0
        jmp     @B                      ; Next comparison
@@:                                     ; Length found
mazer2: shr     esi,1                   ; #Digits
        mov     eax,esi                 ; Store number of digits
        mov     b[ebp],ax

end1:   cmp     ax,0
        jne     @F
        pop     ebp
        jmp     divbyz
@@:     cmp     word ptr a[ebp],0
        jne     @F
        pop     ebp
        jmp     divz
;
;>>>>>> Test a < b ?
;
@@:     mov     ecx,0
        mov     cx,a[ebp]
        cmp     cx,b[ebp]               ; l(a) - l(b)
        jnc     div03
        jmp     dra
div03:  jne     div05                   ; Compare digits
        mov     eax,ecx                 ; cl = l(a) = l(b)
        shl     eax,1
        lea     esi,a[ebp]
        lea     edi,b[ebp]
        add     esi,eax                 ; si points to high-order digit of a
        add     edi,eax                 ; di points to high-order digit of b
        std
repe    cmpsw
        cld
        jnc     div05                   ; If no carry occurs, q is positive
        jmp     dra                     ; Else q := 0 and r := a
div05:  mov     ax,b[ebp]
        shr     ax,1
        jnc     div05a
        inc     ax
div05a: cmp     ax,1                    ; Test if length l(b) = 1
        jne     dstart
        jmp     dshort                  ; Then go to short division
;
;>>>>>> Beginn der Division
;
dstart: mov     eax,0
        mov     ax,b[ebp]
        shr     ax,1
        jnc     dm0
        inc     ax
dm0:    mov     esi,eax
        shl     esi,2
        sub     esi,2                   ; Pointer to low byte of b[l(b)]
		                        ; (pointer to high-order ULONG)
        cmp     esi,10                  ; Does divisor have 3 or more digits?
        jb      dm1a                    ; Two digits are present at mininum!

        mov     ebx,b[ebp][esi]         ; b[l(b)] in bx
        mov     eax,b[ebp][esi-4]
        mov     edx,b[ebp][esi-8]
;
        mov     ecx,0                   ; Prepare counter
;
@@:     cmp     ebx,080000000h          ; v1 >= 2^31 ?
        jae     dm2                     ; If not...
        inc     cx
        clc
        rcl     edx,1
        rcl     eax,1
        rcl     ebx,1                   ; v1 = v1 * 2
        jmp     short @B                ; until v1 >= 2^31

dm1a:   mov     ebx,b[ebp][esi]         ; b[l(b)] in bx
        mov     eax,b[ebp][esi-4]
;
        mov     ecx,0                   ; Prepare counter
;
@@:     cmp     ebx,080000000h          ; v1 >= 2^31 ?
        jae     dm2                     ; If not...
        inc     cx
        clc
        rcl     eax,1
        rcl     ebx,1                   ; v1 = v1 * 2
        jmp     short @B                ; until v1 >= 2^31
;
dm2:    mov     d[ebp],cx               ; Store exponent
        mov     v1[ebp],ebx             ; v1
        mov     v2[ebp],eax             ; v2

dm3:    inc     word ptr a[ebp]         ; l(a) = l(a) + 2
        inc     word ptr a[ebp]
        mov     eax,0
        mov     ax,a[ebp]
        shr     ax,1
        jnc     dm3a
        inc     ax
dm3a:   mov     esi,eax
        shl     esi,2
        sub     esi,2
        mov     dword ptr a[ebp][esi],0 ; a[l(a)] = 0
;
d2:     mov     eax,0
        mov     ax,b[ebp]
        shr     ax,1
        jnc     d2a
        inc     ax
d2a:    mov     esi,eax
        shl     esi,2
        sub     esi,2                   ; si points to low-order byte of b[l(b)]
;
        mov     ecx,0
        mov     cx,a[ebp]
        shr     cx,1
        jnc     d2b
        inc     cx
d2b:    mov     edi,ecx
        shl     edi,2
        sub     edi,2                   ; di points to low-order byte of a[l(a)]
        push    edi
        sub     edi,esi
        inc     edi                     ; di points to high-order byte of a[l(a)-l(b)]
        mov     ecx,edi
        shr     ecx,2                   ; Counter in cx
        pop     esi                     ; si points to low-order byte of a[l(a)] (j+l(b))
        sub     edi,3                   ; di points to low-order byte of a[l(a)-l(b)] (j)
        push    edi                     ; Possible length of q
;
;-----> Division loop
;
d3:     cmp     esi,14
        jb      d3a
        push    edi
        mov     edx,a[ebp][esi]         ; uj   unshifet in dx (hiWord)
        mov     eax,a[ebp][esi-4]       ; uj+1 unshifted
        mov     ebx,a[ebp][esi-8]       ; uj+2 unshifted
        mov     edi,a[ebp][esi-12]      ; uj+3 unshifted
        push    ecx
        mov     cx,d[ebp]
@@:     cmp     cx,0
        je      @F
        rcl     edi,1
        rcl     ebx,1
        rcl     eax,1
        rcl     edx,1
        dec     cx
        jmp     @B
@@:     pop     ecx
        pop     edi
        jmp     qhat
;
d3a:    mov     edx,a[ebp][esi]         ; uj   unshifted
        mov     eax,a[ebp][esi-4]       ; uj+1 unshifted
        mov     ebx,a[ebp][esi-8]       ; uj+2 unshifted
        push    ecx
        mov     cx,d[ebp]
@@:     cmp     cx,0
        je      @F
        rcl     ebx,1
        rcl     eax,1
        rcl     edx,1
        dec     cx
        jmp     @B
@@:     pop     ecx
;
;-----> Calculate and test qhat
;
qhat:   mov uj1[ebp],eax                ;  Store shifted uj+1
        mov uj2[ebp],ebx                ;  Store shifted uj+2
        mov ebx,v1[ebp]                 ;  v1 in bx
        cmp ebx,edx
        je bm1                          ; If v1 = uj then q = b - 1
;
        div ebx                         ; eax <- qhat
;                                       ; edx <- rhat: = uj*b + uj+1 -qhat*v1 = (uj*b + uj+1) mod v1
        mov q1[ebp],eax                 ; store qhat intermediately
        cmp eax,0
        jne @F
        jmp dml                         ; If q=0 process next digit
;
bm1:    mov eax,0ffffffffh              ; eax <- qhat
        mov q1[ebp],eax                 ; Store qhat intermediately
        mov edx,uj1[ebp]                ; edx <- uj+1
        add edx,v1[ebp]                 ; edx <- uj+1 + v1 =: rhat
        jc d4                           ; rhat >= b => v2 * qhat < rhat * b
;
@@:     mov ebx,edx                     ; ebx <- rhat
        mul dword ptr v2[ebp]           ; eax <- low(qhat * v2), edx <- high(qhat * v2)
        cmp edx,ebx                     ; rhat >= high(qhat * v2) ?
        jb  d4                          ; CF == 1?  Then finished
        ja  @F                          ; rhat < high(qhat * v2) => reduce qhat
        cmp eax,uj2[ebp]                ; uj+2 >= low(qhat * v2) ?
        jbe d4                          ; CF == 1 OR ZF == 1? Then finished
;
@@:     dec dword ptr q1[ebp]           ; Correcting qhat by decrementing
        add ebx,v1[ebp]                 ; ebx <- rhat + v1
        jc d4                           ; rhat >= b => v2 * qhat < rhat * b
        sub eax,v2[ebp]                 ; (qhat * v2) - v2
        sbb edx,0
;
        cmp edx,ebx                     ; rhat >= high(qhat * v2) ?
        jb  d4                          ; CF == 1?  Then finished
        ja  @B                          ; rhat < high(qhat * v2) => reduce qhat
        cmp eax,uj2[ebp]                ; uj+2 >= low(qhat * v2) ?
        ja  @B                          ; CF==0 + ZF==0? => repeat decrement of qhat
;
d4:     push    ecx                     ; Store main counter
        push    esi                     ; Store j + l(b) 
        push    edi                     ; Store j
        mov     cx,b[ebp]               ; Load counter with l(b)
        shr     cx,1
        jnc     d4a
        inc     cx
d4a:    mov     ebx,q1[ebp]             ; q in bx
        mov     esi,2
        mov     edx,0                   ; Dummy carry
;
;-----> Multiplication and subtraction
;
dms:    push    edx
        mov     eax,ebx                 ; q in ax
        mul     dword ptr b[ebp][esi]   ; hi:dx lo:ax
        sub     a[ebp][edi],eax
        adc     edx,0                   ; Carry to next subtrahend
        pop     eax
        sub     a[ebp][edi],eax
        adc     edx,0                   ; Carry to next subtrahend
        inc     edi
        inc     edi
        inc     edi
        inc     edi
        inc     esi
        inc     esi
        inc     esi
        inc     esi
        loop    dms
        sub     a[ebp][edi],edx
        jnc     dnc                     ; No correction necessary if carry = 0
;
;-----> Correction
;
        pop     edi
        push    edi
        xor     ecx,ecx
        mov     cx,b[ebp]               ; Load counter with l(b)
        shr     cx,1
        jnc     d4b
        inc     cx
d4b:    mov     esi,2
        clc
d5:     mov     eax,b[ebp][esi]         ; b[i]
        adc     a[ebp][edi],eax         ; a[j+i] + b[i]
        inc     edi
        inc     edi
        inc     edi
        inc     edi
        inc     esi
        inc     esi
        inc     esi
        inc     esi
        loop    d5                      ; Outer loop
        jnc     d51
        inc     dword ptr a[ebp][edi]
d51:    dec     dword ptr q1[ebp]       ; q = q - 1
;
;
dnc:    pop     edi                     ; Get pointers and counters
        pop     esi
        pop     ecx
dml:    mov     eax,q1[ebp]             ; Get q
        mov     q[ebp][edi],eax         ; q[j] = q
        sub     edi,4
        sub     esi,4
        dec     ecx                     ; Inner loop
        jz      d6
        jmp     d3
;
d6:     pop     edi                     ; Get l(a)-l(b)-1 (ULONG digits)
        add     edi,2                   ; di points to low-order byte of last USHORT
        mov     eax,0
@@:     cmp     ax,q[ebp][edi]          ; q[l(a)-l(b)]=0 ?
        jne     d7                      ; If so...
        dec     edi
        dec     edi                     ; ...l(q) = l(a) - l(b) - 1
        cmp     edi,0
        jne     short @B
d7:     mov     edx,edi
        shr     edx,1                   ; l(q) (#USHORTs) in dx
        mov     q[ebp],dx               ; Store l(q)
;
;-----> Calculate length of remainder
;
        mov     eax,0
        mov     ax,b[ebp]
        mov     ecx,eax
        shl     ax,1
        mov     edi,eax
        mov     ebx,0
        inc     edi
        inc     edi
d8:     dec     edi
        dec     edi                     ; di points to low-order byte of a[l(b)]
        cmp     a[ebp][edi],bx
        loope   d8
        jz      d9                      ; If a[bp][di]!= 0...
        inc     cx                      ; ...l(r) has to be incremented
d9:     mov     a[ebp],cx               ; Store l(r)
;
;-----> Store results
;
dstore: mov     esi,ebp
        mov     ebx,ebp
        pop     ebp
        push    esi
        add     esi,q
        mov     edi,_qot                ; Destination offset of quotient
        mov     ecx,0
        mov     cx,[esi]
        shr     cx,1
        jnc     @F
        inc     cx
@@:     cld
        cmp     cx,0
        je      qzero
rep     movsd
qzero:  movsw
;
;-----> Store remainder
;
        pop     esi
drs:
        mov     edi,_rst
        add     esi,a
        mov     ecx,0
        mov     cx,[esi]
        shr     cx,1
        jnc     @F
        inc     cx
@@:     cld
        cmp     cx,0
        je      rzero
rep     movsd
rzero:  movsw
        mov     eax,0                   ; Return-Value = 0: Everything OK
;
;*******************************************************************************
;       Security: Purge stack memory

divret: mov     edi,ebx
        mov     ecx,(WORKSP-4)/4
        cld
rep     stosd                           ; Overwrite with 0 or -1
;*******************************************************************************
;
        pop     esi
        pop     edi
        pop     ebx
        mov     esp,ebp
        pop     ebp
        ret
;
;
divz:   mov     eax,0                   ; Return-Value = 0: Everything OK
        mov     edi,_rst                ; Destination Offset of remainder
        mov     esi,_qot                ; Destination offset of quotient
        mov     [edi],ax                ; Remainder = 0
        mov     [esi],ax                ; Quotient = 0
        jmp     short divret
;
divbyz: mov     eax,-1
        jmp     short divret            ; Return-Value = -1: Division by zero
;
;-----> Case q = 0 and r = a
;
dra:    mov     esi,ebp
        mov     ebx,ebp
        pop     ebp
        mov     edi,_qot
        mov     word ptr [edi],0        ; q = 0
        jmp     short drs               ; Store remainder
;
;
;>>>>>> Short Division
;
;
dshort: mov     ecx,0
        mov     cx,a[ebp]
        shr     cx,1
        jnc     dsh0
        inc     cx
dsh0:   mov     edi,ecx
        shl     edi,2
        sub     edi,2
        mov     edx,0
        mov     ebx,b[ebp+2]
dsh1:   mov     eax,a[ebp][edi]
        div     ebx
        mov     q[ebp][edi],eax
        sub     edi,4
        loop    dsh1
dsh2:   mov     ecx,0
        mov     cx,a[ebp]
        mov     esi,ecx
        shl     esi,1
@@:     mov     bx,q[ebp][esi]
        cmp     bx,0
        jne     dsh3
        dec     esi
        dec     esi
        dec     ecx
        cmp     cx,0
        jne     short @B
dsh3:   mov     q[ebp],cx
        mov     a[ebp+2],edx
        mov     word ptr a[ebp],2
        cmp     word ptr a[ebp+4],0
        jne     dst
        mov     word ptr a[ebp],1
        cmp     word ptr a[ebp+2],0
        jne     dst
        mov     word ptr a[ebp],0
dst:    jmp     dstore
;
div_l  endp
END

