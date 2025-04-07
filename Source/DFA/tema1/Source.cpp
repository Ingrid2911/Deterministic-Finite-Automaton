#include<iostream>
#include<fstream>
#include"DFA.h"
#include"NFA.h"

bool CheckValidRegex(const std::string& expression) 
{
    int OpenParantheses = 0;
    bool lastWasOperator = false; // bool care = true daca ultimul caracter verificat din regex este un operator

    for (size_t i = 0; i < expression.size(); ++i) {
        char c = expression[i];

        // verificare caractere invalide
        if (isalnum(c) == false && c != '|' && c != '*' && c != '+' && c != '.' && c != '(' && c != ')') {
            return false;
        }

        // verificare paranteze lasate deschise
        if (c == '(') {
            OpenParantheses++;
        }
        else if (c == ')') {
            if (OpenParantheses == 0) {
                return false; // in caz ca nu s-a deschis paranteza mai intai
            }
            OpenParantheses--;
        }

        // verificare operatori * + ? | (in caz ca sunt la inceputul sau la finalul regexului)
        if (c == '*' || c == '+' || c == '.' || c == '|') {
            if (i == 0 || i == expression.size()-1) {
                return false;
            }
            lastWasOperator = true;
        } else {
            lastWasOperator = false; // resetat la fiecare caracter valid
        }
    }

    if (OpenParantheses == 0)
        return true;
    else
        return false;
}

int main()
{
	/*DFA M({ "q0","q1" ,"q2"}, {'a','b','c'}, {{{"q0",'a'},"q0"},{{"q0",'b'},"q1"},{{"q1",'a'},"q1"},{{"q1",'b'},"q0"}}, "q0", {"q1"});
	if (M.VerifyAutomaton())
		std::cout << "Automatul este valid";
	else std::cout << "Automat invalid!";
	std::cout <<std:: endl;
	M.PrintAutomaton();
	M.CheckWord("ababab");
	std::cout << "\n";*/
	NFA N;
	std::ifstream f("date.txt");
	std::string expression;
	f >> expression;
	f.close();
    if (CheckValidRegex(expression)) {
        std::vector<char> polishForm = N.GetPolishForm(expression);
        N = N.TransformRegexToNFA(polishForm);
        DFA result;
        result = result.TransformNFAToDFA(N);
        int menu = 1;
        while (menu != 0) {
            std::cout << "Selectati: \n1. Afisarea expresiei regulate\n2. Afisarea automatului\n3. Verificarea unui cuvant in automat\n0. Iesiti din meniu\n\n";
            std::cin >> menu;
            system("cls");
            switch (menu) {
            case 0: {
                return 0;
            }
            case 1: {
                std::cout << "Expresia introdusa: " << expression << "\n";
                std::cout << "Forma poloneza a expresiei: ";
                for (int i = 0; i < polishForm.size(); i++) {
                    std::cout << polishForm[i];
                }
                std::cout << "\n\n";
                break;
            }
            case 2: {
                result.PrintAutomaton();
                std::cout << "\n";
                result.WriteAutomatonToFile("iesire.txt");
                std::cout << "\n";
                break;
            }
            case 3: { //cuvant acceptat in exemplul nostru: aabbb
                std::string cuvant;
                std::cout << "Dati cuvantul: ";
                std::cin >> cuvant;
                result.CheckWord(cuvant);
                std::cout << "\n";
                break;
            }
            default: {
                std::cout << ("Eroare! Valoare invalida!");
                return -2;
            }
            }
        }
    }
    else {
        std::cout << ("Eroare! Regex invalid.");
        return -1;
    }
        return 0;
}