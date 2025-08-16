#include <iostream>
#include <vector>
#include "cme_sbe/MDIncrementalRefreshBook46.h"
#include "cme_sbe/SnapshotFullRefresh52.h"
#include "cme_sbe/MessageHeader.h"

int main() {
    std::cout << "Testing SBE Generated Code" << std::endl;
    
    // Create a buffer for testing
    std::vector<char> buffer(1024);
    
    try {
        // Print schema information without initializing messages
        std::cout << "Schema ID: " << cme_sbe::MDIncrementalRefreshBook46::sbeSchemaId() << std::endl;
        std::cout << "Schema Version: " << cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion() << std::endl;
        std::cout << "Template ID: " << cme_sbe::MDIncrementalRefreshBook46::sbeTemplateId() << std::endl;
        std::cout << "Block Length: " << cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength() << std::endl;
        
        std::cout << "Snapshot Schema ID: " << cme_sbe::SnapshotFullRefresh52::sbeSchemaId() << std::endl;
        std::cout << "Snapshot Schema Version: " << cme_sbe::SnapshotFullRefresh52::sbeSchemaVersion() << std::endl;
        std::cout << "Snapshot Template ID: " << cme_sbe::SnapshotFullRefresh52::sbeTemplateId() << std::endl;
        std::cout << "Snapshot Block Length: " << cme_sbe::SnapshotFullRefresh52::sbeBlockLength() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "SBE Exception: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "SBE test completed successfully!" << std::endl;
    return 0;
}