/* 
 * File:   DictionariesRepository.hpp
 * Author: lennyn
 *
 * Created on August 8, 2014, 3:49 PM
 */

#ifndef DICTIONARIESREPOSITORY_HPP
#define	DICTIONARIESREPOSITORY_HPP

#include "morfeusz2.h"
#include "Dictionary.hpp"
#include "const.hpp"
#include <map>
#include <string>

namespace morfeusz {

    class DictionariesRepository {
    public:
        const Dictionary* getDictionary(const std::string& name, MorfeuszProcessorType processorType);

    private:
        
        bool hasLoadedDictionary(const std::string& name, MorfeuszProcessorType processorType) const;
        
        std::string getPathToDictionary(const std::string& name, MorfeuszProcessorType processorType) const;
        
        const Dictionary* doLoadDictionary(const std::string& name, MorfeuszProcessorType processorType) const;

        struct RepositoryEntry {
            Dictionary* analyzerDictionary;
            Dictionary* generatorDictionary;
        };
        
        std::map<std::string, RepositoryEntry> entriesMap;
    };
}

#endif	/* DICTIONARIESREPOSITORY_HPP */

