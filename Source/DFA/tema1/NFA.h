#pragma once
#include<set>
#include<unordered_set>
#include<unordered_map>
#include<string>
#include<stack>
struct pairHashNFA {
	template <typename T1, typename T2>
	std::size_t operator ( ) (const std::pair<T1, T2>& p) const {
		auto h1 = std::hash<T1>{}(p.first);
		auto h2 = std::hash<T2>{}(p.second);
		return h1 ^ h2; // combinăm hash-urile pentru pair
	}
};
class NFA
{
protected:
	//multimea de stari (Q)
	std::set<std::string> states;

	//alfabetul (Σ)
	std::unordered_set<char> alphabet;

	//tranzitie lambda
	char lambda = 'l';

	//starea initiala
	std::string startState;

	//multimea de stari finale
	std::string endState;

	//functia de tranzitie
	std::unordered_map<std::pair<std::string, char>, std::unordered_set<std::string>, pairHashNFA>  transitionFunction;

public:
	//transformare in NFA cu λ tranzitii
	NFA TransformRegexToNFA(std::vector<char> polishForm);

	//constructor cu parametrii
	NFA(
		const std::set<std::string>& states,
		const std::unordered_set<char>& alphabet,
		const std::unordered_map<std::pair<std::string, char>, std::unordered_set<std::string>, pairHashNFA>& transitionFunction,
		const std::string& startState,
		const std::string& endState
	);
	NFA() = default;
	void PrintNFA();
	std::vector<char> GetPolishForm(std::string expression);
	std::unordered_set<std::string> GetLambdaClosure(const std::unordered_set<std::string> states);
	std::unordered_set<std::string> GetReachablesStates(const std::unordered_set<std::string> states, char symbol);
	std::string GetStartState();
	std::string GetEndState();
	std::unordered_set<char> GetAlphabet();
private:
	std::vector<char> PolishForm(std::string expresion);
	int OperatorPriority(char op);
	std::unordered_set<std::string> CalculateLambdaClosure(const std::unordered_set<std::string> states);
	std::unordered_set<std::string> ReachablesStates(const std::unordered_set<std::string>& states, char symbol);

};


