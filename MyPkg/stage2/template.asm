include 'ebc.inc'
include 'efi.inc'
include 'format.inc'
include 'utf8.inc'

struct EFI_INPUT_KEY
    ScanCode    UINT16
    UnicodeChar UINT16;CHAR16
ends

format peebc efi
entry EfiMain

section '.text' code executable readable

Print:
    ; gST->ConOut->OutputString(g_ST->ConOut, <R0>);
    MOVREL  R1, gST
    MOV     R1, @R1
    MOVn    R1, @R1(EFI_SYSTEM_TABLE.ConOut)
    PUSHn   @R0(0,+16)
    PUSHn   R1
    CALLEX  @R1(SIMPLE_TEXT_OUTPUT_INTERFACE.OutputString)
    MOV     R0, R0(+2,0) ; POP R1,<arg>
    RET

Gtfo:
    ; gST->RunTimeServices(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
    MOVREL  R1, gST
    MOVn    R6, @R1
    MOVn    R6, @R6(EFI_SYSTEM_TABLE.RuntimeServices)
    MOVI    R1, EfiResetShutdown
    MOVI    R2, EFI_SUCCESS
    MOVI    R3, 0
    PUSHn   R3
    PUSHn   R3
    PUSHn   R2
    PUSHn   R1
    CALLEX  @R6(EFI_RUNTIME_SERVICES.ResetSystem)
    MOV     R0, R0(+4,0) ; POP R1,R2,R3,R3
    RET

EfiMain:
    ; Store parameters as global variables
    MOVREL  R1, gST
    MOVn    @R1, @R0(EFI_MAIN_PARAMETERS.SystemTable)

    ; gST->ConOut->OutputString(g_ST->ConOut, L"Password: ");
    MOV     R1, @R1
    MOVn    R1, @R1(EFI_SYSTEM_TABLE.ConOut)
    MOVREL  R2, Prompt
    PUSH    R2
    PUSHn   R1
    CALLEX  @R1(SIMPLE_TEXT_OUTPUT_INTERFACE.OutputString)
    MOV     R0, R0(+2,0) ; POP R1,<arg>

__LP:
    XOR     R2, R2 ; init password counter
_LP:
    PUSH    R2    ; save counter

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; gST->ConIn->Reset(g_ST->ConIn, FALSE);
    MOVREL  R1, gST
    MOV     R1, @R1
    MOVn    R1, @R1(EFI_SYSTEM_TABLE.ConIn)
    MOVI    R2, FALSE
    PUSHn   R2
    PUSHn   R1
    CALLEX  @R1(SIMPLE_TEXT_INPUT_INTERFACE.Reset)
    POPn    R1
    POPn    R2

    ; gBS->WaitForEvent(1, &g_ST->ConIn->WaitForKey, &Event);
    MOVREL  R3, gST
    MOV     R3, @R3
    MOVn    R3, @R3(EFI_SYSTEM_TABLE.BootServices)
    MOVREL  R2, Event
    PUSHn   R2
    MOV     R1, R1(SIMPLE_TEXT_INPUT_INTERFACE.WaitForKey)
    PUSH    R1
    MOVI    R1, 1
    PUSHn   R1
    CALLEX  @R3(EFI_BOOT_SERVICES.WaitForEvent)
    MOV     R0, R0(+3,0)

    ; gST->ConIn->ReadKeyStroke(g_ST->ConIn, &Key);    
    MOVREL  R1, gST
    MOV     R1, @R1
    MOVn    R1, @R1(EFI_SYSTEM_TABLE.ConIn)
    MOVREL  R2, Key
    PUSHn   R2
    PUSHn   R1
    CALLEX  @R1(SIMPLE_TEXT_INPUT_INTERFACE.ReadKeyStroke)
    MOV     R0, R0(+2,0)

    ; gST->ConIn->Reset(g_ST->ConIn, FALSE);
    MOVREL  R1, gST
    MOV     R1, @R1
    MOVn    R1, @R1(EFI_SYSTEM_TABLE.ConIn)
    MOVI    R2, FALSE
    PUSHn   R2
    PUSHn   R1
    CALLEX  @R1(SIMPLE_TEXT_INPUT_INTERFACE.Reset)
    MOV     R0, R0(+2,0)
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    ; gST->ConOut->OutputString(g_ST->ConOut, L"*");
    MOVIw   R1, 0
    PUSH    R1
    MOVIw   R1, 0x2A ; '*'
    PUSH    R1
    MOVIw   R1, 0
    PUSH    R1
    MOV     R1, R0(+1,0)
    PUSH    R1
    MOVREL  R1, gST
    MOV     R1, @R1
    MOVn    R1, @R1(EFI_SYSTEM_TABLE.ConOut)
    PUSHn   R1
    CALLEX  @R1(SIMPLE_TEXT_OUTPUT_INTERFACE.OutputString)
    MOV     R0, R0(+5,0)

    POPn    R2 ; get counter

    ; Get unicode char of key
    MOVREL  R1, Key
    MOVn    R1, R1(EFI_INPUT_KEY.UnicodeChar)
    MOVw    R4, @R1

    ; Get current flag char
    MOVREL  R5, PreFlag_Junk ; get PreFlag base address
    MOVI    R6, 128
    MOVI    R3, 2
    MUL     R6, R3
    MUL     R6, R3 ; offset by 2 * 2 * 128 (2 unicode lines of 128 chars)
    MUL     R3, R2
    ADD     R5, R3
    ADD     R5, R6
    MOVw    R5, @R5 ; flag[2*i] (2: unicode)

    ;; BEGIN JUNK ;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; 1 + 3 + 3 = 7
    MOVI    R1, 1   ; R1 = 1
    ADD     R3, R1  ; R3 = 2 + 1 = 3
    ADD     R1, R3  ; R1 = 1 + 3
    ADD     R1, R3  ; R1 = 1 + 3 + 3
    CMPIeq  R1, 7
    JMP8cc  _JUNK_0
    _JUNK_0:
    ;; END JUNK ;;;;;;;;;;;;;;;;;;;;;;;;;;;

    MOVI    R3, __FLAG_ROT__
    ADD     R5, R3

    CMPeq   R4, R5
    JMPcc   __LP
    
    ; loop condition
    ;POPn R2
    MOVI    R3, 1
    ADD     R2, R3
    CMPIgte R2, __FLAG_LENGTH__ ; flag length
    JMPcc   _LP

    MOVREL  R1, GoodBoy
    PUSH    R1
    CALL    Print
    POP     R1

    CALL Gtfo

LP_:
    ; bad password
    RET


section 'VarBss' data readable writeable
PreFlag_Junk: ; 256
    du "x0cV$2ekF2Qizv6^oyq^pUHKUgFj1Jd__V4LKW45H3R3__QvN3@sMwGeWw0VKBYFzRbviq6u#7RA9ArnM8XDIEEvHQ&HGT@Sv&LUZdb4BF6%2_4dci33595^VZQeoji^"
    du "z^ucPVhc#&cT6#NH0^97O$7WqofM3pHpyMsY4WeTtS&eeNwq466kV6__GHG7e&S&ReuO353pv^UppLd5*$5!TD__nipgduZdxzv#oDWd&DFNzVWAmO_7jEH38DGb%dkA"
Flag:
    du "__FLAG__"
PostFlag_Junk:
    du "4PrhsulP%wMNpg&4cRY7S8x^!Veptn9kK__P8D3j41V%qktB7i_L&ViJdr1%#P&Dhy4C3H"
ImageHandle:
    dq ?
gST:
    dq ?
Event:
    dq ?
GoodBoy:
    du 0x0D, 0x0A
    du "Congratulations!"
    du 0x0D, 0x0A, 0x00
Prompt:
    du "Password: ", 0x00
Key:
    rb EFI_INPUT_KEY.__size
    du 0x00 ; end of string
