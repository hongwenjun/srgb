" Vim syntax file
" Language:	C++
" Maintainer:	Ken Shan <ccshan@post.harvard.edu>
" Last Change:	2002 Jul 15

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

" Read the C syntax to start with
if version < 600
  so <sfile>:p:h/c.vim
else
  runtime! syntax/c.vim
  unlet b:current_syntax
endif

" C++ extentions
syn keyword cppStatement	new delete this friend using
syn keyword cppAccess		public protected private
syn keyword cppType		inline virtual explicit export bool wchar_t
syn keyword cppExceptions	throw try catch
syn keyword cppOperator		operator typeid
syn keyword cppOperator		and bitor or xor compl bitand and_eq or_eq xor_eq not not_eq
syn match cppCast		"\<\(const\|static\|dynamic\|reinterpret\)_cast\s*<"me=e-1
syn match cppCast		"\<\(const\|static\|dynamic\|reinterpret\)_cast\s*$"
syn keyword cppStorageClass	mutable STANDARD_LIBRARY_NAMES abort accumulate adjacent_find allocator append assign at auto_ptr back_inserter bad_alloc bad_cast begin bidirectional_iterator binary_search bind2nd bitset boolalpha c_str capacity cerr cin clear compare const_iterator copy copy_backward count count_if cout data dec deque empty end endl ends equal equal_range erase exception fill fill_n find find_end find_first_not_of find_first_of find_if find_last_not_of find_last_of first fixed flush for_each forward_iterator front_inserter fstream generate generate_n get_allocator getline hex ifstream includes inner_product inplace_merge input_iterator insert inserter internal ios_base isalpha islower ispunct isspace istream istream_iterator istringstream isupper iter_swap iterator key_comp left length less_equal list logic_error lower_bound make_heap make_pair map max max_element max_size merge min min_element mismatch multimap multiset negate next_permutation noboolalpha noshowbase noshowpoint noskipws not1 nounitbuf nouppercase nth_element oct of ofstream ostream ostream_iterator ostringstream out_of_range output_iterator pair partial_sort partial_sort_copy partition plus pop_heap prev_permutation priority_queue ptrdiff_t push_back push_heap queue random_iterator random_shuffle range_error rbegin remove remove_copy remove_copy_if remove_if rend replace replace_copy replace_copy_if replace_if reserve resize reverse reverse_copy reverse_iterator rfind right rotate rotate_copy runtime_error scientific search search_n second set set_difference set_intersection set_union setfill setprecision setw showbase showpoint size size_t size_type skipws sort sort_heap sqrt stable_partition stable_sort stack strcmp strcpy stream string stringstream strlen strncpy substr swap swap_ranges terminate tolower toupper transform type_info unexpected uninitialized_copy unique unique_copy unique_ptr unitbuf unordered_map unordered_multimap unordered_multiset unordered_set upper_bound uppercase valarray value_comp value_type vector wcerr wcin wcout wfilebuf wfstream wifstream wiostream wistream wofstream wregex  CPP_USER T N Any fout fin regex regex_match cmatch smatch regex_search regex_replace algorithm  STANDARD_C_I_O clearerr fclose feof ferror fflush fgetc fgetpos fgets fopen fprintf fputc fputs fread freopen fscanf fseek fsetpos ftell fwrite getc getchar gets perror printf putc putchar puts remove rename rewind scanf setbuf setvbuf sprintf sscanf tmpfile tmpnam ungetc vprintf vfprintf vsprintf  STANDARD_C_STRING_AND_CHARACTER atof atoi atol isalnum isalpha iscntrl isdigit isgraph islower isprint ispunct isspace isupper isxdigit memchr memcmp memcpy memmove memset strcat strchr strcmp strcoll strcpy strcspn strerror strlen strncat strncmp strncpy strpbrk strrchr strspn strstr strtod strtok strtol strtoul strxfrm tolower toupper 
syn keyword cppStructure	class typename template namespace
syn keyword cppNumber		NPOS std
syn keyword cppNumber	    a b c d e f g h i j k l m n o p q r s t u v w x y z
syn keyword cppNumber	    A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
syn keyword cppBoolean		true false


" The minimum and maximum operators in GNU C++
syn match cppMinMax "[<>]?"

" Default highlighting
if version >= 508 || !exists("did_cpp_syntax_inits")
  if version < 508
    let did_cpp_syntax_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif
  HiLink cppAccess		cppStatement
  HiLink cppCast		cppStatement
  HiLink cppExceptions		Exception
  HiLink cppOperator		Operator
  HiLink cppStatement		Statement
  HiLink cppType		Type
  HiLink cppStorageClass	StorageClass
  HiLink cppStructure		Structure
  HiLink cppNumber		Number
  HiLink cppBoolean		Boolean
  delcommand HiLink
endif

let b:current_syntax = "cpp"

" vim: ts=8
