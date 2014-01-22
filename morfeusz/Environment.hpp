/* 
 * File:   Environment.hpp
 * Author: mlenart
 *
 * Created on 22 styczeń 2014, 12:08
 */

#ifndef ENVIRONMENT_HPP
#define	ENVIRONMENT_HPP

#include "charset/CaseConverter.hpp"
#include "charset/CharsetConverter.hpp"
#include "const.hpp"
#include "Tagset.hpp"


class Environment {
public:
    Environment(
            const Tagset& analyzerTagset, 
            const Tagset& generatorTagset,
            MorfeuszCharset charset);
    void setCharset(MorfeuszCharset charset);
    const CharsetConverter& getCharsetConverter() const;
    
    void setAnalyzerTagset(const Tagset& tagset);
    const Tagset& getAnalyzerTagset() const;
    
    void setGeneratorTagset(const Tagset& tagset);
    const Tagset& getGeneratorTagset() const;
    
    const CaseConverter& getCaseConverter() const;
    
    virtual ~Environment();
private:
    const CharsetConverter* currentCharsetConverter;
    const UTF8CharsetConverter utf8CharsetConverter;
    const ISO8859_2_CharsetConverter isoCharsetConverter;
    const Windows_1250_CharsetConverter cp1250CharsetConverter;
    const CP852_CharsetConverter cp852CharsetConverter;
    Tagset analyzerTagset;
    Tagset generatorTagset;
    const CaseConverter caseConverter;
    
    const CharsetConverter* getCharsetConverter(MorfeuszCharset charset) const;
};

#endif	/* ENVIRONMENT_HPP */

