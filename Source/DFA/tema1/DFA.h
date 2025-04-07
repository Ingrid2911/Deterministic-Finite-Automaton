#pragma once
#include<unordered_set>
#include<unordered_map>
#include<string>
#include<queue>
#include"NFA.h"
struct pairHash {
	template <typename T1, typename T2>
	std::size_t operator ( ) (const std::pair<T1, T2>& p) const {
		auto h1 = std::hash<T1>{}(p.first);
		auto h2 = std::hash<T2>{}(p.second);
		return h1 ^ h2; // combinăm hash-urile pentru pair
	}
};
struct hashUnorderedSet {
	template <typename T>
	std::size_t operator()(const std::unordered_set<T>& set) const {
		std::size_t hashValue = 0;
		for (const auto& elem : set) {
			hashValue ^= std::hash<T>{}(elem)+0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
		}
		return hashValue;
	}
};
class DFA 
{
private:
	//multimea de stari (Q)
	std::unordered_set<std::string> states;

	//alfabetul (Σ)
	std::unordered_set<char> alphabet;

	//starea initiala
	std::string startState;

	//multimea de stari finale
	std::unordered_set<std::string> endStates;

	//functia de tranzitie
	std::unordered_map<std::pair<std::string, char>, std::string, pairHash>  transitionFunction;

	
public:
	bool VerifyAutomaton();

	//constructor default
	DFA() = default;

	//constructor cu parametrii
	DFA(
		const std::unordered_set<std::string>& states,
		const std::unordered_set<char>& alphabet,
		const std::unordered_map<std::pair<std::string, char>, std::string, pairHash>& transitionFunction,
		const std::string& startState,
		const std::unordered_set<std::string>& endStates
	);

	//afisare automat
	void PrintAutomaton();

	//afisare automat in consola
	void WriteAutomatonToFile(const std::string& file);
	
	//verificare cuvant
	bool CheckWord(const std::string& word);

	//transformare NFA-DFA
	DFA TransformNFAToDFA(NFA resultNFA);

private:
	bool VerifyStates();
	bool VerifyFunction();


	
};

