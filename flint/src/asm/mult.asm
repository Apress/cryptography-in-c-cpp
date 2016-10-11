;******************************************************************************
;*                                                                            *
;* Functions for arithmetic and number theory with large integers in C        *
;* Software supplement to the book "Cryptography in C and C++"                *
;* by Michael Welschenbach                                                    *
;*                                                                            *
;* Module mult.asm         Revision: 18.12.2000                               *
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
;*      Multiplication, interface compatible with C function mul_l()          *
;*                                                                            *
;*      Product := f1 * f2                                                    *
;*                                                                            *
;*      Stack on calling of mul:         SP+12 ---> Offset product            *
;*                                       SP+ 8 ---> Offset f2                 *
;*                                       SP+ 4 ---> Offset f1                 *
;*                                       SP    ---> Return address            *
;*                                                                            *
;*      Return Value : ax = 0                                                 *
;*                                                                            *
;******************************************************************************
PUBLIC  mult
.386
.MODEL  FLAT
.CODE
mult    proc    near
;
a       equ     0                       ; CLINT factor 1 (working copy)
b       equ     520                     ; CLINT factor 2 (working copy)
p       equ     1040                    ; Product (storage space for result)
;
_a      equ     [ebp+8]                 ; Factor 1 (parameter)
_b      equ     [ebp+12]                ; Factor 2 (parameter)
_p      equ     [ebp+16]                ; Product  (parameter)
;
WORKSP  equ     2200                    ; Working memory in stack area
;
start:  push    ebp                     ; Store base ptr for caller
        mov     ebp,esp
        sub     esp,WORKSP              ; Local memory
        mov     eax,esp                 ;   Store starting address
        push    edi
        push    esi
        push    ebx
        mov     ebx,eax
;
        xor     eax,eax
        xor     edx,edx
        mov     esi,_a                  ; Parameter f1
        mov     edi,_b                  ; Parameter f2
        mov     ax,[esi]
        mov     dx,[edi]

        cmp     ax,0                    ; a = 0 ?
        jne     mult01
        jmp     multz
mult01: cmp     dx,0                    ; b = 0 ?
        jne     mult02                  ; Then we're done
        jmp     multz
;
;>>>>>> Load operands
;
mult02: push    edi                     ; Store Seg:Offs f2
        lea     edi,b[ebx]              ; Destination offset is b[ebx]
        mov     ecx,0
        mov     cx,ax                   ; l(a) (length of a) in ecx
        cld
rep     movsw                           ; Load a in chunks of USHORT
        movsw                           ;  plus one word
        mov     word ptr [edi],0        ;
;
        pop     esi                     ; Get offset of b
        lea     edi,a[ebx]              ; Destination offset is a[ebx]
        mov     cx,dx                   ; l(b) in CX
        cld
rep     movsw                           ; Load b in chunks of USHORT
        movsw                           ;  plus one word
        mov     word ptr [edi],0
;
;>>>>>> Prepare access to local memory
;
        push    ebp                     ; Store bp
        mov     ebp,ebx                 ; Indexed addressing based on ss
;
;>>>>>> Remove leading zeros from operands
;
        xor     eax,eax
        mov     ax,b[ebp]               ; #USHORTs in ax
        cmp     ax,0
        je      next1
        shl     eax,1
        mov     esi,eax                 ; Index to high-order digit
@@:     cmp     word ptr b[ebp][esi],0  ; USHORT = 0 ?
        jne     @F                      ; If not, then finished
        sub     esi,2                   ; Else: step back one USHORT
        cmp     esi,0                   ; Index = 0 ?
        je      mazer1                  ; Then argument = 0
        jmp     @B                      ; Compare next
@@:
mazer1: shr     esi,1                   ; #USHORTs
        mov     eax,esi                 ; Store number of digits
        mov     b[ebp],ax

;Operand2
next1:  mov     ax,a[ebp]               ; #USHORTs in ax
        cmp     ax,0
        je      end1
        shl     eax,1                   ; Index to high-order digit
        mov     esi,eax
@@:     cmp     word ptr a[ebp][esi],0  ; Digit = 0 ?
        jne     @F                      ; If not, then finished
        sub     esi,2                   ; Else: step back one USHORT
        cmp     esi,0                   ; Index = 0 ?
        je      mazer2                  ; Then argument = 0
        jmp     @B                      ; Compare next
@@:
mazer2: shr     esi,1                   ; #Digits
        mov     eax,esi                 ; Store number of digits
        mov     a[ebp],ax

end1:   cmp     ax,0
        jne     @F
        pop     ebp
        jmp     multz
@@:     cmp     word ptr b[ebp],0
        jne     @F
        pop     ebp
        jmp     multz
;
;>>>>>> Start of multiplication
;
;-----> Initialization
;
@@:     mov     esi,2                   ; Initialization of index i
        mov     cx,dx                   ; Length l(b)
        shr     cx,1
        jnc     mult0
        inc     cx
mult0:  mov     eax,0
mult1:  mov     p[ebp][esi],eax
        add     esi,4
        loop    mult1
;
;-----> Outer loop
;
        mov     esi,2                   ; Index i
        mov     ecx,0
        mov     cx,b[ebp]               ; Initialize counter for outer loop
        shr     cx,1
        jnc     mult2
        inc     cx
mult2:  push    ecx                     ; Store counter
;
;-----> Inner loop
;
        mov     edi,2                   ; Index j
        mov     cx,a[ebp]               ; Initialize counter for inner loop
        shr     cx,1
        jnc     mult2a
        inc     cx
mult2a: xor     edx,edx                 ; Dummy carry
        mov     ebx,b[ebp][esi]         ; Load b[i]
        add     esi,edi                 ; Index i+j to the product
mult3:  push    ecx                     ; Store counter
        mov     ecx,edx                 ; Carry into dx
        mov     eax,ebx
        mul     dword ptr a[ebp][edi]   ; ax=lo, dx=hi
        add     eax,ecx                 ; ax + u
        adc     edx,0
        add     p[ebp][esi]-2,eax       ; p[i+j-1]= ax + p[i+j-1]
        adc     edx,0                   ; New carry u into dx
        add     edi,4                   ; j=j+4
        add     esi,4                   ; i+j=i+j+4
        pop     ecx                     ; Get counter for inner loop
        loop    mult3
;
;-----> End of inner loop
;
        mov     p[ebp][esi]-2,edx       ; p[i+j-1]=u
        sub     esi,edi                 ; Restore i
        add     esi,4                   ; i=i+4
        pop     ecx                     ; Get counter for outer loop
        loop    mult2
;
;-----> Ende of outer loop 
;
        add     esi,edi
        sub     esi,4
mult3a: cmp     word ptr p[ebp][esi],0  ; Determine the length of the product
        jne     short mult4
        sub     esi,2
        jmp     mult3a
mult4:  mov     ecx,esi
        shr     ecx,1
        mov     p[ebp],cx
;
;-----> Store product
;
        lea     esi,p[ebp]
        mov     ebx,ebp
        pop     ebp
        mov     edi,_p
        cmp     cx,0
        je      @F
        cld                             ; Length l(p) is still in cx
rep     movsw
@@:     movsw
;
;*******************************************************************************
;       Security: Purge stack

muret:  mov     edi,ebx
        mov     ecx,(WORKSP-4)/4
        cld
        mov     eax,0                   ; Return-Value = 0: Everything OK
rep     stosd                           ; Overwrite with 0
;*******************************************************************************
;
        pop     ebx
        pop     esi                     ; Restore registers
        pop     edi
        mov     esp,ebp
        pop     ebp
        ret
;
multz:  mov     esi,_p
        mov     word ptr [esi],0
        jmp     short muret
;
mult    endp
END

