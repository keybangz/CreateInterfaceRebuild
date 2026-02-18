#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string.h>
#include <dlfcn.h>
#include <limits.h>
#include <unistd.h>
#include <link.h>

typedef void* (*InstantiateInterfaceFn)(); // Function pointer used in 'InterfaceReg'

// This hasn't changed from older Source Engine games to newer ones.
typedef struct InterfaceReg { 
  InstantiateInterfaceFn m_CreateFn; // Instance of function pointer

  const char *m_pName;  // Name of the interface
  struct InterfaceReg *m_pNext; // Pointer to next interface
} InterfaceReg;

/*
 * The end-goal of rewriting 'CreateInterfaceRebuild' @ https://github.com/keybangz/CreateInterfaceRebuild
 * is too rewrite the tool as a demonstration of utilization on Linux, but also to update this code to a
 * higher standard, demonstrating a more precise understanding of memory handling in C.
 *
 * This is a reference project for both myself, and also may serve as a useful breakdown for others
 * that are trying to achieve a similar goal.
 *
 * In C, when you add a number to a pointer, the compiler doesn't just add that number to the address. 
 * It multiplies the number by the size of the type.
 * Breakdown: 'Address' + (N + sizeof(type))
 *
 * ex:  int* p = (int*)baseAddr;
 *      ptr = ptr + 0x10; // 0x1000 + (16 * 4) = 0x1040 // Assuming each integer is 4 bytes in size.
 *      'ptr' is moved 64 bytes forward (16 integers in size)
 *
 * ex:  char* c = (char*)baseAddr;
 *      c = c + 0x10; // 0x100 + (16 * 1) = 0x1010
 *      'c' is moved 16 bytes forward.
 *
 * uintptr_t is provided by <stdin.h>, we use uintptr_t to ensure byte size is guranteed to be large enough to hold a pointer.
 * 32-bit systems = 'unsigned long' might be 4-bytes.
 * 64-bit Linux = 'unsigned long' is 8 bytes.
 * 64-bit windows = 'unsigned long' is 4 bytes, but pointers are 8 bytes.
 *
 * To determine which types to be used when dealing with memory,
 * determining your use-case whether it be calculating or accessing memory becomes a useful
 * tool of seperation for the readability of code.
 *
 * Use-case:
 * - Calculating memory
 *   uintptr_t is useful for dealing with memory calculation, as you can add bytes to these pointers
 *   and assume the size of bytes added will be concrete for most, if not all systems.
 *   
 * - Accessing memory
 *   Pointer casting and dereferencing your desired pointer utilizing your defined type
 *   (InterfaceReg**) for example, allows anyone to easily determine the pattern
 *   between accesssing and calculating memory, ofcourse only if you stick
 *   strictly to defining them based on these use-cases.
 *
 * (uintptr_t)pointerToAddress = Holding a raw number representing the start of some memory.
 *       + 0xOFFSET = Moving forward in this memory by the 0xOFFSET amount of bytes (eg. 0x10)
 *
 * (InterfaceReg**) = There is a pointer to InterfaceReg at this specific location.
 *       *(...) = Give me the value stored at this location.
*/

int client(void*);
void client_dynamic(void*);

int __attribute__ ((constructor)) SoEntryPoint() {
  // dlopen() returns a opaque handle (pointer to structure) used by the linker.
  void *handle = dlopen("../../csgo/bin/linuxsteamrt64/libclient.so", RTLD_NOLOAD | RTLD_NOW);

  if(!handle) {
    fprintf(stderr, "Failed to open libclient.so!\n");
    exit(1);
  }

  // define link map struct to be used to read the handle returned.
  struct link_map *map = NULL;
  
  // use dlinfo() to fill our map struct correctly get the base address of libclient.so
  if(dlinfo(handle, RTLD_DI_LINKMAP, &map) == 0) {
    fprintf(stderr, "libclient.so Handle: %p\n", handle); // retrieve and report handle
    fprintf(stderr, "libclient.so Base Address: %p\n", (void*)map->l_addr); // retrieve and report base address

    client((void*)map->l_addr); // pass base address for interfacing walking
  } else
      fprintf(stderr, "Failed to get link_map for libclient.so!\n");

  client_dynamic(handle);

  return 0;
}

int client(void *baseAddr) {
  // Use-case: Calculate the addition of the baseAddr and add the byte offset (qword_4647580 in IDA as of 18/02/26)
  uintptr_t listAddr = (uintptr_t)baseAddr + 0x4647580;

  // Use-case: Access the pointer stored at the address calculated.
  InterfaceReg* interfaceList = *(InterfaceReg**)listAddr; 

  fprintf(stderr, "client() called.\n");
  fprintf(stderr, "Reading Interface List from: %p\n", (void*)listAddr);
  fprintf(stderr, "Interface list pointer: %p\n", (void*)interfaceList);

  if(!interfaceList) {
    fprintf(stderr, "Interface List pointer is NULL!\n");
    return 1;
  }

  for (InterfaceReg *current = interfaceList; current; current = current->m_pNext) {
      fprintf(stderr, "%s => %p\n", current->m_pName, current->m_CreateFn());
  }

  return 0;
}

// TODO: Update function to scan byte signature of the CreateInterface function
// I have assumed the instructions returned would infer the bytes in the mov instruction
// This will never match unless the mov instruction is the only instruction inside CreateInterface
// Scan bytes in function -> Find matching start byte -> Check trailing bytes (2nd and 3rd) from 1st byte position
// If match, get address of the interface list from the instruction presented
// and (here's hoping) that should always return the inteface list.
// Feels like I'm missing something???
void client_dynamic(void *handle) {
  // Get the address of CreateInterface from libclient.so exports.
  void* fnPointer = dlsym(handle, "CreateInterface");

  if(!fnPointer) {
    fprintf(stderr, "Failed to find CreateInterface export!\n");
    return;
  }

  fprintf(stderr, "client_dynamic() called.\n");
  fprintf(stderr, "CreateInterface found at %p: \n", fnPointer);

  // Cast to byte array to read the assembly opcodes.
  uint8_t* instructions = (uint8_t*)fnPointer;

  // Expected OPCodes: 48 8D 1D
  // mov rbx, [rip+disp32]
  //
  // defuse.ca output:
  // 0:  48 8b 1d 19 51 71 02    mov    rbx,QWORD PTR [rip+0x2715119]        # 0x2715120 
  //      1  2  3  4  5  6  7 = 7 total bytes.
  //
  // 1    - REX Prefix (1 byte)
  // 2    - Opcode (1 byte)
  // 3    - ModR/M byte (1 byte)
  // 4-7  - Immediate displacement (4 bytes)
  //
  // 1-3 (from my understanding) are considered the opcode in the instruction aka `mov`
  // 4-7 is the data for the calculation.
  
  // This is a bit hacky, tbh
  if(instructions[0] == 0x48 && instructions[1] == 0x8B && instructions[2] == 0x1D) {
    // Extract the 4-byte displacement.
    int32_t displacement;
    memcpy(&displacement, &instructions[3], 4); // Data from instruction is 4 bytes long, copy 4 bytes into our displacement variable.

    // Calculate absolute address of InterfaceReg pointer.
    uintptr_t nextInst = (uintptr_t)fnPointer + 7; // Function pointer + size of instruction
    uintptr_t listAddr = nextInst + displacement;

    // Dereference to get the list head.
    InterfaceReg* interfaceList = *(InterfaceReg**)listAddr;

    if(!interfaceList) {
      fprintf(stderr, "Interface list pointer is NULL\n");
      return;
    }

    fprintf(stderr, "client_dynamic() completed! Interface list is at: %p\n", (void*)listAddr);

    for (InterfaceReg *current = interfaceList; current; current = current->m_pNext) {
      fprintf(stderr, "Interface: %s\n", current->m_pName);
    }
  } else {
    fprintf(stderr, "Instruction pattern did not match analyzed output.\n");
  }
}
