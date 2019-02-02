#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

bool isprecomment(char symb) {
    if (symb == '/') {
        return true;
    }
    return false;
}

bool iscomment(std::string comment) {
    if (comment.compare("//") == 0) {
        return true;
    }
    return false;
}

bool islongcomment(std::string comment) {
    if (comment.compare("/*") == 0) {
        return true;
    }
    return false;
}

bool isclosedsymb(std::string comment) {
    if (comment.compare("*/") == 0) {
        return true;
    }
    return false;
}

bool isend(char symb) {
    if ((symb == '\n') || (symb == '\0')) {
        return true;
    }
    return false;
}

void delete_till_end(std::istream &file) {
    char symbol;
    file.get(symbol);
    while(!file.eof()) {
        file.get(symbol);
        if (isend(symbol)) {
            break;
        }
    }
}

void delete_till_closed_symb(std::istream &file) {
    char symbol;
    std::string closed_comment;
    file.get(symbol);
    closed_comment.push_back(symbol);
    file.get(symbol);
    closed_comment.push_back(symbol);
    while(!file.eof()) {
        file.get(symbol);
        closed_comment[0] = closed_comment[1];
        closed_comment[1] = symbol;
        if (isclosedsymb(closed_comment)) {
            break;
        }
    }
}

char delete_one_comment(std::istream &file) {
    char symbol = '\0';
    std::string comm = "/\0";
    file.get(symbol);
    comm.push_back(symbol);
    if (iscomment(comm)) {
        delete_till_end(file);
        file.get(symbol);
        return symbol;
    }
    if (islongcomment(comm)) {
        delete_till_closed_symb(file);
        file.get(symbol);
        return symbol;
    }
    return '\0';
}



// перенос строки \

enum States {
    State_Start,
    State_Skip,
    State_Slash_both,
    State_Slash,
    State_Finit_Dif
};


int main() {
    // std::ios_base::sync_with_stdio(false);
    // std::cin.tie(nullptr);
    std::ifstream f1, f2;
    std::ofstream f3;
    char symbol1, symbol2;
    int64_t num_numatches = 0;
    std::string comm1, comm2, filename1, filename2, filename3;
    filename1 = "/Users/albinaakhmetgareeva/documents/магистратура/cpp/friuts.cpp";
    filename2 = "/Users/albinaakhmetgareeva/documents/магистратура/cpp/friuts_comm.cpp";
    filename3 = "/Users/albinaakhmetgareeva/documents/магистратура/cpp/friuts_orig.cpp";
    f1.open(filename1);
    if (!f1) {
        std::cout << "Unable to open file 1";
        exit(1);
    }
    f2.open(filename2);
    if (!f2) {
        std::cout << "Unable to open file 2";
        exit(1);
    }
    f3.open(filename3);
    if (!f3) {
        std::cout << "Unable to open file 3";
        exit(1);
    }
    States state = State_Start;
    while ((!f1.eof()) || (!f2.eof())) {
        switch (state) {
            case State_Start:
                f1.get(symbol1);
                f2.get(symbol2);
                if (isprecomment(symbol1) && isprecomment(symbol2)) {
                    state = State_Slash_both;
                } else if (isprecomment(symbol1) || isprecomment(symbol2)) {
                    state = State_Slash;
                } else if (symbol1 == symbol2) {
                    f3 << symbol1;
                    state = State_Start;
                } else {
                    state = State_Skip;
                }
                break;
            case State_Skip:
                if (symbol1 == symbol2) {
                    f3 << symbol1;
                    state = State_Start;
                } else if (iscntrl(symbol1) or isspace(symbol1)) {
                    f1.get(symbol1);
                    state = State_Skip;
                } else if (iscntrl(symbol2) or isspace(symbol2)) {
                    f2.get(symbol2);
                    state = State_Skip;
                } else if (symbol1 != symbol2) {
                    state = State_Finit_Dif;
                }
                break;
            case State_Slash_both:
                comm1 = symbol1;
                comm2 = symbol2;
                f1.get(symbol1);
                f2.get(symbol2);
                comm1 += symbol1;
                comm2 += symbol2;
                if (iscomment(comm1) && iscomment(comm2)) {
                    delete_till_end(f1);
                    delete_till_end(f2);
                    state = State_Start;
                } else if (iscomment(comm1) && islongcomment(comm2)) {
                    delete_till_end(f1);
                    delete_till_closed_symb(f2);
                    state = State_Start;
                } else if (islongcomment(comm1) && iscomment(comm2)) {
                    delete_till_end(f2);
                    delete_till_closed_symb(f1);
                    state = State_Start;
                } else {
                    state = State_Finit_Dif;
                }
                break;
            case State_Slash:
                if (isprecomment(symbol1)) {
                    symbol1 = delete_one_comment(f1);
                    if (symbol1 == '\0') {
                        state = State_Finit_Dif;
                    } else {
                        state = State_Skip;
                    }
                } else if (isprecomment(symbol2)) {
                    symbol2 = delete_one_comment(f2);
                    if (symbol2 == '\0') {
                        state = State_Finit_Dif;
                    } else {
                        state = State_Skip;
                    }
                } else {
                    state = State_Finit_Dif;
                }
                break;
            case State_Finit_Dif:
                num_numatches++;
                std::cout << "Unmatch\n";
                state = State_Start;
                if (num_numatches > 20) {
                    std::cout << "Program are not identical, number of unmatches:\n" << num_numatches << "\n";
                    f1.close();
                    f2.close();
                    f3.close();
                    return 0;
                }
                break;
        }
    }
    if (num_numatches > 0) {
        std::cout << "Program are not identical, number of unmatches:\n" << num_numatches << "\n";
    } else {
        std::cout << "Program are identical\n";
    }
    f1.close();
    f2.close();
    f3.close();
    return 0;
}
