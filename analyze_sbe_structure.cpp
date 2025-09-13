#include "cme_sbe/MDIncrementalRefreshBook46.h"
#include "cme_sbe/MessageHeader.h"
#include "utils/hexdump.h"
#include <iostream>

using namespace cme_sbe;

int main()
{
    std::cout << "=== SBE MDIncrementalRefreshBook46 Structure Analysis ===" << std::endl;
    
    // Analyze SBE constants
    std::cout << "SBE Constants:" << std::endl;
    std::cout << "  Template ID: " << MDIncrementalRefreshBook46::sbeTemplateId() << std::endl;
    std::cout << "  Schema ID: " << MDIncrementalRefreshBook46::sbeSchemaId() << std::endl;
    std::cout << "  Schema Version: " << MDIncrementalRefreshBook46::sbeSchemaVersion() << std::endl;
    std::cout << "  Block Length: " << MDIncrementalRefreshBook46::sbeBlockLength() << std::endl;
    std::cout << "  Header Length: " << MessageHeader::encodedLength() << std::endl;
    
    std::cout << "\nGroup Information:" << std::endl;
    std::cout << "  NoMDEntries:" << std::endl;
    std::cout << "    Block Length: " << MDIncrementalRefreshBook46::NoMDEntries::sbeBlockLength() << std::endl;
    std::cout << "    Header Size: " << MDIncrementalRefreshBook46::NoMDEntries::sbeHeaderSize() << std::endl;
    
    std::cout << "  NoOrderIDEntries:" << std::endl;
    std::cout << "    Block Length: " << MDIncrementalRefreshBook46::NoOrderIDEntries::sbeBlockLength() << std::endl;
    std::cout << "    Header Size: " << MDIncrementalRefreshBook46::NoOrderIDEntries::sbeHeaderSize() << std::endl;
    
    // Calculate expected message sizes for different entry counts
    std::cout << "\nExpected message sizes:" << std::endl;
    
    for (int md_entries = 0; md_entries <= 5; ++md_entries) {
        for (int order_entries = 0; order_entries <= 3; ++order_entries) {
            size_t size = MessageHeader::encodedLength() +  // SBE header: 8 bytes
                         MDIncrementalRefreshBook46::sbeBlockLength() +  // Fixed fields: 11 bytes
                         MDIncrementalRefreshBook46::NoMDEntries::sbeHeaderSize() +  // Group header: 3 bytes
                         (md_entries * MDIncrementalRefreshBook46::NoMDEntries::sbeBlockLength()) +  // MD entries
                         MDIncrementalRefreshBook46::NoOrderIDEntries::sbeHeaderSize() +  // Group header: 3 bytes  
                         (order_entries * MDIncrementalRefreshBook46::NoOrderIDEntries::sbeBlockLength());  // Order entries
            
            std::cout << "  " << md_entries << " MDEntries + " << order_entries << " OrderEntries = " << size << " bytes";
            if (size == 88) {
                std::cout << " ← MATCHES CME!";
            }
            std::cout << std::endl;
        }
    }
    
    std::cout << "\nOur current message (62 bytes) breakdown:" << std::endl;
    std::cout << "  SBE Header: 8 bytes" << std::endl;
    std::cout << "  Fixed Fields: 11 bytes" << std::endl;
    std::cout << "  NoMDEntries header: 3 bytes" << std::endl;
    std::cout << "  1 MDEntry: 32 bytes" << std::endl;
    std::cout << "  NoOrderIDEntries header: 3 bytes" << std::endl;
    std::cout << "  0 OrderEntries: 0 bytes" << std::endl;
    std::cout << "  Total: 8 + 11 + 3 + 32 + 3 + 0 = 57 bytes" << std::endl;
    std::cout << "  Actual: 62 bytes (5 bytes padding/alignment?)" << std::endl;
    
    std::cout << "\nTo reach 88 bytes, we need:" << std::endl;
    std::cout << "  88 - 8 - 11 - 3 - 3 = 63 bytes for groups" << std::endl;
    std::cout << "  Current groups: 32 bytes (1 MDEntry)" << std::endl;
    std::cout << "  Missing: 63 - 32 = 31 bytes" << std::endl;
    std::cout << "  Could be:" << std::endl;
    std::cout << "    - 1 more MDEntry (32 bytes) - too much" << std::endl;
    std::cout << "    - 1 OrderEntry (24 bytes) + 7 bytes padding" << std::endl;
    std::cout << "    - Different field encoding or additional data" << std::endl;
    
    return 0;
}