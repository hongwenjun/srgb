// regex_replace example
#include <iostream>
#include <string>
#include <regex>
#include <iterator>

int main()
{
    using namespace std;
    string s("there is a subsequence in the string\n");
    regex e("\\b(sub)([^ ]*)");    // matches words beginning by "sub"

    // using string/c-string (3) version:
    cout << regex_replace(s, e, string("sub-$2"));

    // using range/c-string (6) version:
    string result;
    regex_replace(back_inserter(result), s.begin(), s.end(), e, string("$2"));
    cout << result;

    // with flags:
    cout << regex_replace(s, e, string("$1 and $2"), regex_constants::format_no_copy);
    cout << endl;

    return 0;
}

/*  输出结果
there is a sub-sequence in the string
there is a sequence in the string
sub and sequence
*/

