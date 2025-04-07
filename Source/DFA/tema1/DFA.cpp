#include "DFA.h"
#include "NFA.h"
#include<iostream>
#include<fstream>

DFA::DFA
(
    const std::unordered_set<std::string>& states, 
    const std::unordered_set<char>& alphabet, 
    const std::unordered_map<std::pair<std::string, char>, std::string, pairHash>& transitionFunction, 
    const std::string& startState,
    const std::unordered_set<std::string>& endStates
)
    : states(states), alphabet(alphabet), transitionFunction(transitionFunction), startState(startState), endStates(endStates) {}
void DFA::PrintAutomaton()
{
    std::cout << "   q  || ";
    for (auto const& it : alphabet)
    {
        std::cout << it << "   ";
    }
    std::cout << "\n";
    std::cout << "------++";
    for (int i = 0; i < alphabet.size() * 4; i++)
    {
        std::cout << "-";
    }
    std::cout << "\n";
    for (auto const& state : states)
    {
        if (state== startState)
        {
            std::cout << "->";
            std::cout << " " << state << " || ";
        }
        else if (endStates.find(state) != endStates.end())
        {
            std::cout << " *";
            std::cout << " " << state << " || ";
        }else{ 
            std::cout << "   " << state << " || ";
        }
        
        for (char symbol : alphabet) {
            auto it = transitionFunction.find({ state, symbol });
            if (it != transitionFunction.end()) {
                std::cout <<  it->second<<"  ";
            }
            else {
                std::cout <<  "-"<<"   ";
            }
        }
        std::cout << '\n';
    }

}

void DFA::WriteAutomatonToFile(const std::string& file)
{
    std::ofstream outputFile(file);
    if (!outputFile.is_open()) {
        std::cerr << "Fisierul nu a putut fi deschis!";
        return;
    }

    outputFile << "   q  || ";
    for (auto const& it : alphabet) 
    {
        outputFile << it << "   ";
    }
    outputFile << "\n";
    outputFile << "------++";

    for (int i = 0; i < alphabet.size() * 4; i++) {
        outputFile << "-";
    }
    outputFile << "\n";
    for (auto const& state : states) {
        if (state == startState) {
            outputFile << "->";
            outputFile << " " << state << " || ";
        }
        else if (endStates.find(state) != endStates.end()) {
            outputFile << " *";
            outputFile << " " << state << " || ";
        }
        else {
            outputFile << "   " << state << " || ";
        }

        for (char symbol : alphabet) {
            auto it = transitionFunction.find({ state, symbol });
            if (it != transitionFunction.end()) {
                outputFile << it->second << "  ";
            }
            else {
                outputFile << "-" << "   ";
            }
        }
        outputFile << '\n';
    }

    outputFile.close();
    std::cout << "DFA scris in " << file << ".\n";
}

bool DFA::CheckWord(const std::string& word)
{
    std::string currentState = startState;

    for (char letter : word) {
        if (alphabet.find(letter) == alphabet.end()) {
            std::cout << "Litera '" << letter << "' nu apartine alfabetului.\n";
            return false;
        }

        auto transition = transitionFunction.find({ currentState, letter });
        if (transition == transitionFunction.end()) {
            std::cout << "Nu exista tranzitie pentru starea '" << currentState << "' in litera '" << letter << "'.\n";
            return false;
        }

        currentState = transition->second;
    }

    if (endStates.find(currentState) != endStates.end()) {
        std::cout << "Cuvantul '" << word << "' este acceptat de AFD.";
        return true;
    }
    else {
        std::cout << "Cuvantul '" << word << "' nu este acceptat de AFD.";
        return false;
    }
}
DFA DFA::TransformNFAToDFA(NFA resultNFA)
{
    DFA resultDFA;

    //mapa care asociaza multimea de stari din NFA cu o stare din DFA
    std::unordered_map<std::unordered_set<std::string>, std::string, hashUnorderedSet> functionsNFA_DFA;

    //lambda inchiderea pentru starea initiala a NFA ului
    auto initialClosure = resultNFA.GetLambdaClosure({ resultNFA.GetStartState()});

    resultDFA.startState = "q0";

    //pune in mapa setul de stari din NFA corespunzator cu o stare din DFA
    functionsNFA_DFA[initialClosure] = resultDFA.startState;

    resultDFA.states.insert({ resultDFA.startState });

    //verificam daca starea initiala creata contine o stare finala a NFA ului
    if (initialClosure.find(resultNFA.GetEndState()) != initialClosure.end()) {
        resultDFA.endStates.insert({ resultDFA.startState });
    }

    // Pasul 2: Procesăm stările și construim funcția de tranziție
    std::queue<std::unordered_set<std::string>> statesQueue;
    statesQueue.push(initialClosure);

    while (!statesQueue.empty()) {
        auto currentSetStates = statesQueue.front();
        statesQueue.pop();
        std::string currentState = functionsNFA_DFA[currentSetStates];

        for (char symbol : resultNFA.GetAlphabet()) {
            // Găsim toate stările accesibile prin simbol
            auto reachableStates = resultNFA.GetReachablesStates(currentSetStates, symbol);
            // Calculăm λ-închiderea pentru stările accesibile
            auto closure = resultNFA.GetLambdaClosure(reachableStates);

            if (!closure.empty()) {
                if (functionsNFA_DFA.find(closure) == functionsNFA_DFA.end()) {
                    // Creăm o stare nouă în AFD
                    std::string newStateName = "q" + std::to_string(functionsNFA_DFA.size());
                    functionsNFA_DFA[closure] = newStateName;
                    resultDFA.states.insert(newStateName);

                    // Verificăm dacă include o stare finală
                    if (closure.find(resultNFA.GetEndState()) != closure.end()) {
                        resultDFA.endStates.insert({ newStateName });
                    }

                    // Adăugăm starea în coadă pentru procesare
                    statesQueue.push(closure);
                }

                // Adăugăm tranziția
                resultDFA.transitionFunction[{currentState, symbol}] = functionsNFA_DFA[closure];
            }
        }
    }
    resultDFA.alphabet = resultNFA.GetAlphabet();
    return resultDFA;
}
bool DFA::VerifyStates()
{

    if (states.empty())
        return false;
    for (auto const& st : states)
    {
        if (states.find(startState) == states.end())
        {
            return false;
        }
        for (auto const& it : endStates)
        {
            if (endStates.find(it) == endStates.end())
                return false;
        }
    }
    return true;
}
bool DFA::VerifyFunction()
{
    for (auto const& elem : transitionFunction)
    {
        const auto& pairStateElem = elem.first;
        const auto& nextState = elem.second;

        if (states.find(pairStateElem.first) == states.end())
        {
            return false;
        }
        if (alphabet.find(pairStateElem.second) == alphabet.end())
        {
            return false;
        }
        if (states.find(nextState) == states.end())
        {
            return false;
        }
    }
    return true;
}
bool DFA::VerifyAutomaton()
{
    if (alphabet.empty())
    {
        return false;
    }
    if (!VerifyStates())
    {
        return false;
    }
    if (!VerifyFunction())
    {
        return false;
    }
    return true;
}



