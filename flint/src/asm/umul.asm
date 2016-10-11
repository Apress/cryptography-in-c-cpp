;******************************************************************************
;*                                                                            *
;* Functions for arithmetic and number theory with large integers in C        *
;* Software supplement to the book "Cryptography in C and C++"                *
;* by Michael Welschenbach                                                    *
;*                                                                            *
;* Module umul.asm         Revision: 18.12.2000                               *
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
;*      Multiplication, interface compatible with C function umul_l()         *
;*                                                                            *
;*      Product := a * u                                                      *
;*                                                                            *
;*      Stack on calling of umul:        SP+12 ---> Offset product            *
;*                                       SP+ 8 ---> Offset a (CLINT)          *
;*                                       SP+ 4 ---> Offset u (USHORT)         *
;*                                       SP    ---> Return address            *
;*                                                                            *
;*      Return Value: ax = 0                                                  *
;*                                                                            *
;******************************************************************************
PUBLIC  umul

.386
.MODEL  FLAT
.CODE
umul    proc    near
;
a       equ     0000                    ; CLINT factor  (working copy)
p       equ     0520                    ; Product       (working copy)
;
_a      equ     [ebp+8]                 ; CLINT factor  (parameter)
_u      equ     [ebp+12]                ; USHORT factor (parameter)
_p      equ     [ebp+16]                ; Product (storage space for result)
;
WORKSP  equ     1560                    ; Working memory in stack area
;
umult:  push    ebp                     ; Store value for calling procdure
        mov     ebp,esp
        sub     esp,WORKSP              ; Local memory
        mov     eax,esp                 ; Store starting address
        push    edi
        push    esi
        push    ebx
        mov     ebx,eax
;
        xor     eax,eax
        xor     edx,edx

        mov     dx,_u                   ; USHORT factor into dx
        cmp     dx,0
        jne     @F
        jmp     umultz

@@:     mov     esi,_a                  ; Segment:Offset of variable a
        mov     ax,[esi]
        cmp     ax,0                    ; a = 0 ?
        jne     umult01
        jmp     umultz
;
;>>>>>> Load operands
;
umult01:
        lea     edi,a[ebx]              ; Destination offset is a[ebx]
        mov     ecx,0
        mov     cx,ax                   ; l(a) into ecx
        cld
rep     movsw                           ; Load a as chunks of USHORT into a
        movsw                           ;  plus one word
        mov     word ptr [edi],0
;
;>>>>>> Prepare access to local memory
;
        push    ebp                     ; Store base pointer bp
        mov     ebp,ebx                 ; Indexed addressing based on ss
;
;>>>>>>> Remove leading zeros from operands
;
        xor     eax,eax
        mov     ax,a[ebp]               ; #USHORTs into ax
        cmp     ax,0
        je      end1
        shl     eax,1                   ; #Bytes
        mov     esi,eax
@@:     cmp     word ptr a[ebp][esi],0  ; USHORT = 0 ?
        jne     @F                      ; If not, we're done
        sub     esi,2                   ; Else: Step back one USHORT
        cmp     esi,0                   ; Index = 0?
        je      mazer1                  ; Then argument = 0
        jmp     @B                      ; Compare next digit
@@:
mazer1: shr     esi,1                   ; #USHORTs
        mov     eax,esi                 ; Store number of digits 
        mov     a[ebp],ax
;
@@:
end1:   cmp     ax,0
        jne     @F
        pop     ebp
        jmp     umultz
;
;>>>>>> Start of multiplication
;
;-----> Initialize working memory
;
@@:     mov     esi,2                   ; Initialization of index i
        mov     cx,a[ebp]               ; Length l(a)
        shr     cx,1
        jnc     umult0
        inc     cx
umult0: mov     eax,0
umult1: mov     p[ebp][esi],eax
        add     esi,4
        loop    umult1
;
;-----> Inner loop
;
        mov     edi,2                   ; Index j
        mov     cx,a[ebp]               ; Initialize counter inner loop
        shr     cx,1
        jnc     umult2a
        inc     cx
umult2a:
        xor     ebx,ebx
        mov     bx,dx                   ; Load USHORT-Faktor into bx
        mov     edx,0                   ; Dummy carry
        mov     esi,2
        mov     edi,2
umult3: push    ecx                     ; Store counter
        mov     ecx,edx                 ; Store carry in dx
        mov     eax,ebx                 ; Load USHORT factor into ax
        mul     dword ptr a[ebp][edi]   ; ax=lo, dx=hi
        add     eax,ecx                 ; ax + u
        adc     edx,0
        add     p[ebp][esi],eax         ; p[i+j-1]= ax + p[i+j-1]
        adc     edx,0                   ; New carry u into dx
        add     edi,4                   ; j=j+4
        add     esi,4                   ; i+j=i+j+4
        pop     ecx                     ; Get counter for inner loop
        loop    umult3
;
;-----> End of inner loop
;
        mov     p[ebp][esi],edx         ; p[i+j-1]=u
        add     esi,2
umult3a:
        cmp     word ptr p[ebp][esi],0
        jne     short umult4
        sub     esi,2
        jmp     umult3a
umult4: mov     ecx,esi
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
        mov     eax,0                   ; Return-Value = 0: Everything is OK
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
umultz: mov     esi,_p
        mov     word ptr [esi],0
        jmp     short muret
;
umul    endp
END

