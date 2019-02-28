#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <QRegExp>
#include <QFile>
#include <QString>

bool isprecomment(char& symb) {
    if (symb == '/') {
        return true;
    }
    return false;
}

bool iscomment(QString& comment) {
    if (comment.compare("//") == 0) {
        return true;
    }
    return false;
}

bool islongcomment(QString& comment) {
    if (comment.compare("/*") == 0) {
        return true;
    }
    return false;
}

bool isclosedsymb(QString& comment) {
    if (comment.compare("*/") == 0) {
        return true;
    }
    return false;
}

bool isend(char& symb) {
    if ((symb == '\n') || (symb == '\0')) {
        return true;
    }
    return false;
}


void delete_till_end(QFile &file) {
    char symbol;
    file.getChar(&symbol);
    while(!file.atEnd()) {
        file.getChar(&symbol);
        if (isend(symbol)) {
            break;
        }
    }
}

void delete_till_closed_symb(QFile &file) {
    char symbol;
    QString closed_comment;
    file.getChar(&symbol);
    closed_comment.push_back(symbol);
    file.getChar(&symbol);
    closed_comment.push_back(symbol);
    while(!file.atEnd()) {
        file.getChar(&symbol);
        closed_comment[0] = closed_comment[1];
        closed_comment[1] = symbol;
        if (isclosedsymb(closed_comment)) {
            break;
        }
    }
}

char delete_one_comment(QFile &file) {
    char symbol = '\0';
    QString comm = "/\0";
    file.getChar(&symbol);
    comm.push_back(symbol);
    if (iscomment(comm)) {
        delete_till_end(file);
        file.getChar(&symbol);
        return symbol;
    }
    if (islongcomment(comm)) {
        delete_till_closed_symb(file);
        file.getChar(&symbol);
        return symbol;
    }
    return '\0';
}

enum States {
    State_Start,
    State_Skip,
    State_Slash_both,
    State_Slash,
    State_Finit_Dif
};


void text_preprocessing(const QString& filename1, const QString& filename2, const QString& filename3) {
    QFile f1, f2;
    QFile f3;
    char symbol1, symbol2;
    int64_t num_numatches = 0;
    QString comm1, comm2;
    f1.setFileName(filename1);
    f2.setFileName(filename2);
    f3.setFileName(filename3);

    if (!f1.open(QIODevice::ReadOnly)) {
        std::cout << "Unable to open file 1";
        exit(1);
    }
    if (!f2.open(QIODevice::ReadOnly)) {
        std::cout << "Unable to open file 2";
        exit(1);
    }
    if (!f3.open(QIODevice::WriteOnly)) {
        std::cout << "Unable to open file 3";
        exit(1);
    }

    States state = State_Start;
    while ((!f1.atEnd()) || (!f2.atEnd())) {
        switch (state) {
            case State_Start:
                f1.getChar(&symbol1);
                f2.getChar(&symbol2);
                if (isprecomment(symbol1) && isprecomment(symbol2)) {
                    state = State_Slash_both;
                } else if (isprecomment(symbol1) || isprecomment(symbol2)) {
                    state = State_Slash;
                } else if (symbol1 == symbol2) {
                    f3.putChar(symbol1);
                    state = State_Start;
                } else {
                    state = State_Skip;
                }
                break;
            case State_Skip:
                if (symbol1 == symbol2) {
                    f3.putChar(symbol1);
                    state = State_Start;
                } else if (iscntrl(symbol1) or isspace(symbol1)) {
                    f1.getChar(&symbol1);
                    state = State_Skip;
                } else if (iscntrl(symbol2) or isspace(symbol2)) {
                    f2.getChar(&symbol2);
                    state = State_Skip;
                } else if (symbol1 != symbol2) {
                    state = State_Finit_Dif;
                }
                break;
            case State_Slash_both:
                comm1 = symbol1;
                comm2 = symbol2;
                f1.getChar(&symbol1);
                f2.getChar(&symbol2);
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
}



// перенос строки \



// void tokenizer() {
//     f3.open(filename3);
//     if (!f3) {
//         std::cout << "Unable to open file 3";
//         exit(1);
//     }
//     QString str = "\0";
// }

int main(int argc, char *argv[]) {
    // std::ios_base::sync_with_stdio(false);
    // std::cin.tie(nullptr);
    QString filename1, filename2, filename3;
    if (argc == 4) {
        filename1 = argv[1];
        filename2 = argv[2];
        filename3 = argv[3];
    } else if (argc == 3) {
        filename1 = argv[1];
        filename2 = argv[2];
        filename3 = "orig.cpp";
    }
    text_preprocessing(filename1, filename2, filename3);


    // tokenizer();

    return 0;
}

