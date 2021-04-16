#include "util.h"

#include <Windows.h>
#include <dbghelp.h>

#include <array>

void Util::printStackTrace() {
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    
    CONTEXT context;
    memset(&context, 0, sizeof(context));
    context.ContextFlags = CONTEXT_FULL;
    
    RtlCaptureContext(&context);
    SymInitialize(process, NULL, true);
    
    DWORD image;
    STACKFRAME64 stackframe;
    ZeroMemory(&stackframe, sizeof(STACKFRAME64));
    
    image = IMAGE_FILE_MACHINE_AMD64;
    stackframe.AddrPC.Offset    = context.Rip;
    stackframe.AddrPC.Mode      = AddrModeFlat;
    stackframe.AddrFrame.Offset = context.Rsp;
    stackframe.AddrFrame.Mode   = AddrModeFlat;
    stackframe.AddrStack.Offset = context.Rsp;
    stackframe.AddrStack.Mode   = AddrModeFlat;
    
    for (size_t i = 0; i < 25; i++) {
        bool result = StackWalk64(
            image, process, thread,
            &stackframe, &context,
            nullptr,
            SymFunctionTableAccess64, SymGetModuleBase64,
            nullptr
        );
        
        if (!result) {
            break;
        }
        
        static std::array<char, sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)> buffer;
        PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(buffer.data());
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;
        
        DWORD64 displacement = 0;
        if (SymFromAddr(process, stackframe.AddrPC.Offset, &displacement, symbol)) {
            printf("[%i] 0x%x %s\n", i, symbol->Address, symbol->Name);
        } else {
            printf("[%i] 0x%x ???\n", i, symbol->Address, symbol->Name);
        }
    }
    
    SymCleanup(process);
}
