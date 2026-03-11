# This script adds a new symbol or patch address to `main.ld`, checking that the order is correct.
# When creating a patch, it also adds template asm instructions to `patches.s` and `hooks.s`.
# Use by passing, in any order, `s` or `p` for Symbol or Patch, the hexadecimal address, a name,
# and optionally `KOR` or `TWN` for those regions (e.g. `p 0x4352F4 PlayInit`).
# Call with no arguments to just check the address order in `main.ld`.

import sys, re

TYPE_PATCH = 'p'
TYPE_SYMBOL = 's'

MAIN_LD_FILE_NAME = 'main'

newAddress = None
newName = None
type = None
ldFile = MAIN_LD_FILE_NAME

for arg in sys.argv[1:]:
    if arg in [TYPE_PATCH, TYPE_SYMBOL]:
        type = arg
    elif arg in ['KOR', 'TWN']:
        ldFile = arg
    else:
        try:
            newAddress = int(arg, 16)
        except ValueError:
            newName = arg

if type == None:
    type = TYPE_PATCH

LD_SYM_TEMPLATE='''\
	%s = 0x%X%s;
'''

LD_PATCH_TEMPLATE='''\
	.patch_%s 0x%X%s : { *(.patch_%s) }
'''

ASM_PATCH_TEMPLATE='''
.section .patch_%s
    bl hook_%s
'''

ASM_HOOK_TEMPLATE='''
.global hook_%s
hook_%s:
    push {r0-r12, lr}
    bl %s
    pop {r0-r12, lr}
    cpy r0,r0
    bx lr
'''

lastAddress = 0
isOrderCorrect = True
insertIndex = 0

with open('linker_scripts/%s.ld' % ldFile, 'r+', newline='') as file:
    lines = file.readlines()
    for lineIndex, line in enumerate(lines):
        # Search lines with memory addresses
        result = re.search("0x[a-f-A-F0-9]{6,}", line)
        if result:
            mustCheckLineAddress = type == TYPE_PATCH if "patch_" in line else type == TYPE_SYMBOL
            addressString = result.group(0)
            address = int(addressString, 16)
            # Detect errors in the address order
            if lastAddress > address:
                print('Order error detected: line %d, address %s' % (lineIndex, addressString))
                isOrderCorrect=False
            lastAddress = address
            # Choose where the new address will be added
            if newAddress:
                if mustCheckLineAddress:
                    typeString = 'patch' if type == TYPE_PATCH else 'symbol'
                    if address == newAddress:
                        print('ERROR: another %s is at address 0x%X' % (typeString, address))
                        sys.exit()
                    if abs(address - newAddress) < 0x10:
                        print('WARNING: another %s is at address 0x%X' % (typeString, address))
                if not insertIndex and address > newAddress:
                    insertIndex = lineIndex
    # Add the new address if no errors have been detected
    if newAddress and isOrderCorrect:
        if not newName:
            newName = input('Insert name for the new %s' % 'patch: patch_' if type == TYPE_PATCH else 'symbol: ')
        offsetStr = ' + _LD_OFF' if ldFile == MAIN_LD_FILE_NAME else ''
        if type == TYPE_PATCH:
            newLine = LD_PATCH_TEMPLATE % (newName, newAddress, offsetStr, newName)
        else:
            newLine = LD_SYM_TEMPLATE % (newName, newAddress, offsetStr)
        lines.insert(insertIndex, newLine)
        file.seek(0)
        file.writelines(lines)

if isOrderCorrect:
    if not newAddress:
        print('Order is correct')
    elif type == TYPE_PATCH:
        if ldFile == MAIN_LD_FILE_NAME:
            # Add the template asm to `patches.s` and `hooks.s`
            with open('src/asm/patches.s', 'a', newline='') as file:
                file.write(ASM_PATCH_TEMPLATE % (newName, newName))
            with open('src/asm/hooks.s', 'a', newline='') as file:
                file.write(ASM_HOOK_TEMPLATE % (newName, newName, newName))
        print('Patch added')
    else:
        print('Symbol added')
