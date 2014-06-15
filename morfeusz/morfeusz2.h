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

namespace morfeusz {

    class MorphInterpretation;
    class Morfeusz;
    class ResultsIterator;
    class Environment;
    class MorfeuszException;

    enum Charset {
        UTF8,
        //    UTF16LE,
        //    UTF16BE,
        //    UTF32,
        ISO8859_2,
        CP1250,
        CP852
    };

    /**
     * Performs morphological analysis (analyze methods) and syntesis (generate methods).
     * 
     * It is NOT thread-safe
     * but it is possible to use separate Morfeusz instance for each concurrent thread.
     */
    class Morfeusz {
    public:

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

        /**
         * Destroys Morfeusz object.
         */
        virtual ~Morfeusz();

        /**
         * Analyze given text and return the results as iterator.
         * 
         * @param text - text for morphological analysis
         * @return - iterator over morphological analysis results
         */
        virtual ResultsIterator analyze(const std::string& text) const = 0;

        /**
         * Perform morphological analysis on a given text and put results in a vector.
         * 
         * @param text - text to be analyzed
         * @param result - results vector
         */
        virtual void analyze(const std::string& text, std::vector<MorphInterpretation>& result) const = 0;

        /**
         * Perform morphological synthesis on a given lemma and return the results as iterator.
         * 
         * @param text - text for morphological analysis
         * @return - iterator over morphological analysis results
         */
        virtual ResultsIterator generate(const std::string& lemma) const = 0;

        /**
         * Perform morphological synthesis on a given lemma and return the results as iterator.
         * Limit results to interpretations with the specified tag.
         * 
         * @param text - text for morphological analysis
         * @param tag - tag of result interpretations
         * @return - iterator over morphological analysis results
         */
        virtual ResultsIterator generate(const std::string& lemma, int tagnum) const = 0;

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
         * If set to true characters case in analyzed text must match
         * the case in the recognized forms from dictionary.
         * 
         * @param caseSensitive
         */
        virtual void setCaseSensitive(bool caseSensitive) = 0;

        /**
         * Set debug option value.
         * 
         * @param debug
         */
        virtual void setDebug(bool debug) = 0;

        friend class ResultsIterator;
    };

    class ResultsIterator {
    public:
        ResultsIterator(const std::vector<MorphInterpretation>& res);
        MorphInterpretation getNext();
        bool hasNext();
        friend class Morfeusz;
    private:
        const char* rawInput;
        std::list<MorphInterpretation> resultsBuffer;
        int startNode;
    };

    /**
     * Morphological interpretation as seen by the user in the analysis/generation results.
     */
    class MorphInterpretation {
    public:
        MorphInterpretation(
                int startNode,
                int endNode,
                const std::string& orth,
                const std::string& lemma,
                int tagnum,
                int namenum,
                int qualifiersNum,
                const Environment& env);
        MorphInterpretation();
        static MorphInterpretation createIgn(int startNode, const std::string& orth, const Environment& env);
        //    virtual ~MorphInterpretation();
        int getStartNode() const;
        int getEndNode() const;
        const std::string& getOrth() const;
        const std::string& getLemma() const;
        //    const std::string& getHomonymId() const;
        bool hasHomonym(const std::string& homonymId) const;
        int getTagnum() const;
        int getNamenum() const;
        const std::string& getTag() const;
        const std::string& getName() const;
        const std::vector<std::string>& getQualifiers() const;

        std::string toString(bool includeNodeNumbers) const;
    private:
        MorphInterpretation(
                int startNode,
                const std::string& orth,
                const Environment& env);
        int startNode;
        int endNode;
        std::string orth;
        std::string lemma;
        //    std::string homonymId;
        int tagnum;
        int namenum;
        std::string tag;
        std::string name;
        const std::vector<std::string>* qualifiers;
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

    class FileFormatException : public std::exception {
    public:

        FileFormatException(const std::string& what) : msg(what.c_str()) {
        }

        virtual ~FileFormatException() throw () {
        }

        virtual const char* what() const throw () {
            return this->msg.c_str();
        }
    private:
        const std::string msg;
    };
}

#endif	/* MORFEUSZ2_H */
