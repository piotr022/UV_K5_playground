.globl PrintTextOnScreen
PrintTextOnScreen = 0x874C + 1;

.globl DelayMs
DelayMs = 0xD0EC + 1;

.globl DelayUs
DelayUs = 0xD100 + 1;

.globl WriteSerialData
WriteSerialData = 0xBE44 + 1;

.globl BK4819Write
BK4819Write = 0xAF00 + 1;

.globl BK4819Read
BK4819Read = 0xA960 + 1;

.globl FlushFramebufferToScreen
FlushFramebufferToScreen = 0xB638 + 1;

.globl PollKeyboard
PollKeyboard = 0xb0b8 + 1;

.globl FormatString
FormatString = 0xC6E8 + 1;

.globl PrintSmallDigits
PrintSmallDigits = 0x870C + 1;

.globl PrintFrequency
PrintFrequency = 0x864C + 1;

.globl AirCopy72
AirCopy72 = 0xA67C + 1;

.globl AirCopyFskSetup
AirCopyFskSetup = 0xA518 + 1;

.globl BK4819Reset
BK4819Reset = 0xa7cc + 1;

.globl IntDivide
IntDivide = 0x128 + 1;

.globl Strlen
Strlen = 0x1c0 + 1;

.globl BK4819SetChannelBandwidth
BK4819SetChannelBandwidth = 0xaa48 + 1;

.globl BK4819WriteFrequency
BK4819WriteFrequency = 0xaabc + 1;

.globl BK4819SetPaGain
BK4819SetPaGain = 0xaad4 + 1;

.globl BK4819ConfigureAndStartTxFsk
BK4819ConfigureAndStartTxFsk = 0x1cd8 + 1;

.globl BK4819ConfigureAndStartRxFsk
BK4819ConfigureAndStartRxFsk = 0xa63c + 1;

.globl BK4819SetGpio
BK4819SetGpio = 0xa794 + 1;

.globl FlushStatusbarBufferToScreen
FlushStatusbarBufferToScreen = 0xb6b0 + 1;

.globl UpdateStatusBar
UpdateStatusBar = 0x9c10 + 1;

.globl AdcReadout
AdcReadout = 0x9d7c + 1;

.globl SomeAmStuff
SomeAmStuff = 0xc158 + 1;

.globl IRQ_RESET
IRQ_RESET = 0xd4 + 1;

.globl IRQ_SYSTICK
IRQ_SYSTICK = 0xc398 + 1;

.globl ConfigureTrimValuesFromNVR
ConfigureTrimValuesFromNVR = 0x20000230 + 1;

.globl FLASH_ReadNvrWord
FLASH_ReadNvrWord = 0x20000214 + 1;

.globl SystemReset
SystemReset = 0x200001f8 + 1;

.globl FLASH_SetProgramTime
FLASH_SetProgramTime = 0x200001d4 + 1;

.globl FLASH_SetMode
FLASH_SetMode = 0x200001b4 + 1;

.globl FLASH_WakeFromDeepSleep
FLASH_WakeFromDeepSleep = 0x20000194 + 1;

.globl FLASH_SetEraseTime
FLASH_SetEraseTime = 0x20000170 + 1;

.globl FLASH_SetReadMode
FLASH_SetReadMode = 0x20000148 + 1;

.globl FLASH_Set_NVR_SEL
FLASH_Set_NVR_SEL = 0x20000128 + 1;

.globl FLASH_ReadByAPB
FLASH_ReadByAPB = 0x200000e8 + 1;

.globl FLASH_ReadByAHB
FLASH_ReadByAHB = 0x200000dc + 1;

.globl FLASH_Unlock
FLASH_Unlock = 0x200000d0 + 1;

.globl FLASH_Lock
FLASH_Lock = 0x200000c4 + 1;

.globl FLASH_MaskUnlock
FLASH_MaskUnlock = 0x200000b0 + 1;

.globl FLASH_SetMaskSel
FLASH_SetMaskSel = 0x20000094 + 1;

.globl FLASH_MaskLock
FLASH_MaskLock = 0x20000080 + 1;

.globl FLASH_Init
FLASH_Init = 0x2000005c + 1;

.globl FLASH_Start
FLASH_Start = 0x20000044 + 1;

.globl FLASH_IsInitBusy
FLASH_IsInitBusy = 0x2000002c + 1;

.globl FLASH_IsBusy
FLASH_IsBusy = 0x20000014 + 1;

.globl FLASH_RebootToBootloader
FLASH_RebootToBootloader = 0x20000000 + 1;

.globl gDisplayBuffer
gDisplayBuffer = 0x20000704;

.globl gSmallDigs
gSmallDigs = 0xD620;

.globl gSmallLeters
gSmallLeters = 0xD348;

.globl gFlashLightStatus
gFlashLightStatus = 0x200003b3;

.globl gStatusBarData
gStatusBarData = 0x20000684;

.globl gVoltage
gVoltage = 0x20000406;

