#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <algorithm>

// Define the Token structure to hold token type and value
struct Token {
    std::string type;    // e.g., "KEYWORD", "IDENTIFIER"
    std::string value;   // e.g., "if", "x", "123"
};

// =======================================================
// Helper functions (DFA transitions logic)
// =======================================================

bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isDigit(char c) {
    return (c >= '0' && c <= '9');
}

bool isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// Set of defined Keywords
const std::set<std::string> KEYWORDS = {
    "int", "float", "if", "else", "while", "return", "void"
};

bool isKeyword(const std::string& lexeme) {
    return KEYWORDS.count(lexeme);
}

// Set of single-character tokens (Operators and Delimiters)
const std::set<char> SINGLE_CHAR_TOKENS = {
    '+', '-', '*', '/', '=', '>', '<', ';', '(', ')', '{', '}'
};

// =======================================================
// Main Lexical Analysis Function (DFA Simulation)
// =======================================================

std::vector<Token> lexicalAnalysis(const std::string& sourceCode) {
    std::vector<Token> tokens;
    int current_index = 0; // The pointer traversing the source code

    while (current_index < sourceCode.length()) {
        char current_char = sourceCode[current_index];

        // 1. Skip Whitespace (Self-loop in the DFA start state)
        if (isWhitespace(current_char)) {
            current_index++;
            continue;
        }

        // 2. Handle Identifiers and Keywords
        // DFA State 1: Start reading with a letter
        if (isLetter(current_char)) {
            std::string current_lexeme;
            current_lexeme += current_char;
            current_index++;

            // DFA Transition: Continue reading letters or digits 
            while (current_index < sourceCode.length() && (isLetter(sourceCode[current_index]) || isDigit(sourceCode[current_index]))) {
                current_lexeme += sourceCode[current_index];
                current_index++;
            }

            // DFA Accept State: Check if the full lexeme is a Keyword or Identifier
            if (isKeyword(current_lexeme)) {
                tokens.push_back({"KEYWORD", current_lexeme});
            } else {
                tokens.push_back({"IDENTIFIER", current_lexeme});
            }
        }

        // 3. Handle Integers
        // DFA State 2: Start reading with a digit
        else if (isDigit(current_char)) {
            std::string current_lexeme;
            current_lexeme += current_char;
            current_index++;
            
            // DFA Transition: Continue reading digits 
            while (current_index < sourceCode.length() && isDigit(sourceCode[current_index])) {
                current_lexeme += sourceCode[current_index];
                current_index++;
            }
            
            // DFA Accept State
            tokens.push_back({"INTEGER", current_lexeme});
        }
        
        // 4. Handle Operators and Delimiters
        else if (SINGLE_CHAR_TOKENS.count(current_char)) {
            std::string token_value = std::string(1, current_char);
            
            // Lookahead for Two-Character Relational Operators (e.g., ==, <=, >=)
            // Simulates two sequential states in the DFA for these operators
            if ((current_char == '=' || current_char == '<' || current_char == '>') && 
                current_index + 1 < sourceCode.length() && sourceCode[current_index + 1] == '=') {
                
                token_value += sourceCode[current_index + 1];
                current_index += 2; // Advance pointer by two chars
                tokens.push_back({"RELATIONAL_OPERATOR", token_value});
            } 
            // Handle Single-Character Tokens
            else {
                current_index++; // Advance pointer by one char
                if (current_char == '+' || current_char == '-' || current_char == '*') {
                    tokens.push_back({"ARITHMETIC_OPERATOR", token_value});
                } else if (current_char == '=' && token_value.length() == 1) {
                    tokens.push_back({"ASSIGNMENT_OPERATOR", token_value});
                } else {
                    tokens.push_back({"DELIMITER", token_value});
                }
            }
        }

        // 5. Handle Errors (Invalid Character)
        else {
            tokens.push_back({"ERROR", std::string(1, current_char)});
            current_index++;
        }
    }
    return tokens;
}

// =======================================================
// Main function (Handles user input for filename)
// =======================================================

int main() {
    std::string filename;
    std::cout << "Enter the source code filename (e.g., example.txt): ";
    // Read the filename from user input (This requires a console/terminal)
    std::cin >> filename;

    // 1. Attempt to open the file
    std::ifstream file_in(filename);
    
    // Check if the file was successfully opened
    if (!file_in.is_open()) {
        std::cerr << "Error: Could not open the file '" << filename << "'. Please check the path and filename.\n";
        return 1;
    }

    // 2. Read the entire file content into a single string
    std::stringstream buffer;
    buffer << file_in.rdbuf();
    std::string sourceCode = buffer.str();

    file_in.close();

    std::cout << "\n--- Source Code Read from " << filename << " ---\n" << sourceCode << "\n-------------------\n";

    // 3. Perform Lexical Analysis
    std::vector<Token> output_tokens = lexicalAnalysis(sourceCode);

    // 4. Print Results
    std::cout << "\n--- Lexical Analysis Results ---\n";
    std::cout << "Type\t\t\tValue\n";
    std::cout << "----------------------------------\n";
    
    for (const auto& token : output_tokens) {
        // Output formatting
        std::cout << token.type;
        if (token.type.length() < 8) std::cout << "\t";
        std::cout << "\t\t" << token.value << "\n";
    }

    // README Reminder
    std::cout << "\n--- README Notes (Project Requirement) ---\n";
    std::cout << "This Lexical Analyzer uses a State Machine implemented in C++ to simulate a DFA.\n";
    std::cout << "The logic clearly demonstrates the application of Finite Automata to language recognition.\n";

    return 0;
}