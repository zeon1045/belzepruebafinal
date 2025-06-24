#include "ct_loader.h"
#include <iostream>
#include <string>
#include <vector>

namespace CTLoader {

    // Función de ayuda para extraer texto
    std::string extractTextBetween(const std::string& content, const std::string& start, const std::string& end, size_t& search_pos) {
        size_t start_pos = content.find(start, search_pos);
        if (start_pos == std::string::npos) return "";
        start_pos += start.length();
        size_t end_pos = content.find(end, start_pos);
        if (end_pos == std::string::npos) return "";
        
        search_pos = end_pos + end.length();

        std::string result = content.substr(start_pos, end_pos - start_pos);
        if (result.length() > 1 && result.front() == '"' && result.back() == '"') {
            return result.substr(1, result.length() - 2);
        }
        return result;
    }
    
    // Función recursiva con logging detallado
    void CTParser::parseEntriesRecursive(const std::string& block, std::vector<MemoryEntry>& entries, int depth) {
        std::string indent(depth * 2, ' ');
        size_t current_pos = 0;
        int entry_count = 0;

        while (true) {
            size_t entry_start = block.find("<CheatEntry>", current_pos);
            if (entry_start == std::string::npos) break;
            
            size_t entry_end = block.find("</CheatEntry>", entry_start);
            if (entry_end == std::string::npos) break;

            entry_count++;
            std::cout << indent << "[CT PARSER] Procesando Entrada #" << entry_count << " en nivel " << depth << std::endl;

            std::string entry_block = block.substr(entry_start + 12, entry_end - (entry_start + 12));
            
            MemoryEntry new_entry = {};
            size_t field_pos = 0;
            
            std::string id_str = extractTextBetween(entry_block, "<ID>", "</ID>", field_pos);
            if (!id_str.empty()) new_entry.id = std::stoi(id_str);
            new_entry.description = extractTextBetween(entry_block, "<Description>", "</Description>", field_pos);
            new_entry.type = extractTextBetween(entry_block, "<VariableType>", "</VariableType>", field_pos);
            new_entry.address = extractTextBetween(entry_block, "<Address>", "</Address>", field_pos);

            std::cout << indent << "  -> ID: " << new_entry.id << ", Desc: '" << new_entry.description << "'" << std::endl;

            size_t children_start = entry_block.find("<CheatEntries>");
            if (children_start != std::string::npos) {
                std::cout << indent << "  -> Encontradas entradas anidadas. Recusando..." << std::endl;
                size_t children_end = entry_block.find("</CheatEntries>", children_start);
                if (children_end != std::string::npos) {
                    std::string children_block = entry_block.substr(children_start + 14, children_end - (children_start + 14));
                    parseEntriesRecursive(children_block, new_entry.children, depth + 1);
                }
            }
            
            entries.push_back(new_entry);
            current_pos = entry_end + 13;
        }
    }
    
    // Función principal con logging
    bool CTParser::parse(const std::string& content, CheatTable& out_table) {
        std::cout << "\n[CT PARSER] ========================================" << std::endl;
        std::cout << "[CT PARSER] INICIANDO PARSEO. Tamano del contenido: " << content.length() << " bytes." << std::endl;
        
        out_table.entries.clear();
        
        size_t entries_start = content.find("<CheatEntries>");
        if (entries_start == std::string::npos) {
            std::cerr << "[CT PARSER] FALLO: No se encontro el bloque principal <CheatEntries>." << std::endl;
            return false;
        }
        
        size_t entries_end = content.rfind("</CheatEntries>");
        if (entries_end == std::string::npos) {
            std::cerr << "[CT PARSER] FALLO: No se encontro el cierre </CheatEntries>." << std::endl;
            return false;
        }

        std::cout << "[CT PARSER] Bloque principal encontrado. Iniciando parseo recursivo." << std::endl;
        std::string main_block = content.substr(entries_start + 14, entries_end - (entries_start + 14));
        parseEntriesRecursive(main_block, out_table.entries, 0);

        if (out_table.entries.empty()) {
            std::cerr << "[CT PARSER] FALLO: El parseo finalizo sin encontrar entradas validas." << std::endl;
            return false;
        }
        
        std::cout << "[CT PARSER] EXITO: Parseo finalizado. Entradas principales: " << out_table.entries.size() << std::endl;
        std::cout << "[CT PARSER] ========================================" << std::endl;
        return true;
    }

} // namespace CTLoader 