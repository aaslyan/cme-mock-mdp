# GDB script to debug apply_packet function
# Run with: source debug_script.gdb

echo \n=== APPLY_PACKET DEBUG SCRIPT ===\n

echo \n1. FUNCTION ENTRY:\n
echo    data parameter: 
p data
echo    len parameter: 
p len
echo    fp parameter: 
p fp

echo \n2. INITIAL PACKET BYTES (first 32):\n
x/32xb data

echo \n3. AFTER: uint8_t *lastbyte = data+len;\n
echo    lastbyte: 
p lastbyte
echo    lastbyte - data: 
p lastbyte - data

echo \n4. AFTER: PacketHeader *pkthdr = (PacketHeader *)data;\n
echo    pkthdr: 
p pkthdr
echo    pkthdr == data: 
p pkthdr == data
echo    sizeof(*pkthdr): 
p sizeof(*pkthdr)

echo \n5. PACKET HEADER CONTENTS:\n
echo    MsgSeqNum: 
p pkthdr->MsgSeqNum
echo    SendingTime: 
p/x pkthdr->SendingTime

echo \n6. BEFORE: data += sizeof(*pkthdr);\n
echo    Current data: 
p data
echo    data - pkthdr: 
p data - (uint8_t*)pkthdr

echo \n7. STEP OVER: data += sizeof(*pkthdr);\n
next
echo    After increment data: 
p data
echo    data - pkthdr: 
p data - (uint8_t*)pkthdr

echo \n8. MESSAGE SIZE LOCATION:\n
echo    Bytes at offset 12-13: 
x/2xb (uint8_t*)pkthdr + 12
echo    Bytes at current data: 
x/2xb data

echo \n9. WHILE LOOP CONDITION:\n
echo    data < lastbyte: 
p data < lastbyte
echo    data: 
p data
echo    lastbyte: 
p lastbyte

echo \n10. IF ENTERING LOOP - READ MSGSIZE:\n
echo    Step to: uint16_t msgsize = *(uint16_t *)data;\n
next
echo    msgsize value: 
p msgsize
echo    msgsize hex: 
p/x msgsize

echo \n11. AFTER: data += sizeof(msgsize);\n
next
echo    data after msgsize: 
p data
echo    data - pkthdr: 
p data - (uint8_t*)pkthdr
echo    lastbyte - data: 
p lastbyte - data

echo \n12. BEFORE msghdr.wrap() CALL:\n
echo    Buffer pointer (data): 
p data
echo    Buffer contents: 
x/8xb data
echo    Buffer length (lastbyte-data): 
p lastbyte - data

echo \n=== END DEBUG SCRIPT ===\n