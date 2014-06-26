/* 
 * File:   morfeusz2.h
 * Author: mlenart
 *
 * Created on 13 czerwiec 2014, 17:28
 */

#ifndef MORFEUSZ2_H
#define	MORFEUSZ2_H

#include <vector>
#include <string>
#include <list>

#include "morfeusz2_version.h"

namespace morfeusz {

    class MorphInterpretation;
    class Morfeusz;
    class ResultsIterator;
    template <class T> class Tagset;
    class MorfeuszException;

    enum Charset {
        UTF8 = 101,
        //    UTF16LE,
        //    UTF16BE,
        //    UTF32,
        ISO8859_2 = 102,
        CP1250 = 103,
        CP852 = 104
    };

    enum TokenNumbering {
        /**
         * Start from 0. Reset counter for every invocation of Morfeusz::analyze
         */
        SEPARATE = 201,

        /**
         * Also start from 0. Reset counter for every invocation of Morfeusz::setTokenNumbering only
         */
        CONTINUOUS = 202
    };

    enum CaseHandling {
        /**
         * Case-sensitive but allows interpretations that do not match case but there are no alternatives
         */
        WEAK = 100,

        /**
         * Strictly case-sensitive, reject all interpretations that do not match case
         */
        STRICT = 101,

        /**
         * Case-insensitive - ignores case
         */
        IGNORE = 102
    };
    
    enum WhitespaceHandling {
        
        /**
         * Ignore whitespaces
         */
        SKIP = 301,
        
        /**
         * Append whitespaces to previous MorphInterpretation
         */
        APPEND = 302,
        
        /**
         * Whitespaces are separate MorphInterpretation objects
         */
        KEEP = 303
    };

    /**
     * Performs morphological analysis (analyze methods) and syntesis (generate methods).
     * 
     * It is NOT thread-safe
     * but it is possible to use separate Morfeusz instance for each concurrent thread.
     */
    class Morfeusz {
    public:

        static std::string getVersion();

        /**
         * Creates actual instance of Morfeusz class.
         * The caller is responsible for destroying it.
         * 
         * @return 
         */
        static Morfeusz* createInstance();

        /**
         * Set a file used for morphological analysis.
         * 
         * @param filename
         */
        virtual void setAnalyzerDictionary(const std::string& filename) = 0;

        /**
         * Set a file used for morphological synthesis.
         * 
         * @param filename
         */
        virtual void setGeneratorDictionary(const std::string& filename) = 0;

        virtual ~Morfeusz();

        /**
         * Analyze given text and return the results as iterator.
         * 
         * @param text - text for morphological analysis
         * @return - iterator over morphological analysis results
         */
        virtual ResultsIterator* analyze(const std::string& text) const = 0;

        /**
         * Perform morphological analysis on a given text and put results in a vector.
         * 
         * @param text - text to be analyzed
         * @param result - results vector
         */
        virtual void analyze(const std::string& text, std::vector<MorphInterpretation>& result) const = 0;

        /**
         * Perform morphological synthesis on a given lemma and put results in a vector.
         * 
         * @param lemma - lemma to be analyzed
         * @param result - results vector
         */
        virtual void generate(const std::string& lemma, std::vector<MorphInterpretation>& result) const = 0;

        /**
         * Perform morphological synthesis on a given lemma and put results in a vector.
         * Limit results to interpretations with the specified tag.
         * 
         * @param lemma - lemma to be analyzed
         * @param tag - tag of result interpretations
         * @param result - results vector
         */
        virtual void generate(const std::string& lemma, int tagnum, std::vector<MorphInterpretation>& result) const = 0;

        /**
         * Set encoding for input and output string objects.
         * 
         * @param encoding
         */
        virtual void setCharset(Charset encoding) = 0;

        /**
         * Set aggl segmentation option value.
         * 
         * @param aggl
         */
        virtual void setAggl(const std::string& aggl) = 0;

        /**
         * Set praet segmentation option value.
         * 
         * @param praet
         */
        virtual void setPraet(const std::string& praet) = 0;

        /**
         * Set case handling.
         * 
         * @param caseSensitive
         */
        virtual void setCaseHandling(CaseHandling caseHandling) = 0;

        /**
         * Set token numbering policy.
         * 
         * @param numbering
         */
        virtual void setTokenNumbering(TokenNumbering numbering) = 0;
        
        /**
         * Set whitespace handling.
         * 
         * @param numbering
         */
        virtual void setWhitespaceHandling(WhitespaceHandling whitespaceHandling) = 0;

        /**
         * Set debug option value.
         * 
         * @param debug
         */
        virtual void setDebug(bool debug) = 0;
        
        /**
         * Gets default tagset used for morphological analysis.
         * @return 
         */
        virtual const Tagset<std::string>& getDefaultAnalyzerTagset() const = 0;
        
        /**
         * Gets default tagset used for morphological synthesis.
         * @return 
         */
        virtual const Tagset<std::string>& getDefaultGeneratorTagset() const = 0;
    };

    class ResultsIterator {
    public:
        virtual bool hasNext() const = 0;
        virtual const MorphInterpretation& peek() const = 0;
        virtual MorphInterpretation next() = 0;
        ~ResultsIterator() {}
    };
    
        /**
     * Represents a tagset
     */
    template <class T>
    class Tagset {
    public:
        
        /**
         * Returns tag (denoted by its index).
         * 
         * @param tagNum - tag index in the tagset.
         * @return - the tag
         */
        virtual const T& getTag(const int tagNum) const = 0;

        /**
         * Returns named entity type (denoted by its index).
         * 
         * @param nameNum - name index in the tagset.
         * @return - the named entity type
         */
        virtual const T& getName(const int nameNum) const = 0;
        
        /**
         * Returs number of tags this tagset contains.
         * 
         * @return 
         */
        virtual size_t getTagsSize() const = 0;
        
        /**
         * Returs number of named entity types this tagset contains.
         * 
         * @return 
         */
        virtual size_t getNamesSize() const = 0;
        
        virtual ~Tagset() {}
    };

    /**
     The result of analysis is  a directed acyclic graph with numbered
     nodes representing positions  in text (points _between_ segments)
     and edges representing interpretations of segments that span from
     one node to another.  E.g.,

         {0,1,"ja","ja","ppron12:sg:nom:m1.m2.m3.f.n1.n2:pri"}
         |
         |      {1,2,"został","zostać","praet:sg:m1.m2.m3:perf"}
         |      |
       __|  ____|   __{2,3,"em","być","aglt:sg:pri:imperf:wok"}
      /  \ /     \ / \
     * Ja * został*em *
     0    1       2   3

     Note that the word 'zostałem' got broken into 2 separate segments.

     The structure below describes one edge of this DAG:

    */
    class MorphInterpretation {
    public:
        
        /**
         * 
         * @param startNode - number of start node in DAG.
         * @param endNode - number of end node in DAG.
         * @param orth - orthographic form
         * @param lemma - base form
         * @param tagnum - tag identifier (0 for "unrecognized", 1 for "whitespace")
         * @param namenum - named entity identifier (0 for "not a named entity")
         * @param qualifiers - pointer to vector of qualifiers (not owned by this)
         * @param tagset - pointer to default tagset used by Morfeusz (not owned by this)
         */
        MorphInterpretation(
                int startNode,
                int endNode,
                const std::string& orth,
                const std::string& lemma,
                int tagnum,
                int namenum,
                const std::vector<std::string>* qualifiers,
                const Tagset<std::string>* tagset);
        
        MorphInterpretation();
        
        /**
         * Creates new instance with "ign" tag (meaning: "not found in the dictionary")
         */
        static MorphInterpretation createIgn(int startNode, int endNode, const std::string& orth, const Tagset<std::string>& tagset);
        
        /**
         * Creates new instance with "sp" tag (meaning: "this is a sequence of whitespaces")
         */
        static MorphInterpretation createWhitespace(int startNode, int endNode, const std::string& orth, const Tagset<std::string>& tagset);

        inline int getStartNode() const {
            return startNode;
        }

        inline int getEndNode() const {
            return endNode;
        }

        inline const std::string& getOrth() const {
            return orth;
        }

        inline const std::string& getLemma() const {
            return lemma;
        }

        inline int getTagnum() const {
            return tagnum;
        }

        inline int getNamenum() const {
            return namenum;
        }
        
        inline bool isIgn() const {
            return tagnum == 0;
        }
        
        inline bool isWhitespace() const {
            return tagnum == 1;
        }

        inline const std::string& getTag() const {
            return tagset->getTag(tagnum);
        }

        inline const std::string& getName() const {
            return tagset->getName(namenum);
        }

        inline const std::vector<std::string>& getQualifiers() const {
            return *qualifiers;
        }

        bool hasHomonym(const std::string& homonymId) const;

        std::string toString(bool includeNodeNumbers) const;
    private:
        int startNode;
        int endNode;
        std::string orth;
        std::string lemma;
        int tagnum;
        int namenum;

        /**
         * not owned by this
         */
        const std::vector<std::string>* qualifiers;

        /**
         * not owned by this
         */
        const Tagset<std::string>* tagset;
    };

    class MorfeuszException : public std::exception {
    public:

        MorfeuszException(const std::string& what) : msg(what.c_str()) {
        }

        virtual ~MorfeuszException() throw () {
        }

        virtual const char* what() const throw () {
            return this->msg.c_str();
        }
    private:
        const std::string msg;
    };

    class FileFormatException : public MorfeuszException {
    public:

        FileFormatException(const std::string& what) : MorfeuszException(what) {
        }
    };
}

#endif	/* MORFEUSZ2_H */

