#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<stdbool.h>

// Maximum character length for a dictionary or letter input
#define MAX_CHAR_LEN 52
#define MAX_LETTER_LEN 100000

void invalid_command(void);
void validate_commandline(int argc, char** argv);
void validate_letters(int argc, char** argv, int k);
void validate_dictionary(int argc, char** argv, int m);
void repetition_check(int argc, char** argv, int start, int argument);
int number_of_chars(char letters[], char c);
char* lower_case(char word[]);
void get_words(int argc, char** argv);
int argument_check(int argc, char** argv, int checkPosition);
void sort_words(int alpha, int len, int longest,
        int include, char includedLetter, char letter[], char dictionary[]);
int unjumble_check(char letters[], char dictWord[]);
int check_include(char c, char word[]);
int alpha_sort(const void* word1, const void* word2);
int len_sort(const void* word1, const void* word2);
void longest_sort(char** matchedList, int matches);
char** add_word(char** matchedList, char dictWord[], int matches);

// Handles the validation of input and prints out accordingly
int main(int argc, char** argv) {

    // Validates the command line returns an approprite error message
    // if it does not conform to the format required
    validate_commandline(argc, argv);

    // Prints out the words according to the arguments given
    get_words(argc, argv);

    exit(0);
}

// Validates the command given
// Throws an error if an invalid command is detected
void validate_commandline(int argc, char** argv) {
    if (argc > 6 || argc == 1) {
        invalid_command();
    }
    if (argument_check(argc, argv, 1) == 4) {
        if (argv[1][0] == '-') {
            invalid_command();
        }
        validate_letters(argc, argv, 1);
        return;
    }

    if (argc > 2 && (argument_check(argc, argv, 1) == 0
            || argument_check(argc, argv, 1) == 1
            || argument_check(argc, argv, 1) == 2)) {
        repetition_check(argc, argv, 1, 0);
        if (argument_check(argc, argv, 2) == 3) {
            if (strlen(argv[3]) != 1 || isalpha(argv[3][0]) == 0
                    || argc <= 4) {
                invalid_command();
            }
            repetition_check(argc, argv, 2, 1);
            validate_letters(argc, argv, 4);
            return;
        }
        validate_letters(argc, argv, 2);
        return;
    }
    
    if (argc > 3 && argument_check(argc, argv, 1) == 3) {
        repetition_check(argc, argv, 1, 1);
        if (strlen(argv[2]) != 1 || isalpha(argv[2][0]) == 0) {
            invalid_command();
        }
        if (argument_check(argc, argv, 3) == 0
                || argument_check(argc, argv, 3) == 1
                || argument_check(argc, argv, 3) == 2) {
            if (argc <= 4) {
                invalid_command();
            }
            repetition_check(argc, argv, 3, 0);
            validate_letters(argc, argv, 4);
            return;
        }
        validate_letters(argc, argv, 3);
        return;
    } 
    invalid_command();
}

// Prints an invalid mes:sage if 
// - an argument prior to the letters arguments begins with 
//   '-' but is not one of -alpha, -len, -longest or -include.
// - one of the valid option arguments apprears more than once
//   in the command line.
// - the argument following -include is not a single letter
// - the letters argument is not present
// - there are one or more arguments after the dictionary file name
// - 
void invalid_command(void) {
    fprintf(stderr, "Usage: unjumble [-alpha|-len|-longest]");
    fprintf(stderr, " [-include letter] letters [dictionary]\n");
    exit(1);
}

//Validates the letters argument
void validate_letters(int argc, char** argv, int k) {
    // Checks if letters argument exists by checking if the limit of the
    if (argc == (k - 1)) {
        invalid_command();
    }
    if (strlen(argv[k]) < 3) {
        fprintf(stderr, "unjumble: must supply at least three letters\n");
        exit(3);
    }
    for (int l = 0; l < strlen(argv[k]); l++) {
        if (isalpha(argv[k][l]) == 0) {
            fprintf(stderr,
                    "unjumble: can only unjumble alphabetic characters\n");
            exit(4);
        }
    }
    validate_dictionary(argc, argv, k);
}

// Validates the dictionary argument and checks
// if additional arguments are given
void validate_dictionary(int argc, char** argv, int m) {
    // Checks if dictionary argument is given
    if (argc == (m + 1)) {
        return;
    }
    if (argc > (m + 2)) {
        invalid_command();
    }
    FILE* dictionary = fopen(argv[m + 1], "r");

    // Checks if dictionary can be opened
    if (dictionary == 0) {
        fprintf(stderr,
                "unjumble: file \"%s\" can not be opened\n", argv[m + 1]);
        exit(2);
    }
}

// Checks for repetition of -alpha, -len, -longest or -include
// argument parameter can only be 1 - meaning its checking for include
// or 0 for the other case;
// start is the point from which duplicates should be checked
void repetition_check(int argc, char** argv, int start, int argument) {
    for (int i = start + 1; i < argc; i++) {
        // include case
        if (argument == 1 && argument_check(argc, argv, i) == 3) {
            if (argc == i + 1) {
                FILE* dictionary = fopen(argv[i], "r");
                if (dictionary == 0) {
                    invalid_command();
                }
                fclose(dictionary);
            } else {
                invalid_command();
            }
        }
        // For -alpha, -len and -longest cases
        if (argument == 0 && (argument_check(argc, argv, i) == 0
                || argument_check(argc, argv, i) == 1
                || argument_check(argc, argv, i) == 2)) {
            if (argc == i + 1) {
                FILE* dictionary = fopen(argv[i], "r");
                if (dictionary == 0) {
                    invalid_command();
                }
                fclose(dictionary);
            } else {
                invalid_command();
            }
        }
    }
}

// reads the input of the function and returns the words
// that satisfy the condition
void get_words(int argc, char** argv) {
    int alpha, len, longest, include;
    alpha = len = longest = include = 0;
    char letters[MAX_LETTER_LEN];
    char defaultDictionary[MAX_CHAR_LEN] = "/usr/share/dict/words";
    char dictionary[MAX_CHAR_LEN] = "";
    char includedLetter = '-';

    if (argument_check(argc, argv, 1) == 4) {
        if (argc == 3) {
            strcpy(letters, argv[1]);
            strcpy(dictionary, argv[2]);
        } else {
            strcpy(letters, argv[1]);
        }
    } else {
        for (int i = 1; i < argc; i++) {
            if (argument_check(argc, argv, i) == 4) {
                if (i + 1 == argc) {
                    strcpy(letters, argv[i]);
                } else {
                    strcpy(letters, argv[i]);
                    strcpy(dictionary, argv[i + 1]);
                    break;
                }
            }
            if (argument_check(argc, argv, i) == 0) {
                alpha = 1;
            } else if (argument_check(argc, argv, i) == 1) {
                len = 1;
            } else if (argument_check(argc, argv, i) == 2) {
                longest = 1;
            } else if (argument_check(argc, argv, i) == 3) {
                include = 1;
                includedLetter = argv[i + 1][0];
                i++;
            }
        }
    }
    if (strlen(dictionary) == 0) {
        sort_words(alpha, len, longest, include,
                includedLetter, letters, defaultDictionary);
    } else {
        sort_words(alpha, len, longest, include,
                includedLetter, letters, dictionary);
    }
}

// Sorts the inputed words based on the the given arguments
void sort_words(int alpha, int len, int longest,
        int include, char includedLetter, char letters[], char dictionary[]) {
    FILE* filePath = fopen(dictionary, "r");
    if (filePath == 0) {
        fprintf(stderr, "unjumble: file \"%s\" can not be opened", dictionary);
        exit(2); 
    }
    char dictWord[MAX_CHAR_LEN];
    int matches = 0;
    char** matchedList = (char**)malloc(sizeof(char*));
    while (fgets(dictWord, MAX_CHAR_LEN, filePath)) {
        if (include == 1) {
            if (unjumble_check(letters, dictWord) == 0
                    && check_include(includedLetter, dictWord) == 0) {
                matchedList = add_word(matchedList, dictWord, matches);
                matches++;
            }
        } else {
            if (unjumble_check(letters, dictWord) == 0) {
                matchedList = add_word(matchedList, dictWord, matches);
                matches++;
            }
        }
    }
    if (matches == 0) {
        exit(10);
    } else if (alpha == 1) {
        qsort(matchedList, matches, sizeof(char*), alpha_sort);
    } else if (len == 1 || longest == 1) {
        qsort(matchedList, matches, sizeof(char*), alpha_sort);
        qsort(matchedList, matches, sizeof(char*), len_sort);
    }
    for (int i = 0; i < matches; i++) {
        if (longest == 1) {
            longest_sort(matchedList, matches);
            break;
        }
        fprintf(stdout, "%s", matchedList[i]);
    }
    fclose(filePath);
}

// Returns 0 if the word should be outputed, 1 otherwise
int unjumble_check(char letters[], char dictWord[]) {
    if (strlen(dictWord) < 4) {
        return 1;
    }
    char* lowercaseLetters = lower_case(strdup(letters));
    char* lowercaseDictWord = lower_case(strdup(dictWord));
    for (int i = 0; i < strlen(lowercaseDictWord) - 1; i++) {
        if (number_of_chars(lowercaseDictWord, lowercaseDictWord[i])
                > number_of_chars(lowercaseLetters, lowercaseDictWord[i])) {
            return 1;
        } else if (number_of_chars(lowercaseLetters, lowercaseDictWord[i]) 
                == 0) {
            return 1;
        }
    }
    return 0;
}

// Checks for what type of argument is given on the position given
int argument_check(int argc, char** argv, int checkPosition) {
    if (checkPosition >= argc) {
        return -1;
    } else if (strcmp(argv[checkPosition], "-alpha") == 0) {
        return 0;
    } else if (strcmp(argv[checkPosition], "-len") == 0) {
        return 1;
    } else if (strcmp(argv[checkPosition], "-longest") == 0) {
        return 2;
    } else if (strcmp(argv[checkPosition], "-include") == 0) {
        return 3;
    } else {
        return 4;
    }
}

// Calculates the number of chars
int number_of_chars(char word[], char c) {
    int count = 0;
    for (int i = 0; i < strlen(word); i++) {
        if (word[i] == c) {
            count++;
        }
    }
    return count;
}

// Changes an array to all lower case
char* lower_case(char word[]) {
    for (int i = 0; i < strlen(word); i++) {
        word[i] = (char)tolower((int)word[i]);
    }
    return word;
}

// // Checks if the word given contains all letters
// int is_letters(int argc, char* word) {
//     for (int i = 0; i < argc; i++) {
//         if (isalpha(word[i]) == 0) {
//             return 0;
//         }
//     }
//     return 1;
// }
// Checks if the include character given is in the word
// returns 0 if true false otherwise
int check_include(char c, char word[]){
    char lowercaseC = tolower(c);
    char* lowercaseWord = lower_case(strdup(word));
    for (int i = 0; i < strlen(lowercaseWord); i++) {
        if (lowercaseWord[i] == lowercaseC) {
            return 0;
        }
    }
    return 1;
}

// Sorts the words lexographically
int alpha_sort(const void* word1, const void* word2) {
    char* firstWord = *((char**) word1);
    char* secondWord = *((char**) word2);   
    return strcasecmp(firstWord, secondWord);
}

// Sorts the words according to length of the word
int len_sort(const void* word1, const void* word2) {
    char* firstWord = *((char**) word1);
    char* secondWord = *((char**) word2);
    int sizeOfFirstWord = strlen(firstWord);
    int sizeOfSecondWord = strlen(secondWord);
    if (sizeOfFirstWord == sizeOfSecondWord) {
        return strcasecmp(firstWord, secondWord);
    }
    return strlen(secondWord) - strlen(firstWord);
}

// Prints out the longest words that are returned by
// the unjumble letters
void longest_sort(char** matchedList, int matches) {
    for (int i = 0; i < matches; i++) {
        if (strlen(matchedList[0]) == strlen(matchedList[i])) {
            fprintf(stdout, "%s", matchedList[i]);
        }
    }
}

// Adds words to the list of unjumbled letters
char** add_word(char** matchedList, char dictWord[], int matches) {
    matchedList 
            = realloc(matchedList, (matches + 1) * sizeof(char*)); 
    matchedList[matches]
            = (char*)malloc(strlen(dictWord) * sizeof(char)); 
    matchedList[matches] = strdup(dictWord);
    return matchedList;
}
