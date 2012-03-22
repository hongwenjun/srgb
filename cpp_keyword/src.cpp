/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "3ed_query.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>

using std::set;
using std::string;
using std::getline;
using std::map;
using std::vector;
using std::cerr;
using std::cout;
using std::cin;
using std::ifstream;
using std::endl;
using std::pair;
using std::make_pair;
using std::ispunct;
using std::tolower;
using std::strlen;

// read input file: store each line as element in lines_of_text 
void TextQuery::store_file(ifstream &is)
{
    string textline;
    while (getline(is, textline))
       lines_of_text.push_back(textline);
}
set<string> TextQuery::exclusion_set = build_exclusion_set();

// \v: vertical tab; \f: formfeed; \r: carriage return are
// treated as whitespace characters along with space, tab and newline
string TextQuery::whitespace_chars(" \t\n\v\r\f");

// finds the whitespace separated words in the input vector
// then puts them in a cannonical form: eliminate suffixes,
// make lowercase etc. Finally, test if its an excluded word or not.
// If not excluded store the word in word_map along with
// its location (line # and character position) info
void TextQuery::build_map()
{
    // process each line from the input vector
    for (line_no line_num = 0; 
                 line_num != lines_of_text.size();
                 ++line_num)
    {
        string textline = lines_of_text[line_num];
        if (textline.empty()) continue; //ignore blank lines

        // make line lower case and remove extraneous punctuation
        strip_caps(textline);
        strip_punct(textline);

        /* 
         * Because we want to keep track of position as well as
         * line number, we have to process the line a character at a time.
         * We can't use an istringstream to read the words.
         * prev_pos will denote first character in each word,
         * pos will denote whitespace that separates the word from the next
         * initially pos denotes first non-whitespace at beginning of line
        */
        str_size pos = skip_whitespace(textline, 0), prev_pos = pos;

        // find each whitespace separated word
        while ((pos = textline.find_first_of(whitespace_chars, pos)) 
                   != string::npos)
        {
            // remove suffixes and put the word into the map if apporpriate
            test_insert(textline, prev_pos, pos, line_num);

            // if there's more text to process, increment pos to get next char
            if (pos != textline.size())
                ++pos;  
            // read and discard adjacent spaces, if any, updating prev_pos too
            pos = prev_pos = skip_whitespace(textline, pos);
        }

        // don't forget last word in the line
        if (pos != prev_pos)  // false if line ends in whitespace
            test_insert(textline, prev_pos, pos, line_num);
    }
}

TextQuery::str_size 
TextQuery::skip_whitespace(const string &line, str_size pos)
{
    // ignore adjacent whitespace
    str_size next = line.find_first_not_of(whitespace_chars, pos);
    if (next != string::npos)
        return next;
    else
        return line.size();
}

void TextQuery::test_insert(const string &line, str_size prev_pos, 
                  str_size pos, line_no line_num)
{
    // make copy of the whitespace delimited word
    string word(line.substr(prev_pos, pos - prev_pos));

    strip_suffixes(word); // last of the cleanup operations

    // if there's anything left after stripping punctuation 
    // and it's not an excluded word, add to the map
    // appending line num & char pos to vector for this word
    if (!word.empty() && !exclude_word(word))
         word_map[word].push_back(make_pair(line_num,prev_pos));

    return;
}

void TextQuery::strip_suffixes(string &word)
{
     if (word.size() <= 3)    // too short to have any suffixes
            return;

     if (word[word.size() - 1] == 's')  // only handle plurals so far
             suffix_s(word);

     // additional suffix handling goes here
}

void TextQuery::suffix_s(string &word)
{
    // some words ending in s aren't suffixes, they're part of the word
    static char* ok_endings[] = {"ous", "ius", "ss", "is"};
    size_t sz = sizeof(ok_endings)/sizeof(char*);  // how many elements?
    for (size_t i = 0; i != sz; ++i)
        if (chk_ending(word, ok_endings[i]) == 0)
              return;

    // replace common suffixes by their base word ending
    // repl_endings first dimension is the ending we'll remove
    //              second dimension is the new ending we'll insert
    static char* repl_endings[][2] = 
          { {"ies", "y"}, {"ses", "s"}, {"\'s", ""}, {"s", ""} };

    sz = sizeof(repl_endings)/(sizeof(char*) * 2);  // two-dimensions
    for (size_t i = 0; i != sz; ++i) 
        if (chk_ending(word, repl_endings[i][0]) == 0) {
           size_t sz = strlen(repl_endings[i][0]);
           word.replace(word.size() - sz, sz, repl_endings[i][1]);
           return;
        }
}

// compare end of the word with the ending we're given
int TextQuery::chk_ending(const string &word, const char *ending)
{
    size_t sz = strlen(ending);
    return word.compare(word.size() - sz, sz, ending);
}

void TextQuery::strip_caps(string &line)
{
    // not changing the size of line, so safe to cache the size
    str_size sz = line.size();
    for (str_size pos = 0; pos != sz; ++pos)
        line[pos] = tolower(line[pos]);
}

// except for apostrophe, replace punctuation by a space
// apostrophe is special: it might precede by 's, which is a suffix
void TextQuery::strip_punct(string &line)
{
    for (str_size pos = 0; pos != line.size(); ++pos)
        if (ispunct(line[pos])) {
             if (line[pos] != '\'') line[pos] = ' ';
        }
}

// this function should be changed to let the user specify a
// file of words to ignore, but for now, we'll keep it simple
// and assume a specific file
set<string> TextQuery::build_exclusion_set()
{
    set<string> ret;
    ifstream infile("exclusion_set");
    if (!infile)
    {
        static string default_excluded_words[] = {
          "the","and","but","that","then","are","been",
          "can","can't","cannot","could","did","for",
          "had","have","him","his","her","its","into",
          "were","which","when","with","would"
         };

        cerr << "warning! unable to open word exclusion file! -- "
            << "using default set" << endl;

        ret = set<string>(default_excluded_words, 
                          default_excluded_words + 
                          sizeof(default_excluded_words)/sizeof(string));
    } else {
        string word;
        while (infile >> word) ret.insert(word);
    }
    return ret;
}

bool TextQuery::exclude_word(const string &word)
{
    return (exclusion_set.find(word) != exclusion_set.end());
}

vector<TextQuery::location> TextQuery::run_query(const string &s)
{
    // make local copy so we can clean it up to match words
    // entered in the map; but when communicate back to the
    // user always use their original version
    string sought = s;  
    strip_caps(sought);
    strip_punct(sought);
    strip_suffixes(sought);

    // Note: must use find and not subscript the map directly
    // Subscripting a map adds the element if it's not already there
    // We want to know whether the element was there to begin with
    if (word_map.find(sought) == word_map.end())
         return vector<location>();  // not found, return empty location vector
    else
        // fetch list of locations for this word
        return word_map[sought];

}

void TextQuery::display_map()
{
    typedef map< string,vector<location> > map_text;
    map_text::iterator iter = word_map.begin(),
                   iter_end = word_map.end();

    // Note: map iter returns index, value pair
    // so iter->first is the index word, 
    // iter->second the vector of its locations

    // for each word in the map
    while (iter != iter_end) {
        cout << "word: " << iter->first << " {";

        vector<location> text_locs = iter->second;
        vector<location>::iterator liter = text_locs.begin(),
                               liter_end = text_locs.end();

        // print all (line,char) locations for this word
        // because this is a debugging routine, don't adjust
        // line/pos numbers.  As normal for programmers, start from 0
        while (liter != liter_end)
        {
            cout << "(" << liter->first
                 << "," << liter->second << ")";

            if (++liter != liter_end)
                 cout << ", ";

         }

         cout << "}\n";  // end list of output this word
         ++iter;         // get next word in the map
    }
    cout << endl;  // finished printing entire map
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#ifndef TEXTQUERY_H
#define TEXTQUERY_H
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>

/*
 * This version of the TextQuery class is more complicated than
 * the one presented in the 4th Edition and is derived from the
 * version presented in the 3rd Edition.
 *
 * It keeps track of character position as well as line number.
 * It also processes the text and query words to:
 *    strips suffixes and 
 *    ignore common words that occur too frequently to bother with.
 *
 * However, we have updated this version to use the standard library
 * facilities, thus making is simpler than the version presented in
 * the 3rd edition of the Primer.  Its basic structure is similar to
 * the version presented in the current edition.
*/
class TextQuery {
public:
    // typedef to make declarations easier
    typedef std::string::size_type str_size;
    typedef std::vector<std::string>::size_type line_no;
    typedef std::pair<line_no,str_size> location;

    /* interface:
     *    read_file builds internal data structures for the given file
     *    run_query finds the given word and returns set of lines on which it appears
     *    text_line returns a requested line from the input file
    */
    void read_file(std::ifstream &is) 
               { store_file(is); build_map(); }

    std::vector<location> run_query(const std::string&); 
    std::string text_line(line_no line) { return lines_of_text[line]; }
    str_size size() const { return lines_of_text.size(); }
    void display_map();                  // debugging aid: print the map

private:
    void store_file(std::ifstream&);  // read and store input file
    void build_map();                  // build map of each word in file

    // used by store words, ignore adjacent whitespace
    str_size skip_whitespace(const std::string&, str_size);

    // test word and if not an excluded word update map 
    void test_insert(const std::string&, str_size, str_size, line_no);

    bool exclude_word(const std::string&); // test for excluded words

    void strip_caps(std::string&);         // make lower case
    void strip_punct(std::string&);        // remove punctuation
    void strip_suffixes(std::string&);     // remove common suffixes

    void suffix_s(std::string&);           // remove suffixes ending in s
    int chk_ending(const std::string&, const char*); //used by suffix_s

    // populate exclusion_set with words to ignoare
    // Chapter 13 discusses static class members
    static std::set<std::string> build_exclusion_set(); 

private:
    // remember the whole input file
    std::vector<std::string> lines_of_text; 

    // map word to vector of all the line/char positions on which it occurs
    std::map< std::string, std::vector<location> > word_map;  

    // set of words to ignore
    static std::set<std::string> exclusion_set;

    // characters that constitute whitespace
    static std::string whitespace_chars;     
};
#endif
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "3ed_query.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>

using std::set;
using std::string;
using std::map;
using std::vector;
using std::cerr;
using std::cout;
using std::cin;
using std::ifstream;
using std::endl;
using std::pair;
using std::make_pair;

ifstream& open_file(ifstream&, const string&);
int main(int argc, char **argv)
{

    if (argc != 2) {cerr << "No input file" << endl; return -2;}

    // get a file to read from which user will query words
    ifstream infile;
    if (!open_file(infile, argv[1])) {
        cerr << "No input file!" << endl;
        return -1;
    }
    TextQuery tq;
    tq.read_file(infile);  // builds query map

    // iterate with the user: prompt for a word to find and print results
    string sought;
    do {
        cout << "enter a word against which to search the text.\n"
             << "to quit, enter a single character ==>  ";
        cin  >> sought;

        // stop if hit eof on input or single character entered
        if (!cin || sought.size() < 2) break;

        // find all the occurrences of the users requested string
        vector<TextQuery::location> locs = tq.run_query(sought);

        // report no matches
        if (locs.empty()) {
            cout << "\nSorry. There are no entries for " 
                 << sought << ".\nTry again." << endl;
            continue;
        }
    
        // if the word was found, then print count and all occurrences
        vector<TextQuery::location>::size_type size = locs.size();
        cout << "\n" << sought << " occurs " << size
             << (size == 1 ? " time:" : " times:")
             << "\n" << endl;
    
        // print each line in which the word appeared
        vector<TextQuery::location>::iterator it = locs.begin();
        while (it != locs.end()) {
            cout << "\t(line: "
                 // don't confound user with text lines starting at 0
                 << it->first + 1 << ", pos: " << it->second + 1 << ") "
                 << tq.text_line(it->first) << endl;
            ++it;
         }
    } while (!sought.empty());

    cout << "Ok, bye!" << endl;

    // debugging aid -- look at the map that was built
    tq.display_map();
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;

struct absInt {
    int operator()(int val) {
        return val < 0 ? -val : val;
    }
};

int main() {
    int i = -42;
    absInt absObj;  // object that defines function call operator
    unsigned int ui = absObj(i);   // calls absInt::operator(int)
    cout << i << " " << ui << endl;
    return 0;
}
    
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>

class Account {
public:
    // interface functions here
    void applyint() { amount += amount * interestRate; }
friend int main();
    double balance() { return amount; }
public:
    static double rate() { return interestRate; }
    static void rate(double);  // sets a new rate
private:
    std::string owner;
    double amount; 
    static double interestRate;
    static double initRate();
private:
    static const std::string accountType;
private:
    static const int period = 30;  // interest posted every 30 days
    double daily_tbl[period]; // ok: period is constant expression
};
using std::string;
const string Account::accountType("Savings Account");

void Account::rate(double newRate) 
{
    interestRate = newRate; 
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>

/* Simple main function: Read two numbers and write their sum */
int main()
{
	// prompt user to enter two numbers
	std::cout << "Enter two numbers:" << std::endl; 

	int v1, v2;
	std::cin >> v1 >> v2;   

	std::cout << "The sum of " << v1 << " and " << v2
	          << " is " << v1 + v2 << std::endl;

	return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
#include "Sales_item.h"

int main() 
{
    Sales_item item1, item2;

    std::cin >> item1 >> item2;   //read a pair of transactions
    std::cout << item1 + item2 << std::endl; //print their sum

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
#include "Sales_item.h"

int main() 
{
    Sales_item item1, item2;

    std::cin >> item1 >> item2;
    // first check that item1 and item2 represent the same book
    if (item1.same_isbn(item2)) {
        std::cout << item1 + item2 << std::endl;
        return 0;   // indicate success
    } else {
        std::cerr << "Data must refer to same ISBN" 
                  << std::endl;
        return -1;  // indicate failure
    }
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>

// using declarations for names from the standard library
using std::cin;
using std::cout;
using std::endl;
int main()
{
	cout << "Enter two numbers:" << endl;

	int v1, v2;
	cin >> v1 >> v2;

	cout << "The sum of " << v1 
	     << " and " << v2
	     << " is " << v1 + v2 << endl;

	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

// following include and using only to inject abort into the list of names
// for the name to header mapping in the appendix
#include <cstdlib>
using std::abort;
#include <cstddef>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <utility>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <iterator>

using std::vector; using std::list; using std::map; using std::set;  
using std::pair; using std::string; 
using std::istream; using std::ostream; using std::ifstream; 
using std::ofstream; using std::fstream;
using std::cin; using std::cout; using std::endl; using std::cerr;
using std::istream_iterator; using std::ostream_iterator;
using std::find;  using std::sort; using std::stable_sort; using std::for_each;
using std::count; using std::count_if; using std::copy; using std::unique;
using std::unique_copy; using std::back_inserter; using std::reverse_iterator;
using std::inserter; using std::front_inserter;
using std::inner_product; using std::getline; using std::equal_range;
using std::fill; using std::fill_n; using std::accumulate;
using std::partial_sort; using std::nth_element; using std::find_first_of;
using std::replace; using std::replace_copy; using std::set_intersection;

ifstream& open_file(ifstream&, const string&);
// copy constructor used to copy the return value; 
// parameters are references, so they aren't copied
string make_plural(size_t, const string&, const string&);

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::endl;
using std::cout;
#include <string>
using std::string;
#include <cctype>
using std::isspace;
using std::toupper;

int main() 
{
    string s("Expressions in C++ are composed...");

    string::iterator it = s.begin();
    // convert first word in s to uppercase
    while (it != s.end() && !isspace(*it)) {
        *it = toupper(*it);  
        ++it;
    }
    cout << s << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
#include <iostream>

class Endangered {
public:
    virtual ~Endangered();
    virtual std::ostream& print(std::ostream&) const;
    virtual void highlight() const;
    // ...
};

class ZooAnimal;
extern std::ostream&
operator<<(std::ostream&, const ZooAnimal&);

class ZooAnimal {
public:
    ZooAnimal();
    ZooAnimal(std::string animal, bool exhibit,
              std::string family): nm(animal), 
                                   exhibit_stat(exhibit), 
                                   fam_name(family) { } 
    virtual ~ZooAnimal();

    virtual std::ostream& print(std::ostream&) const;
    virtual int population() const;

    // accessors
    std::string name() const { return nm; }
    std::string family_name() const { return fam_name; }
    bool onExhibit() const { return exhibit_stat; }
    // ...
protected:
    std::string nm;
    bool        exhibit_stat;
    std::string fam_name;
    // ...
private:
};

class Bear : public ZooAnimal {
enum DanceType { two_left_feet, macarena, fandango, waltz };
public:
    Bear();
    Bear(std::string name, bool onExhibit=true, 
         std::string family = "Bear"):
                         ZooAnimal(name, onExhibit, family),
                         ival(0), dancetype(two_left_feet) { }

    virtual std::ostream &print(std::ostream&) const;
    virtual int toes() const;
    int mumble(int);
    void dance(DanceType) const;

    virtual ~Bear();
private:
    int         ival;
    DanceType   dancetype;
};

class Panda : public Bear, public Endangered {
public:
    Panda();
    Panda(std::string name, bool onExhibit=true);
    virtual ~Panda();
    virtual std::ostream& print(std::ostream&) const;
    void highlight();
    virtual int toes();
    virtual void cuddle();
// ...
};

Panda::Panda(std::string name, bool onExhibit)
      : Bear(name, onExhibit, "Panda") { }

inline
std::ostream& Panda::print(std::ostream &os) const
{
    Bear::print(os);          // print the Bear part
    Endangered::print(os);    // print the Endangered part
    return os;
}

class PolarBear : public Bear { /* . . . */ };
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;

int ia[] = {0,1,2,3,4,5,6,7,8,9};

int main()
{
   // sizeof(ia)/sizeof(*ia) returns the number of elements in ia
   int sz = sizeof(ia)/sizeof(*ia);

   cout << "ia size: " << sz << endl;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;
#include <string>
using std::string;
#include <list>
using std::list;
#include <cstddef>
using std::size_t;

int main()
{
    char *words[] = {"stately", "plump", "buck", "mulligan"};

    // calculate how many elements in words
    size_t words_size = sizeof(words)/sizeof(char *);

    // use entire array to initialize words2
    list<string> words2(words, words + words_size);

    for (size_t i = 0; i != words_size; ++i)
	    cout << words[i] << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "algs_preamble.h"
#include "Sales_item.h"

int main() 
{
    istream_iterator<Sales_item> item_iter(cin), eof;
    Sales_item sum;     // initially empty Sales_item
    sum = *item_iter++; // read first transaction into sum and get next record

    while (item_iter != eof) {
        if (item_iter->same_isbn(sum))
            sum = sum + *item_iter;
        else {
            cout << sum << endl;
            sum = *item_iter;
        }
        ++item_iter;  // read next transaction
    }
    cout << sum << endl;  // remember to print last set of records
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <cstddef>    // for the definition of size_t
using std::size_t;
int main()
{
    const size_t array_size = 10;
    int ia[array_size]; // 10 ints, elements are uninitialized
    
    // loop through array, assigning value of its index to each element
    for (size_t ix = 0; ix != array_size; ++ix)
          ia[ix] = ix;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;

int main() {
    int i;   double d;

    d = i = 3.5;  
    cout << i << " " << d << endl;  // prints 3 3

    i = d = 3.5;  
    cout << i << " " << d << endl;  // prints 3 3.5
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
#include <map>
#include <set>
#include <utility>
#include <iostream>
#include <algorithm>

using std::map; using std::set;  using std::multimap; using std::multiset; 
using std::pair; using std::string; 
using std::find; using std::cin; using std::cout; using std::endl;
using std::make_pair; using std::ifstream;

ifstream& open_file(ifstream&, const string&);
string make_plural(size_t, const string &, const string &);
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
#include "Sales_item.h"

int main() 
{
    // declare variables to hold running sum and data for the next record 
    Sales_item total, trans;

    // is there data to process?
    if (std::cin >> total) {
        // if so, read the transaction records 
        while (std::cin >> trans)
            if (total.same_isbn(trans)) 
                // match: update the running total 
                total = total + trans;
            else {   
                // no match: print & assign to total
                std::cout << total << std::endl;
                total = trans;
            }
        // remember to print last record
        std::cout << total << std::endl; 
    } else {
        // no input!, warn the user
        std::cout << "No data?!" << std::endl;
        return -1;  // indicate failure
    }

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 


#include "io_preamble.h"

int main()
{
    char ch;    // Using a char here invites disaster!

    // return from cin.get is converted from int to char and then compared to an int
    while ((ch = cin.get()) != EOF)
             cout.put(ch);
    cout << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

// incorrect version of swap: The arguments are not changed!
void swap(int v1, int v2) 
{
    int tmp = v2;
    v2 = v1;    // assigns new value to local copy of the argument
    v1 = tmp;   
}               // local objects v1 and v2 no longer exist
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Basket.h"
#include <algorithm>
using std::multiset; using std::map; using std::pair; using std::size_t;
using std::string; using std::ostream; using std::endl; using std::min;
using std::cout;

// debugging routine to check contents in a Basket
void Basket::display(ostream &os) const
{
    os << "Basket size: " << items.size() << endl;

    // print each distinct isbn in the Basket along with
    // count of how many copies are ordered and what their price will be
    // upper_bound returns an iterator to the next item in the set
    for (const_iter next_item = items.begin();
                  next_item != items.end();
                  next_item = items.upper_bound(*next_item))
    {
        // we know there's at least one element with this key in the Basket
        os << (*next_item)->book() << " occurs " 
           << items.count(*next_item) << " times" 
           << " for a price of " 
           << (*next_item)->net_price(items.count(*next_item)) 
           << endl;
    }
}

void print_total(ostream &, const Item_base&, size_t);

// calculate and print price for given number of copies, applying any discounts 
void print_total(ostream &os, 
                 const Item_base &item, size_t n)
{
    os << "ISBN: " << item.book() // calls Item_base::book
       << "\tnumber sold: " << n << "\ttotal price: "
       // virtual call: which version of net_price to call is resolved at run time
       << item.net_price(n) << endl;
}

double Basket::total() const
{
    double sum = 0.0;    // holds the running total 


    for (const_iter iter = items.begin(); 
                    iter != items.end();
                    iter = items.upper_bound(*iter))
    {
        // we know there's at least one element with this key in the Basket
        print_total(cout, *(iter->p), items.count(*iter));
        // virtual call to net_price applies appropriate discounts, if any
        sum += (*iter)->net_price(items.count(*iter));
    }
    return sum;
}

// use-counted assignment operator; use is a pointer to a shared use count
Sales_item&
Sales_item::operator=(const Sales_item &rhs)
{
    ++*rhs.use;
    decr_use();
    p = rhs.p;
    use = rhs.use;
    return *this;
}

// if specified number of items are purchased, use discounted price 
double Bulk_item::net_price(size_t cnt) const
{
    if (cnt >= min_qty)
        return cnt * (1 - discount) * price;
    else
        return cnt * price;
}

// use discounted price for up to a specified number of items
// additional items priced at normal, undiscounted price
double Lim_item::net_price(size_t cnt) const
{
    size_t discounted = min(cnt, max_qty);
    size_t undiscounted = cnt - discounted;
    return discounted * (1 - discount) * price 
           + undiscounted * price;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#ifndef BASKET_H
#define BASKET_H

#include <iostream>
#include <string>
#include <set>
#include <map>
#include <utility>
#include <cstddef>
#include <stdexcept>

// Item sold at an undiscounted price
// derived classes will define various discount strategies
class Item_base {
friend std::istream& operator>>(std::istream&, Item_base&);
friend std::ostream& operator<<(std::ostream&, const Item_base&);
public:
    virtual Item_base* clone() const 
                       { return new Item_base(*this); }
public:
    Item_base(const std::string &book = "", 
              double sales_price = 0.0):
                     isbn(book), price(sales_price) { }

    std::string book() const { return isbn; }

    // returns total sales price for a specified number of items
    // derived classes will override and apply different discount algorithms
    virtual double net_price(std::size_t n) const 
               { return n * price; }

    // no work, but virtual destructor needed 
    // if base pointer that points to a derived object is ever deleted
    virtual ~Item_base() { } 
private:
    std::string isbn;   // identifier for the item
protected:
    double price;       // normal, undiscounted price

};

class Sales_item;

bool compare(const Sales_item &lhs, const Sales_item &rhs);

// discount kicks in when a specified number of copies of same book are sold
// the discount is expressed as a fraction used to reduce the normal price
class Bulk_item : public Item_base {
public:
    std::pair<size_t, double> discount_policy() const
        { return std::make_pair(min_qty, discount); }
    // other members as before
    Bulk_item* clone() const 
        { return new Bulk_item(*this); }
    Bulk_item(): min_qty(0), discount(0.0) { }
    Bulk_item(const std::string& book, double sales_price, 
              std::size_t qty = 0, double disc_rate = 0.0):
                 Item_base(book, sales_price), 
                 min_qty(qty), discount(disc_rate) { }

    // redefines base version so as to implement bulk purchase discount policy
    double net_price(std::size_t) const;
private:
    std::size_t min_qty;   // minimum purchase for discount to apply
    double discount;       // fractional discount to apply
};

// discount (a fraction off list) for only a specified number of copies, 
// additional copies sold at standard price
class Lim_item : public Item_base {
public:
    Lim_item(const std::string& book = "", 
             double sales_price = 0.0,
             std::size_t qty = 0, double disc_rate = 0.0):
                 Item_base(book, sales_price), 
                 max_qty(qty), discount(disc_rate) { }

    // redefines base version so as to implement limited discount policy
    double net_price(std::size_t) const;
private:
    std::size_t max_qty;   // maximum number sold at discount
    double discount;       // fractional discount to apply
public:
    Lim_item* clone() const { return new Lim_item(*this); }
    std::pair<size_t, double> discount_policy() const
        { return std::make_pair(max_qty, discount); }
};

// use counted handle class for the Item_base hierarchy 
class Sales_item {
friend class Basket;
public:
    // default constructor: unbound handle
    Sales_item(): p(0), use(new std::size_t(1)) { }

    // attaches a handle to a copy of the Item_base object
    Sales_item(const Item_base&); 

    // copy control members to manage the use count and pointers
    Sales_item(const Sales_item &i): 
                      p(i.p), use(i.use) { ++*use; }
    ~Sales_item() { decr_use(); }
    Sales_item& operator=(const Sales_item&);

    // member access operators
    const Item_base *operator->() const { if (p) return p; 
        else throw std::logic_error("unbound Sales_item"); }
    const Item_base &operator*() const { if (p) return *p; 
        else throw std::logic_error("unbound Sales_item"); }
private:
    Item_base *p;         // pointer to shared item
    std::size_t *use;     // pointer to shared use count

    // called by both destructor and assignment operator to free pointers
    void decr_use() 
         { if (--*use == 0) { delete p; delete use; } }
};


// holds items being purchased
class Basket {
    // type of the comparison function used to order the multiset
    typedef bool (*Comp)(const Sales_item&, const Sales_item&);
public:
    // make it easier to type the type of our set
    typedef std::multiset<Sales_item, Comp> set_type;

    // typedefs modeled after corresponding container types
    typedef set_type::size_type size_type;
    typedef set_type::const_iterator const_iter;

    void display(std::ostream&) const;

    // workaround MS compiler bug: must explicitly pass function address
    Basket(): items(&compare) { }  // initialze the comparator 
    void add_item(const Sales_item &item) 
                        { items.insert(item); }
    size_type size(const Sales_item &i) const
                         { return items.count(i); }
    double total() const;  // sum of net prices for all items in the basket
private:
    std::multiset<Sales_item, Comp> items;
};

inline
Sales_item::Sales_item(const Item_base &item):
            p(item.clone()), use(new std::size_t(1)) { }

// compare defines item ordering for the multiset in Basket
inline bool 
compare(const Sales_item &lhs, const Sales_item &rhs) 
{
    return lhs->book() < rhs->book(); 
} 
#endif

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#ifndef BASKET_H
#define BASKET_H

#include <iostream>
#include <string>
#include <set>
#include <map>
#include <utility>
#include <cstddef>

// Item sold at an undiscounted price
// derived classes will define various discount strategies
class Item_base {
friend std::istream& operator>>(std::istream&, Item_base&);
friend std::ostream& operator<<(std::ostream&, const Item_base&);
public:
    virtual Item_base* clone() const 
                       { return new Item_base(*this); }
public:
    Item_base(const std::string &book = "", 
              double sales_price = 0.0):
                     isbn(book), price(sales_price) { }

    std::string book() const { return isbn; }

    // returns total sales price for a specified number of items
    // derived classes will override and apply different discount algorithms
    virtual double net_price(std::size_t n) const 
                                   { return n * price; }

    // no work, but virtual destructor needed 
    // if base pointer that points to a derived object is ever deleted
    virtual ~Item_base() { } 
private:
    std::string isbn;   // identifier for the item
protected:
    double price;       // normal, undiscounted price

};

// class to hold discount rate and quantity 
// derived classes will implement pricing strategies using these data
class Disc_item : public Item_base {
public:
    std::pair<size_t, double> discount_policy() const
        { return std::make_pair(quantity, discount); }
    // other members as before
    double net_price(std::size_t) const = 0;
    Disc_item(const std::string& book = "", 
              double sales_price = 0.0, 
              std::size_t qty = 0, double disc_rate = 0.0):
                 Item_base(book, sales_price), 
                 quantity(qty), discount(disc_rate) { }
protected:
    std::size_t quantity;  // purchase size for discount to apply
    double discount;       // fractional discount to apply
};

// discount kicks in when a specified number of copies of same book are sold
// the discount is expressed as a fraction to use to reduce the normal price
class Bulk_item : public Disc_item {
public:
    std::pair<size_t, double> discount_policy() const
        { return std::make_pair(quantity, discount); }
    // other members as before
    Bulk_item* clone() const 
        { return new Bulk_item(*this); }
    Bulk_item(const std::string& book = "", 
              double sales_price = 0.0, 
              std::size_t qty = 0, double disc_rate = 0.0):
          Disc_item(book, sales_price, qty, disc_rate) { }

    // redefines base version so as to implement bulk purchase discount policy
    double net_price(std::size_t) const;
};

// discount (a fraction off list) for only a specified number of copies, 
// additional copies sold at standard price
class Lim_item : public Disc_item {
public:
    Lim_item(const std::string& book = "", 
             double sales_price = 0.0,
             std::size_t qty = 0, double disc_rate = 0.0):
             Disc_item(book, sales_price, qty, disc_rate) { }

    // redefines base version so as to implement limited discount policy
    double net_price(std::size_t) const;
    Lim_item* clone() const { return new Lim_item(*this); }
    std::pair<size_t, double> discount_policy() const
        { return std::make_pair(quantity, discount); }
};

// use counted handle class for the Item_base hierarchy 
class Sales_item {
    // let compare use the Item_base pointer to use Item_base compare function

    // this friend is needed for the hidden call to print_total, 
    friend class Basket;   
public:
    // default constructor: unbound handle
    Sales_item(): p(0), use(new std::size_t(1)) { }
    // attaches a handle to a copy of the Item_base object
    Sales_item(const Item_base&); 

    // copy control members to manage the use count and pointers
    Sales_item(const Sales_item &i): p(i.p), use(i.use) 
                                     { ++*use; }
    ~Sales_item() { decr_use(); }
    Sales_item& operator=(const Sales_item&);

    // member access operators
    const Item_base *operator->() const { return p; }
    const Item_base &operator*() const { return *p; }
private:
    Item_base *p;         // pointer to shared item
    std::size_t *use;     // pointer to shared use count

    // called by both destructor and assignment operator to free pointers
    void decr_use() 
         { if (--*use == 0) {delete p; delete use;} }
};

bool compare(const Sales_item &, const Sales_item &);
// holds items being purchased
class Basket {
    // type of the comparison function used to order the multiset
    typedef bool (*Comp)(const Sales_item&, const Sales_item&);
    std::multiset<Sales_item, Comp> items;
public:
    // useful typedefs modeled after corresponding container types
    typedef std::multiset<Sales_item, Comp>::size_type size_type;
    typedef std::multiset<Sales_item, Comp>::const_iterator const_iter;

    // workaround MS compiler bug: must explicitly pass function address
    Basket(): items(&compare) { }  // initialze the comparator
    void display(std::ostream&) const;

    void add_item(const Sales_item &item) 
                        { items.insert(item); }
    size_type size(const Sales_item &i) const
                         { return items.count(i); }
    double total() const;  // sum of net prices for all items in the basket
};

inline
Sales_item::Sales_item(const Item_base &item):
            p(item.clone()), use(new std::size_t(1)) { }

// compare defines item ordering for the multiset in Basket
inline bool 
compare(const Sales_item &lhs, const Sales_item &rhs) 
{
    return lhs->book() < rhs->book();
} 

#endif
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Basket.h"
#include <iostream>
using std::cout; using std::endl;

int main()
{
	Sales_item item1(Item_base("123", 45));
	Sales_item item2(Bulk_item("345", 45, 3, .15));
	Sales_item item3(Bulk_item("678", 55, 5, .25));
	Sales_item item4(Lim_item("abc", 35, 2, .10));
	Sales_item item5(Item_base("def", 35));

	Basket sale;
	sale.add_item(item1);
cout << "added first item" << endl;
	sale.add_item(item1);
	sale.add_item(item1);
	sale.add_item(item1);
	sale.add_item(item2);
	sale.add_item(item2);
	sale.add_item(item2);
	sale.add_item(item2);
	sale.add_item(item2);
	sale.add_item(item2);
	sale.add_item(item2);
	sale.add_item(item2);
	sale.add_item(item3);
	sale.add_item(item3);
	sale.add_item(item3);
	sale.add_item(item3);
	sale.add_item(item3);
	sale.add_item(item3);
	sale.add_item(item3);
	sale.add_item(item3);
	sale.add_item(item4);
	sale.add_item(item4);
	sale.add_item(item4);
	sale.add_item(item4);
	sale.add_item(item4);
	sale.add_item(item4);
	sale.add_item(item5);
	sale.add_item(item5);
cout << "added last item" << endl;

	sale.display(cout);
	cout << sale.total() << endl;
{
	// arguments are the isbn, price, minimum quantity, and discount
	Bulk_item bulk("0-201-82470-1", 50, 5, .19);
	Basket sale;
	sale.add_item(Bulk_item("0-201-82470-1", 50, 5, .19));
	sale.add_item(Bulk_item("0-201-82470-1", 50, 5, .19));
	sale.add_item(Bulk_item("0-201-82470-1", 50, 5, .19));
	sale.add_item(Bulk_item("0-201-82470-1", 50, 5, .19));
	sale.add_item(Bulk_item("0-201-82470-1", 50, 5, .19));
	sale.add_item(Lim_item("0-201-54848-8", 35, 2, .10));
	sale.add_item(Lim_item("0-201-54848-8", 35, 2, .10));
	sale.add_item(Lim_item("0-201-54848-8", 35, 2, .10));
	double total = sale.total();
	cout << "Total Sale: " << total << endl;
}
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <cstddef>
#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::string;
using std::size_t;

#include <bitset>
using std::bitset;
int main() 
{
    bitset<32> bitvec; // 32 bits, all zero
    bool is_set = bitvec.any();      // false, all bits are zero
    bool is_not_set = bitvec.none(); // true, all bits are zero

    cout << "bitvec: " << bitvec << endl;


    size_t sz = bitvec.size();  // returns 32

    size_t bits_set = bitvec.count();  // returns number of bits that are on


    // assign 1 to even numbered bits
    for (int index = 0; index != 32; index += 2)
                bitvec[index] = 1; 

    // equivalent loop using set operation
    for (int index = 0; index != 32; index += 2)
                bitvec.set(index); 


    unsigned i;

    cout << "bitvec: positions turned on:\n\t";
    for (int index = 0; index != 32; ++index)
             if (bitvec[index])
                  cout << index << " ";
    cout << endl;


    // equivalent; turn off first bit
    bitvec.reset(0);
    bitvec[0] = 0;


    bitvec.reset(); // set all the bits to 0.
    bitvec.set();   // set all the bits to 1


    bitvec.flip(0);   // reverses value of first bit
    bitvec[0].flip(); // also reverses the first bit
    bitvec.flip();    // reverses value of all bits

    // leaves bitvec unchanged; yields a copy of bitvec with all the bits reversed
    bitvec = ~bitvec;          


    // bitvec1 is smaller than the initializer
    bitset<16> bitvec1(0xffff);  // bits 0 ... 15 are set to 1

    // bitvec2 same size as initializer
    bitset<32> bitvec2(0xffff);  // bits 0 ... 15 are set to 1; 16 ... 31 are 0

    // on a 32-bit machine, bits 0 to 31 initialized from 0xffff
    bitset<128> bitvec3(0xffff); // bits 32 through 127 initialized to zero
    cout << "bitvec1: " << bitvec1 << endl;
    cout << "bitvec2: " << bitvec2 << endl;
    cout << "bitvec2[0] " << bitvec2[0] << endl;
    cout << "bitvec2[31] " << bitvec2[31] << endl;
    cout << "bitvec3: " << bitvec3 << endl;

    string strval("1100");
    bitset<32> bitvec4(strval);

    cout << "strval: " << strval << endl;
    cout << "bitvec4: " << bitvec4 << endl;


{
    string str("1111111000000011001101");
    bitset<32> bitvec5(str, 5, 4); // 4 bits starting at str[5], 1100
    bitset<32> bitvec6(str, str.size() - 4); // use last 4 characters 

    cout << "str: " << str << endl;
    cout << "bitvec5: " << bitvec5 << endl;

    cout << "str: " << str << endl;
    cout << "bitvec6: " << bitvec6 << endl;
}


{
    unsigned long ulong = bitvec3.to_ulong();
    cout << "ulong = " << ulong << endl;
}

    bitset<32> bitvec7 = bitvec2 & bitvec4;

    cout << "bitvec7: " << bitvec7 << endl;

    bitset<32> bitvec8 = bitvec2 | bitvec4;

    cout << "bitvec8: " << bitvec8 << endl;

    cout << "bitvec2: " << bitvec2 << endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "bookexcept.h"
#include <iostream>
using std::cin; using std::cerr; using std::endl; using std::cout;
#include "Sales_item.h"

void process(double) { }

int main()
{
    // use hypothetical bookstore exceptions 
    Sales_item item1, item2, sum;
    while (cin >> item1 >> item2) {  // read two transactions
        try {
            sum = item1 + item2;     // calculate their sum
            // use sum
            cout << sum << endl;
        } catch (const isbn_mismatch &e) {
          cerr << e.what() << ": left isbn(" << e.left
               << ") right isbn(" << e.right << ")" 
               << endl;
        }
    }
     return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#ifndef BOOK_EXCEPT
#define BOOK_EXCEPT
#include <stdexcept>

// hypothetical exception classes for a bookstore application
class out_of_stock: public std::runtime_error {
public:
    explicit out_of_stock(const std::string &s):
                       std::runtime_error(s) { }
};

class isbn_mismatch: public std::logic_error {
public:
    explicit isbn_mismatch(const std::string &s): 
                          std::logic_error(s) { }
    isbn_mismatch(const std::string &s,
        const std::string &lhs, const std::string &rhs):
        std::logic_error(s), left(lhs), right(rhs) { }
    const std::string left, right;
    // myref(section)(exceptSpec) explains the destructor and why we need one
    virtual ~isbn_mismatch() throw() { }
};

#endif
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::ostream;

#include "Basket.h"
void print_total(ostream&, const Item_base&, size_t);

int main()
{
Item_base base("999999999", 24.99);
Bulk_item derived("888888888", 35.99, 5, .10);
// print_total makes a virtual call to net_price
print_total(cout, base, 10);    // calls Item_base::net_price
print_total(cout, derived, 10); // calls Bulk_item::net_price

Item_base *baseP = &derived;

// calls version from the base class regardless of the dynamic type of baseP
double d = baseP->Item_base::net_price(42);
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "io_preamble.h"

int main()
{
    cout << "hi!" << flush; // flushes the buffer; adds no data
    cout << "hi!" << ends;  // inserts a null, then flushes the buffer
    cout << "hi!" << endl;  // inserts a newline, then flushes the buffer
    cout << unitbuf << "first" << " second" << nounitbuf;
    cout << "first" << flush << " second" << flush;
    cout << endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#ifndef CACHED_OBJ_H
#define CACHED_OBJ_H
#include <cstddef>

template <class T> class CachedObj {
public:
    void *operator new(std::size_t);
    void operator delete(void *, std::size_t);
    virtual ~CachedObj() { }
    void operator delete[](void *, std::size_t);
    void *operator new[](std::size_t);
protected:
    T *next;
private:
    static void add_to_freelist(T*);
    static std::allocator<T> alloc_mem;
    static T *freeStore;
    static const std::size_t chunk;
};

#include "CachedObj.cc"  // inclusion compilation model

#endif
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "seq_preamble.h"


int main()
{
    vector<int> ivec;

    // size should be zero; capacity is implementation defined
    cout << "ivec: size: " << ivec.size()
         << " capacity: "  << ivec.capacity() << endl;

    // give ivec 24 elements
    for (vector<int>::size_type ix = 0; ix != 24; ++ix) 
         ivec.push_back(ix);
     
    // size should be 24; capacity will be >= 24 and is implementation defined
    cout << "ivec: size: " << ivec.size()
         << " capacity: "  << ivec.capacity() << endl;
    ivec.reserve(50); // sets capacity to at least 50; might be more
    // size should be 24; capacity will be >= 50 and is implementation defined
    cout << "ivec: size: " << ivec.size()
         << " capacity: "  << ivec.capacity() << endl;

    // add elements to use up the excess capacity
    while (ivec.size() != ivec.capacity())
         ivec.push_back(0);

    // size should be 50; capacity should be unchanged
    cout << "ivec: size: " << ivec.size()
         << " capacity: "  << ivec.capacity() << endl;
    ivec.push_back(42); // add one more element

    // size should be 51; capacity will be >= 51 and is implementation defined
    cout << "ivec: size: " << ivec.size()
         << " capacity: "  << ivec.capacity() << endl;


    return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
using std::string;
#include <cctype>
using std::isupper; using std::toupper;
using std::islower; using std::tolower;
using std::isalpha; using std::isspace;
#include <iostream>
using std::cout; using std::endl;

int main()
{
    string s("Hello World!!!");
    string::size_type punct_cnt = 0; 

    // count number of punctuation characters in s
    for (string::size_type index = 0; index != s.size(); ++index)
        if (ispunct(s[index]))
            ++punct_cnt;

    cout << punct_cnt 
         << " punctuation characters in " << s << endl;

{
    // convert s to lowercase
    for (string::size_type index = 0; index != s.size(); ++index) 
        s[index] = tolower(s[index]);

    cout << s << endl;
}
    return 0;
}
const char *str = "hello";

void *malloc(int);
char *strcpy(char *, const char *);
int printf(const char *, ...);
int exit(int);
int strlen(const char *);

int main()
{   /* C language program */
    /* allocate space to hold a copy of str */
    char* s = malloc(strlen(str)+1);
    strcpy(s, str);           /* copy s to str */
    printf("%s, world\n", s); /* print s followed by ", world\n" */
    exit(0);              /* exit program and return 0 to the OS */
}
const char *str = "hello";

void *malloc(int);
char *strcpy(char *, const char *);
int printf(const char *, ...);
int exit(int);
int strlen(const char *);

int main()
{   /* C language program */
    /* allocate space to hold a copy of str */
    char* s = malloc(strlen(str)+1);
    strcpy(s, str);           /* copy s to str */
    printf("%s, world\n", s); /* print s followed by ", world\n" */
    exit(0);              /* exit program and return 0 to the OS */
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "char_queue.h"

#include <cstring>
using std::strncpy;
#include <string>
using std::string;

#include <iostream>
using std::cout; using std::endl;

int main()
{
    Queue<const char*> q1;

    // dynamically allocate and initialize a char array
    char *cp = "hello";
    const size_t len = strlen("hello");
    char* hi = new char[len + 1];
    strncpy(hi, cp, len + 1);

    q1.push(hi);   // put pointer to dynamically allocated array in q1

    // delete the dynamically allocated array -- 
    // disaster if Queue holds original pointer and not a copy of the array!
    delete[] hi;     
    q1.push("world");
    q1.push("bye");

    cout << q1 << endl;

    for (int i = 0; i != 3; ++i) {
        cout << q1.front() << endl;
        q1.pop();
    }
    
    cout << "q1 is empty? " << ((q1.empty()) ? "yes" : "no") << endl;

    string hi2 = "hello";
    Queue<string> q2;
    q2.push(hi2);
    q2.push("world");
    q2.push("bye");

    cout << q2 << endl;

    for (int i = 0; i != 3; ++i) {
        cout << q2.front() << endl;
        q2.pop();
    }
    
    cout << "q2 is empty? " << ((q2.empty()) ? "yes" : "no") << endl;

    return 0;
}
#include "MS_Queue.h"
// workaround MS compiler bug: doesn't properly handle member specializations
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

// push and pop specialized for const char*
#include"queue_push_char.cpp"


#include <string>
using std::string;

#include <iostream>
using std::cout; using std::endl;

int main()
{
	char* hi = "hello";
	Queue<const char*> q1;
	q1.push(hi);
	q1.push("world");
	q1.push("bye");

	cout << q1 << endl;

	for (int i = 0; i != 3; ++i) {
		cout << q1.front() << endl;
		q1.pop();
	}
	
	cout << "q1 is empty? " << ((q1.empty()) ? "yes" : "no") << endl;

	string hi2 = "hello";
	Queue<string> q2;
	q2.push(hi2);
	q2.push("world");
	q2.push("bye");

	cout << q2 << endl;

	for (int i = 0; i != 3; ++i) {
		cout << q2.front() << endl;
		q2.pop();
	}
	
	cout << "q2 is empty? " << ((q2.empty()) ? "yes" : "no") << endl;

	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "char_queue.h"
#include <iostream>

//NB: if change the Queue interface must also change this file!

using std::ostream; using std::cout; using std::endl;

ostream& operator<<(ostream &os, const Queue<const char*> &q)
{
    os << q.real_queue;
    return os;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#ifndef QUEUE_CHAR_H
#define QUQUE_CHAR_H

#include "MS_Queue.h"
#include <stdexcept>
#include <iostream>
#include <string>

//NB: if change the Queue interface must also change this file!

template<> class Queue<char*>; // declares specialization for Queue<char*>

template<> class Queue<const char*> {
    friend std::ostream& operator<< 
                      (std::ostream&, const Queue<const char*>&);
public:
    // no copy control: Synthesized  versions work for this class
    // similarly, no need for explicit default constructor either
    void push(const char*);
    void pop()               {real_queue.pop();}
    bool empty() const       {return real_queue.empty();}

    // Note: return type does not match template parameter type
    std::string front()     {return real_queue.front();}
    const std::string &front() const 
                             {return real_queue.front();}
private:
    Queue<std::string> real_queue; // forward calls to real_queue
};

inline
void Queue<const char*>::push(const char* val)
{
    return real_queue.push(val);
}

#endif
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;

int main()
{
	string str("some string");

	for (string::size_type ix = 0; ix != str.size(); ++ix)
		cout << str[ix] << endl;

	for (string::size_type ix = 0; ix != str.size(); ++ix)
		str[ix] = '*';

	cout << str << endl;
	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "CheckedPtr.h"
#include <iostream>
using std::cerr; 
#include <stdexcept>
using std::out_of_range;

int& CheckedPtr::operator*() 
{
    if (curr == end)
        throw out_of_range("dereference past the end");
    return *curr; 
}

const int& CheckedPtr::operator*() const
{
    if (curr == end)
        throw out_of_range("dereference past the end");
     return *curr; 
}


// postfix: increment/decrement object but return unchanged value
CheckedPtr CheckedPtr::operator++(int)
{
    // no check needed here, the call to prefix increment will do the check
    CheckedPtr ret(*this);   // save current value
    ++*this;                 // advance one element, checking the increment
    return ret;              // return saved state
}

CheckedPtr CheckedPtr::operator--(int)
{
    // no check needed here, the call to prefix decrement will do the check
    CheckedPtr ret(*this);  // save current value
    --*this;                // move backward one element and check
    return ret;             // return saved state
}

// prefix: return reference to incremented/decremented object
CheckedPtr& CheckedPtr::operator++()
{
    if (curr == end) 
        throw out_of_range
              ("increment past the end of CheckedPtr");

    ++curr;                // advance current state
    return *this;
}

CheckedPtr& CheckedPtr::operator--()
{
    if (curr == beg) 
        throw out_of_range
          ("decrement past the beginning of CheckedPtr");

    --curr;              // move current state back one element
    return *this;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <cstddef>

class CheckedPtr {
public:
    // no default constructor; CheckedPtrs must be bound to an object
    CheckedPtr(int *b, int *e): beg(b), end(e), curr(b) { }

    // dereference and increment operations

    // dereference operator
    int& operator*();
    const int& operator*() const;

    // increment and decrement
    CheckedPtr operator++(int);     // postfix operators
    CheckedPtr operator--(int);
    // other members as before

    CheckedPtr& operator++();       // prefix operators
    CheckedPtr& operator--();
    // other members as before
private:
    int* beg;    // pointer to beginning of the array
    int* end;    // one past the end of the array
    int* curr;   // current position within the array
};
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
#include <iostream>

int main()
{
	// read a word to look for
	std::string search_word;
	std::cin >> search_word;

	// initialize variables to detect and count
	// occurences of search_word in the input
	bool found = false;
	int occurrence_count = 0;
	
	std::string next_word;
	while (std::cin >> next_word) {
		found = (next_word == search_word);
		occurrence_count += found;
	}
	
	std::cout << "found " << occurrence_count 
		  << " copies of " << search_word
		  << " in the input" << std::endl;


	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "tmpl_preamble.h"
#include "Sales_item.h"

// implement strcmp-like generic compare function 
// returns 0 if the values are equal, 1 if v1 is larger, -1 if v1 is smaller
template <typename T>
int compare(const T &v1, const T &v2)
{
    if (v1 < v2) return -1;
    if (v2 < v1) return 1;
    return 0;
}

// repeats body of compare but with comments
int f(int v1, int v2)
{
    if (v1 < v2) return -1;  // < on two objects of type T
    if (v2 < v1) return 1;   // < on two objects of type T
    return 0;                // return int;  not dependent on T
}

// special version of compare to handle C-style character strings
template <>
int compare<const char*>(const char* const &v1, 
                         const char* const &v2)
{
    return strcmp(v1, v2);
}

// returns 0 if the values are equal, -1 if v1 is smaller, 1 if v2 is smaller
int compare(const string &v1, const string &v2)
{
    if (v1 < v2) return -1;
    if (v2 < v1) return 1;
    return 0;
}

int compare(const double &v1, const double &v2)
{
    if (v1 < v2) return -1;
    if (v2 < v1) return 1;
    return 0;
}

int main()
{
    // T is int; 
    // compiler instantiates int compare(const int&, const int&)
    cout << compare(1, 0) << endl;  

    // T is string; 
    // compiler instantiates int compare(const string&, const string&)
    string s1 = "hi", s2 = "world";
    cout << compare(s1, s2) << endl; 
{
    short s1, s2;
    int i1, i2;
    compare(i1, i2);    // ok: instantiate compare(int, int)
    compare(s1, s2);    // ok: instantiate compare(short, short)
}
    char *cp1 = "world", *cp2 = "hi";
    cout << compare(cp1, cp2) << endl;       // compares pointer values!

{
    const char *cp1 = "world", *cp2 = "hi";
    int i1, i2;
    compare(cp1, cp2); // calls the specialization
    compare(i1, i2);   // calls the generic version instantiated with int
}
{
    const char *cp1 = "world", *cp2 = "hi";
    compare<string>(cp1, cp2);  // converts arguments to string
}
    return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::endl;
using std::cout;


int main()
{
    int i = 10, j = 20, k = 30;

    // if i > j then maxVal = i else maxVal = j
    int maxVal = i > j ? i : j;

    cout << "The value of " << i << " is"
         << (i % 2  ? " odd." : " even.")
         << endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::endl;
using std::cout;


int main()
{
    int i = 10, j = 20, k = 30;

    int max = i > j
                  ? i > k ? i : k
                  : j > k ? j : k;

    cout << "The larger value of "
         << i << ", " << j << " and " << k
         << " is " << max << endl;

// A better way to perform the same test
{
    int max = i;
    if (j > max)
        max = j;
    if (k > max)
        max = k;

    cout << "The larger value of "
         << i << ", " << j << " and " << k
         << " is " << max << endl;
}
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

/* Note: This code is an example of *bad* style and serves 
 * only to illustrate the issues involved:
 *
 * First: overloading between int and unsigned often leads to ambiguities
 * Second: relying on the promotion type of an enumeration to determine
 *         which function is matched is inherently machine-dependent.
 * A better design would include a specific 'format' function that took
 * an argument of type 'e2'
*/

#include <string>
using std::string;

enum e1 {a1, b1, c1};
enum e2 {a2, b2, c2=0x80000000};

string format(int);
string format(unsigned int);

enum Stat {Fail, Pass};

void ff(int);
void ff(char);

int main() {
    // ok: enumeration constant Pass promoted to int
    ff(Pass);  // ff(int)
    ff(0);     // ff(int)

    void ff(Stat);
    ff(Fail);  // ff(Stat)

    string s1 = format(a1); // calls format(int)
    string s2 = format(a2); // calls format(unsigned int)

    return 0;
}

#include <iostream>
using std::cout; using std::endl;
void ff(int) { cout << "ff(int)" << endl; }

void ff(char) { cout << "ff(char)" << endl; }

void ff(Stat) { cout << "ff(Stat)" << endl; }

string format(int) { cout << "format(int)" << endl; return ""; }

string format(unsigned int) { cout << "format(unsigned int)" << endl; 
				return ""; }

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <cstddef>
using std::size_t;

int main()
{
    const size_t array_size = 7;
    int ia1[] = { 0, 1, 2, 3, 4, 5, 6 };
    int ia2[array_size]; // local array, elements uninitialized 
    
    // copy elements from ia1 into ia2
    for (size_t ix = 0; ix != array_size; ++ix)
          ia2[ix] = ia1[ix];

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;

size_t count_calls()
{
	static size_t ctr = 0;  // value will persist across calls
	return ++ctr;
}

int main() 
{
	for (size_t i = 0; i != 10; ++i)
		cout << count_calls() << endl;
	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "algs_preamble.h"
#include <fstream>
using std::ifstream;
#include <cstddef>
using std::size_t;

// comparison function to be used to sort by word length
bool isShorter(const string &s1, const string &s2)
{
    return s1.size() < s2.size();
}

// determine whether a length of a given word is 6 or more
bool GT6(const string &s) 
{
    return s.size() >= 6;
}

int main()
{
    vector<string> words;

    // copy contents of each book into a single vector
    string next_word;
    while (cin >> next_word) {
        // insert next book's contents at end of words
        words.push_back(next_word);
    }

    // sort words alphabetically so we can find the duplicates
    sort(words.begin(), words.end());

    vector<string>::iterator end_unique =
                unique(words.begin(), words.end());
    words.erase(end_unique, words.end());

    // sort words by size, but maintain alphabetic order for words of the same size
    stable_sort(words.begin(), words.end(), isShorter);
    vector<string>::size_type wc = 
                 count_if(words.begin(), words.end(), GT6);
    cout << wc << " " << make_plural(wc, "word", "s")
         << " 6 characters or longer" << endl;
    return 0;  
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>

int main()
{
// same code as before
	// prompt user for number range to print
	std::cout << "Enter two numbers: " << std::endl;
	int v1, v2, lower, upper;
	std::cin >> v1 >> v2;
	
	// use smaller value for lower bound, larger for upper
	if (v1 <= v2) {
		lower = v1;
		upper = v2;
	} else {
		lower = v2;
		upper = v1;
	}

	// sum values from lower up to and including upper
	int sum = 0;
	int val = lower;
	while (val <= upper) 
	{
		sum += val;  // sum = sum + val
		++val;       // increment val (val = val + 1)
	}

	std::cout << "Sum of " << lower << " to " << upper
	          << " inclusive is " << sum << std::endl;
	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <fstream>

// ---- Sales_item.h ----
namespace cplusplus_primer {
    class Sales_item { /* . . . */};
    Sales_item operator+(const Sales_item&,
                         const Sales_item&);
    // declarations for remaining functions in the Sales_item interface
}

// ---- Query.h ----
namespace cplusplus_primer {
    class Query { 
    public:
        Query(const std::string&);
        std::ostream &display(std::ostream&) const;
        //...
    };
    class Query_base { /* . . . */};
}

// ---- Sales_item.cc ----

namespace cplusplus_primer {
// definitions for Sales_item members and overloaded operators
}

// ---- Query.cc ----
namespace cplusplus_primer {
	// definitions for Query members and related functions
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <cstring>

int main()
{
    //  C-style character string implementation 
        const char *pc = "a very long literal string";
        const size_t  len = strlen(pc +1);    // space to allocate

        // performance test on string allocation and copy
        for (size_t ix = 0; ix != 1000000; ++ix) {
            char *pc2 = new char[len + 1]; // allocate the space
            strcpy(pc2, pc);               // do the copy
            if (strcmp(pc2, pc))           // use the new string
                ;   // do nothing

            delete [] pc2;                 // free the memory
        }

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
using std::string;
#include <iostream>
using std::cout; using std::endl;
#include <cstring>

int main() {
    const char *cp1 = "A string example";
    const char *cp2 = "A different string";

    int i = strcmp(cp1, cp2);    // i is positive
    i = strcmp(cp2, cp1);        // i is negative
    i = strcmp(cp1, cp1);        // i is zero

    cout << strlen(cp1) << endl; // prints 16; strlen ignores the null

    if (cp1 < cp2)  // compares addresses, not the values pointed to
     ;

    char ca[] = {'C', '+', '+'};  // not null-terminated
    cout << strlen(ca) << endl;   // disaster: ca isn't null-terminated
                                  // behavior of this print statement is undefined

    // Dangerous: What happens if we miscalculate the size of largeStr?
    char largeStr[16 + 18 + 2]; // will hold cp1 a space and cp2
    strcpy(largeStr, cp1);      // copies cp1 into largeStr
    strcat(largeStr, " ");      // adds a space at end of largeStr
    strcat(largeStr, cp2);      // concatenates cp2 to largeStr
    // prints A string example A different string
    cout << largeStr << endl;       

    // Somewhat safer: use the ``strn'' versions to copy and concatenate
{
    char largeStr[16 + 18 + 2]; // to hold cp1 a space and cp2
    strncpy(largeStr, cp1, 17); // size to copy includes the null
    strncat(largeStr, " ", 2);  // pedantic, but a good habit
    strncat(largeStr, cp2, 19); // adds at most 18 characters, plus a null
    cout << largeStr << endl;       
}

    // Best approach: use strings not char arrays!
{
    string largeStr = cp1; // initialize largeStr as a copy of cp1
    largeStr += " ";       // add space at end of largeStr
    largeStr += cp2;       // concatenate cp2 onto end of largeStr
    cout << largeStr << endl;       
}

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
#include <iostream>
using std::cout; using std::endl;
using std::string;

char *cp = "Hiya";            // null-terminated array
char c_array[] = "World!!!!"; // null-terminated
char no_null[] = {'H', 'i'};  // not null-terminated

string s1(cp);             // s1 == "Hiya"
string s2(c_array, 5);     // s2 == "World"
string s3(c_array + 5, 4); // s3 == "!!!!"
string s4(no_null);        // runtime error: no_null not null-terminated
string s5(no_null, 2);     // ok: s5 == "Hi"

    string s6(s1, 2);    // s6 == "ya"

    string s7(s1, 0, 2); // s7 == "Hi"

    string s8(s1, 0, 8); // s8 == "Hiya"

int main()
{
    cout << s1 << '\n' << s2 << '\n'
         << s3 << '\n' << s5 << '\n'
         << s6 << '\n' << s7 << endl;

{
    string s1;           // s1 is the empty string
    string s2(5, 'a');   // s2 == "aaaaa"
    string s3(s2);       // s3 is a copy of s2
    string s4(s3.begin(), 
              s3.begin() + s3.size() / 2);  // s4 == "aa"
    cout << s1 << '\n' << s2 << '\n'
         << s3 << '\n' << s4 << endl;
}

return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout;
using std::endl;

int main()
{
    int ival = 1024;
    int *pi = &ival;   // pi points to an int
    int **ppi = &pi;   // ppi points to a pointer to int
    cout << "The value of ival\n"
         << "direct value: " << ival << "\n"
         << "indirect value: " << *pi << "\n"
         << "doubly indirect value: " << **ppi
         << endl;

    int i = 42, j = 1024;
    int *p1 = &i, *p2 = &j;
    *p2 = *p1 * *p2;
    *p1 *= *p1;
    cout << "j  = " << j << endl;
    cout << "i  = " << i << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

void swap(int, int);
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

void swap(int&, int&);
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Sales_item.h"
#include <iostream>
using std::cout; using std::endl;

int main()
{
    Sales_item item1, item2;
    std::cin >> item1 >> item2;
    cout << item1 + item2 << endl;
    cout << 42 + 5 << endl;
    cout << endl;

    // equivalent direct call to nonmember operator function
    cout << operator+(item1, item2) << endl;

    item1 += item2;           // expression based ``call''
    item1.operator+=(item2);  // equivalent call to member operator function
    cout << endl;

    return 0;
}
#include <iostream>
using std::cin; using std::cout; using std::endl;
#include <string>
using std::string;

int main()
{
    // repeatedly ask user for pair of numbers to sum
    string rsp;  // used in the condition; can't be defined inside the do
    do {
        cout << "please enter two values: ";
        int val1, val2;
        cin  >> val1 >> val2;
        cout << "The sum of " << val1 << " and " << val2
             << " = " << val1 + val2 << "\n\n"
             << "More? [yes][no] ";
        cin  >> rsp;
    } while (!rsp.empty() && rsp[0] != 'n');
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <typeinfo>
using std::bad_cast;

#include <iostream>
using std::cout; using std::endl;

void exercises() {
struct A { virtual ~A() { } };
struct B : virtual public A { /* ... */ };
struct C : public B { /* ... */ };
struct D : public B, virtual public A { /* ... */ };

 A *pa = new C;
    if (B *pb = dynamic_cast< B* >(pa))
         cout << "cast from C to B* ok" << endl;
    else
         cout << "cast from C to B* not ok" << endl;
 B *pb = new B;
    if (C *pc = dynamic_cast< C* >(pb))
         cout << "cast from B to C* ok" << endl;
    else
         cout << "cast from B to C* not ok" << endl;

 A *pc = new C;
    if (B *pb = dynamic_cast< B* >(pc))
         cout << "cast C to B* ok" << endl;
    else
         cout << "cast C to B* not ok" << endl;

 A *pd = new D;
    if (B *pb = dynamic_cast< B* >(pd))
         cout << "cast D to B* ok" << endl;
    else
         cout << "cast D to B* not ok" << endl;
}

struct Base {
   virtual void f() {};
   virtual ~Base() {};
};

struct Derived: public Base { };

int main()
{
Base *basePtr; 
basePtr = new Derived;  // basePtr actually points to a Derived object
if (Derived *derivedPtr = dynamic_cast<Derived*>(basePtr)) 
{
    // use the Derived object to which derivedPtr points
} else {  // BasePtr points at a Base object
    // use the Base object to which basePtr points
}
exercises();
}

void f(const Base &b)
{
    try {
        const Derived &d = dynamic_cast<const Derived&>(b);
	// use the Derived object to which b referred
    } catch (bad_cast) {
        // handle the fact that the cast failed
    }
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "algs_preamble.h"
#include <fstream>
using std::ifstream;
#include <cstddef>
using std::size_t;

int main()
{
	vector<string> words;
	// copy contents of each book into a single vector
	string next_word;
	while (cin >> next_word) {
		// insert next book's contents at end of words
		words.push_back(next_word);
	}

	// sort words alphabetically so we can find the duplicates
	sort(words.begin(), words.end());

	// print contents to see how things look at this point
	for(vector<string>::iterator it = words.begin(); it != words.end(); ++it)
		cout << *it << " ";
	cout << endl;

	vector<string>::iterator end_unique =
	            unique(words.begin(), words.end());

	// print contents to see how things look at this point
	for(vector<string>::iterator it = words.begin(); it != words.end(); ++it)
		cout << *it << " ";
	cout << endl;

	words.erase(end_unique, words.end());
	
	// print contents to see how things look at this point
	for(vector<string>::iterator it = words.begin(); it != words.end(); ++it)
		cout << *it << " ";
	cout << endl;
	return 0;  
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

enum Tokens {INLINE = 128, VIRTUAL = 129};

void ff(Tokens);
void ff(int);

int main() {
    Tokens curTok = INLINE;
    ff(128);    // exactly matches ff(int)
    ff(INLINE); // exactly matches ff(Tokens)
    ff(curTok); // exactly matches ff(Tokens)

void newf(unsigned char);
void newf(int);
unsigned char uc = 129;

newf(VIRTUAL);  // calls newf(int)
newf(uc);       // calls newf(unsigned char)

newf(129);      // calls newf(int)

    return 0;
}

#include <iostream>
using std::cout; using std::endl;
void ff(Tokens) { cout << "ff(Tokens)" << endl; }
void ff(int) { cout << "ff(int)" << endl; }
void newf(int) { cout << "newf(int)" << endl; }
void newf(unsigned char) { cout << "newf(unsigned char)" << endl; }

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "algs_preamble.h"

int main() {
    istream_iterator<int> in_iter(cin);  // read ints from cin
    istream_iterator<int> eof;      // istream ``end'' iterator
    vector<int> vec(in_iter, eof);  // construct vec from an iterator range

    // print vec to see what we read
    for (vector<int>::const_iterator it = vec.begin();
                                     it != vec.end(); ++it)
	    cout << *it << " ";
    cout << endl;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "seq_preamble.h"
#include <map>
using std::map;

int main() 
{
    map<string, int> word_count;  // empty map from string to int
    string word;
    while (cin >> word)
      ++word_count[word];

    string removal_word = "the";

    // erase of a key returns number of elements removed
    if (word_count.erase(removal_word))
         cout << "ok: " << removal_word << " removed\n";
    else cout << "oops: " << removal_word << " not found!\n";


    map<string,int>::iterator where;
    where = word_count.find(removal_word);

    if (where == word_count.end())
         cout << "oops: " << removal_word << " not found!\n";
    else {
         word_count.erase(where);   // erase iterator returns void
         cout << "ok: " << removal_word << " removed!\n";
    }
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <stdexcept>
using std::runtime_error;
#include <iostream>
#include "Sales_item.h"
using std::cin; using std::cout; using std::endl;

int main() 
{
    Sales_item item1, item2;

    while (cin >> item1 >> item2) {
        try {
            // execute code that will add the two Sales_items
            // if the addition fails, the code throws a runtime_error exception
            // first check that data is for the same item 
            if (!item1.same_isbn(item2))
                throw runtime_error("Data must refer to same ISBN");

            // ok, if we're still here the ISBNs are the same
            std::cout << item1 + item2 << std::endl;
        } catch (runtime_error err) {
            // remind the user that ISBN must match and prompt for another pair
            cout << err.what() 
                 << "\nTry Again?  Enter y or n" << endl;
            char c;
            cin >> c;
            if (cin && c == 'n')
                break;      // break out of the while loop
        }
    }
    return 0;   // indicate success
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout;
using std::endl;

// calculate val!, which is 1 * 2 * 3 ... * val 
int factorial(int val) 
{
    if (val > 1)
        return factorial(val-1) * val;
    return 1;
}

bool is_pos(int i)
{
	return i > 0 ? true : false;
}

// calculate val!, which is val * val - 1 * val - 2 ... * val - (val - 1) * 1
int fact(int val)
{
	int ret = 1; // local variable to hold result as we calculate it

	while (val > 1) {
		ret *= val;  
		--val;
	}

	return ret;            // return result
}

int main()
{
	cout << factorial(5) << endl;
	cout << fact(5) << endl;
	cout << factorial(0) << endl;
	cout << fact(0) << endl;
	return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <functional>
using std::plus; using std::negate;

#include <iostream>
using std::cout; using std::endl;

#include <vector>
#include <algorithm>
using std::count_if; using std::bind2nd; using std::not1; using std::ptr_fun;
using std::less_equal; using std::vector;

#include <iostream>
using std::cin;

#include <string>
using std::string;

bool size_compare(string s, string::size_type sz)
{
    return s.size() >= sz;
}

int main() {

    cout << plus<int>()(3,4) << endl; // prints 7

    plus<int> intAdd;        // function object that can add two int values
    negate<int>  intNegate;  // function object that can negate an int value

    // uses intAdd::operator(int, int) to add 10 and 20
    int sum = intAdd(10, 20);        // sum = 30

    cout << sum << endl;

    // uses intNegate::operator(int) to generate -10 as second parameter
    // to intAdd::operator(int, int)
    sum = intAdd(10, intNegate(10));  // sum = 0

    cout << sum << endl;

    int arry[] = {0,1,2,3,4,5,16,17,18,19};

    vector<int> vec(arry, arry + 10);

    cout <<
    count_if(vec.begin(), vec.end(),
             bind2nd(less_equal<int>(), 10));
    cout << endl;

    cout <<
    count_if(vec.begin(), vec.end(),
             not1(bind2nd(less_equal<int>(), 10)));
    cout << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 


// ff.h
int ff(int = 0);
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
#include <iostream>
using std::string; using std::cout; using std::endl;

int main() 
{
    string name("AnnaBelle");
    string::size_type pos1 = name.find("Anna"); // pos1 == 0
    cout << pos1 << endl;
    string lowercase("annabelle");
    pos1 = lowercase.find("Anna");   // pos1 == npos
    if (pos1 != string::npos)
        cout << " " << pos1 << endl;
    else
        cout << "not found!" << endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
#include <iostream>
using std::string; using std::cout; using std::endl;

int main() 
{
    string numerics("0123456789");
    string name("r2d2");

    string::size_type pos = name.find_first_of(numerics);
    cout << "found number at index: " << pos 
         << " element is " << name[pos] << endl;

{
    string::size_type pos = 0;  
    // each trip reset pos to the next instance in name
    while ((pos = name.find_first_of(numerics, pos)) 
                  != string::npos) {
        cout << "found number at index: " << pos 
             << " element is " << name[pos] << endl;

        ++pos; // move to the next character
    }
}

    string river("Mississippi");

    string::size_type first_pos = river.find("is");  // returns 1
    string::size_type last_pos = river.rfind("is");  // returns 4
    cout << "find returned: " << first_pos 
         << " rfind returned: " << last_pos << endl;

{
    string numbers("0123456789");
    string dept("03714p3");

    // returns 5, which is the index to the character 'p'
    string::size_type pos = dept.find_first_not_of(numbers);
    cout << "first_not returned: " << pos << endl;
}

    return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
#include <iostream>

int main()
{
	// read a word to look for
	std::string search_word;
	std::cin >> search_word;

	// initialize a bool variable to false
	bool found = false;
	
	std::string next_word;
	while ( std::cin >> next_word ) {
		if ( next_word == search_word )
			found = true;
	}
	
	// shorthand notation for: if ( found == true ) }
	if ( found )
		std::cout << "ok, we found the word" 
		          << std::endl;
	else 
		std::cout << "nope, the word was not present." 
		          << std::endl;

	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Folder.h"
#include <iostream>
using std::cerr; using std::endl;
using std::set; using std::string; using std::vector;

Message::Message(const Message &m): 
    contents(m.contents), folders(m.folders) 
{
    // add this Message to each Folder that points to m
    put_Msg_in_Folders(folders);  
}


Message& Message::operator=(const Message &rhs)
{
    if (&rhs != this) {
        remove_Msg_from_Folders(); // update existing Folders
        contents = rhs.contents;   // copy contents from rhs
        folders = rhs.folders;     // copy Folder pointers from rhs
        // add this Message to each Folder in rhs 
        put_Msg_in_Folders(rhs.folders); 
    }
    return *this;
}

Message::~Message()
{
    remove_Msg_from_Folders();
}

// add this Message to Folders that point to rhs
void Message::put_Msg_in_Folders(const set<Folder*> &rhs)
{
    for(std::set<Folder*>::const_iterator beg = rhs.begin(); 
                                     beg != rhs.end(); ++beg)
        (*beg)->addMsg(this);  // *beg points to a Folder
}

// remove this Message from corresponding Folders 
void Message::remove_Msg_from_Folders()
{
    // remove this message from corresponding folders
    for(std::set<Folder*>::const_iterator beg =
          folders.begin(); beg != folders.end(); ++beg)
        (*beg)->remMsg(this);  // *beg points to a Folder
}

void Folder::copy_msgs(const set<Message*> &m)
{
    for (Msg_iter beg = m.begin(); beg != m.end(); ++beg)
        (*beg)->save(*this);   // add this Folder to each Message
}

Folder::Folder(const Folder &f)
{
    copy_msgs(f.msgs);  // add this Folder to each Message in f.msgs
}

Folder& Folder::operator=(const Folder &f)
{
    if (&f != this) {
        empty_msgs();  // remove this folder from each Message in msgs
        copy_msgs(f.msgs); // add this folder to each Message in msgs
    }
    return *this;
}

Folder::~Folder()
{
    empty_msgs();
}


void Folder::empty_msgs()
{
    Msg_iter it = msgs.begin(); 
    while (it != msgs.end()) {
        Msg_iter next = it;
        ++next;                // remember next element in msgs
        (*it)->remove(*this);
        it = next;
    }
}

void Message::save(Folder &f)
{
    // add f to Folders and this Message to f's list of Messages
    folders.insert(&f);  
    f.addMsg(this);
}

void Message::remove(Folder &f)
{
    // remove f from Folders and this Message from f's list of Messages
    folders.erase(&f);
    f.remMsg(this);
}

void Folder::save(Message &m)
{
    // add m and add this folder to m's set of Folders
    msgs.insert(&m);
    m.addFldr(this);
}

void Folder::remove(Message &m)
{
    // erase m from msgs and remove this folder from m
    msgs.erase(&m);
    m.remFldr(this);
}

vector<Folder*> Message::get_folders()
{
    return vector<Folder*>(folders.begin(), folders.end());
}

vector<Message*> Folder::messages()
{
    return vector<Message*>(msgs.begin(), msgs.end());
}

void Folder::debug_print()
{
    cerr << "Folder contains " << msgs.size() << " messages" << endl;
    int ctr = 1;
    for (Msg_iter beg = msgs.begin(); beg != msgs.end(); ++beg)
        cerr << "Message " << ctr++ << ":\n\t" 
             << (*beg)->print_message() << endl;
}

void Message::debug_print()
{
    cerr << "Message:\n\t" << contents << endl;
    cerr << "Appears in " << folders.size() << " Folders" << endl;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
#include <set>
#include <vector>

class Folder;

class Message {
    friend class Folder;
public:
    // folders is initialized to the empty set automatically
    Message(const std::string &str = ""): 
                  contents (str) { }  
    // copy control: we must manage pointers to this Message
    // from the Folders pointed to by folders
    Message(const Message&);
    Message& operator=(const Message&); 
    ~Message();  

    // add/remove this Message from specified Folder's set of messages
    void save(Folder&);   
    void remove(Folder&); 

    std::vector<Folder*> get_folders(); // return list of folders 
                               // in which this message resides
    std::string print_message() { return contents; }
    void debug_print(); // print contents and it's list of Folders, 
                        // printing each Folder as well
private:
    std::string contents;      // actual message text
    std::set<Folder*> folders; // Folders that have this Message

    // Utility functions used by copy constructor, assignment, and destructor:
    // Add this Message to the Folders that point to the parameter
    void put_Msg_in_Folders(const std::set<Folder*>&); 
    // remove this Message from every Folder in folders
    void remove_Msg_from_Folders(); 

    // used by Folder class to add self to this Message's set of Folder's
    void addFldr(Folder *f) { folders.insert(f); }
    void remFldr(Folder *f) { folders.erase(f); }
};

class Folder {
    friend class Message;
public:
    ~Folder(); // remove self from Messages in msgs
    Folder(const Folder&); // add new folder to each Message| in msgs
    Folder& operator=(const Folder&); // delete Folder from lhs messages
                                      // add Folder to rhs messages
    Folder() { } // defaults ok

    void save(Message&); // add this message to folder
    void remove(Message&); // remove this message from this folder
    
    // don't reveal implementation because it's likely to change
    // also better to copy than ref to the internal data structure so
    // have a copy to operate on not the actual contents is safer
    std::vector<Message*> messages(); // return list of messages in this folder
    void debug_print(); // print contents and it's list of Folders, 
private:
    typedef std::set<Message*>::const_iterator Msg_iter;
    std::set<Message*> msgs;  // messages in this folder

    void copy_msgs(const std::set<Message*>&);// add this Folder to each Message
    void empty_msgs();           // remove this Folder from each Message
    void addMsg(Message *m) { msgs.insert(m); }
    void remMsg(Message *m) { msgs.erase(m); }
};
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Folder.h"
#include <string>
using std::string;

int main()
{
	string s1("contents1");
	string s2("contents2");
	string s3("contents3");
	string s4("contents4");
	string s5("contents5");
	string s6("contents6");
	
	// all new messages, no copies yet
	Message m1(s1);
	Message m2(s2);
	Message m3(s3);
	Message m4(s4);
	Message m5(s5);
	Message m6(s6);

	Folder f1;   // use to make point it is not Folder f1() that declare
	Folder f2;   // a function returning a Folder!

	m1.save(f1); m3.save(f1); m5.save(f1);
	m1.save(f2);
	m2.save(f2); m4.save(f2); m6.save(f2);
	
	m1.debug_print();
	f2.debug_print();

	// create some copies
	Message c1(m1);
	Message c2(m2), c4(m4), c6(m6);
	
	m1.debug_print();
	f2.debug_print();

	// now some assignments
	m2 = m3;
	m4 = m5;
	m6 = m3;
	m1 = m5;

	m1.debug_print();
	f2.debug_print();

	// finally, self-assignment
	m2 = m2;
	m1 = m1;

	m1.debug_print();
	f2.debug_print();

	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>

int main()
{
    int sum = 0;

    // sum values from 1 up to 10 inclusive
    for (int val = 1; val <= 10; ++val) 
        sum += val;  // equivalent to sum = sum + val
    
    std::cout << "Sum of 1 to 10 inclusive is " 
              << sum << std::endl;

    return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
int main()
{
    int sum = 0;
    for (int i = -100; i <= 100; ++i)
        sum += i;
    std::cout << sum << std::endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 


#include "io_preamble.h"

int main()
{
    int ch;    // NOTE: int, not char!!!!

    // loop to read and write all the data in the input
    while ((ch = cin.get()) != EOF)
             cout.put(ch);
    cout << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string> 
#include <iostream> 

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::getline;

int main() 
{
	string line;

	// read line at time until end-of-file
	while (getline(cin, line))
		cout << line << endl;
	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "io_preamble.h"

int main()
{
    char ch;
    while (cin.get(ch))
            cout.put(ch);
    cout << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "io_preamble.h"

// opens in binding it to the given file
ifstream& open_file(ifstream &in, const string &file)
{
    in.close();  // close in case it was already open
    in.clear();  // clear any existing errors

    // if the open fails, the stream will be in an invalid state
    in.open(file.c_str()); // open the file we were given

    return in; // condition state is good if open succeeded
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Query.h"
#include "TextQuery.h"
#include <string>
#include <set>
#include <iostream>
#include <fstream>

using std::set;
using std::string;
using std::cerr;
using std::cout;
using std::cin;
using std::ifstream;
using std::endl;

TextQuery build_textfile(const string &filename)
{
    // get a file to read from which user will query words
    ifstream infile;
    if (!open_file(infile, filename)) {
        cerr << "No input file!" << endl;
        return TextQuery();
    }

    TextQuery ret;
    ret.read_file(infile);  // builds query map
    return ret;  // builds query map
}

bool get_word(string &s1)
{
    cout << "enter a word to search for, or q to quit: ";
    cin >> s1;
    if (!cin || s1 == "q") return false;
    else return true;
}

bool get_words(string &s1, string &s2)
{

    // iterate with the user: prompt for a word to find and print results
    cout << "enter two words to search for, or q to quit: ";
    cin  >> s1;

    // stop if hit eof on input or a "q" is entered
    if (!cin || s1 == "q") return false;
    cin >> s2;
    return true;
}

void print_results(const set<TextQuery::line_no>& locs, const TextQuery &file)
{
    // report no matches
    if (locs.empty()) {
        cout << "\nSorry. There are no entries for your query." 
             << "\nTry again." << endl;
        return;
    }

    // if the word was found, then print count and all occurrences
    set<TextQuery::line_no>::size_type size = locs.size();
    cout << "match occurs " 
         << size << (size == 1 ? " time:" : " times:") << endl;

    // print each line in which the word appeared
    set<TextQuery::line_no>::const_iterator it = locs.begin();
    for ( ; it != locs.end(); ++it) {
        cout << "\t(line "
             // don't confound user with text lines starting at 0
             << (*it) + 1 << ") "
             << file.text_line(*it) << endl;
    }
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout;
using std::endl;
// const int ia[] is equivalent to const int* ia
// size is passed explicitly and used to control access to elements of ia
void printValues(const int ia[], size_t size) 
{
    for (size_t i = 0; i != size; ++i) {
        cout << ia[i] << endl;
    }
}

int main() 
{
    int j[] = { 0, 1 };  // int array of size 2

    printValues(j, sizeof(j)/sizeof(*j));  

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <stdexcept>

template <class T> class Handle {
public:
    // unbound handle
    Handle(T *p = 0): ptr(p), use(new size_t(1)) { }

    // overloaded operators to support pointer behavior
    T& operator*();
    T* operator->();
    const T& operator*() const;
    const T* operator->() const;

    // copy control: normal pointer behavior, but last Handle deletes the object
    Handle(const Handle& h): ptr(h.ptr), use(h.use) 
                                        { ++*use; }
    Handle& operator=(const Handle&);
    ~Handle() { rem_ref(); }
private:
    T* ptr;          // shared object
    size_t *use;     // count of how many Handles point to *ptr
    void rem_ref() 
        { if (--*use == 0) { delete ptr; delete use; } }
};

template <class T> 
inline Handle<T>& Handle<T>::operator=(const Handle &rhs)
{
    ++*rhs.use;    // protect against self-assignment 
    rem_ref();     // decrement use count and delete pointers if needed
    ptr = rhs.ptr;
    use = rhs.use;
    return *this;
}

template <class T> inline T& Handle<T>::operator*() 
{ 
    if (ptr) return *ptr; 
    throw std::runtime_error
                   ("dereference of unbound Handle"); 
}
template <class T> inline T* Handle<T>::operator->() 
{ 
    if (ptr) return ptr; 
    throw std::runtime_error
                   ("access through unbound Handle");

}

template <class T> inline
const T& Handle<T>::operator*() const
{ 
    if (ptr) return *ptr; 
    throw std::runtime_error("dereference of unbound Handle");
}
template <class T> inline
const T* Handle<T>::operator->() const
{ 
    if (ptr) return ptr; 
    throw std::runtime_error("access through unbound Handle");

}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>

using std::cout;
using std::endl;

int main() 
{
    int i = 0, j;
    j = ++i; // j = 1, i = 1: prefix yields incremented value

    cout << j << endl;
    j = i++; // j = 1, i = 2: postfix yields unincremented value

    cout << j << endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>

#include <vector>

using std::cout;
using std::endl;
using std::vector;


int main()
{
    // empty vector 
    vector<int> ivec;   
    int val;
    // give each element a new value
    for (vector<int>::size_type ix = 0;
                                ix != 10; ++ix)
          ivec.push_back(ix);

    cout << "ivec.size: " << ivec.size() << endl;  // prints 10

    // reset the elements in the vector to zero
    for (vector<int>::size_type ix = 0; ix != ivec.size(); ++ix)
        ivec[ix] = 0;

    // is there anything to print?
    if (ivec.empty() == false) {
        cout << "vector contents: " << endl;
        // print each element separated by a newline
        for (vector<int>::size_type ix = 0; 
                          ix != ivec.size(); ++ix)
            cout << ivec[ix] << endl;
     }
     return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
using std::string;
#include <iostream>
using std::cout; using std::endl;

//inline version: find longer of two strings
inline const string &
shorterString(const string &s1, const string &s2)
{
        return s1.size() < s2.size() ? s1 : s2;
}

int main()
{
	string s1("successes"), s2("failure");
	cout << shorterString(s1, s2) << endl;

	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "algs_preamble.h"
// combine elements into a parenthesized, comma-separated pair
string combine(string x, string y)
{
	return "(" + x + ", " + y + ")";
}
// add two strings, each separated by a comma
string concatenate(string x, string y)
{
	if (x.empty())
		return y;
	return x + ", " + y;
}

int main()
{
	vector<string> names, values;
	names.push_back("if");
	values.push_back("keyword");
	names.push_back("string");
	values.push_back("library type");
	names.push_back("sort");
	values.push_back("algorithm");

	cout << inner_product(names.begin(), names.end(),
	                      values.begin(), string(),
	                      concatenate, combine);
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "algs_preamble.h"

int main() 
{

    list<int> ilst, ilst2, ilst3;     // empty lists

    // after this loop ilst contains: 1 2 3 4
    for (list<int>::size_type i = 0; i != 4; ++i)
         ilst.push_front(i);

    // print list to see the ordering
    cout << "ilst" << endl;
    for (list<int>::iterator iter = ilst.begin(); iter != ilst.end(); ++iter)
	    cout << *iter << " ";
    cout << endl;

    // after copy ilst2 contains: 4 3 2 1
    copy(ilst.begin(), ilst.end(), front_inserter(ilst2));

    // after copy, ilst3 contains: 1 2 3 4
    copy(ilst.begin(), ilst.end(), 
                inserter(ilst3, ilst3.begin()));

    // print ilst2 and ilst3 
    cout << "ilst2" << endl;
    for (list<int>::iterator iter = ilst2.begin(); iter != ilst2.end(); ++iter)
	    cout << *iter << " ";
    cout << endl;
    cout << "ilst3" << endl;
    for (list<int>::iterator iter = ilst3.begin(); iter != ilst3.end(); ++iter)
	    cout << *iter << " ";
    cout << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout;
using std::endl;

int main() 
{
    int ival1 = 21/6;  // integral result obtained by truncating the remainder 
    int ival2 = 21/7;  // no remainder, result is an integral value

    cout << ival1 << " " << ival2 << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
//NB: if change the Queue interface must also change this file!

// simulated version of Queue instantiated for type int
template <class Type> class Queue<int> {
public:
    Queue();                  // this bound to Queue<int>*
    int &front();             // return type bound to int
    const int &front() const; // return type bound to int

    void push(const int &);   // parameter type bound to int
    void pop();               // type invariant code

    bool empty() const;       // type invariant code
private:
    // . . .
    // implementation TBD
    QueueItem<int> *head;     // use QueueItem of int
    QueueItem<int> *tail;   
};
#endif

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Sales_item.h"
#include "algs_preamble.h"
int main()
{
    istream_iterator<int> cin_it(cin);   // reads ints from cin
    istream_iterator<int> end_of_stream; // end iterator value

    // initialize vec from the standard input:
    vector<int> vec(cin_it, end_of_stream);

    sort(vec.begin(), vec.end());

    // writes ints to cout using " " as the delimiter
    ostream_iterator<int> output(cout, " ");  

    // write only the unique elements in vec to the standard output
    unique_copy(vec.begin(), vec.end(), output);
    cout << endl;  // write a newline after the output
    return 0;
}

void read_write() {
    ofstream outfile;
    outfile.open("dictionary");

    // write a sequence of strings to the ofstream named outfile
    // placing each string on a separate line
    ostream_iterator<string> os_string(outfile, "\n");

    istream_iterator<int> input(cin), eof;
    ostream_iterator<int> output(cout);  
    while (input != eof)
        *output++ = *input++;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <iomanip>

using std::sqrt; using std::getline;
using std::vector; using std::list; using std::map; using std::set;  
using std::pair; using std::string;  
using std::istream; using std::ostream; 
using std::ifstream; using std::ofstream; using std::fstream;
using std::istringstream; using std::ostringstream; using std::stringstream;
using std::cin; using std::cout; using std::endl; using std::cerr;
using std::istream_iterator; using std::ostream_iterator;
using std::find;  using std::sort; using std::for_each;
using std::count; using std::count_if; using std::copy;
using std::back_inserter; using std::reverse_iterator;
using std::ios_base;
using std::boolalpha; using std::noboolalpha; 
using std::oct; using std::dec; using std::hex;
using std::showbase; using std::noshowbase;
using std::uppercase; using std::nouppercase;
using std::setprecision; using std::showpoint; using std::noshowpoint;
using std::scientific; using std::fixed;
using std::skipws; using std::noskipws; using std::flush; using std::ends;
using std::unitbuf; using std::nounitbuf; 
using std::setw; using std::right; using std::left; using std::internal;
using std::setfill;

ifstream& open_file(ifstream&, const string&);
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
#include "Sales_item.h"

int main() 
{
    Sales_item book;

    // read ISBN, number of copies sold, and sales price
    std::cin >> book;

    // write ISBN, number of copies sold, total revenue, and average price
    std::cout << book << std::endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#ifndef LOCALMATH_H
#define LOCALMATH_H

//definition in gcd.cc
int gcd(int, int);
#endif
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "io_preamble.h"
#include <stdexcept>
using std::range_error;
// prototypical main function that takes a file name to open
int main(int argc, char **argv)
{
    if (argc < 2)
        throw range_error("No file name given");
    ifstream in;
    open_file(in, argv[1]);
    string word;
    while (in >> word)
        cout << word << endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Query.h"
#include "TextQuery.h"
#include <string>
#include <vector>
#include <set>
#include <iostream>

using std::set;
using std::string;
using std::cout;
using std::endl;

int main(int, char **argv)
{
    // get a file to read and builds map to support queries
    TextQuery file = build_textfile(argv[1]);

    // iterate with the user: prompt for a word to find and print results
    string sought;
    do {
        if (!get_word(sought)) break;
        // find all the occurrences of the requested string
        Query name(sought);
        const set<TextQuery::line_no> locs = name.eval(file);
        cout << "\nExecuted Query for: " << name << endl;

        // report no matches
        print_results(locs, file);
     } while (true);  // loop indefinitely; the exit is inside the loop
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Query.h"
#include "TextQuery.h"
#include <string>
#include <vector>
#include <set>
#include <iostream>

using std::set;
using std::string;
using std::cout;
using std::endl;

int main(int, char **argv)
{
    // get a file to read and builds map to support queries
    TextQuery file = build_textfile(argv[1]);

    // iterate with the user: prompt for a word to find and print results
    do {
        string sought;
        if (!get_word(sought)) break;
        // find all the occurrences of the requested string
        Query name(sought);
        Query notq = ~name;
        const set<TextQuery::line_no> locs = notq.eval(file);
        cout << "\nExecuted Query for: " << notq << endl;

        // report no matches
        print_results(locs, file);
    } while (true);  // loop indefinitely; the exit is inside the loop
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Query.h"
#include "TextQuery.h"
#include <string>
#include <set>
#include <iostream>

using std::set;
using std::string;
using std::cout;
using std::endl;

int main(int, char **argv)
{
    // gets file to read and builds map to support queries
    TextQuery file = build_textfile(argv[1]);

    // iterate with the user: prompt for a word to find and print results
    do {
        string sought1, sought2;
        if (!get_words(sought1, sought2)) break;
        // find all the occurrences of the requested string
        Query orq = Query(sought1) | Query(sought2);
        cout << "\nExecuting Query for: " << orq << endl;
        const set<TextQuery::line_no> locs = orq.eval(file);

        // report matches
        print_results(locs, file);
    } while (true);  // loop indefinitely; the exit is inside the loop
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Query.h"
#include "TextQuery.h"
#include <string>
#include <set>
#include <iostream>

using std::set;
using std::string;
using std::cout; using std::cerr;
using std::endl;

int main(int, char **argv)
{
    // gets file to read and builds map to support queries
    TextQuery file = build_textfile(argv[1]);

    // iterate with the user: prompt for a word to find and print results
    do {
        string sought1, sought2;
        // stop if hit eof on input or a "q" is entered
        if (!get_words(sought1, sought2)) break;
    
        // find all the occurrences of the requested string
        Query andq = Query(sought1) & Query(sought2);
        set<TextQuery::line_no> locs = andq.eval(file);
        cout << "\nExecuted query: " << andq << endl;
        // report matches
        print_results(locs, file);
    
        locs = Query(sought1).eval(file);
        cout << "\nExecuted query: " << Query(sought1) << endl;
        // report matches
        print_results(locs, file);
    
        locs = Query(sought2).eval(file);
        cout << "\nExecuted query: " << Query(sought2) << endl;
        // report matches
        print_results(locs, file);
    } while(true);
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Query.h"
#include "TextQuery.h"
#include <string>
#include <set>
#include <iostream>

using std::set;
using std::string;
using std::cin; using std::cout; using std::cerr;
using std::endl;

int main(int, char **argv)
{
    // gets file to read and builds map to support queries
    TextQuery file = build_textfile(argv[1]);

    // iterate with the user: prompt for a word to find and print results
    while (true) {
        string sought1, sought2, sought3;
        if (!get_words(sought1, sought2)) break;
        cout << "\nenter third word: " ;
        cin  >> sought3;
        // find all the occurrences of the requested string
        Query q = Query(sought1) & Query(sought2) 
                             | Query(sought3);

        cout << "\nExecuting Query for: " << q << endl;
        const set<TextQuery::line_no> locs = q.eval(file);
        // report matches
        print_results(locs, file);
     } 
     return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "localMath.h"

// rest of main as before
#include <iostream>

using std::cout;
using std::endl;
using std::cin;

int main()
{
    // get values from standard input
    cout << "Enter two values: \n";
    int i, j;
    cin >> i >> j;

    // call gcd on arguments i and j
    // and print their greatest common divisor
    cout << "gcd: " << gcd(i, j) << endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <cstdlib>
int main()
{
    bool some_failure = false;
    if (some_failure)
        return EXIT_FAILURE;
    else
        return EXIT_SUCCESS;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

int main() 
{
	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "io_preamble.h"

int main()
{
    cout << "default bool values: "
         << true << " " << false
         << "\nalpha bool values: "
         << boolalpha
         << true << " " << false
         << endl;

    bool bool_val = true;
    cout << boolalpha    // sets internal state of cout
         << bool_val
         << noboolalpha; // resets internal state to default formatting
    cout << endl;

    const int ival = 15, jval = 1024;  // const, so values never change

    cout << "default: ival = " << ival 
         << " jval = " << jval << endl;
    cout << "printed in octal: ival = " << oct << ival 
         << " jval = " << jval << endl;

    cout << "printed in hexadecimal: ival = " << hex << ival 
         << " jval = " << jval << endl;

    cout << "printed in decimal: ival = " << dec << ival 
         << " jval = " << jval << endl;

{
    const int ival = 15, jval = 1024;  // const so values never change

    cout << showbase;   // show base when printing integral values

    cout << "default: ival = " << ival 
         << " jval = " << jval << endl;
    cout << "printed in octal: ival = " << oct << ival 
         << " jval = " << jval << endl;
    cout << "printed in hexadecimal: ival = " << hex << ival 
         << " jval = " << jval << endl;
    cout << "printed in decimal: ival = " << dec << ival 
         << " jval = " << jval << endl;

    cout << noshowbase;  // reset state of the stream
}

    cout << uppercase << showbase << hex
         << "printed in hexadecimal: ival = " << ival 
         << " jval = " << jval << endl
         << nouppercase << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "localMath.h"
// return the greatest common divisor
int gcd(int v1, int v2)
{
    while (v2) {
        int temp = v2;
        v2 = v1 % v2;
        v1 = temp;
    }
    return v1;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
using std::string;
#include <iostream>
using std::cout; using std::endl;

//return plural version of word if ctr isn't 1
string make_plural(size_t ctr, const string &word, 
                               const string &ending)
{
	return (ctr == 1) ? word : word + ending;
}

int main()
{
	size_t cnt = 1;
	cout << make_plural(cnt, "success", "es") << endl;
	cnt = 2;
	cout << make_plural(cnt, "failure", "s") << endl;

	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <cstddef>
#include <iostream>
#include <string>

using std::cout; using std::endl; using std::size_t; using std::string;

// return plural version of word if ctr isn't 1
string make_plural(size_t ctr, const string &word, 
                               const string &ending)
{
        return (ctr != 1) ?  word + ending : word;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <exception>
#include <stdexcept>

void handle_out_of_memory(const std::bad_alloc&) { }

template <class T> class Handle {
public:
    Handle(T *p = 0) try : ptr(p), use(new size_t(1)) { 
    } catch(const std::bad_alloc &e) { handle_out_of_memory(e); }

    // overloaded operators to support pointer behavior
    T& operator*();
    T* operator->();
    const T& operator*() const;
    const T* operator->() const;

    // copy control: normal pointer aliasing; but last Handle deletes the object
    Handle(const Handle& h): ptr(h.ptr), use(h.use) { ++*use; }
    Handle& operator=(const Handle&);
    ~Handle() { rem_ref(); }
private:
    T* ptr;
    size_t *use;
    void rem_ref() { if (--*use == 0) {delete ptr; delete use;} }
};

/*  workaround: MS Compiler doesn't support out of line definition
 *  of function try block for template class constructor
template <class T> Handle<T>::Handle(T *p)
try : ptr(p), use(new size_t(1)) 
{
     // empty function body
}  catch(const std::bad_alloc &e) 
       { handle_out_of_memory(e); }
*/

template <class T> 
inline
Handle<T>& Handle<T>::operator=(const Handle &rhs)
{
    ++*rhs.use;
    rem_ref();     // decrement use count and delete pointers if needed
    ptr = rhs.ptr;
    use = rhs.use;
    return *this;
}

template <class T> inline T& Handle<T>::operator*() 
{ 
    if (ptr) return *ptr; 
    else throw std::runtime_error
                   ("derference of unbound Handle"); 
}
template <class T> inline T* Handle<T>::operator->() 
{ 
    if (ptr) return ptr; 
    else throw std::runtime_error
                   ("access through unbound Handle");

}

template <class T> inline
const T& Handle<T>::operator*() const
{ 
    if (ptr) return *ptr; 
    else throw std::runtime_error("derference of unbound Handle");
}
template <class T> inline
const T* Handle<T>::operator->() const
{ 
    if (ptr) return ptr; 
    else throw std::runtime_error("access through unbound Handle");

}
#include "MS_Queue.h"
#include <stdexcept>
using std::out_of_range;
#include <iostream>
using std::ostream; 

template <class Type>
void Queue<Type>::copy_elems(const Queue &orig)
{
    // copy elements from orig into this Queue
    // loop stops when pt == 0, which happens when we reach orig.tail
    for (QueueItem<Type> *pt = orig.head; pt; pt = pt->next)
        push(pt->item);  // copy the element
}

template <class Type>
Queue<Type>& Queue<Type>::operator=(const Queue &rhs)
{
    if (this != &rhs) {
        // destroy elements on this Queue
        destroy();
        copy_elems(rhs);    // copy from rhs into left-hand Queue
    }
    return *this;
}

template <class Type> void Queue<Type>::destroy()
{
    while (!empty())
        pop();
}


template <class Type> void Queue<Type>::push(const Type &val)
{
    // allocate a new QueueItem object
    QueueItem<Type> *pt = new QueueItem<Type>(val);

    // put item onto existing queue
    if (empty())
        head = tail = pt;   // the queue now has only one element
    else {
        tail->next = pt;    // add new element to end of the queue
        tail = pt;
    }
}

template <class Type> void Queue<Type>::pop()
{
    // pop is unchecked: Popping off an empty Queue is undefined 
    QueueItem<Type>* p = head;  // keep pointer to head so we can delete it
    head = head->next;          // head now points to next element
    delete p;                   // delete old head element
}

template <class Type>
ostream& operator<<(ostream &os, const Queue<Type> &q)
{
    os << "< ";
    QueueItem<Type> *p;
    for (p = q.head; p; p = p->next)
            os << p->item << " ";
    os <<">";
    return os;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#ifndef QUEUE_H
#define QUEUE_H

//NB: if change the interface in this file, also update Queue_interface.h!!!!
//    and int-Queue.h

#include <iostream>

// declaration that Queue is a template needed for friend declaration in QueueItem
template <class Type> class Queue;
// function template declaration must precede friend declaration in QueueItem
template <class T> 
std::ostream& operator<<(std::ostream&, const Queue<T>&);

template <class Type> class QueueItem {
    friend class Queue<Type>;

    // needs access to item and next
    friend std::ostream& 
    operator<< <Type> (std::ostream&, const Queue<Type>&);

// private class: no public section
    QueueItem(const Type &t): item(t), next(0) { }
    Type item;           // value stored in this element
    QueueItem *next;     // pointer to next element in the Queue
};


template <class Type> class Queue {
    // needs access to head 
    friend std::ostream& 
    operator<< <Type> (std::ostream&, const Queue<Type>&);
public:
    // empty Queue
    Queue(): head(0), tail(0) { }
public:
    // construct a Queue from a pair of iterators on some sequence
    template <class It> 
    Queue(It beg, It end): 
          head(0), tail(0) { copy_elems(beg, end); }

    // copy control to manage pointers to QueueItems in the Queue
    Queue(const Queue &Q): head(0), tail(0) 
                                  { copy_elems(Q); }
    Queue& operator=(const Queue&);
    ~Queue() { destroy(); }

    // replace current Queue by contents delimited by a pair of iterators
    // MS: requires member templates to be defined inside the class body
    template <class Iter> void assign(Iter beg, Iter end) 
    { destroy(); copy_elems(beg, end); }   
    // rest of Queue class as before

    // return element from head of Queue
    // unchecked operation: front on an empty Queue is undefined
    Type& front()             { return head->item; }
    const Type &front() const { return head->item; }

    void push(const Type &);      // add element to back of Queue
    void pop();                   // remove element from head of Queue

    bool empty() const {          // true if no elements in the Queue
        return head == 0;
    }
private:
    QueueItem<Type> *head;         // pointer to first element in Queue
    QueueItem<Type> *tail;         // pointer to last element in Queue

    // utility functions used by copy constructor, assignment, and destructor
    void destroy();                // delete all the elements
    void copy_elems(const Queue&); // copy elements from parameter
private:
    // version of copy to be used by assign to copy elements from iterator range
    // MS: requires member templates to be defined inside the class body
    template <class Iter> void copy_elems(Iter beg, Iter end) {
        while (beg != end) {
           push(*beg);
           ++beg;
        }
    }
};

// Inclusion Compilation Model: include member function definitions as well
#include "MS_Queue.cpp"

#endif

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

template <class Type> class Queue {
    // interface functions to Queue are unchanged
public:
    Queue(): head(0), tail(0) { }

    // copy control to manage pointers to QueueItems in the Queue
    Queue(const Queue &Q): head(0), tail(0) {copy_elems(Q);}
    Queue& operator=(const Queue&);
    ~Queue() { destroy(); }

    // return element from head of Queue
    // unchecked operation: front on an empty Queue is undefined
    Type& front()            { return head->item; }
    const Type front() const { return head->item; }

    void push(const Type &);// add element to back of Queue
    void pop();             // remove element from head of Queue

    bool empty() const {    // true if no elements in the Queue
        return head == 0;
    }
private:
    // workaround: MS Compiler doesn't allow nested template class
    // to be defined outside the class body
    //struct QueueItem;  // forward declaration of nested type QueueItem
    struct QueueItem {
        QueueItem(const Type &t): item(t), next(0) { }
        Type item;           // value stored in this element
        QueueItem *next;     // pointer to next element in the Queue
        // MS won't allow definition outside the class so no static members
        // of nested template classes
        //static int static_mem;  
    };

    QueueItem *head;   // pointer to first element in Queue
    QueueItem *tail;   // pointer to last element in Queue

    // utility functions used by copy constructor, assignment and destructor
    void destroy();          // delete all the elements
    void copy_elems(const Queue&); // copy all elements from parameter
private:
    // copy all elements from input range denoted by iterator pair
    // workaround: MS Compiler requires member templates to be 
    // defined inside the class body
    template <class Iter> void copy_elems(Iter, Iter)
    {
        while (beg != end) {
           push(*beg);
           ++beg;
        }
    }
};

template <class Type>
void
Queue<Type>::copy_elems(const Queue &orig)
{
    // copy elements from orig into this Queue
    QueueItem *pt = orig.head;
    while (pt) {     // last element has next pointer == 0
        push(pt->item);  // copy the element
        pt = pt->next;   // get the next one from orig
    }
}

template <class Type>
Queue<Type>& Queue<Type>::operator=(const Queue &rhs)
{
    if (this != &rhs) {
        // destroy elements on this Queue
        destroy();
        copy_elems(rhs);    // copy from rhs into left-hand Queue
    }
    return *this;
}

template <class Type>
void
Queue<Type>::destroy()
{
    while (!empty())
        pop();
}


template <class Type>
void Queue<Type>::push(const Type &val)
{
    // allocate a new QueueItem object
    QueueItem *pt = new QueueItem(val);

    // put item onto existing queue
    if (empty())
        head = tail = pt;   // queue has only one element
    else
    {
        tail->next = pt;     // add new element to end of queue
        tail = pt;
    }
}


template <class Type>
void Queue<Type>::pop()
{
    // pop is unchecked: popping off an empty Queue is undefined 
    QueueItem* p = head;     // keep pointer to head so can delete it
    head = head->next;       // head now points to next element
    delete p;                // delete old head element
}

/* workaround: MS Complier doesn't allow member templates to be
 * defined outside the class body
template <class Type> template <class Iter>
void Queue<Type>::copy_elems(Iter beg, Iter end)
{
    while (beg != end) {
       push(*beg);  
       ++beg;
    }
}

* because can't define a member oustide the class, also can't
* have static members of nested template classes
// defines an int static member of QueueItem,
// which is a type nested inside Queue<Type>
template <class Type>
int Queue<Type>::QueueItem::static_mem = 1024;
*/


#include <iostream>
using std::cout; using std::endl;

Queue<int> queue1;

int main() 
{
    for (int ix = 0; ix != 1024; ++ix)
        queue1.push(ix);
    
    for (int ix = 0; ix != 1024; ++ix) {
        int i = queue1.front();  // check next item
        if (i != ix)
             cout << "Something's wrong! i = " << i
                  << " ix = " << ix << endl;
        queue1.pop();   // and remove it
    }

    if (!queue1.empty())
        cout << "Queue is not empty but should be!" << endl;
    else
        cout << "OK, queue empty again" << endl;


    for (int ix = 0; ix != 1024; ++ix)
        queue1.push(ix);

    Queue<int> queue2(queue1);  // use copy constructor

    for (int ix = 0; ix != 1024; ++ix) {
        int i = queue2.front();   // check next item
        if (i != ix)
             cout << "Something's wrong! i = " << i
                  << " ix = " << ix << endl;
        queue2.pop();  // and remove it
    }

    if (!queue2.empty())
        cout << "queue2 is not empty but should be!" << endl;
    else
        cout << "OK, queue2 empty again" << endl;

    queue2 = queue1;  // use asssignment operator
    for (int ix = 0; ix != 1024; ++ix) {
        int i = queue2.front();
        if (i != ix)
             cout << "Something's wrong! i = " << i
                  << " ix = " << ix << endl;
        queue2.pop();
    }

    if (!queue2.empty())
        cout << "queue2 is not empty but should be!" << endl;
    else
        cout << "OK, queue2 empty again" << endl;
    
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout;
using std::endl;

    int ia[3][4];      // array of size 3, each element is an array of ints of size 4 

int main()
{
    // row 0 is explicitly initialized; other elements implicitly 0
    int ix[3][4] = {0, 3, 6, 9};
    cout << ix[0][3] << " " << ix[1][0] << " " << ix[2][0] << endl; //prints 9 0 0
    const size_t rowSize = 3;
    const size_t colSize = 4;
    int ia[rowSize][colSize];   // 12 uninitialized elements 

    // for each row
    for (size_t i = 0; i != rowSize; ++i)     
        // for each column within the row
        for (size_t j = 0; j != colSize; ++j) 
            // initialize to its positional index
            ia[i][j] = i * colSize + j;   

    cout << ia[0][0] << " " 
         << 
            ia[2][3] // fetches last element from the array in the last row
         << endl;  // prints 0 11
    // prints garbage: mising second dimension so this statement prints a pointer
    cout << ia[0] << " "  << endl;  

    int (*ip)[4] = ia; // ip points to an array of 4 ints
    ip = &ia[2];       // ia[2] is an array of 4 ints
{
    typedef int int_array[4];
    int_array *ip = ia;

    // use pointers to print the contents of the array
    for (int_array *p = ia; p != ia + 3; ++p)
        for (int *q = *p; q != *p + 4; ++q)
             cout << *q << endl;
}
    cout << ia[0][0] << " " 
         << ia[2][3] // fetches last element from the array in the last row
         << endl;  // prints 0 11
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "assoc_preamble.h"

#include "assoc_preamble.h"

int main()
{
    // map from author to title; there can be multiple titles per author
    multimap<string, string> authors;
    // add data to authors
    authors.insert(pair<string, string>("Alain de Botton", "book1"));
    authors.insert(pair<string, string>("Alain de Botton", "book2"));
    authors.insert(pair<string, string>("Alain de Botton", "book3"));
    authors.insert(pair<string, string>("Alain de Botton", "book4"));

    // author we'll look for
    string search_item("Alain de Botton");

    // how many entries are there for this author
    typedef multimap<string, string>::size_type sz_type;
    sz_type entries = authors.count(search_item);

    // get iterator to the first entry for this author
    multimap<string,string>::iterator iter = 
                             authors.find(search_item);

    // loop through the number of entries there are for this author
    for (sz_type cnt = 0; cnt != entries; ++cnt, ++iter)
           cout << iter->second << endl; // print each title

    // definitions of authors and search_item as above

    // beg and end denote range of elements for this author
    typedef multimap<string, string>::iterator authors_it;
    authors_it beg = authors.lower_bound(search_item),
               end = authors.upper_bound(search_item);

    // loop through the number of entries there are for this author
    while (beg != end) {
        cout << beg->second << endl; // print each title
        ++beg;
    }

    // definitions of authors and search_item as above

    // pos holds iterators that denote range of elements for this key
    pair<authors_it, authors_it> 
                     pos = authors.equal_range(search_item);

    // loop through the number of entries there are for this author
    while (pos.first != pos.second) {
        cout << pos.first->second << endl; // print each title
        ++pos.first;
    }
    return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

// ---- myMath.h ----
extern "C" double calc(double);
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream> 

int main() 
{
	int sum = 0, value;
	// read till end-of-file, calculating a running total of all values read
	while (std::cin >> value) 
		sum += value; // equivalent to sum = sum + value
	std::cout << "Sum is: " << sum << std::endl;

	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#ifndef QUEUE_H
#define QUEUE_H

//NB: if change the interface in this file, also update Queue_interface.h!!!!
//    and int-Queue.h

#include <iostream>
#include <list>

// declaration that Queue is a template needed for friend declaration in QueueItem
template <class Type> class Queue;
// function template declaration must precede friend declaration in QueueItem
template <class T> 
std::ostream& operator<<(std::ostream&, const Queue<T>&);

template <class Type> class Queue {
    friend std::ostream& 
    operator<< <Type> (std::ostream&, const Queue<Type>&);
public:
    Queue() { }
public:
    // construct a Queue from a pair of iterators on some sequence
    template <class It> Queue(It beg, It end): items(beg, end) { }

    // replace current Queue by contents delimited by a pair of iterators
    template <class Iter> void assign(Iter beg, Iter end)
                             { items.assign(beg, end); }

    // return element from head of Queue
    // unchecked operation: front on an empty Queue is undefined
    Type& front()             { return items.front(); }
    const Type &front() const { return items.front(); }

    // add element to back of Queue
    void push(const Type &t) { items.push_back(t); }
    // remove element from head of Queue
    void pop()               { items.erase(items.begin()); }

    // true if no elements in the Queue
    bool empty() const       { return items.empty(); }
private:
    std::list<Type> items;
};

template <class Type>
std::ostream& operator<<(std::ostream &os, const Queue<Type> &q)
{
    os << "< ";
    typename std::list<Type>::const_iterator beg = q.items.begin();
    while (beg != q.items.end()) {
        os << *beg << " ";
        ++beg;
    }
    os <<">";
    return os;
}

#endif

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;

#include "newQueue.h"

Queue<int> queue1;

int main() 
{
    for (int ix = 0; ix != 1024; ++ix)
        queue1.push(ix);
    
    for (int ix = 0; ix != 1024; ++ix) {
        int i = queue1.front();  // check next item
        if (i != ix)
             cout << "Something's wrong! i = " << i
                  << " ix = " << ix << endl;
        queue1.pop();   // and remove it
    }

    if (!queue1.empty())
        cout << "Queue is not empty but should be!" << endl;
    else
        cout << "OK, queue empty again" << endl;


    for (int ix = 0; ix != 1024; ++ix)
        queue1.push(ix);

    Queue<int> queue2(queue1);  // use copy constructor

    for (int ix = 0; ix != 1024; ++ix) {
        int i = queue2.front();   // check next item
        if (i != ix)
             cout << "Something's wrong! i = " << i
                  << " ix = " << ix << endl;
        queue2.pop();  // and remove it
    }

    if (!queue2.empty())
        cout << "queue2 is not empty but should be!" << endl;
    else
        cout << "OK, queue2 empty again" << endl;

    queue2 = queue1;  // use asssignment operator
    for (int ix = 0; ix != 1024; ++ix) {
        int i = queue2.front();
        if (i != ix)
             cout << "Something's wrong! i = " << i
                  << " ix = " << ix << endl;
        queue2.pop();
    }

    if (!queue2.empty())
        cout << "queue2 is not empty but should be!" << endl;
    else
        cout << "OK, queue2 empty again" << endl;
    
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;
#include "newQueue.h"

int main()
{
    // the class Queue<int> is instantiated
    // new expression requires that Queue<int> be defined
    Queue<int> *p_qi = new Queue<int>;

    int ival;
    for (ival = 0; ival != 10; ++ival)
        // the member function push() is instantiated
        p_qi->push(ival);

    int err_cnt = 0;
    for (ival = 0; ival != 10; ++ival) {
        // the member function front is instantiated
        int qval = p_qi->front();

        if (ival != qval) err_cnt++;
        p_qi->pop();                // instantiates pop
    }

    if (!err_cnt)
        cout << "!! queue executed ok" << endl;
    else 
        cout << "?? " << err_cnt << " Queue errors" << endl;

    delete p_qi;   // deletes elements in the queue

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;
#include "newQueue.h"

int main() {
    Queue<int> qi;
    // instantiates ostream& operator<<(ostream&, const Queue<int>&)
    cout << qi << endl;

    int ival;
    for (ival = 0; ival != 10; ++ival)
        qi.push(ival);   // instantiates Queue<int>::push(int)
    cout << qi << endl;

    for (ival = 0; ival != 10; ++ival) {
        qi.pop();        // instantiates Queue<int>::pop()
    }

    cout << qi << endl;

    return 0;
}    // qi goes out of scope, elements in qi deleted automatically
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "io_preamble.h"

int main()
{
    char ch;
    cin >> noskipws;      // set cin so that it reads whitespace
    while (cin >> ch)
            cout << ch;
    cin >> skipws;        // reset cin to default state so that it discards whitespace
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;
#include <cstddef>
using std::size_t;

template <class T, size_t N> size_t arr_sz(T (&parm)[N])
{
    return N;
}


// initialize elements of an array to zero
template <class T, size_t N> void array_init(T (&parm)[N]) 
{ 
    for (size_t i = 0; i != N; ++i) {
        parm[i] = 0;
    }
    cout << "calling arr_sz " << arr_sz(parm) << endl;
}

int main()
{
    int x[42]; 
    double y[10];
    array_init(x);  // instantiates array_init(int(&)[42]
    array_init(y);  // instantiates array_init(double(&)[10]
    for (size_t i = 0; i != 42; ++i)
        cout << x[i] << " ";
    cout << endl;
}

void f() {
    int x[42];
    const int sz = 40;
    int y[sz + 2];
    array_init(x);  // instantiates array_init(int(&)[42])
    array_init(y);  // equivalent instantiation
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
#include <iostream>

class Endangered {
public:
    enum Status { critical, environment, improving };
    Endangered(Status stat): animal_status(stat) { }
    virtual ~Endangered();
    virtual std::ostream& print(std::ostream&) const;
    virtual void highlight() const;
    virtual int population() const;
    // ...
private:
    Status animal_status;
};

class ZooAnimal;
extern std::ostream&
operator<<(std::ostream&, const ZooAnimal&);

class ZooAnimal {
public:
    ZooAnimal();
    ZooAnimal(std::string animal, bool exhibit,
              std::string family): nm(animal), 
                                   exhibit_stat(exhibit), 
                                   fam_name(family) { } 
    virtual ~ZooAnimal();

    virtual std::ostream& print(std::ostream&) const;

    // accessors
    std::string name() const { return nm; }
    std::string family_name() const { return fam_name; }
    bool onExhibit() const { return exhibit_stat; }
    // ...
protected:
    std::string nm;
    bool        exhibit_stat;
    std::string fam_name;
    // ...
private:
};

class Bear : public ZooAnimal {
enum DanceType { two_left_feet, macarena, fandango, waltz };
public:
    Bear();
    Bear(std::string name, bool onExhibit=true, 
         std::string family = "Bear"):
                         ZooAnimal(name, onExhibit, family),
                         ival(0), dancetype(two_left_feet) { }

    virtual std::ostream &print(std::ostream&) const;
    virtual int toes() const;
    int mumble(int);
    void dance(DanceType) const;

    virtual ~Bear();
private:
    int         ival;
    DanceType   dancetype;
};
class Panda : public Bear, public Endangered {
public:
    Panda();
    Panda(std::string name, bool onExhibit=true);
    virtual ~Panda();
    // no print function in this version
    void highlight();
    virtual int toes();
    virtual void cuddle();
// ...
};

// explicitly initialize both base classes
Panda::Panda(std::string name, bool onExhibit)
      : Bear(name, onExhibit, "Panda"),
        Endangered(Endangered::critical) { }

// implicitly use Bear default constructor to initialize the Bear subobject
Panda::Panda()
      : Endangered(Endangered::critical) { }
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
using std::string;
#include <iostream>
using std::cout; using std::endl;

int main() 
{
    int i(1024);             // value of i is 1024
    int *pi = new int(1024); // object to which pi points is 1024

    string s(10, '9');                  // value of s is "9999999999"
    string *ps = new string(10, '9');   // *ps is "9999999999"
    cout << "*pi: " << *pi
         << "\ti: " << i << endl
         << "*ps: " << *ps
         << "\ts: " << s << endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cin;
using std::cout;
using std::endl;

int main()
{
    char ch;
    // initialize counters for each vowel
    int aCnt = 0, eCnt = 0, iCnt = 0, 
        oCnt = 0, uCnt = 0;
    int otherCnt = 0;  // count anything that isn't a vowel

    while (cin >> ch) {
        // if ch is a vowel, increment the appropriate counter
        switch (ch) {
            case 'a':
                ++aCnt;
                break;
            // remaining vowel cases as before
            case 'e':
                ++eCnt;
                break;
            case 'i':
                ++iCnt;
                break;
            case 'o':
                ++oCnt;
                break;
            case 'u':
                ++uCnt;
                break;
            default:
                ++otherCnt;
                break;
        }
    }
    // print results
    cout << "Number of vowel a: \t" << aCnt << '\n'
         << "Number of vowel e: \t" << eCnt << '\n'
         << "Number of vowel i: \t" << iCnt << '\n'
         << "Number of vowel o: \t" << oCnt << '\n'
         << "Number of vowel u: \t" << uCnt << '\n'
         << "Total non-vowels : \t" << otherCnt << '\n';

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <vector>
using std:: vector;
#include <iostream>
using std::cout; using std::endl;
#include <string>
using std::string;

// implement strcmp-like generic compare function 
template <typename T> int compare2(T, T);

template <typename T> int compare2(T v1, T v2)
{
    // as before
	cout << "compare2(T, T)" << endl;
	if (v1 < v2) return -1;
	if (v2 < v1) return 1;
	return 0;
}

// plain functions to handle C-style character strings
int compare2(const char* v1, const char* v2)
{
	cout << "compare2(const char*, const char*)" << endl;
	return strcmp(v1, v2);
}

// compares two objects
template <typename T> int compare(const T&, const T&);

// compares elements in two sequences
template <class U, class V> int compare(U, U, V);

// plain functions to handle C-style character strings
int compare(const char*, const char*);


// compares two objects
template <typename T> int compare(const T &v1, const T &v2)
{
	cout << "compare(const T&, const T&)" << endl;
	if (v1 < v2) return -1;
	if (v2 < v1) return 1;
	return 0;
}

// compares elements in two sequences
template <class U, class V> int compare(U beg1, U end1, V beg2)
{
	cout << "compare(U, U, V)" << endl;
	while (beg1 != end1) {
		if (*beg1 < *beg2) return -1;
		if (*beg2 < *beg1) return 1;
		++beg1; ++beg2;
	}
	return 0;  // if we got here, the two sequences are equal
}

// plain functions to handle C-style character strings
int compare(const char* v1, const char* v2)
{
	cout << "compare(const char*, const char*)" << endl;
	return strcmp(v1, v2);
}

int main()
{
cout << "calling  compare(1, 0);" << endl;
// calls compare(const T&, const T&) with T bound to int
compare(1, 0);     

cout << "\ncalling  compare(vector iters);" << endl;


// calls compare(U, U, V), with U and V bound to vector<int>::iterator
vector<int> ivec1(10), ivec2(20);
compare(ivec1.begin(), ivec1.end(), ivec2.begin());

int ia1[] = {0,1,2,3,4,5,6,7,8,9};

// calls compare(U, U, V) with U bound to int* 
// and V bound to vector<int>::iterator
compare(ia1, ia1 + 10, ivec1.begin());

cout << "\ncalling  compare(const_arr1, const_arr2);" << endl;


// calls the ordinary function taking const char* parameters
const char const_arr1[] = "world", const_arr2[] = "hi";
compare(const_arr1, const_arr2); 

cout << "\ncalling  compare(s1, s2);" << endl;
// calls compare(const T&,const T&) with T bound to string
string s1 = "hi", s2 = "world";
compare(s1, s2);

cout << "\ncalling  compare(ch_arr1, ch_arr2);" << endl;


// calls the ordinary function taking const char* parameters
char ch_arr1[] = "world", ch_arr2[] = "hi";
compare(ch_arr1, ch_arr2); 

cout << "\ncalling  compare(p1, p2);" << endl;
char *p1 = ch_arr1, *p2 = ch_arr2;
compare(p1, p2);

cout << "\ncalling  compare(ch_arr1, const_arr1);" << endl;
cout << "calling  compare(ch_arr2, const_arr1);" << endl;
cout << "calling  compare(0, 0);" << endl;

compare(ch_arr1, const_arr1);
compare(ch_arr2, const_arr2);
compare(0, 0);

cout << "\ncalling  compare2(ch_arr1, ch_arr2);" << endl;
cout << "calling  compare2(p1, p2);" << endl;
cout << "calling  compare(const_arr1, const_arr2);" << endl;
cout << "calling  compare(cp1, cp2);" << endl;

// calls compare2(T, T) with T bound to char*
compare2(ch_arr1, ch_arr2); 

// calls compare2(T, T) with T bound to char*
compare2(p1, p2); 

// calls the ordinary function taking const char* parameters
compare2(const_arr1, const_arr2); 

const char *cp1 = const_arr1, *cp2 = const_arr2;
// calls the ordinary function taking const char* parameters
compare2(cp1, cp2); 
return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <vector>
using std:: vector;
#include <iostream>
using std::cout; using std::endl;
#include <string>
using std::string;

// implement strcmp-like generic compare function 
// returns 0 if the values are equal, 1 if v1 is larger, -1 if v1 is smaller
template <typename T>
int compare(const T &v1, const T &v2)
{
	cout << "compare(const T&, const T&)" << endl;

	if (v1 < v2) return -1;
	if (v2 < v1) return 1;
	return 0;
}

// compares elements in two sequences
template <class U, class V>
int compare(U beg1, U end1, V beg2)
{
	cout << "compare(U, U, V)" << endl;

	while (beg1 != end1) {
		if (*beg1 < *beg2) return -1;
		if (*beg2 < *beg1) return 1;
		++beg1, ++beg2;
	}
	return 0;  // if we got here the two sequences are equal
}

// plain functions to handle C-style character strings
int compare(char* v1, char* v2)
{
	cout << "compare(char*, char*)" << endl;

	return strcmp(v1, v2);
}

int compare(const char* v1, const char* v2)
{
	cout << "compare(const char*, const char*)" << endl;

	return strcmp(v1, v2);
}

int main()
{
	// calls compare(const T&,const T&) with T bound to int
	compare(1, 0);     

	// calls compare(const T&,const T&) with T bound to string
	string s1 = "hi", s2 = "world";
	compare(s1, s2);

	// calls ordinary function 
	char *cp1 = "world", *cp2 = "hi";
	compare(cp1, cp2); 

	// calls compare(const T&,const T&) with T bound to const char*
	const char *ccp1 = "world", *ccp2 = "hi";
	compare(ccp1, ccp2); 

	// calls compare(U, U, V)
	vector<int> ivec1(10), ivec2(20);
	compare(ivec1.begin(), ivec1.end(), ivec2.begin());
	
	int ia1[] = {0,1,2,3,4,5,6,7,8,9};
	int ia2[] = {9,8,7,6,5,4,3,2,1,0};

	// calls compare(U, U, V)
	compare(ia1, ia1 + 9, ia2);
	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
#include <limits>

using std::cerr;
using std::cout;
using std::endl;

int main() 
{
    // max value if shorts are 8 bits
    short short_value = 32767; 
    short ival = 1;

    // this calculation overflows
    short_value += ival; 
    cout << "short_value: " << short_value << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "algs_preamble.h"

int main()
{
    string line;
    getline(cin, line);
    // find first element in a comma-separated list
    string::iterator comma = find(line.begin(), line.end(), ',');
    cout << string(line.begin(), comma) << endl;

    // find last element in a comma-separated list
    string::reverse_iterator rcomma = 
                           find(line.rbegin(), line.rend(), ',');

    // wrong: will generate the word in reverse order
    cout << string(line.rbegin(), rcomma) << endl;

    // ok: get a forward iterator and read to end of line
    cout << string(rcomma.base(), line.end()) << endl;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::ostream; using std::cout; using std::endl;
#include <string>

// class that has a pointer member that behaves like a plain pointer
class HasPtr {
public:
    friend ostream& operator<<(ostream&, const HasPtr&);
    // copy of the values we're given
    HasPtr(int *p, int i): ptr(p), val(i) { }

    // const members to return the value of the indicated data member
    int *get_ptr() const { return ptr; }     
    int get_int() const { return val; }

    // nonconst members to change the indicated data member
    void set_ptr(int *p) { ptr = p; }
    void set_int(int i) { val = i; }

    // return or change the value pointed to, so ok for const objects
    int get_ptr_val() const { return *ptr; } 
    void set_ptr_val(int val) const { *ptr = val; }

private:
    int *ptr;
    int val;
};

void show_copy_control()
{
    int i = 42;
    HasPtr p1(&i, 42);
    HasPtr p2 = p1;
    cout << p2.get_ptr_val() << endl;

    p1.set_ptr_val(0);
    cout << p2.get_ptr_val() << endl;
}

int main()
{
    show_copy_control();

    int obj = 0;

    HasPtr ptr1(&obj, 42);  // int* member points to obj, val is 42
    HasPtr ptr2(ptr1);      // int* member points to obj, val is 42

    cout << "(1) ptr1: " << ptr1 << endl << "ptr2: " << ptr2 << endl;

    ptr1.set_ptr_val(42); // sets object to which both ptr1 and ptr2 point
    ptr2.get_ptr_val();   // returns 42
    cout << "(2) ptr1: " << ptr1 << endl << "ptr2: " << ptr2 << endl;

    ptr1.set_int(0);   // changes val member only in ptr1
    ptr2.get_int();    // returns 42
    ptr1.get_int();    // returns 0

    cout << "(3) ptr1: " << ptr1 << endl << "ptr2: " << ptr2 << endl;

    int *ip = new int(42); // dynamically allocated int initialized to 42
    HasPtr ptr(ip, 10);    // HasPtr points to same object as ip does
    delete ip;             // object pointed to by ip is freed
    ptr.set_ptr_val(0); // disaster: The object to which HasPtr points was freed!

    // this statement or the previous one are likely to crash -- 
    // the object to which ptr points was deleted
    cout << "(4) ptr: " << ptr << endl; 

    return 0;
}

ostream& operator<<(ostream &os, const HasPtr &hp)
{
    cout << "*ptr: " << hp.get_ptr_val() << "\tval: " << hp.get_int() << endl;
    return os;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "io_preamble.h"

int main()
{
    cout << 10.0 << endl;         // prints 10
    cout << showpoint << 10.0     // prints 10.0000
         << noshowpoint << endl;  // revert to default handling of decimal point

    cout << 10.0 << endl;  // prints 10

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
int main()
{
    // a first, not very good, solution
    std::cout << "2 raised to the power of 10: ";
    std::cout << 2*2*2*2*2*2*2*2*2*2;
    std::cout << std::endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
int main()
{
    // local objects of type int
    int value = 2;
    int pow = 10;
    int result = 1;

    // repeat calculation of result until cnt is equal to pow
    for (int cnt = 0; cnt != pow; ++cnt)
        result *= value;   // result = result * value;

    std::cout << value 
              << " raised to the power of "
              << pow << ": \t"
              << result << std::endl;
    return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

int
pow( int val, int exp )
{
    int res;
    for ( res = 1; exp > 0; --exp )
          res = res * val;
    return res;

}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

/*
 * Note -- this function duplicates the functionality of the
 * 'pow' function in the <cmath> header.  However, it has an
 * important limitation: It does integer calculations and so
 * is more likely to overflow than the library version that 
 * does its computations in floating point.
 * 
 * As usual, it is better to use the library function than one
 * of our own.  The library defines six overloaded versions of
 * pow: pow(float, int), pow(float, float), pow(double, int),
 * pow(double, double), pow(long double, int) and pow(long double, long double)
*/
extern int power(int,int);

#include <iostream>
using std::cout; using std::endl;
#include <cmath>
using std::pow;

int main()
{
    cout << power(2, 10) << endl;
    cout << power(2, 16) << endl;
    // using our integer based power, this call likely to overflow
    cout << power(10, 10) << endl;  

    // use the library instead:  
    // Note we must change the first argument to be a double 
    // otherwise the call would be ambiguous and would not compile
    cout << pow(2.0, 10) << endl;
    cout << pow(2.0, 16) << endl;
    cout << pow(10.0, 10) << endl;  // should not overflow

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
extern int pow(int,int);

int main()
{
    int val = 2;
    int exp = 15;

    std::cout << "The Powers of 2" << std::endl;

    for (int cnt = 0; cnt <= exp; ++cnt)
    {
        std::cout << cnt << ":"
             << pow(val,cnt) << std::endl;
        ++cnt;
    }

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout;
using std::endl;

int main() 
{
    int ival = 
               6 + 3 * 4 / 2 + 2;
    cout << ival << endl;                      // prints 14

    // parentheses on this expression match default precedence/associativity
    cout << ((6 + ((3 * 4) / 2)) + 2) << endl; // prints 14

    // parentheses result in alternative groupings
    cout << (6 + 3) * (4 / 2 + 2) << endl;     // prints 36
    cout << ((6 + 3) * 4) / 2 + 2 << endl;     // prints 20
    cout << 6 + 3 * 4 / (2 + 2) << endl;       // prints 9


    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
#include <vector>
#include <string>
using std::cout;
using std::endl;
using std::vector;
using std::string;

enum open_modes {input = 1, output, append};

int main() 
{
    open_modes om = append;

    cout << input << " " << om << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
#include <vector>
#include <string>
using std::cout;
using std::endl;
using std::vector;
using std::string;

enum open_modes {input = 1, output, append};

int main() 
{
    open_modes om = append;

    vector<string> open_mode_table;
    open_mode_table.push_back("");  // no enumerator 0
    open_mode_table.push_back("input");
    open_mode_table.push_back("output");
    open_mode_table.push_back("append");

    cout << open_mode_table[input] << " "
         << open_mode_table[om]    << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>

using std::cout;  using std::endl;

int main()
{
    const size_t arr_sz = 5;
    int int_arr[arr_sz] = { 0, 1, 2, 3, 4 };
    // pbegin points to first element, pend points just after the last
    for (int *pbegin = int_arr, *pend = int_arr + arr_sz; 
              pbegin != pend; ++pbegin)
        cout << *pbegin << ' '; // print the current element
    cout << endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Query.h"
#include "TextQuery.h"
#include <set>
#include <algorithm>
#include <iostream>

using std::set;
using std::ostream;
using std::inserter; 
using std::set_difference; 
using std::set_union; 
using std::set_intersection;

// returns lines not in its operand's result set
set<TextQuery::line_no>
NotQuery::eval(const TextQuery& file) const
{
    // virtual call through the Query handle to eval
    set<TextQuery::line_no> has_val = query.eval(file);

    set<line_no> ret_lines;

    // for each line in the input file, check whether that line is in has_val
    // if not, add that line number to ret_lines
    for (TextQuery::line_no n = 0; n != file.size(); ++n)
        if (has_val.find(n) == has_val.end())
            ret_lines.insert(n);
    return ret_lines;
}

// returns intersection of its operands' result sets
set<TextQuery::line_no>
AndQuery::eval(const TextQuery& file) const
{
    // virtual calls through the Query handle to get result sets for the operands
    set<line_no> left = lhs.eval(file), 
                 right = rhs.eval(file);

    set<line_no> ret_lines;  // destination to hold results 

    // writes intersection of two ranges to a destination iterator
    // destination iterator in this call adds elements to ret
    set_intersection(left.begin(), left.end(), 
                  right.begin(), right.end(),
                  inserter(ret_lines, ret_lines.begin()));
    return ret_lines;
}

// returns union of its operands' result sets
set<TextQuery::line_no>
OrQuery::eval(const TextQuery& file) const
{
    // virtual calls through the Query handle to get result sets for the operands
    set<line_no> right = rhs.eval(file),
             ret_lines = lhs.eval(file);  // destination to hold results

    // inserts the lines from right that aren't already in ret_lines
    ret_lines.insert(right.begin(), right.end());

    return ret_lines;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#ifndef QUERY_H
#define QUERY_H
#include "TextQuery.h"
#include <string>
#include <set>
#include <iostream>
#include <fstream>

// private, abstract class acts as a base class for concrete query types
class Query_base {
    friend class Query;  
protected:
    typedef TextQuery::line_no line_no;
    virtual ~Query_base() { }
private:
    // eval returns the |set| of lines that this Query matches
    virtual std::set<line_no> 
        eval(const TextQuery&) const = 0; 
    // display prints the query
    virtual std::ostream& 
        display(std::ostream& = std::cout) const = 0;
};


// handle class to manage the Query_base inheritance hierarchy
class Query {
    // these operators need access to the Query_base* constructor
    friend Query operator~(const Query &);
    friend Query operator|(const Query&, const Query&);
    friend Query operator&(const Query&, const Query&);
public:
    Query(const std::string&);  // builds a new WordQuery

    // copy control to manage pointers and use counting
    Query(const Query &c): q(c.q), use(c.use) { ++*use; }
    ~Query() { decr_use(); }
    Query& operator=(const Query&);

    // interface functions: will call corresponding Query_base operations
    std::set<TextQuery::line_no> 
      eval(const TextQuery &t) const { return q->eval(t); }
    std::ostream &display(std::ostream &os) const
                            { return q->display(os); }
private:
    Query(Query_base *query): q(query), 
                              use(new std::size_t(1)) { }
    Query_base *q;
    std::size_t *use;
    void decr_use() 
    { if (--*use == 0) { delete q; delete use; } }
};

inline Query& Query::operator=(const Query &rhs)
{
    ++*rhs.use; 
    decr_use(); 
    q = rhs.q; 
    use = rhs.use; 
    return *this; 
}

inline std::ostream& 
operator<<(std::ostream &os, const Query &q)
{
    return q.display(os);
}

class WordQuery: public Query_base {
    friend class Query; // Query uses the WordQuery constructor
    WordQuery(const std::string &s): query_word(s) { }

    // concrete class: WordQuery defines all inherited pure virtual functions
    std::set<line_no> eval(const TextQuery &t) const
                      { return t.run_query(query_word); }
    std::ostream& display (std::ostream &os) const 
                          { return os << query_word; }
    std::string query_word;   // word for which to search 
};

inline
Query::Query(const std::string &s): q(new WordQuery(s)),
                               use(new std::size_t(1)) { }

class NotQuery: public Query_base {
    friend Query operator~(const Query &);
    NotQuery(Query q): query(q) { }

    // concrete class: NotQuery defines all inherited pure virtual functions
    std::set<line_no> eval(const TextQuery&) const;
    std::ostream& display(std::ostream &os) const
          { return os << "~(" << query << ")"; }
    const Query query;
};

class BinaryQuery: public Query_base {
protected:
    BinaryQuery(Query left, Query right, std::string op): 
          lhs(left), rhs(right), oper(op) { }

    // abstract class: BinaryQuery doesn't define eval 
    std::ostream& display(std::ostream &os) const
    { return os << "(" << lhs  << " " << oper << " " 
                       << rhs << ")"; }

    const Query lhs, rhs;   // right- and left-hand operands
    const std::string oper; // name of the operator
};
    
class AndQuery: public BinaryQuery {
    friend Query operator&(const Query&, const Query&);
    AndQuery(Query left, Query right): 
                        BinaryQuery(left, right, "&") { }

    // concrete class: AndQuery inherits display and defines remaining pure virtual
    std::set<line_no> eval(const TextQuery&) const;
};

class OrQuery: public BinaryQuery {
    friend Query operator|(const Query&, const Query&);
    OrQuery(Query left, Query right): 
                BinaryQuery(left, right, "|") { }

    // concrete class: OrQuery inherits display and defines remaining pure virtual
    std::set<line_no> eval(const TextQuery&) const;
};

inline Query operator&(const Query &lhs, const Query &rhs)
{
    return new AndQuery(lhs, rhs);
}

inline Query operator|(const Query &lhs, const Query &rhs)
{
    return new OrQuery(lhs, rhs);
}

inline Query operator~(const Query &oper)
{
    return new NotQuery(oper);
}

std::ifstream& open_file(std::ifstream&, const std::string&);
TextQuery build_textfile(const std::string&);
bool get_word(std::string&);
bool get_words(std::string&, std::string&);
void print_results(const std::set<TextQuery::line_no>&, const TextQuery&);

#endif
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "TextQuery.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>
#include <cstdlib>

using std::set;
using std::string;
using std::map;
using std::vector;
using std::cerr;
using std::cout;
using std::cin;
using std::ifstream;
using std::endl;

string make_plural(size_t, const string&, const string&);
ifstream& open_file(ifstream&, const string&);
void print_results(const set<TextQuery::line_no>& locs, 
                   const string& sought, const TextQuery &file)
{
    // if the word was found, then print count and all occurrences
    typedef set<TextQuery::line_no> line_nums; 
    line_nums::size_type size = locs.size();
    cout << "\n" << sought << " occurs "
         << size << " "
         << make_plural(size, "time", "s") << endl;

    // print each line in which the word appeared
    line_nums::const_iterator it = locs.begin();
    for ( ; it != locs.end(); ++it) {
        cout << "\t(line "
             // don't confound user with text lines starting at 0
             << (*it) + 1 << ") "
             << file.text_line(*it) << endl;
    }
}

// program takes single argument specifying the file to query
int main(int argc, char **argv)
{
    // open the file from which user will query words
    ifstream infile;
    if (argc < 2 || !open_file(infile, argv[1])) {
        cerr << "No input file!" << endl;
        return EXIT_FAILURE;
    }

    TextQuery tq;
    tq.read_file(infile);  // builds query map

    // iterate with the user: prompt for a word to find and print results
    // loop indefinitely; the loop exit is inside the while
    while (true) {
        cout << "enter word to look for, or q to quit: ";
        string s;
        cin >> s;

        // stop if hit eof on input or a 'q' is entered
        if (!cin || s == "q") break;

        // get the set of line numbers on which this word appears
        set<TextQuery::line_no> locs = tq.run_query(s);

        // print count and all occurrences, if any
        print_results(locs, s, tq);
     }
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <stdexcept>
#include <cstddef>
#include <list>
using std::list;
#include <vector>
using std::vector;
#include <iostream>
using std::cout; using std::endl;

class SmallInt {
public:
    SmallInt(int i = 0): val(i)
    { if (i < 0 || i > 255)
        throw std::out_of_range("Bad SmallInt initializer");
    }
    operator int() { return val; }
private:
    std::size_t val;
};

#include "MS_Queue.h"

int main()
{
    short a[4] = { 0, 3, 6, 9 };
    // instantiates Queue<int>::Queue(short *, short *)
    Queue<int> qi(a, a + 4);  // copies elements from a into qi
    cout << qi << endl;


    vector<int> vi(a, a + 4);
    // instantiates Queue<int>::assign(vector<int>::iterator,
    //                                 vector<int>::iterator)
    qi.assign(vi.begin(), vi.end());
    cout << qi << endl;

    list<SmallInt> li; // give |li| some values
    li.assign(vi.begin(), vi.end());
    qi.assign(li.begin(), li.end());
    cout << qi << endl;

}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#ifndef QUEUE_H
#define QUEUE_H

//NB: if change the interface in this file, also update Queue_interface.h!!!!
//    and int-Queue.h

#include <iostream>

// declaration that Queue is a template needed for friend declaration in QueueItem
template <class Type> class Queue;
// function template declaration must precede friend declaration in QueueItem
template <class T> 
std::ostream& operator<<(std::ostream&, const Queue<T>&);

template <class Type> class QueueItem {
    friend class Queue<Type>;
    // . . .
    // needs access to item and next
    friend std::ostream& 
    operator<< <Type> (std::ostream&, const Queue<Type>&);
    // . . .

// private class: no public section
    QueueItem(const Type &t): item(t), next(0) { }
    Type item;           // value stored in this element
    QueueItem *next;     // pointer to next element in the Queue
};


template <class Type> class Queue {
    // needs access to head 
    friend std::ostream& 
    operator<< <Type> (std::ostream&, const Queue<Type>&);
public:
    // empty Queue
    Queue(): head(0), tail(0) { }
public:
    // construct a Queue from a pair of iterators on some sequence
    template <class It> 
    Queue(It beg, It end): 
          head(0), tail(0) { copy_elems(beg, end); }
    // . . .

    // copy control to manage pointers to QueueItems in the Queue
    Queue(const Queue &Q): head(0), tail(0) 
                                  { copy_elems(Q); }
    Queue& operator=(const Queue&);
    ~Queue() { destroy(); }

    // replace current Queue by contents delimited by a pair of iterators
    template <class Iter> void assign(Iter, Iter);
    // rest of Queue class as before

    // return element from head of Queue
    // unchecked operation: front on an empty Queue is undefined
    Type& front()             { return head->item; }
    const Type &front() const { return head->item; }

    void push(const Type &);      // add element to back of Queue
    void pop();                   // remove element from head of Queue

    bool empty() const {          // true if no elements in the Queue
        return head == 0;
    }
private:
    QueueItem<Type> *head;         // pointer to first element in Queue
    QueueItem<Type> *tail;         // pointer to last element in Queue

    // utility functions used by copy constructor, assignment, and destructor
    void destroy();                // delete all the elements
    void copy_elems(const Queue&); // copy elements from parameter
private:
    // version of copy to be used by assign to copy elements from iterator range
    template <class Iter> void copy_elems(Iter, Iter); 
};

// Inclusion Compilation Model: include member function definitions as well
#include "Queue.cc"

#endif

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;

#include "MS_Queue.h"

Queue<int> queue1;

int main() 
{
    for (int ix = 0; ix != 1024; ++ix)
        queue1.push(ix);
    
    for (int ix = 0; ix != 1024; ++ix) {
        int i = queue1.front();  // check next item
        if (i != ix)
             cout << "Something's wrong! i = " << i
                  << " ix = " << ix << endl;
        queue1.pop();   // and remove it
    }

    if (!queue1.empty())
        cout << "Queue is not empty but should be!" << endl;
    else
        cout << "OK, queue empty again" << endl;


    for (int ix = 0; ix != 1024; ++ix)
        queue1.push(ix);

    Queue<int> queue2(queue1);  // use copy constructor

    for (int ix = 0; ix != 1024; ++ix) {
        int i = queue2.front();   // check next item
        if (i != ix)
             cout << "Something's wrong! i = " << i
                  << " ix = " << ix << endl;
        queue2.pop();  // and remove it
    }

    if (!queue2.empty())
        cout << "queue2 is not empty but should be!" << endl;
    else
        cout << "OK, queue2 empty again" << endl;

    queue2 = queue1;  // use asssignment operator
    for (int ix = 0; ix != 1024; ++ix) {
        int i = queue2.front();
        if (i != ix)
             cout << "Something's wrong! i = " << i
                  << " ix = " << ix << endl;
        queue2.pop();
    }

    if (!queue2.empty())
        cout << "queue2 is not empty but should be!" << endl;
    else
        cout << "OK, queue2 empty again" << endl;
    
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;
#include "MS_Queue.h"

int main()
{
    // the class Queue<int> is instantiated
    // new expression requires that Queue<int> be defined
    Queue<int> *p_qi = new Queue<int>;

    int ival;
    for (ival = 0; ival != 10; ++ival)
        // the member function push() is instantiated
        p_qi->push(ival);

    int err_cnt = 0;
    for (ival = 0; ival != 10; ++ival) {
        // the member function front is instantiated
        int qval = p_qi->front();

        if (ival != qval) err_cnt++;
        p_qi->pop();                // instantiates pop
    }

    if (!err_cnt)
        cout << "!! queue executed ok" << endl;
    else 
        cout << "?? " << err_cnt << " Queue errors" << endl;

    delete p_qi;   // deletes elements in the queue

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;
#include "MS_Queue.h"

int main() {
    Queue<int> qi;
    // instantiates ostream& operator<<(ostream&, const Queue<int>&)
    cout << qi << endl;

    int ival;
    for (ival = 0; ival != 10; ++ival)
        qi.push(ival);   // instantiates Queue<int>::push(int)
    cout << qi << endl;

    for (ival = 0; ival != 10; ++ival) {
        qi.pop();        // instantiates Queue<int>::pop()
    }

    cout << qi << endl;

    return 0;
}    // qi goes out of scope, elements in qi deleted automatically
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
//NB: if change the Queue interface must also change this file!

template <class Type> class Queue {
public:
    Queue();                  // default constructor

    Type &front();            // return element from head of Queue
    const Type &front() const;

    void push(const Type &);  // add element to back of Queue
    void pop();               // remove element from head of Queue

    bool empty() const;       // true if no elements in the Queue
private:
    // . . .
};
#endif

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "MS_Queue.h"

#include <stdexcept>
using std::out_of_range;
#include <iostream>
using std::ostream; 
#include <cstring>
using std::strlen; using std::strncpy;

void Queue<const char*>::push(const char *const &val)
{
    // allocate a new character array and copy characters from val
    char* new_item = new char[strlen(val) + 1];
    strncpy(new_item, val, strlen(val) + 1);

    // store pointer to newly allocated and initialized element
    QueueItem<const char*> *pt =
        new QueueItem<const char*>(new_item); 

    // put item onto existing queue
    if (empty())
        head = tail = pt;   // queue has only one element
    else {
        tail->next = pt;    // add new element to end of queue
        tail = pt;
    }
}

void Queue<const char*>::pop()
{
    // remember head so we can delete it
    QueueItem<const char*> *p = head; 
    delete head->item;    // delete the array allocated in push
    head = head->next;    // head now points to next element
    delete p;             // delete old head element
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
int main()
{
    int i, &ri = i;
    i = 5; ri = 10;
    std::cout << i << " " << ri << std::endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
#include <string>
using std::string;
using std::cout;
using std::endl;

char &get_val(string &str, string::size_type ix) 
{
    return str[ix];
}

int main() 
{
    string s("a value");
    cout << s << endl;   // prints a value

    get_val(s, 0) = 'A'; // changes s[0] to A
    
    cout << s << endl;   // prints A value
    return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;

int &get(int *arry, int index) { return arry[index]; }

int main() {
    int ia[10];
    for (int i = 0; i != 10; ++i)
        get(ia, i) = 0;
    for (int i = 0; i != 10; ++i)
        get(ia, i) = i;
    for (int i = 0; i != 10; ++i)
        cout << ia[i] << " ";
    cout << endl;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "algs_preamble.h"

int main()
{
    vector<int> vec;
    for (vector<int>::size_type i = 0; i != 10; ++i)
        vec.push_back(i);  // elements are 0,1,2,...9

    // reverse iterator of vector from back to front
    vector<int>::reverse_iterator r_iter;
    for (r_iter = vec.rbegin(); // binds r_iter to last element
         r_iter != vec.rend();  // rend refers 1 before 1st element
         ++r_iter)              // decrements iterator one element
        cout << *r_iter << endl;// prints 9,8,7,...0
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "algs_preamble.h"

int main()
{
	vector<int> vec;
	istream_iterator<int> in(cin), eof;
	copy (in, eof, back_inserter(vec));

	// sorts vec in ``normal'' order
	sort(vec.begin(), vec.end());

	vector<int> vec2(vec);

	// sorts in reverse: puts smallest element at the end of vec
	sort(vec.rbegin(), vec.rend());
	ostream_iterator<int> out(cout, " ");

	copy(vec.begin(), vec.end(), out);    // reverse sort
	cout << endl;
	copy(vec2.begin(), vec2.end(), out);  // forward sort
	cout << endl;

	// it refers to the first element in vec
	vector<int>::iterator it = vec.begin();

	// rev_it refers one before the first element in vec
	vector<int>::reverse_iterator rev_it(it);
	cout << *(rev_it.base()) << endl; // prints first element in vec

	// ways to print value at beginning of vec through it and rev_it
	cout << *it << endl;
	cout << *(rev_it.base()) << endl;  
	cout << *(rev_it - 1) << endl;
	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 *
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 *
 *
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 *
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address:
 *
 *  Pearson Education, Inc.
 *  Rights and Contracts Department
 *  75 Arlington Street, Suite 300
 *  Boston, MA 02216
 *  Fax: (617) 848-7047
*/

#include <iostream>
using std::cout;
using std::endl;

// recursive version greatest common divisor program
int rgcd(int v1, int v2)
{
    if (v2 != 0)                 // we're done once v2 gets to zero
        return rgcd(v2, v1 % v2); // recurse, reducing v2 on each call
    return v1;
}

int main()
{
    cout << rgcd(15, 123) << endl;
    const int i = 8, j = 4;
    int k = rgcd(i, 1);   // ok: k initialized to 3
    cout << k;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Sales_item.h"
#include <iostream>
using std::istream; using std::ostream;

// assumes that both objects refer to the same isbn
Sales_item& Sales_item::operator+=(const Sales_item& rhs) 
{
    units_sold += rhs.units_sold; 
    revenue += rhs.revenue; 
    return *this;
}

// assumes that both objects refer to the same isbn
Sales_item 
operator+(const Sales_item& lhs, const Sales_item& rhs) 
{
    Sales_item ret(lhs);  // copy lhs into a local object that we'll return
    ret += rhs;           // add in the contents of rhs 
    return ret;           // return ret by value
}

istream& 
operator>>(istream& in, Sales_item& s)
{
    double price;
    in >> s.isbn >> s.units_sold >> price;
    // check that the inputs succeeded
    if (in)
        s.revenue = s.units_sold * price;
    else 
        s = Sales_item();  // input failed: reset object to default state
    return in;
}

ostream& 
operator<<(ostream& out, const Sales_item& s)
{
    out << s.isbn << "\t" << s.units_sold << "\t" 
        << s.revenue << "\t" <<  s.avg_price();
    return out;
}

double Sales_item::avg_price() const
{
    if (units_sold) 
        return revenue/units_sold; 
    else 
        return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#ifndef SALESITEM_H
#define SALESITEM_H

// Definition of Sales_item class and related functions goes here


#include <iostream>
#include <string>

class Sales_item {
friend bool operator==(const Sales_item&, const Sales_item&);
// other members as before
public:
    // added constructors to initialize from a string or an istream
    Sales_item(const std::string &book):
              isbn(book), units_sold(0), revenue(0.0) { }
    Sales_item(std::istream &is) { is >> *this; }
    friend std::istream& operator>>(std::istream&, Sales_item&);
    friend std::ostream& operator<<(std::ostream&, const Sales_item&);
public:
    // operations on Sales_item objects
    // member binary operator: left-hand operand bound to implicit this pointer
    Sales_item& operator+=(const Sales_item&);
    // other members as before
    
public:
    // operations on Sales_item objects
    double avg_price() const;
    bool same_isbn(const Sales_item &rhs) const
        { return isbn == rhs.isbn; }
    // default constructor needed to initialize members of built-in type
    Sales_item(): units_sold(0), revenue(0.0) { }
// private members as before
private:
    std::string isbn;
    unsigned units_sold;
    double revenue;

};


// nonmember binary operator: must declare a parameter for each operand
Sales_item operator+(const Sales_item&, const Sales_item&);

inline bool 
operator==(const Sales_item &lhs, const Sales_item &rhs)
{
    // must be made a friend of Sales_item
    return lhs.units_sold == rhs.units_sold &&
           lhs.revenue == rhs.revenue &&
	   lhs.same_isbn(rhs);
}

inline bool 
operator!=(const Sales_item &lhs, const Sales_item &rhs)
{
    return !(lhs == rhs); // != defined in terms of operator==
}

using std::istream; using std::ostream;

// assumes that both objects refer to the same isbn
inline
Sales_item& Sales_item::operator+=(const Sales_item& rhs) 
{
    units_sold += rhs.units_sold; 
    revenue += rhs.revenue; 
    return *this;
}

// assumes that both objects refer to the same isbn
inline
Sales_item 
operator+(const Sales_item& lhs, const Sales_item& rhs) 
{
    Sales_item ret(lhs);  // copy lhs into a local object that we'll return
    ret += rhs;           // add in the contents of rhs 
    return ret;           // return ret by value
}

inline
istream& 
operator>>(istream& in, Sales_item& s)
{
    double price;
    in >> s.isbn >> s.units_sold >> price;
    // check that the inputs succeeded
    if (in)
        s.revenue = s.units_sold * price;
    else 
        s = Sales_item();  // input failed: reset object to default state
    return in;
}

inline
ostream& 
operator<<(ostream& out, const Sales_item& s)
{
    out << s.isbn << "\t" << s.units_sold << "\t" 
        << s.revenue << "\t" <<  s.avg_price();
    return out;
}

inline
double Sales_item::avg_price() const
{
    if (units_sold) 
        return revenue/units_sold; 
    else 
        return 0;
}


#endif
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "io_preamble.h"

int main() 
{
    cout << sqrt(2.0) << '\n' << endl;

    cout << "scientific: " << scientific << sqrt(2.0) << '\n'
         << "fixed decimal: " << fixed << sqrt(2.0) << "\n\n";

    cout << uppercase
         << "scientific: " << scientific << sqrt(2.0) << '\n'
         << "fixed decimal: " << fixed << sqrt(2.0) << endl
         << nouppercase;

    // reset to default handling for notation
    cout.unsetf(ostream::floatfield);
    cout << '\n' << sqrt(2.0) << endl;  

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;

struct Base {
    Base(): mem(0) { }
protected:
    int mem;
};

struct Derived : Base {
    Derived(int i): mem(i) { }     // initializes Derived::mem
    int get_mem() { return mem; }  // returns Derived::mem
    int get_base_mem() { return Base::mem; }
protected:
    int mem;   // hides mem in the base
};

int main()
{
    Derived d(42);
    cout << d.get_mem() << endl;       // prints 42
    cout << d.get_base_mem() << endl;  // prints 0
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
#include <string>

std::string s1 = "hello";  // s1 has global scope

int main()
{
	std::string s2 = "world"; // s2 has local scope

	// uses global s1; prints ``hello world''
	std::cout << s1 << " " << s2 << std::endl;   

	int s1 = 42; // s1 is local and hides global s1

	// uses local s1; prints ``42 world''
	std::cout << s1 << " " <<  s2 << std::endl;  
	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
#include <string>


int main()
{
    std::string s = "hello";   // s is local to main
    {   // new local scope
        std::string s = "bye"; // new definition of s hides outer s

        // uses inner definition of s; prints ``bye''
        std::cout << s << std::endl;  
    }

    // uses s from the outer scope; prints ``hello''
    std::cout << s << std::endl;   
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "algs_preamble.h"
#include <ctime>

int main() {
    vector<int> scores;

    // populate with random values
    for (vector<int>::size_type i =  0; i != 10; ++i)
	    scores.push_back(rand());
    // print what we got
    for (vector<int>::size_type i =  0; i != 10; ++i)
	    cout << scores[i] << " ";
    cout << endl;

    // put top 3 scorse at the front of the vector
    partial_sort(scores.begin(), 
                 scores.begin() + 3, scores.end());

    // use nth_element to find the median score
    nth_element(scores.begin(), 
            scores.begin() + scores.size()/2, scores.end());

    cout << "median value " << *(scores.begin() + scores.size()/2) << endl;
    for (vector<int>::size_type i =  0; i != 10; ++i)
	    cout << scores[i] << " ";
    cout << endl;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Screen.h"
#include <cstddef>
#include <memory>
using std::allocator;

#include <iostream>
using std::cout; using std::endl;

void Screen::do_display(std::ostream& os) const
{
    os << contents << '\n';
}

Screen& Screen::set(char c)
{
    contents[cursor] = c;
    return *this;
}

//utility function to do the work of moving the cursor
void Screen::do_move(index r, index c) const
{
    index row = r * width; // row location
    cursor = row + c;  // ok to change cursor -- it is a mutable member
}

Screen& Screen::set(index r, index c, char ch)
{
    index row = r * width; // row location
    contents[row + c] = ch;
    return *this;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
class Screen {
public:
    typedef std::string::size_type index;
    // interface member functions
    // constructor: build screen of given size containing all blanks
    Screen(index ht = 0, index wd = 0): 
                  contents(ht * wd, ' '), cursor(0), 
                  height(ht), width(wd) { }

    // return character at the cursor or at a given position
    char get() const { return contents[cursor]; }
    char get(index ht, index wd) const;
    // remaining members
private:
    std::string contents;
    index cursor;
    index height, width;
};


/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "screen3.h"
#include <iostream>
using std::cout; using std::endl;
#include <string>
using std::string;

int main()
{
	Screen myScreen(5,3);

	// move cursor to given position, and set that character 
	myScreen.move(4,0).set('#');
	// move cursor to given position, set that character and display the screen
	myScreen.move(4,0).set('#').display(cout);
	cout << endl;
	const Screen blank(5, 3);
	myScreen.set('#').display(cout);  // calls nonconst version
	cout << endl;
	blank.display(cout);              // calls const version
	cout << endl;

	myScreen.move(4,0);
	myScreen.set('#');
	myScreen.display(cout);

	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
#include <iostream>

class Screen {
public:
    // interface member functions
    typedef std::string::size_type index;
    char get() const { return contents[cursor]; }
    inline char get(index ht, index wd) const;
    Screen(index ht = 0, index wd = 0): 
       contents(ht * wd, ' '), cursor(0), height(ht), width(wd) { }
    Screen& move(index r, index c);
    Screen& set(char);
    Screen& set(index, index, char);
    // display overloaded on whether the object is const or not
    Screen& display(std::ostream &os) 
                  { do_display(os); return *this; }
    const Screen& display(std::ostream &os) const
                  { do_display(os); return *this; }
private:
     // single function to do the work of displaying a Screen,
     // will be called by the display operations
     void do_display(std::ostream &os) const
                       { os << contents; }
    // as before
private:
    std::string contents;
    index cursor;
    index height, width;
    // other members as before
};

Screen& Screen::set(char c) 
{ 
    contents[cursor] = c; 
    return *this; 
}

Screen& Screen::move(index r, index c)
{
    index row = r * width; // row location
    cursor = row + c;
    return *this;
}

Screen& Screen::set(index r, index c, char ch)
{
    index row = r * width; // row location
    contents[row + c] = ch;
    return *this;
}

char Screen::get(index r, index c) const
{
    index row = r * width; // row location
    return contents[row + c];
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
#include <iostream>

class Screen {
public:
// interface member functions
    typedef std::string::size_type index;
    char get() const { return contents[cursor]; }
    inline char get(index ht, index wd) const;
    Screen(index ht = 0, index wd = 0): 
       contents(ht * wd, ' '), cursor(0), height(ht), width(wd) { }
    Screen& move(index r, index c);
    Screen& set(char);
    Screen& set(index, index, char);
    const Screen& display(std::ostream& os) const;
    // as before
private:
    index cursor;
    std::string contents;
    int height, width;
};

const Screen& Screen::display(std::ostream& os) const
{	
	os << contents << '\n';
	return *this; 
}

Screen& Screen::set(char c) 
{ 
	contents[cursor] = c; 
	return *this; 
}

Screen& Screen::move(index r, index c)
{
	index row = r * width; // row location
	cursor = row + c;
	return *this;
}

Screen& Screen::set(index r, index c, char ch)
{
	index row = r * width; // row location
	contents[row + c] = ch;
	return *this;
}

char Screen::get(index r, index c) const
{
	index row = r * width; // row location
	return contents[row + c];
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#ifndef SCREEN5_H
#define SCREEN5_H
#include <string>
#include <iostream>

class Screen {
public:
// interface member functions
    typedef std::string::size_type index;
    char get() const { return contents[cursor]; }
    inline char get(index ht, index wd) const;
    Screen(index ht = 0, index wd = 0): 
       contents(ht * wd, ' '), cursor(0), height(ht), width(wd), access_ctr(0) { }
    Screen& move(index r, index c) 
              { do_move(r, c); return *this; }
    const Screen& move(index r, index c) const 
              { do_move(r, c); return *this; }
private:
    void do_move(index r, index c) const;
    void do_display(std::ostream &os) const;
public:
    Screen& set(char);
    Screen& set(index, index, char);
    const Screen& display(std::ostream& os) const;
    // as before
private:
    mutable size_t access_ctr; // may change in a const members
    // other data members as before
    std::string contents;
    index cursor;  
    index height, width;
};

void Screen::do_display(std::ostream& os) const
{	
	++access_ctr;    // keep count of calls to any member function
	os << contents;
}

Screen& Screen::set(char c) 
{ 
	contents[cursor] = c; 
	return *this; 
}

Screen& Screen::set(index r, index c, char ch)
{
	index row = r * width; // row location
	contents[row + c] = ch;
	return *this;
}

char Screen::get(index r, index c) const
{
	index row = r * width; // row location
	return contents[row + c];
}
#endif
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <vector>
using std::vector;
#include <iostream>
using std::cout; using std::endl;

#include "Screen.h"

int main()
{
    Screen myScreen(3,4,'h');
    Screen *ps = new Screen(3,4,'e');
    myScreen.display(cout);
    cout << endl;
    ps->display(cout);
    vector<Screen*> vp;
    for (int i = 0; i != 5; ++i)
        vp.push_back(new Screen(1,3, 'a'));
    delete ps;
    for (int i = 0; i != 5; ++i)
        delete vp[i];
    for (int i = 0; i != 5; ++i)
        vp.push_back(new Screen(1,3, 'a'));
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <cstddef>
using std::size_t;
#include "screen5.h"

//private class for use by ScreenPtr only
class ScrPtr {
    friend class ScreenPtr;
    Screen *sp;
    size_t use;
    ScrPtr(Screen *p): sp(p), use(1) { }
    ~ScrPtr() { delete sp; }
};

class ScreenPtr {
public:
    // no default constructor: ScreenPtrs must be bound to an object
    ScreenPtr(Screen *p): ptr(new ScrPtr(p)) { }

    // copy members and increment the use count
    ScreenPtr(const ScreenPtr &orig):
       ptr(orig.ptr) { ++ptr->use; }
    ScreenPtr& operator=(const ScreenPtr&);

    // if use count goes to zero, delete the ScrPtr object
    ~ScreenPtr() { if (--ptr->use == 0) delete ptr; } 

    // constructor and copy control members as before
    Screen &operator*() { return *ptr->sp; }
    Screen *operator->() { return ptr->sp; }

    const Screen &operator*() const { return *ptr->sp; }
    const Screen *operator->() const { return ptr->sp; }
private:
    ScrPtr *ptr;        // points to use-counted ScrPtr class
};

ScreenPtr& ScreenPtr::operator=(const ScreenPtr &rhs)
{
    ++rhs.ptr->use;     // increment use count on rhs first
    if (--ptr->use == 0)
         delete ptr;    // if use count goes to 0 on this object, delete it
    ptr = rhs.ptr;      // copy the ScrPtr object
    return *this;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "io_preamble.h"

int main()
{
    // open for input and output and pre-position file pointers to end of file
    fstream inOut("copyOut", 
                   fstream::binary | fstream::ate | fstream::in | fstream::out);
    if (!inOut) {
        cerr << "Unable to open file!" << endl;
        return EXIT_FAILURE;
    } 

    // inOut is opened in  ate mode, so it starts out positioned at the end,
    // which we must remember as it is the original end-of-file position
    ifstream::pos_type end_mark = inOut.tellg();

    inOut.seekg(0, fstream::beg); // reposition to start of the file

    int cnt = 0;                  // accumulator for byte count
    string line;                  // hold each line of input

    // while we haven't hit an error and are still reading the original data 
    // and successfully read another line from the file
    while (inOut.good() && inOut.tellg() != end_mark
                 && getline(inOut, line))
    {
        cnt += line.size() + 1;   // add 1 to account for the newline

	// remember current read marker
	ifstream::pos_type mark = inOut.tellg();  
        inOut.seekp(0, fstream::end);// set write marker to end
        inOut << cnt;             // write the accumulated length
        // print separator if this is not the last line
        if (mark != end_mark) inOut << " "; 
        inOut.seekg(mark);        // restore read position
    }
    inOut.clear();                // clear flags in case we hit an error
    inOut.seekp(0, fstream::end); // seek to end
    inOut << "\n";                // write a newline at end of file

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
#include <vector>
#include <list>
#include <deque>
#include <stack>
#include <queue>
#include <iostream>
#include <algorithm>

using std::vector; using std::list; using std::string; using std::deque;
using std::stack; using std::queue; using std::priority_queue;
using std::find; using std::cin; using std::cout; using std::endl;
using std::cerr;
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "assoc_preamble.h"
#include "seq_preamble.h"
#include <fstream>
using std::ifstream;

void restricted_wc(ifstream &remove_file,
                   map<string, int> &word_count)
{
    set<string> excluded;   // set to hold words we'll ignore
    string remove_word;
    while (remove_file >> remove_word)
        excluded.insert(remove_word);

    // print contents of the word exclusion set
    for (set<string>::iterator set_it = excluded.begin();
                               set_it != excluded.end();
                               ++set_it)
         cout << *set_it << endl;

    // read input and keep a count for words that aren't in the exclusion set
    string word;
    while (cin >> word)
       // increment counter only if the word is not in excluded
       if (!excluded.count(word))
            ++word_count[word];
}

// call with the name of the exclusion set file
int main(int argc, char **argv) 
{
    ifstream remove_file;
    open_file(remove_file, argv[1]);  // file of words to ignore
    map<string, int> word_count;
    restricted_wc(remove_file, word_count);
    // get iterator positioned on the first element
    map<string, int>::iterator map_it = word_count.begin();
    // for each element in the map
    while (map_it != word_count.end()) {
        // print the element key, value pairs
        cout << map_it->first << " occurs "
             << map_it->second << " times" << endl;
        ++map_it;  // increment iterator to denote the next element
    }
    return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "io_preamble.h"

int main()
{
    int i = -16;
    double d = 3.14159;

    // pad first column to use minimum of 12 positions in the output
    cout << "i: " << setw(12) << i << "next col" << '\n'
         << "d: " << setw(12) << d << "next col" << '\n';

    // pad first column and left-justify all columns
    cout << left 
         << "i: " << setw(12) << i << "next col" << '\n'
         << "d: " << setw(12) << d << "next col" << '\n'
         << right;           // restore normal justification

    // pad first column and right-justify all columns
    cout << right
         << "i: " << setw(12) << i << "next col" << '\n'
         << "d: " << setw(12) << d << "next col" << '\n';

    // pad first column but put the padding internal to the field
    cout << internal
         << "i: " << setw(12) << i << "next col" << '\n'
         << "d: " << setw(12) << d << "next col" << '\n';

    // pad first column, using # as the pad character
    cout << setfill('#')
         << "i: " << setw(12) << i << "next col" << '\n'
         << "d: " << setw(12) << d << "next col" << '\n'
         << setfill(' ');   // restore normal pad character

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;
int main()
{
    int i = 15, j = 20;
    cout << (i < j ? i : j);  // ok: prints larger of i and j
    cout << endl;
    cout << (i < j) ? i : j;  // prints 1 or 0!
    cout << endl;


    cout << (i < j); // prints 1 or 0
    cout ? i : j;    // test cout and then evaluate i or j
                     // depending on whether cout evaluates to true or false
    cout << endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout;
using std::endl;

#include <cstddef>
int main()
{
    int ia[] = { 0, 1, 2 };

    // sizeof returns size of the entire array
    size_t array_size = sizeof ia;

    // sizeof returns size of the pointer, not size of object to which it points
    int *pi = ia;
    size_t ptr_size = sizeof(pi);

    //calculate # of elements in the array
    size_t element_cnt = array_size / sizeof(int);

    cout << "array_size: " << array_size 
         << "\nptr_size: " << ptr_size 
         << "\nelement_cnt: " << element_cnt << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
#include "Sales_item.h"
using std::cout;
using std::endl;


int main() 
{
    Sales_item item, *p;

    // three ways to obtain size required to hold an object of type Sales_item
    sizeof(Sales_item); // size required to hold an object of type Sales_item 
    sizeof item; // size of item's type, e.g., sizeof(Sales_item)
    sizeof *p;   // size of type to which p points, e.g., sizeof(Sales_item)

    cout << "short: " << sizeof(short) << "\n"
         << "short[3]: " << sizeof(short[3]) << "\n"
         << "short*: " << sizeof(short*) << "\n"
         << "short&: " << sizeof(short&) << endl;

    cout << endl; // to separate output

    cout << "Sales_item: " << sizeof(Sales_item) << "\n"
         << "Sales_item[3]: " << sizeof(Sales_item[3]) << "\n"
         << "Sales_item*: " << sizeof(Sales_item*) << "\n"
         << "Sales_item&: " << sizeof(Sales_item&) << endl;

// Differentiate use of sizeof array and sizeof on a pointer:
// First output statement will print 10; second prints 1
// The first statement divides size of entire array by size of an element
// The second divides size of a pointer by the size of an int
{
    int x[10];   int *p = x;

    cout << sizeof(x)/sizeof(*x) << endl;
    cout << sizeof(p)/sizeof(*p) << endl;
}
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "io_preamble.h"

int main()
{
    char ch;
    while (cin >> ch)
	    cout << ch;
    cout << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::ostream; using std::cout; using std::endl;
#include <string>
#include <cstddef>
using std::size_t;
/* smart pointer class: takes ownership of the dynamically allocated
 *                      object to which it is bound 
 * User code must dynamically allocate an object to initialize a HasPtr
 * and must not delete that object; the HasPtr class will delete it
*/
//private class for use by HasPtr only
class U_Ptr {
	friend class HasPtr;
	int *ip;
	size_t use;
	U_Ptr(int *p): ip(p), use(1) { }
	~U_Ptr() { delete ip; }
};

class HasPtr {
public:
    // HasPtr owns the pointer; p must have been dynamically allocated
    HasPtr(int *p, int i): ptr(new U_Ptr(p)), val(i) { }

    // copy members and increment the use count
    HasPtr(const HasPtr &orig):
       ptr(orig.ptr), val(orig.val) { ++ptr->use; }
    HasPtr& operator=(const HasPtr&);

    // if use count goes to zero, delete the U_Ptr object
    ~HasPtr() { if (--ptr->use == 0) delete ptr; } 

    friend ostream& operator<<(ostream&, const HasPtr&);
    // copy control and constructors as before

    // accessors must change to fetch value from U_Ptr object
    int *get_ptr() const { return ptr->ip; } 
    int get_int() const { return val; }

    // change the appropriate data member
    void set_ptr(int *p) { ptr->ip = p; }
    void set_int(int i) { val = i; }

    // return or change the value pointed to, so ok for const objects
    // Note: *ptr->ip is equivalent to *(ptr->ip)
    int get_ptr_val() const { return *ptr->ip; } 
    void set_ptr_val(int i) { *ptr->ip = i; }
private:
    U_Ptr *ptr;        // points to use-counted U_Ptr class
    int val;
};

HasPtr& HasPtr::operator=(const HasPtr &rhs)
{
    ++rhs.ptr->use;     // increment use count on rhs first
    if (--ptr->use == 0)
         delete ptr;    // if use count goes to 0 on this object, delete it
    ptr = rhs.ptr;      // copy the U_Ptr object
    val = rhs.val;      // copy the int member
    return *this;
}

ostream& operator<<(ostream &os, const HasPtr &hp)
{
    os << "*ptr: " << hp.get_ptr_val() << "\tval: " << hp.get_int() << endl;
    return os;
}

int main()
{
    int obj = 0;

    HasPtr ptr1(&obj, 42);
    HasPtr ptr2(ptr1);
    cout << "(1) ptr1: " << ptr1 << endl << "ptr2: " << ptr2 << endl;

    ptr1.set_ptr_val(42); // sets object to which both ptr1 and ptr2 point
    ptr2.get_ptr_val();   // returns 42

    cout << "(2) ptr1: " << ptr1 << endl << "ptr2: " << ptr2 << endl;

    ptr1.set_int(0);   // changes s member only in ptr1
    ptr2.get_int();    // returns 42
    ptr1.get_int();    // returns 0

    cout << "(3) ptr1: " << ptr1 << endl << "ptr2: " << ptr2 << endl;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "tmpl_preamble.h"

// generic template definition
template <class T> 
T max(T t1, T t2)
{
    return (t1 > t2 ? t1 : t2);
}

// char* explicit specialization:
// overrides the generic template definition
template<>
char* max<char*>(char* s1, char* s2)
{
    return (strcmp(s1, s2) > 0 ? s1 : s2);
}

int main()
{
    cout << max(120, 42) << endl;       // calls generic version
    cout << max("hi", "world") << endl; // calls specialized char* version
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "io_preamble.h"

void f2()
{

    ostringstream format_message;
    format_message << "val1: " << 512 << "\n"
                   << "val2: " << 1024 << "\n";
    istringstream input_istring(format_message.str());
    int val1 = 0;
    int val2 = 0;
    input_istring >> val1 >> val2;
    input_istring.clear();
    string dump;
    input_istring >> dump >> val1 >> dump >> val2;
    cout << val1 <<  " " << val2 << endl;
}

int main()
{
     int val1 = 512, val2 = 1024;
     ostringstream format_message;

     //  ok: converts values to a string representation
     format_message << "val1: " << val1 << "\n"
                    << "val2: " << val2 << "\n";
    cout << format_message.str() << endl;

{
    // str member obtains the string associated with a stringstream
    istringstream input_istring(format_message.str());

    string dump;  // place to dump the labels from the formatted message

    // extracts the stored ascii values, converting back to arithmetic types
    input_istring >> dump >> val1 >> dump >> val2;

    cout << val1 << " " << val2 << endl; // prints 512 1024
}

    f2();  // call f2, which prints val1 and val2 again

{
    istringstream input_istring;   // empty istringstream
    // bind input_istring to string held in format_message
    input_istring.str(format_message.str());  
}
    return 0;
}

// Sample code to use getline and istringstream to read a line of text
// and then process the line a word at at time
void f3()
{
    string line, word;    // will hold a line and word from input, respectively

    while (getline(cin, line)) {    // read a line from the input into line
        // do per-line processing
        istringstream stream(line); // bind to stream to the line we read
        while (stream >> word) {    // read a word from line 
            // do per-word processing
        }
    }
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <deque>
#include <stack>
#include <iostream>

using std::deque;
using std::stack; using std::cout; using std::cerr; using std::endl;

int main()
{
    // number of elements we'll put in our stack
    const stack<int>::size_type stk_size = 10;
    stack<int> intStack;  // empty stack

    // fill up the stack
    int ix = 0;
    while (intStack.size() != stk_size)
        // use postfix increment; want to push old value onto intStack
        intStack.push(ix++);   // intStack holds 0...9 inclusive

    int error_cnt = 0;
    // look at each value and pop it off the stack
    while (intStack.empty() == false) {
        int value = intStack.top();
        // read the top element of the stack
        if (value != --ix) {
            cerr << "oops! expected " << ix 
                 << " received " << value << endl;
            ++error_cnt;
        }
        intStack.pop(); // pop the top element, and repeat
    }
    cout << "Our program ran with " 
         << error_cnt << " errors!" << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 


#include <iostream>
using std::endl;
using std::cout;

void printValues(const int *beg, const int *end) 
{
    while (beg != end) {
        cout << *beg++ << endl;
    }
}

int main() 
{
    int j[2] = {0, 1};

    // ok: j is converted to pointer to 0th element in j
    //     j + 2 refers one past the end of j
    printValues(j, j + 2);  

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
using std::string;
#include <iostream>
using std::cout; using std::endl;

int main()
{
    string substr = "Hello";
    string phrase = "Hello World";
    string slang  = "Hiya";

    if (substr < phrase) cout << "substr is smaller" << endl;
    if (slang > substr) cout << "slang is greater" << endl;
    if (slang > phrase) cout << "slang is greater" << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
#include <string>

using std::string; using std::cout; using std::endl;

int main() 
{

    string s1("hello, ");
    string s2("world\n");

    string s3 = s1 + s2;   // s3 is hello, world\n
    
    // print what we've got so far
    cout << "s1: " << s1 << " s2: " << s2 << " s3: " << s3 << endl;

    s1 += s2;   // equivalent to s1 = s1 + s2

    // print after update to s1
    cout << "s1: " << s1 << " s2: " << s2 << " s3: " << s3 << endl;

{
    // a better way to ``add'' punctuation
    string s1("hello");
    string s2("world");

    string s3 = s1 + ", " + s2 + "\n";

    // print again, now there won't be a newline after printing s2
    cout << "s1: " << s1 << " s2: " << s2 << " s3: " << s3 << endl;

}
return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>

using std::string;

int main()
{
    //  string implementation 
        string str("a very long literal string");

        // performance test on string allocation and copy
        for (int ix = 0; ix != 1000000; ++ix) {
            string str2 = str; // do the copy, automatically allocated
            if (str != str2)         // use the new string
                  ;  // do nothing
        }                            // str2 is automatically freed
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;
using std::string;

// Note: #include and using declarations must be added to compile this code
int main()
{
    string s;          // empty string
    cin >> s;          // read whitespace-separated string into s
    cout << s << endl; // write s to the output
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;
using std::string;

int main()
{
	string s1, s2;

	cin >> s1 >> s2; // read first input into s1, second into s2
	cout << s1 << s2 << endl; // write both strings

	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
using std::string;

string st1;         // empty string

string st2(st1);  // st2 is a copy of st1

string st3("Hello World");  // st3 holds Hello World
#include <iostream>

using std::cout;
using std::endl;
int main()
{
    string st("The expense of spirit\n");
    cout << "The size of " << st << "is " << st.size()
         << " characters, including the newline" << endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "seq_preamble.h"

vector<char> c_vec;
//string s("some string"), s2("some other string");
string s, s2;

int main() 
{
    s = "some string";
    s2 = "some other string";
    // 3 equivalent ways to insert all the characters from s2 at beginning of s
    // insert iterator range before s.begin()
    s.insert(s.begin(), s2.begin(), s2.end());  

    cout << "insert iterators version:        " << s << endl;
    s = "some string";

    // insert copy of s2 before position 0 in s
    s.insert(0, s2);                  

    cout << "insert string at given position: " << s << endl;
    s = "some string";

    // insert s2.size() characters from s2 starting at s2[0] before s[0]
    s.insert(0, s2, 0, s2.size());   

    cout << "insert positional version:       " << s << endl;
    s = "";

    c_vec.push_back('a');   // temporary hack to workaround g++ bug

    // replace characters in s by elements from c_vec
    s.insert(s.begin(), c_vec.begin(), c_vec.end());  

    s.insert(s.size(), 5, '!'); // insert five exclamation points at end of s
    cout << s << endl;
    s.erase(s.size() - 5, 5);  // erase last five characters from s
    cout << s << endl;
{
    char *cp = "Stately plump Buck";
    string s;

    s.assign(cp, 7);            // s == "Stately"
    cout << s << endl;
    s.insert(s.size(), cp + 7); // s == "Stately plump Buck"
    cout << s << endl;
}

{
    string s("C++ Primer");         // initialize s to "C++ Primer"
    s.append(" 3rd Ed.");           // s == "C++ Primer 3rd Ed."

    // equivalent to s.append(" 3rd Ed.")
    s.insert(s.size(), " 3rd Ed."); 
    cout << s << endl;

    // starting at position 11, erase 3 characters and then insert "4th"
    s.replace(11, 3, "4th");        // s == "C++ Primer 4th Ed."

    cout << s << endl;

    // equivalent way to replace "3rd" by "4th"
    s.erase(11, 3);                 // s == "C++ Primer Ed."
    s.insert(11, "4th");            // s == "C++ Primer 4th Ed."

    cout << s << endl;

    s.replace(11, 3, "4th");        // s == "C++ Primer 4th Ed."

    cout << s << endl;

    s.replace(11, 3, "Fourth"); // s == "C++ Primer Fourth Ed."
    cout << s << endl;

{
    string s("C++ Primer, 3rd Ed.");
    s.replace(s.find("3rd"), 3, "Fourth");
    cout << s << endl;
}
}
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "seq_preamble.h"

int main() 
{
    string s("Hiya!");
    string::iterator iter = s.begin();
    while (iter != s.end())
        cout << *iter++ << endl; // postfix increment: print old value 
    return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout;
using std::endl;


int main()
{
    const char *st = "The expense of spirit\n";
    int len = 0;
    while (*st) { ++len; ++st; }
    cout << len << ": " << st << endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout;
using std::endl;

const char *st = "The expense of spirit\n";

int main()
{
    int len = 0;
    const char *p = st;
    while (*p) { ++len; ++p; }
    cout << len << ": " << st << endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <vector>
using std::vector;
#include <iostream>
using std::cout; using std::endl;

class Foo {
public:
Foo(): data(100) { for (int i = 0; i != 100; ++i) data[i] = i; }
    int &operator[](const size_t);
    const int &operator[](const size_t) const;
    // other interface members
private:
    vector<int> data;
    // other member data and private utility functions
};

int& Foo::operator[](const size_t index)
{
	return data[index];   // no range checking on index
}

const int& Foo::operator[](const size_t index) const
{
	return data[index];   // no range checking on index
}

int main() {
    Foo f;

    cout << f[50] << endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "seq_preamble.h"

int main()
{
    string s("hello world");
    // return substring of 5 characters starting at position 6
    string s2 = s.substr(6, 5);  // s2 = world

    // return substring from position 6 to the end of s
    string s3 = s.substr(6);    // s3 = world
    cout << s << "\n" << s2 << "\n" << s3 << "\n" << s.substr() << endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl; using std::cin;
#include <string>
using std::string;

template <class T> T min(T a, T b) 
{
    return a < b ? a : b;
}


// swap parameter values
template <typename T> 
inline
void swap(T& val1, T& val2)
{
    T tmp = val1;
    val1 = val2;
    val2 = tmp;
}

int main()
{
    string s1("hello"), s2("world"), s3("hiya");
    cout << s1 << " " << s2 << endl;  // print hello world
    swap(s1, s3);
    cout << s1 << " " << s2 << endl;  // print hiya world

    double d1 = 3.14, d2 = 2.7, d3 = 1.4;
    cout << d1 << " " << d2 << endl;  // print 3.14 2.7
    swap(d3, d2);
    cout << d1 << " " << d3 << endl;  // print 3.14 2.7

{
    short s1, s2;
    int i1, i2;
    cin >> s1 >> s2;
    cin >> i1 >> i2;
    min(i1, i2);    // ok: instantiate min(int, int)
    min(s1, s2);    // ok: instantiate min(short, short)
    // now print the results
    cout << min(i1, i2) << endl;    
    cout << min(s1, s2) << endl;
}
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "decl1.h"
#include "swapmain.pgm"
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "decl2.h"
#include "swapmain.pgm"
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "TextQuery.h"
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>
#include <stdexcept>

using std::istringstream;
using std::set;
using std::string;
using std::getline;
using std::map;
using std::vector;
using std::cerr;
using std::cout;
using std::cin;
using std::ifstream;
using std::endl;
using std::ispunct;
using std::tolower;
using std::strlen;
using std::out_of_range;

string TextQuery::text_line(line_no line) const
{
    if (line < lines_of_text.size())
        return lines_of_text[line];
    throw std::out_of_range("line number out of range");
}

// read input file: store each line as element in lines_of_text 
void TextQuery::store_file(ifstream &is)
{
    string textline;
    while (getline(is, textline))
       lines_of_text.push_back(textline);
}

// \v: vertical tab; \f: formfeed; \r: carriage return are
// treated as whitespace characters along with space, tab and newline
string TextQuery::whitespace_chars(" \t\n\v\r\f");

// finds whitespace-separated words in the input vector
// and puts the word in word_map along with the line number
void TextQuery::build_map()
{
    // process each line from the input vector
    for (line_no line_num = 0; 
                 line_num != lines_of_text.size();
                 ++line_num)
    {
        // we'll use line to read the text a word at a time
        istringstream line(lines_of_text[line_num]);
        string word;
        while (line >> word)
            // add this line number to the set;
            // subscript will add word to the map if it's not already there
            word_map[cleanup_str(word)].insert(line_num);
    }
}

set<TextQuery::line_no>
TextQuery::run_query(const string &query_word) const
{
    // Note: must use find and not subscript the map directly
    // to avoid adding words to word_map!
    map<string, set<line_no> >::const_iterator 
                          loc = word_map.find(cleanup_str(query_word));
    if (loc == word_map.end()) 
        return set<line_no>();  // not found, return empty set
    else
        // fetch and return set of line numbers for this word
        return loc->second;
}

void TextQuery::display_map()
{
    map< string, set<line_no> >::iterator iter = word_map.begin(),
                                       iter_end = word_map.end();

    // for each word in the map
    for ( ; iter != iter_end; ++iter) {
        cout << "word: " << iter->first << " {";

        // fetch location vector as a const reference to avoid copying it
        const set<line_no> &text_locs = iter->second;
        set<line_no>::const_iterator loc_iter = text_locs.begin(),
                                     loc_iter_end = text_locs.end();

        // print all line numbers for this word
        while (loc_iter != loc_iter_end)
        {
            cout << *loc_iter;

            if (++loc_iter != loc_iter_end)
                 cout << ", ";

         }

         cout << "}\n";  // end list of output this word
    }
    cout << endl;  // finished printing entire map
}

string TextQuery::cleanup_str(const string &word)
{
    string ret;
    for (string::const_iterator it = word.begin(); it != word.end(); ++it) {
        if (!ispunct(*it))
            ret += tolower(*it);
    }
    return ret;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#ifndef TEXTQUERY_H
#define TEXTQUERY_H
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>

class TextQuery {
    // as before
public:
    // typedef to make declarations easier
    typedef std::string::size_type str_size;
    typedef std::vector<std::string>::size_type line_no;

    /* interface:
     *    read_file builds internal data structures for the given file
     *    run_query finds the given word and returns set of lines on which it appears
     *    text_line returns a requested line from the input file
    */
    void read_file(std::ifstream &is) 
               { store_file(is); build_map(); }
    std::set<line_no> run_query(const std::string&) const; 
    std::string text_line(line_no) const;
    str_size size() const { return lines_of_text.size(); }
    void display_map();        // debugging aid: print the map

private:
    // utility functions used by read_file
    void store_file(std::ifstream&); // store input file
    void build_map(); // associated each word with a set of line numbers

    // remember the whole input file
    std::vector<std::string> lines_of_text; 

    // map word to set of the lines on which it occurs
    std::map< std::string, std::set<line_no> > word_map;  
    // characters that constitute whitespace
    static std::string whitespace_chars;     
    // canonicalizes text: removes punctuation and makes everything lower case
    static std::string cleanup_str(const std::string&);
};
#endif
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <cstring>
#include <cstddef>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <utility>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <iterator>

using std::vector; using std::list; using std::map; using std::set;  
using std::pair; using std::string; 
using std::istream; using std::ostream; using 
std::ifstream; using std::ofstream; using std::fstream;
using std::cin; using std::cout; using std::endl; using std::cerr;
using std::istream_iterator; using std::ostream_iterator;
using std::find;  using std::sort; using std::for_each;
using std::count; using std::count_if; using std::copy;
using std::back_inserter; using std::reverse_iterator;
using std::strcmp;

std::ifstream& open_file(std::ifstream&, const std::string);
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
int main() 
{
	std::cout << "two" // ok to break long literal across lines
                     "some" << std::endl;
	return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;
#include <typeinfo>
#include <string>
using std::string;

struct Base {
    virtual ~Base() { }
};

struct Derived : Base { };

int main()
{
int iobj;

cout << typeid(iobj).name() << endl
     << typeid(8.16).name() << endl
     << typeid(std::string).name() << endl
     << typeid(Base).name() << endl
     << typeid(Derived).name() << endl;

return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "io_preamble.h"
#include <stdexcept>
using std::range_error;
// prototypical main function that takes a file name to open
int main()
{
    ifstream in;
    open_file(in, "get_openfile.cpp");
    string word;
    while (in >> word)
        cout << word << endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;

#include "Handle.h"

int main()
{
{  // new scope
   // user allocates but must not delete the object to which the Handle is attached
   Handle<int> hp(new int(42));
   { // new scope
       Handle<int> hp2 = hp;   // copies pointer; use count incremented
       cout << *hp << " " << *hp2 << endl;   // prints 42 42
       *hp2 = 10;              // changes value of shared underlying int
   }   // hp2 goes out of scope; use count is decremented
   cout << *hp << endl;    // prints 10
}  // hp goes out of scope; its destructor deletes the int
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "algs_preamble.h"

int main() {
    vector<int> vec;

    istream_iterator<int> in_iter(cin);  // read ints from cin
    istream_iterator<int> eof;           // istream ``end'' iterator

    // read until end of file, storing what was read in vec
    while (in_iter != eof)
            // increment advances the stream to the next value
            // dereference reads next value from the istream
            vec.push_back(*in_iter++);

    // print what we got
    for (vector<int>::const_iterator it = vec.begin();
                                     it != vec.end(); ++it)
	    cout << *it << " ";
    cout << endl;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "algs_preamble.h"
int main() 
{
    // write one string per line to the standard output
    ostream_iterator<string> out_iter(cout, "\n");

    // read strings from standard input and the end iterator
    istream_iterator<string> in_iter(cin), eof;

    // read until eof and write what was read to the standard output
    while (in_iter != eof) 
	    // write value of in_iter to standard output
	    // and then increment the iterator to get the next value from cin
	    *out_iter++ = *in_iter++;  

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Query.h"
#include "TextQuery.h"
#include <string>
#include <set>
#include <iostream>

using std::set;
using std::string;
using std::cin; using std::cout; using std::cerr;
using std::endl;

int main(int, char **argv)
{
    // gets file to read and builds map to support queries
    TextQuery file = build_textfile(argv[1]);

    // iterate with the user: prompt for a word to find and print results
    while (true) {
        string sought1, sought2, sought3;
        if (!get_words(sought1, sought2)) break;
        cout << "\nenter third word: " ;
        cin  >> sought3;
        // find all the occurrences of the requested string
        Query orq = Query(sought1) & Query(sought2) | Query(sought3);

        cout << "\nExecuting Query for: " << orq << endl;
        const set<TextQuery::line_no> locs = orq.eval(file);
        // report matches
        print_results(locs, file);
     } 
     return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
#include <vector>
using std::vector;
using std::endl;
using std::cout;

// function to return minimum element in an vector of ints
int min_element(vector<int>::iterator, 
                vector<int>::iterator);

// pointer to function, initialized to point to min_element
int (*pf)(vector<int>::iterator, vector<int>::iterator) 
        = min_element;

int main() 
{
    vector<int> ivec;
    // give ivec some values
    for (vector<int>::size_type i = 0; i < 10; ++i)
        ivec.push_back(i);

    cout << "Direct call: "   
         << min_element(ivec.begin(), ivec.end()) << endl;

    cout << "Indirect call: " 
         << pf(ivec.begin(), ivec.end()) << endl;

    return 0;
}

// returns minimum element in an vector of ints
int min_element(vector<int>::iterator beg, 
                vector<int>::iterator end) {
    int minVal = 0;
    while (beg != end) {
        if (minVal > *beg)
            minVal = *beg;
        ++beg;
    }
    return minVal;
}


/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
#include <vector>
using std::vector;
using std::endl;
using std::cout;

void reset(int *ip)
{
    *ip = 0;  // changes the value of the object to which ip points
    ip = 0;   // changes only the local value of ip; the argument is unchanged
}

int main() 
{
    int i = 42;
    int *p = &i;

    cout << "i: " << *p << '\n';   // prints i: 42
    reset(p);                      // changes *p but not p
    cout << "i: " << *p << endl;   // ok: prints i: 0
    return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

// swap values of two pointers to int
void ptrswap(int *&v1, int *&v2) 
{
    int *tmp = v2;
    v2 = v1;
    v1 = tmp;
}


#include <iostream>
using std::endl;
using std::cout;

void ptrswap(int *&v1, int *&v2);

int main() 
{
    int i = 10;
    int j = 20;

    int *pi = &i;  // pi points to i
    int *pj = &j;  // pj points to j

    cout << "Before ptrswap():\t*pi: "
         << *pi << "\t*pj: " << *pj << endl;

    ptrswap(pi, pj); // now pi points to j; pj points to i
    cout << "After ptrswap():\t*pi: "
         << *pi << "\t*pj: " << *pj << endl;
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>

int main()
{
	std::cout << "Enter two numbers:" << std::endl;
	int v1, v2; 
	std::cin >> v1 >> v2; // read input

	// use smaller number as lower bound for summation
	// and larger number as upper bound
	int lower, upper;
	if (v1 <= v2) {
		lower = v1;
		upper = v2;
	} else {
		lower = v2;
		upper = v1;
	}

	int sum = 0;

	// sum values from lower up to and including upper
	for (int val = lower; val <= upper; ++val) 
		sum += val;  // sum = sum + val
	
	std::cout << "Sum of " << lower 
	          << " to " << upper 
	          << " inclusive is " 
	          << sum << std::endl;

	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "CheckedPtr.h"
#include <iostream>
using std::cout; using std::endl;
#include <exception>
using std::exception;

int main()
{
    int ia[] = {1,2,3,4,5,6,7,8,9};
    const int size = sizeof(ia)/sizeof(int);

    CheckedPtr p1(ia, ia + size);

    CheckedPtr parr(ia, ia + size);  // ia points to an array of ints
    parr.operator++(0);              // call postfix operator++
    parr.operator++();               // call prefix  operator++

    try {
        int i = 0;
        while(i++ != size)
          cout << *p1++ << endl;
    } catch (exception& e) { cout << e.what() << endl; }
    //p1 now positioned one past end of array

    // should throw dereference error
    try { *p1; } catch (exception &e) {cout << e.what() << endl; }

    // should throw increment error
    try { p1++; } catch (exception &e) {cout << e.what() << endl; }
    try { ++p1; } catch (exception &e) {cout << e.what() << endl; }

    cout << endl;

    try {
        int i = size; --p1; // reset p1 to denote last element
        while(i-- > 1)
          cout << *p1-- << endl;
        cout << *p1 << endl;
    } catch (exception& e) { cout << e.what() << endl; }
    //p1 now positioned at beginning of array

    cout << endl;

    // repeat using prefix ops
    try {
        int i = 0;
        cout << *p1 << endl;
        while(i++ != size-1)
          cout << *++p1 << endl;
    } catch (exception& e) { cout << e.what() << endl; }
    // p1 now positioned on end of array

    cout << endl;

    try {
        int i = size; ++p1;  // now positioned one past end
        while(i-- > 0)
          cout << *--p1 << endl;
    } catch (exception& e) { cout << e.what() << endl; }
    // p1 now positioned at beginning of array

    // should throw decrement error
    try { --p1; } catch (exception &e) {cout << e.what() << endl; }
    try { p1--; } catch (exception &e) {cout << e.what() << endl; }

    return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "algs_preamble.h"

int main() 
{
	vector<string> words;
	string tmp;
	while (cin >> tmp)
		words.push_back(tmp);
	sort(words.begin(), words.end());
	typedef vector<string>::iterator veciter;
	veciter iter = words.begin();
	while (iter != words.end()) {
		pair<veciter, veciter> range = 
		    equal_range(words.begin(), words.end(), *iter);
		cout << *iter << " occurs " 
		     << range.second - range.first
		     << " times" << endl;
		iter = range.second;
	}
	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "algs_preamble.h"

int main()
{
    // use find to search an array
    int ia[6] = {27, 210, 12, 47, 109, 83};
    int search_value = 83;

    int *result = find(ia, ia + 6, search_value);

    cout << "The value " << search_value
         << (result == ia + 6
               ? " is not present" : " is present")
         << endl;

    // use find to search a vector
{
    // initialize the vector with some values
    int ia[6] = {27, 210, 12, 47, 109, 83};
    vector<int> vec(ia, ia+6);

    // value we'll look for
    int search_value = 42;

    // call find to see if that value is present
    vector<int>::const_iterator result =
            find(vec.begin(), vec.end(), search_value);

    // report the result
    cout << "The value " << search_value
         << (result == vec.end()
               ? " is not present" : " is present")
         << endl;
}

    // use find to search a list
{
    list<int> lst(ia, ia+6);

    int search_value = 47;

    // call find to look through elements in a list
    list<int>::const_iterator result =
             find(lst.begin(), lst.end(), search_value);

    cout << "The value " << search_value
         << (result == lst.end()
               ? " is not present" : " is present")
         << endl;
}
    return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Basket2.h"
#include <algorithm>
using std::multiset; using std::map; using std::pair; using std::size_t;
using std::string; using std::ostream; using std::endl; using std::min;
using std::cout;

// debugging routine to check contents in a Basket
void Basket::display(ostream &os) const
{
    os << "Basket size: " << items.size() << endl;

    // print each distinct isbn in the Basket along with
    // count of how many copies are ordered and what their price will be
    // upper_bound returns an iterator to the next item in the set
    for (const_iter next_item = items.begin();
                  next_item != items.end();
                  next_item = items.upper_bound(*next_item))
    {
        // we know there's at least one element with this key in the Basket
        os << (*next_item)->book() << " occurs " 
           << items.count(*next_item) << " times" 
           << " for a price of " 
           << (*next_item)->net_price(items.count(*next_item)) 
           << endl;
    }
}

void print_total(ostream &, const Item_base&, size_t);

// calculate and print price for given number of copies, applying any discounts 
void print_total(ostream &os, 
                 const Item_base &item, size_t n)
{
    os << "ISBN: " << item.book() // calls Item_base::book
       << "\tnumber sold: " << n << "\ttotal price: "
       // virtual call: which version of net_price to call is resolved at run time
       << item.net_price(n) << endl;
}

double Basket::total() const
{
    double sum = 0.0;    // holds the running total 


    for (const_iter iter = items.begin(); 
                    iter != items.end();
                    iter = items.upper_bound(*iter))
    {
        // we know there's at least one element with this key in the Basket
        print_total(cout, *(iter->p), items.count(*iter));
        // virtual call to net_price applies appropriate discounts, if any
        sum += (*iter)->net_price(items.count(*iter));
    }
    return sum;
}

// use-counted assignment operator; use is a pointer to a shared use count
Sales_item&
Sales_item::operator=(const Sales_item &rhs)
{
    ++*rhs.use;
    decr_use();
    p = rhs.p;
    use = rhs.use;
    return *this;
}

// if specified number of items are purchased, use discounted price 
double Bulk_item::net_price(size_t cnt) const
{
    if (cnt >= quantity)
        return cnt * (1 - discount) * price;
    else
        return cnt * price;
}

// use discounted price for up to a specified number of items
// additional items priced at normal, undiscounted price
double Lim_item::net_price(size_t cnt) const
{
    size_t discounted = min(cnt, quantity);
    size_t undiscounted = cnt - discounted;
    return discounted * (1 - discount) * price 
           + undiscounted * price;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "Basket2.h"
#include <iostream>
using std::cout; using std::endl;

int main()
{
	Sales_item item1(Item_base("123", 45));
	Sales_item item2(Bulk_item("345", 45, 3, .15));
	Sales_item item3(Bulk_item("678", 55, 5, .25));
	Sales_item item4(Lim_item("abc", 35, 2, .10));
	Sales_item item5(Item_base("def", 35));

	Basket sale;
	sale.add_item(item1);
cout << "added first item" << endl;
	sale.add_item(item1);
	sale.add_item(item1);
	sale.add_item(item1);
	sale.add_item(item2);
	sale.add_item(item2);
	sale.add_item(item2);
	sale.add_item(item2);
	sale.add_item(item2);
	sale.add_item(item2);
	sale.add_item(item2);
	sale.add_item(item2);
	sale.add_item(item3);
	sale.add_item(item3);
	sale.add_item(item3);
	sale.add_item(item3);
	sale.add_item(item3);
	sale.add_item(item3);
	sale.add_item(item3);
	sale.add_item(item3);
	sale.add_item(item4);
	sale.add_item(item4);
	sale.add_item(item4);
	sale.add_item(item4);
	sale.add_item(item4);
	sale.add_item(item4);
	sale.add_item(item5);
	sale.add_item(item5);
cout << "added last item" << endl;

	sale.display(cout);
	cout << sale.total() << endl;
{
	// arguments are the isbn, price, minimum quantity, and discount
	Bulk_item bulk("0-201-82470-1", 50, 5, .19);
	Basket sale;
	sale.add_item(Bulk_item("0-201-82470-1", 50, 5, .19));
	sale.add_item(Bulk_item("0-201-82470-1", 50, 5, .19));
	sale.add_item(Bulk_item("0-201-82470-1", 50, 5, .19));
	sale.add_item(Bulk_item("0-201-82470-1", 50, 5, .19));
	sale.add_item(Bulk_item("0-201-82470-1", 50, 5, .19));
	sale.add_item(Lim_item("0-201-54848-8", 35, 2, .10));
	sale.add_item(Lim_item("0-201-54848-8", 35, 2, .10));
	sale.add_item(Lim_item("0-201-54848-8", 35, 2, .10));
	double total = sale.total();
	cout << "Total Sale: " << total << endl;
}
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::ostream; using std::cout; using std::endl;
#include <string>

/*
 * Valuelike behavior even though HasPtr has a pointer member:
 * Each time we copy a HasPtr object, we make a new copy of the
 * underlying int object to which ptr points.
*/
class HasPtr {
public:
    // no point to passing a pointer if we're going to copy it anyway
    // store pointer to a copy of the object we're given
    HasPtr(const int &p, int i): ptr(new int(p)), val(i) {}

    // copy members and increment the use count
    HasPtr(const HasPtr &orig):
       ptr(new int (*orig.ptr)), val(orig.val) { }

    HasPtr& operator=(const HasPtr&);
    // wrong: don't define a destructor without also defining copy and assign
    ~HasPtr() { delete ptr; } 
    friend ostream& operator<<(ostream&, const HasPtr&);

    // accessors must change to fetch value from Ptr object
    int get_ptr_val() const { return *ptr; } 
    int get_int() const { return val; }

    // change the appropriate data member
    void set_ptr(int *p) { ptr = p; }
    void set_int(int i)  { val = i; }

    // return or change the value pointed to, so ok for const objects
    int *get_ptr() const { return ptr; } 
    void set_ptr_val(int p) const { *ptr = p; }
private:
    int *ptr;        // points to an int
    int val;
};

HasPtr& HasPtr::operator=(const HasPtr &rhs)
{
    // Note: Every HasPtr is guaranteed to point at an actual int;
    //       We know that ptr cannot be a zero pointer
    *ptr = *rhs.ptr;      // copy the value pointed to
    val = rhs.val;        // copy the int 
    return *this;
}

ostream& operator<<(ostream &os, const HasPtr &hp)
{
    os << "*ptr: " << hp.get_ptr_val() << "\tval: " << hp.get_int() << endl;
    return os;
}

int main()
{
    int obj = 0;

    HasPtr ptr1(obj, 42);  // int* member points copy of obj, val is 42
    HasPtr ptr2(ptr1);     // int* member points new copy obj, val is 42

    cout << "(1) ptr1: " << ptr1 << endl << "ptr2: " << ptr2 << endl;

    ptr1.set_ptr_val(42); // sets copy in ptr1; value in ptr2 unchanged
    ptr2.get_ptr_val();   // returns 0

    cout << "(2) ptr1: " << ptr1 << endl << "ptr2: " << ptr2 << endl;

    ptr1.set_int(0);   // changes val member only in ptr1
    ptr2.get_int();    // returns 42
    ptr1.get_int();    // returns 0

    cout << "(3) ptr1: " << ptr1 << endl << "ptr2: " << ptr2 << endl;

    int *ip = new int(42); // dynamically allocated int initialized to 42
    HasPtr ptr(*ip, 10);    // HasPtr points to same object as ip does
    delete ip;          // object pointed to by ip is freed
    ptr.set_ptr_val(0); // ok: ptr has its own copy

    cout << "(4) ptr: " << ptr << endl;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;
#include <memory>

// pseudo-implementation of memory allocation strategy for a vector-like class
template <class T> class Vector {
public:
    Vector(): elements(0), first_free(0), end(0) { }
    void push_back(const T&);
    // . . .
    T& operator[](size_t n) { return elements[n]; }
    const T& operator[](size_t n) const { return elements[n]; }
private:
    static std::allocator<T> alloc; // object to get raw memory
    void reallocate(); // get more space and copy existing elements
    T* elements;       // pointer to first element in the array
    T* first_free;     // pointer to first free element in the array
    T* end;            // pointer to one past the end of the array
    // . . .
};

#include <algorithm>
using std::allocator;
template <class T> allocator<T> Vector<T>::alloc;

using std::max;
using std::uninitialized_copy;
template <class T>
void Vector<T>::reallocate()
{
    // compute size of current array and allocate space for twice as many elements
    std::ptrdiff_t size = first_free - elements; 
    std::ptrdiff_t newcapacity = 2 * max(size, 1);
    T* newelements = alloc.allocate(newcapacity);
 
    // construct copies of the existing elements in the new space
    uninitialized_copy(elements, first_free, newelements);

    // destroy the old elements in reverse order
    for (T *p = first_free; p != elements; /*empty*/ )
        alloc.destroy(--p);
    
    // deallocate the memory that they occupied
    // deallocate cannot be called on a 0 pointer
    if (elements)
        alloc.deallocate(elements, end - elements);
    // make our data structure point to the new elements
    elements = newelements;
    first_free = elements + size;
    end = elements + newcapacity;
}

template <class T>
void Vector<T>::push_back(const T& t)
{
    // are we out of space?
    if (first_free == end)
      reallocate(); // gets more space and copies existing elements to it
    alloc.construct(first_free, t);
    ++first_free;
}

int main()
{
    Vector<int> vi;

    for (int i = 0; i != 10; ++i) {
      vi.push_back(i);
      cout << vi[i] << endl;
    }

    for (int i = 0; i != 10; ++i)
      cout << vi[i] << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;
#include <vector>
using std::vector;

// pass iterators to the first and one past the last element to print
void print(vector<int>::const_iterator beg,
           vector<int>::const_iterator end)
{
    while (beg != end) {
        cout << *beg++; 
        if (beg != end) cout << " ";  // no space after last element
    }
    cout << endl;
}

// vec is potentially large, so copying vec might be expensive
// use a const reference to avoid the copy
void print(const vector<int> &vec)
{
    for (vector<int>::const_iterator it = vec.begin();
                                it != vec.end(); ++it) {
        if (it != vec.begin()) cout << " ";
        cout << *it;
    }
    cout << endl;
}

int main()
{
    vector<int> vec(42);
    print(vec.begin(), vec.end());
    print(vec);
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cout; using std::endl;
#include <memory>

// psuedo-implementation of memory allocation strategy for a vector-like class
template <class T> class Vector {
public:
    Vector(): elements(0), first_free(0), end(0) { }
    void push_back(const T&);
    size_t size() const { return first_free - elements; }
    size_t capacity() const { return end - elements; }
    // . . .
    T& operator[](size_t n) { return elements[n]; }
    const T& operator[](size_t n) const { return elements[n]; }
private:
    static std::allocator<T> alloc; // member to handle allocation
    void reallocate(); // get more space and copy existing elements
    T* elements;       // pointer to first element in the array
    T* first_free;     // pointer to first free element in the array
    T* end;            // pointer to one past the end of the array
    // . . .
};

#include <algorithm>
using std::allocator;
template <class T> allocator<T> Vector<T>::alloc;

using std::max;
using std::uninitialized_copy;
template <class T> void Vector<T>::reallocate()
{
    // compute size of current array and allocate space for twice as many elements
    std::ptrdiff_t size = first_free - elements; 
    std::ptrdiff_t newcapacity = 2 * max(size, 1);
    // allocate space to hold newcapacity number of elements of type T
    T* newelements = alloc.allocate(newcapacity);
 
    // construct copies of the existing elements in the new space
    uninitialized_copy(elements, first_free, newelements);

    // destroy the old elements in reverse order
    for (T *p = first_free; p != elements; /*empty*/ )
        alloc.destroy(--p);

    
    // deallocate cannot be called on a 0 pointer
    if (elements)
        // return the memory that held the elements
        alloc.deallocate(elements, end - elements);

    // make our data structure point to the new elements
    elements = newelements;
    first_free = elements + size;
    end = elements + newcapacity;
}

template <class T> void Vector<T>::push_back(const T& t)
{
    // any space left?
    if (first_free == end)
      reallocate(); // gets more space and copies existing elements to it
    // construct a copy t in the element to which first_free points
    alloc.construct(first_free, t);  
    ++first_free;
}

int main()
{
    Vector<int> vi;

    for (int i = 0; i != 10; ++i) {
      vi.push_back(i);
      cout << vi[i] << endl;
    }

    for (int i = 0; i != 10; ++i)
      cout << vi[i] << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
#include <vector>
using std::vector;
using std::cout;
using std::endl;

void printValues(const vector<int>& vec)
{
    cout << "(" << vec.size() << "): ";
    for (int i = 0; i != vec.size(); ++i) {
        cout << vec[i];
        if (i != vec.size()-1)
            cout << ", ";
    }
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
#include <vector>
using std::vector; using std::cout; using std::endl;

void printValues(const vector<int>&);

int main() 
{
    int i = 0, j[2] = {0, 1};

    vector<int> vec1(1); // create a vector of 1 element
    vec1[0] = i;
    printValues(vec1);
    cout << endl;

    vector<int> vec2;    // create an empty vector
    // add elements to vec2
    for (int ix = 0; ix != sizeof(j) / sizeof(j[0]); ++ix)
        vec2.push_back(j[ix]);
    printValues(vec2);

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <vector>
#include <string>
#include <iostream>

using std::vector;
using std::string;
using std::cin;
using std::cout;
using std::endl;

int main()
{
    vector<int> ivec(10);

    // reset all the elements in ivec to 0
    for (vector<int>::size_type ix = 0; ix != ivec.size(); ++ix)
            ivec[ix] = 0;

    // print what we've got so far: should print 10 0's
    for (vector<int>::size_type ix = 0; ix != ivec.size(); ++ix)
            cout << ivec[ix] << " ";
    cout << endl;

    // equivalent loop using iterators to reset all the elements in ivec to 0
    for (vector<int>::iterator iter = ivec.begin();
                               iter != ivec.end(); ++iter)
	    *iter = 0;  // set element to which iter refers to 0

    // print using iterators: should print 10 0's
    for (vector<int>::iterator iter = ivec.begin();
                               iter != ivec.end(); ++iter)
            cout << *iter << " "; // print the element to which iter refers 
    cout << endl;

    vector<int>::iterator iter = ivec.begin();
    while (iter != ivec.end()) {
            *iter = 0;
            ++iter;
    }
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <vector>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;


int main()
{
    
    vector<int> ivec;                // empty vector
    int cnt = 10;
    // add elements 10...1 to ivec
    while (cnt > 0)
        ivec.push_back(cnt--);        // int postfix decrement

    vector<int>::iterator iter = ivec.begin();
    // prints 10 9 8 ... 1 
    while (iter != ivec.end())
        cout << *iter++ << endl; // iterator postfix increment

// Alternative loop using comma operator in the for expression
{
    int cnt = ivec.size();
    // add elements from size...1 to ivec
    for(vector<int>::size_type ix = 0; 
                    ix != ivec.size(); ++ix, --cnt)   
        ivec[ix] = cnt;

    vector<int>::iterator iter = ivec.begin();
    // prints 10 9 8 ... 1 
    while (iter != ivec.end())
        cout << *iter++ << endl; // iterator postfix increment
}
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string>
#include <iostream>

class Endangered {
public:
    enum Status { critical, environment, improving };
    Endangered(Status stat): animal_status(stat) { }
    virtual ~Endangered();
    virtual std::ostream& print(std::ostream&) const;
    virtual void highlight() const;
    // ...
private:
    Status animal_status;
};

class ZooAnimal;
extern std::ostream&
operator<<(std::ostream&, const ZooAnimal&);

class ZooAnimal {
public:
    ZooAnimal() { }
    ZooAnimal(std::string animal, bool exhibit,
              std::string family): nm(animal), 
                                   exhibit_stat(exhibit), 
                                   fam_name(family) { } 
    virtual ~ZooAnimal();

    virtual std::ostream& print(std::ostream&) const;
    virtual int population() const;

    // accessors
    std::string name() const { return nm; }
    std::string family_name() const { return fam_name; }
    bool onExhibit() const { return exhibit_stat; }
    // ...
protected:
    std::string nm;
    bool        exhibit_stat;
    std::string fam_name;
    // ...
private:
};

// the order of the keywords public and virtual is not significant
class Raccoon : public virtual ZooAnimal {
public:
    Raccoon();
    Raccoon(std::string name, bool onExhibit=true);

    virtual std::ostream& print(std::ostream&) const;

    bool pettable() const {return pettable_flag;  }
    void pettable(bool petval) {pettable_flag = petval;}
    // ...

protected:
    bool pettable_flag;
    // ...
};

class Bear : virtual public ZooAnimal {
public:
    // when the most derived class
    Bear(std::string name, bool onExhibit=true);
protected:
    // when an intermediate derived class
    Bear() : dance_flag(two_left_feet) { }

public:
    enum DanceType { two_left_feet, macarena, fandango };

    virtual std::ostream &print(std::ostream&) const;
    virtual std::string isA() const;
    int mumble(int);
    void dance(DanceType) const;

virtual ~Bear();
private:
    std::string name;
    int    ival;
    DanceType dance_flag;
};

class Panda : public Bear,
              public Raccoon, public Endangered {
public:
    Panda();
    Panda(std::string name, bool onExhibit=true);
    virtual std::ostream& print(std::ostream&) const;

    bool sleeping() const {return sleeping_flag;}
    void sleeping(bool newval) {sleeping_flag = newval;}
    // ...

protected:
    bool sleeping_flag;
    // ...
};


Bear::Bear(std::string name, bool onExhibit):
         ZooAnimal(name, onExhibit, "Bear") { }
Raccoon::Raccoon(std::string name, bool onExhibit)
       : ZooAnimal(name, onExhibit, "Raccoon") { }

Panda::Panda(std::string name, bool onExhibit)
      : ZooAnimal(name, onExhibit, "Panda"),
        Bear(name, onExhibit),
        Raccoon(name, onExhibit),
        Endangered(Endangered::critical),
        sleeping_flag(false)   { }

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "algs_preamble.h"
#include <fstream>
using std::ifstream;
#include <cstddef>
using std::size_t;

// comparison function to be used to sort by word length
bool isShorter(const string &s1, const string &s2)
{
	return s1.size() < s2.size();
}

// determine whether a given word is greater than length 6
// will reimplement later as a more general version using function objects
bool GT6(const string &s)
{
	return s.size() >= 6;
}

inline double
percent(double numerator, double denominator)
{
	return 100 * numerator/denominator;
}

// report number of words greater than 6
// later chapter will reimplement to be more general
void report_complexity(vector<string>::iterator beg,
                       vector<string>::iterator end,
                       ostream &output = cout)
{

	vector<string>::difference_type cnt = 
	                         count_if(beg, end, GT6);

	output << "Number of unique words in the input " 
	       << end - beg << endl;
	output << cnt << " "
	       <<  make_plural(cnt, "word", "s")
	       << " 6 characters or longer" << endl;
	output << "Percentage of words 6 characters or more " 
	       << percent(cnt, end - beg) << endl;
}

ifstream &open_file(ifstream&, const string&);

ifstream &prompt_user(ifstream &stream)
{
    // loops until we get a valid stream or the user asks to quit
    while (true) {
        // ask for name of the file to open
        string filename;
        cout << "Please enter file name."
             << "Hit Enter to quit: \n";
        getline(cin, filename);  // read the filename
        // terminate the loop on either empty line or eof
        if (!cin || filename == "") {
            // set condition state to indicate failure & eof
            stream.setstate(
                 ifstream::eofbit | ifstream::failbit);
            return stream;
        }

        // use getline in case user enters a filename with spaces
        if (open_file(stream, filename)) return stream;

        // otherwise, stream wasn't valid, prompt user to try again
        cerr << "oops! unable to open file"
             << filename
             << " please try again!" << endl;
    }
}

// calls open_file to ask the user for the next book to process
// builds a vector<book> that contains the contents of each book 
vector<string> get_books()
{
	ifstream infile;    // stream bound to the next book file
	vector<string> ret; // data structure we'll return to our caller

	// prompt_user asks user for next book and binds infile to it
	while (prompt_user(infile)) {

		// iterator to read the text of the book
		istream_iterator<string> in_iter(infile), eos;

		// read contents of the book into this book's vector
		ret.insert(ret.end(), in_iter, eos);

		infile.close();  // close the file when we're done with it
	}
	return ret;
}


int main()
{
	// read the contents of the books to analyze
	vector<string> words = get_books();

	// sort the input so that we can eliminate duplicates
	sort(words.begin(), words.end());

	// unique reorders elements and returns iterator to end of unique range
	// erase uses that iterator to erase non-unique elements
	words.erase(unique(words.begin(), words.end()), 
	            words.end());

	// sort the words by size, but maintain alphabetic order for words of the same size
	stable_sort(words.begin(), words.end(), isShorter);

	// analyze complexity: report frequency of words of each size
	// and total number/% of words greaten than length 6
	report_complexity(words.begin(), words.end());

	return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>
using std::cin;
using std::cout;
using std::endl;

int main()
{
    char ch;
    // initialize counters for each vowel
    int aCnt = 0, eCnt = 0, iCnt = 0, 
        oCnt = 0, uCnt = 0;

    while (cin >> ch) {
    	// if ch is a vowel, increment the appropriate counter
    	switch (ch) {
    		case 'a':
    			++aCnt;
    			break;
    		case 'e':
    			++eCnt;
    			break;
    		case 'i':
    			++iCnt;
    			break;
    		case 'o':
    			++oCnt;
    			break;
    		case 'u':
    			++uCnt;
    			break;
    	}
    }
    // print results
    cout << "Number of vowel a: \t" << aCnt << '\n'
         << "Number of vowel e: \t" << eCnt << '\n'
         << "Number of vowel i: \t" << iCnt << '\n'
         << "Number of vowel o: \t" << oCnt << '\n'
         << "Number of vowel u: \t" << uCnt << endl;

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>

using std::cout;
using std::endl;

int main()
{
    int ival = 0;
    ival = 3.541 + 3; //typically compiles with a warning

    cout << ival << endl;  // prints 6
    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <cassert>
#include <string> 
#include <iostream> 

using std::endl; using std::cerr; using std::cin;
using std::string;

int main() 
{
#ifndef NDEBUG
cerr << "starting main" << endl;
#endif
// ...
    string word = "foo";
    const string::size_type threshold = 5;
    if (word.size() < threshold) 
        cerr << "Error: " << __FILE__
             << " : line " << __LINE__ << endl
             << "       Compiled on " << __DATE__ 
             << " at " << __TIME__ << endl
             << "       Word read was \"" << word 
             << "\":  Length too short" << endl;
    word = "something longer than five chars";
    assert(word.size() > threshold);

    return 0;
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <iostream>

int main()
{
    int sum = 0, val = 1;
    // keep executing the while until val is greater than 10
    while (val <= 10) {
        sum += val;  // assigns sum + val to sum
        ++val;       // add 1 to val
    }
    std::cout << "Sum of 1 to 10 inclusive is " 
              << sum << std::endl;

    return 0;
}

/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include "assoc_preamble.h"
#include <map>

int main() 
{
    // count number of times each word occurs in the input
    map<string, int> word_count;  // empty map from string to int
    string word;
    while (cin >> word)
      ++word_count[word];

    // get iterator positioned on the first element
    map<string, int>::const_iterator 
                            map_it = word_count.begin();
    // for each element in the map
    while (map_it != word_count.end()) {
        // print the element key, value pairs
        cout << map_it->first << " occurs " 
             << map_it->second << " times" << endl;
        ++map_it;  // increment iterator to denote the next element
    }
return 0;
}

// Alternative implementation using insert instead of subscript
void alt_word_count()
{
    // count number of times each word occurs in the input
    map<string, int> word_count;  // empty map from string to int
    string word;

    while (cin >> word) {
        // inserts element with key equal to word and value 1;
        // if word already in word_count, insert does nothing
        pair<map<string, int>::iterator, bool> ret =
                word_count.insert(make_pair(word, 1));
        if (!ret.second)         // word already in word_count
            ++ret.first->second; // increment counter
    }
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <string> 
#include <iostream> 

using std::cin;
using std::cout;
using std::endl;
using std::string;

int main() 
{
	string word;

	// read until end-of-file, writing each word to a new line
	while (cin >> word) 
		cout << word << endl;
	return 0;
	
}
/*
 * This file contains code from "C++ Primer, Fourth Edition", by Stanley B.
 * Lippman, Jose Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2005 by Objectwrite, Inc., Jose Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Contracts Department
 * 	75 Arlington Street, Suite 300
 * 	Boston, MA 02216
 * 	Fax: (617) 848-7047
*/ 

#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <sstream>

using std::map; using std::string; using std::vector;
using std::ifstream; using std::cout; using std::endl;
using std::getline; using std::make_pair;
using std::runtime_error; using std::istringstream;

ifstream& open_file(ifstream&, const string&);

int main(int argc, char **argv)
{
    // map to hold the word transformation pairs: 
    // key is the word to look for in the input; value is word to use in the output
    map<string, string> trans_map;
    string key, value;

    if (argc != 3)
        throw runtime_error("wrong number of arguments");

    // open transformation file and check that open succeeded
    ifstream map_file;
    if (!open_file(map_file, argv[1]))
        throw runtime_error("no transformation file");
    // read the transformation map and build the map 
    while (map_file >> key >> value) 
        trans_map.insert(make_pair(key, value));
{
    // ok: let's display it
    map<string, string>::iterator map_it = trans_map.begin();

    //cout << "Here is our transformation map: \n\n";
    while (map_it != trans_map.end()) {
        cout << "key: "   << map_it->first; 
        if (map_it->first.size() == 1)
            cout << "       ";
        if (map_it->first.size() == 3)
            cout << "     ";
        else if (map_it->first.size() == 4)
            cout << "    ";
        else if (map_it->first.size() == 5)
            cout << "   ";
        cout << "value: " << map_it->second << endl;
        ++map_it;
    }
    cout << "\n\n";

    {  // this block just produces the vector so that we can print it
       // for the book
    cout << "Here is our original string input:\n\n";
    // read some text to transform
    ifstream input;
    if (!open_file(input, argv[2]))
        throw runtime_error("no input file");
    string word;
    while (getline(input, word)) 
         cout << word << endl;
    cout << "\n\n\n";
    input.close(); input.clear();
    }
}

    // ok, now we're ready to do the transformations
    // open the input file and check that the open succeeded
    ifstream input;
    if (!open_file(input, argv[2]))
        throw runtime_error("no input file");

    string line;   // hold each line from the input

    // read the text to transform it a line at a time
    while (getline(input, line)) {
        istringstream stream(line);  // read the line a word at a time
        string word;
        bool firstword = true;  // controls whether a space is printed 
        while (stream >> word) {
           // ok: the actual mapwork, this part is the heart of the program
           map<string, string>::const_iterator map_it =
                               trans_map.find(word);

           // if this word is in the transformation map
           if (map_it != trans_map.end()) 
               // replace it by the transformation value in the map
               word = map_it->second;  
           if (firstword)
               firstword = false;
           else
               cout << " ";  // print space between words
           cout << word;
        }
        cout << endl;        // done with this line of input
    }
    return 0;
}

说明性文件（如头文件.h文件、.inc文件、.def文件、编译说明文件.cfg等）头部应
进行注释，注释必须列出：版权说明、版本号、生成日期、作者、内容、功能、与其它文件的
关系、修改日志等，头文件的注释中还应有函数功能简要说明。

/*************************************************
 Copyright (C), 1988-1999, Huawei Tech. Co., Ltd.
 File name: // 文件名
 Author: Version: Date: // 作者、版本及完成日期
 Description: // 用于详细说明此程序文件完成的主要功能，与其他模块
              // 或函数的接口，输出值、取值范围、含义及参数间的控
              // 制、顺序、独立或依赖等关系
 Others: // 其它内容的说明
 Function List: // 主要函数列表，每条记录应包括函数名及功能简要说明
 1. ....
 History: // 修改历史记录列表，每条修改记录应包括修改日期、修改
          // 者及修改内容简述
 1. Date:
 Author:
 Modification:
 2. ...
*************************************************/


源文件头部应进行注释，列出：版权说明、版本号、生成日期、作者、模块目的/功能、
主要函数及其功能、修改日志等。
/************************************************************
 Copyright (C), 1988-1999, Huawei Tech. Co., Ltd.
 FileName: test.cpp
 Author: Version : Date:
 Description: // 模块描述
 Version:     // 版本信息
 Function List: // 主要函数及其功能
 1. -------
 History:     // 历史修改记录
 <author>    <time>     <version >   <desc>
 David       96/10/12    1.0 build   this moudle
***********************************************************/
说明：Description 一项描述本文件的内容、功能、内部各部分之间的关系及本文件与
其它文件关系等。History 是修改历史记录列表，每条修改记录应包括修改日期、修改
者及修改内容简述。


函数头部应进行注释，列出：函数的目的/功能、输入参数、输出参数、返回值、调用
关系（函数、表）等。
/*************************************************
 Function: // 函数名称
 Description: // 函数功能、性能等的描述
 Calls: // 被本函数调用的函数清单
 Called By: // 调用本函数的函数清单
 Table Accessed: // 被访问的表（此项仅对于牵扯到数据库操作的程序）
 Table Updated:  // 被修改的表（此项仅对于牵扯到数据库操作的程序）
 Input: // 输入参数说明，包括每个参数的作
        // 用、取值说明及参数间关系。
 Output: // 对输出参数的说明。
 Return: // 函数返回值的说明
 Others: // 其它说明
*************************************************/
