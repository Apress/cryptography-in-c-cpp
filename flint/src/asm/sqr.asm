;******************************************************************************
;*                                                                            *
;* Functions for arithmetic and number theory with large integers in C        *
;* Software supplement to the book "Cryptography in C and C++"                *
;* by Michael Welschenbach                                                    *
;*                                                                            *
;* Module sqr.asm          Revision: 18.12.2000                               *
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
;*      Squaring, interface compatible with C function sqr_l()                *
;*                                                                            *
;*      Q := a * a mod M                                                      *
;*                                                                            *
;*      Stack on calling of sqr:          SP+ 8 ---> Offset q                 *
;*                                        SP+ 4 ---> Offset a                 *
;*                                        SP    ---> Return address           *
;*                                                                            *
;*      Return Value : ax = 0                                                 *
;*                                                                            *
;******************************************************************************
PUBLIC  sqr
.386
.MODEL  FLAT
.CODE
sqr     proc    near
;
a       equ     0                       ; Factor (working memory)
q       equ     520                     ; Square (working memory)
;
_a      equ     [ebp+8]                 ; Factor (parameter)
_q      equ     [ebp+12]                ; Square (parameter)
;
WORKSP  equ     1560                    ; Working memory in stack area
;
square: push    ebp                     ; Store base ptr for caller
        mov     ebp,esp
        sub     esp,WORKSP              ; Local memory
        mov     eax,esp
        push    ebx
        push    edi
        push    esi
        mov     ebx,eax                 ; Store starting address
;
        mov     esi,_a                  ; Offset of variable a
        mov     dx,[esi]                ; Length l(a)
        cmp     dx,0
        jnz     sqr011
        jmp     sqrnul
sqr011:
;
;>>>>>> Load operands
;
sqr02:  lea     edi,a[ebx]              ; Destination offset is a[bx]
        mov     ecx,0
        mov     cx,dx                   ; l(a) into cx
@@:     cld
rep     movsw                           ; Load a in chunks of USHORT into a[bx]
        movsw                           ;  plus one USHORT
        mov     word ptr [edi],0        ;
;
;
;>>>>>> Prepare addressing of local working memory
;
        push    ebp                     ; Store bp
        mov     ebp,ebx                 ; Indexed addressing based on ss
;
;>>>>>> Remove leading zeros from operands
;
        xor     eax,eax
        mov     ax,a[ebp]               ; #USHORTs into ax
        cmp     ax,0
        je      end1
        shl     eax,1                   ; #Bytes, index to high-order digit
        mov     esi,eax
@@:     cmp     word ptr a[ebp][esi],0  ; digit = 0 ?
        jne     @F                      ; If not, then we're done
        sub     esi,2                   ; Else: step back one USHORT
        cmp     esi,0                   ; Index = 0?
        je      mazer1                  ; Then argument = 0
        jmp     @B                      ; Compare next digit
@@:
mazer1: shr     esi,1                   ; #USHORTS
        mov     eax,esi                 ; Store number of digits
        mov     a[ebp],ax

end1:   cmp     ax,0
        jne     sqrmul
        pop     ebp
        jmp     sqrnul
;
;
;>>>>>> Start squaring
;
;----->  Initialize working environment
;
sqrmul: mov     ecx,0
        mov     cx,a[ebp]               ; Get length l(f)
        shr     cx,1
        jnc     @F
        inc     cx
@@:     cmp     cx,1
        jne     _sqr09
        jmp     _xrgsq                  ; If l(f)=1 then squaring in registers
_sqr09: mov     eax,0
        mov     esi,2                   ; Initialization of index i
_sqr10: mov     q[ebp][esi],eax
        add     esi,4
        loop    _sqr10
;
;-----> Outer loop
;
        mov     esi,2                   ; Index i
        mov     ecx,0
        mov     cx,a[ebp]               ; Counter for outer loop
        shr     cx,1
        jc      _sqr11
        dec     cx                      ; in cx = l(a)-1
_sqr11: push    ecx                     ; Store counter
;
;-----> Inner loop
;
        mov     edi,esi                 ; Index j=i+4
        add     edi,4
        xor     ecx,ecx
        mov     cx,a[ebp]               ; Counter for inner loop
        shr     cx,1
        jnc     @F
        inc     cx
@@:     push    esi
        add     esi,2
        shr     esi,2
        sub     ecx,esi
        pop     esi                     ; cx = l(a)-i
        mov     edx,0                   ; Dummy carry
        mov     ebx,a[ebp][esi]         ; Load a[i]
        add     esi,edi                 ; Index i+j to product
_sqr12: push    ecx                     ; Store counter
        mov     ecx,edx                 ; Carry from dx into cx
        mov     eax,ebx
        mul     dword ptr a[ebp][edi]   ; After mul: eax=low, edx=high-order
        add     eax,ecx                 ; ax + u
        adc     edx,0
        add     q[ebp+esi]-2,eax        ; q[i+j-1]= ax + q[i+j-1]
        adc     edx,0                   ; New carry u in dx
        add     edi,4                   ; j=j+4
        add     esi,4                   ; i+j=i+j+4
        pop     ecx                     ; Get counter for inner loop
        loop    _sqr12
;
;-----> End of inner loop
;
        mov     q[ebp+esi]-2,edx        ; q[i+j-1]=u
        sub     esi,edi                 ; Restore index i
        add     esi,4                   ; i=i+4
        pop     ecx                     ; Get counter for outer loop
        loop    _sqr11
;
;-----> End of outer loop
;
        add     esi,edi
        mov     dword ptr q[ebp][esi]-2,0
        mov     dword ptr q[ebp][esi]+2,0
;
;-----> Multiply intermediate result by 2
;
        mov     esi,6                   ; First word is zero
        xor     ecx,ecx
        mov     cx,a[ebp]
_xsll:
        rcl     dword ptr q[ebp][esi],1 ; Shift q in chunks of ULONG
        inc     esi
        inc     esi
        inc     esi
        inc     esi
        loop    _xsll
;
;-----> Add squares to intermediate result q
;
_xsle:  mov     cx,a[ebp]
@@:     shl     cx,1
        mov     q[ebp],cx
        mov     esi,2
        mov     edi,2
        mov     cx,a[ebp]               ; l(a) is counter
        shr     cx,1
        jnc     @F
        inc     cx
@@:     xor     eax,eax
        pushf                           ; Dummy carry
_sqr20: mov     eax,a[ebp][esi]
        mul     eax                     ; ax = lo, dx = hi
        popf
        adc     q[ebp][edi],eax
        adc     q[ebp][edi+4],edx
        pushf
        add     esi,4
        add     edi,8
        loop    _sqr20
        popf                            ; Adjust stack pointer
        jmp     _sqr20a
;
;-----> Squaring in registers in case of l(a) = 1
;
_xrgsq: mov     eax,a[ebp+2]
        mul     eax
        mov     q[ebp+2],eax
        mov     q[ebp+6],edx
        mov     word ptr q[ebp],4
;
;-----> Calculate length of remainder
;
_sqr20a:
        xor     eax,eax
        mov     ax,q[ebp]
        mov     edi,eax
        shl     edi,1
        cmp     word ptr q[ebp][edi],0
        jnz     _sqr60
        dec     word ptr q[ebp]
        jmp     _sqr20a
;
;-----> Store result
;
_sqr60: lea     esi,q[ebp]
        mov     ebx,ebp
        pop     ebp
        mov     edi,_q
        mov     ecx,0
        mov     cx,[esi]
        shr     cx,1
        jnc     @F
        inc     cx
@@:     cmp     cx,0
        je      @F
        cld
rep     movsd
@@:     movsw
;
;*******************************************************************************
;       Security: Purge stack

sqrret: mov     edi,ebx
        mov     ecx,(WORKSP-4)/4
        cld
        mov     eax,0                   ; Return-Value = 0: Everything is OK
rep     stosd                           ; Overwrite with 0
;*******************************************************************************
;
        pop     esi
        pop     edi
        pop     ebx
        mov     esp,ebp
        pop     ebp
        ret
;
sqrnul: mov     esi,_q
        mov     word ptr [esi], 0
        jmp     short sqrret
;
sqr     endp
END

