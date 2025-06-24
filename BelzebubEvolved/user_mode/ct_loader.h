#pragma once
#include <string>
#include <vector>

namespace CTLoader {
    struct MemoryEntry {
        int id = 0;
        std::string description;
        std::string type;
        std::string address;
        std::string value;
        std::vector<MemoryEntry> children;
    };

    struct CheatTable {
        std::vector<MemoryEntry> entries;
    };

    class CTParser {
    public:
        // La función pública principal: parsea desde un string de contenido
        bool parse(const std::string& content, CheatTable& out_table);
    private:
        // Función de ayuda recursiva con nivel de profundidad para logging
        void parseEntriesRecursive(const std::string& block, std::vector<MemoryEntry>& entries, int depth);
    };
    
    // Función de utilidad para extraer texto entre delimitadores
    std::string extractTextBetween(const std::string& content, const std::string& start, const std::string& end);
} 