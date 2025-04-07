#include "NFA.h"
#include<iostream>
NFA NFA::TransformRegexToNFA(std::vector<char> polishForm)
{
	std::stack<NFA> NFAElements;
	int contor = 0;
	for (int i = 0; i < polishForm.size(); i++)
	{
		if (isalpha(polishForm[i]) != 0)
		{
			NFA letterNFA;

			//alfabet
			letterNFA.alphabet.insert(polishForm[i]);

			//starea initiala
			letterNFA.startState = "q" + std::to_string(contor);

			//starea finala
			std::string endState = "q" + std::to_string(contor + 1);
			letterNFA.endState=endState;

			//multimea de stari
			letterNFA.states.insert(letterNFA.startState);
			letterNFA.states.insert(endState);

			//functia de tranzitie
			letterNFA.transitionFunction[{letterNFA.startState, polishForm[i]}].insert( {endState} );

			NFAElements.push(letterNFA);


			contor = contor + 2;
		}
		else if (polishForm[i] == '.')
		{

			NFA NFA2 = NFAElements.top();
			NFAElements.pop();
			NFA NFA1 = NFAElements.top();
			NFAElements.pop();
			NFA NFA3;

			//concatenare alfabet
			NFA3.alphabet.insert(NFA1.alphabet.begin(), NFA1.alphabet.end());
			NFA3.alphabet.insert(NFA2.alphabet.begin(), NFA2.alphabet.end());

			//stare initiala
			NFA3.startState = NFA1.startState;

			//stare finala
			NFA3.endState = NFA2.endState;

			//functia de tranzitie
			std::unordered_map<std::pair<std::string, char>, std::unordered_set<std::string>, pairHashNFA> updatedTransitions;
			for (const auto& transition : NFA2.transitionFunction) {
				std::pair<std::string, char> newKey = transition.first;
				if (newKey.first == NFA2.startState) { // Presupunând că q2 este vechea stare inițială
					newKey.first = NFA1.endState; // Înlocuiește cu noua stare
				}

				updatedTransitions[newKey] = transition.second;
			}
			NFA2.transitionFunction = updatedTransitions;
			NFA3.transitionFunction.insert(NFA2.transitionFunction.begin(), NFA2.transitionFunction.end());
			NFA3.transitionFunction.insert(NFA1.transitionFunction.begin(), NFA1.transitionFunction.end());

			//multimea de stari
			NFA3.states.insert(NFA1.states.begin(), NFA1.states.end());
			if (NFA2.states.size() ==2)
				NFA3.states.insert(NFA2.endState);
			else
				NFA3.states.insert(NFA2.states.begin(), NFA2.states.end());

			//adaugare in stiva
			NFAElements.push(NFA3);

		}
		else if (polishForm[i] == '|')
		{
			NFA NFA2 = NFAElements.top();
			NFAElements.pop();
			NFA NFA1 = NFAElements.top();
			NFAElements.pop();
			NFA NFA3;

			//concatenare alfabet
			NFA3.alphabet.insert(NFA1.alphabet.begin(), NFA1.alphabet.end());
			NFA3.alphabet.insert(NFA2.alphabet.begin(), NFA2.alphabet.end());

			//stare initiala
			NFA3.startState= "q" + std::to_string(contor);

			//stare finala
			std::string endState = "q" + std::to_string(contor + 1);
			NFA3.endState=endState;

			//multime de stari
			NFA3.states.insert(NFA1.states.begin(), NFA1.states.end());
			NFA3.states.insert(NFA2.states.begin(), NFA2.states.end());
			NFA3.states.insert(NFA3.startState);
			NFA3.states.insert(NFA3.endState);

			//functia de tranzitie
			NFA3.transitionFunction.insert(NFA1.transitionFunction.begin(), NFA1.transitionFunction.end());
			NFA3.transitionFunction.insert(NFA2.transitionFunction.begin(), NFA2.transitionFunction.end());
			NFA3.transitionFunction[{ NFA3.startState, lambda }].insert({ NFA1.startState });
			NFA3.transitionFunction[{ NFA3.startState, lambda }].insert({ NFA2.startState });
			NFA3.transitionFunction[ { NFA1.endState,lambda }].insert({NFA3.endState } );
			NFA3.transitionFunction[{ NFA2.endState,lambda }].insert( {NFA3.endState} );
			
			NFAElements.push(NFA3);
			contor = contor + 2;
		}
		else if (polishForm[i] == '*')
		{
			NFA NFA1 = NFAElements.top();
			NFAElements.pop();
			NFA NFA2;


			//alfabet
			NFA2.alphabet = NFA1.alphabet;

			//starea initiala
			NFA2.startState = "q" + std::to_string(contor);

			//stare finala
			std::string endState = "q" + std::to_string(contor + 1);
			NFA2.endState = endState;

			//multime de stari
			NFA2.states.insert(NFA2.startState);
			NFA2.states.insert(NFA2.endState);
			NFA2.states.insert(NFA1.states.begin(), NFA1.states.end());

			//functia de tranzitie
			NFA2.transitionFunction[{ NFA2.startState,lambda }].insert ({NFA1.startState} );
			NFA2.transitionFunction[{ NFA2.startState, lambda }].insert({ NFA2.endState });
			NFA2.transitionFunction[ { NFA1.endState,lambda }].insert( {NFA2.endState} );
			NFA2.transitionFunction[{ NFA1.endState,lambda }].insert( {NFA1.startState} );
			NFA2.transitionFunction.insert(NFA1.transitionFunction.begin(), NFA1.transitionFunction.end());

			NFAElements.push(NFA2);
			contor = contor + 2;
		}
	}
	if (NFAElements.empty()) {
		throw std::runtime_error("NFAElements stack is empty. Check the input polishForm.");
	}
	return NFAElements.top();
	
		
}	
NFA::NFA(
	const std::set<std::string>& states,
	const std::unordered_set<char>& alphabet,
	const std::unordered_map<std::pair<std::string, char>, std::unordered_set<std::string>, pairHashNFA>& transitionFunction,
	const std::string& startState,
	const std::string& endStates	
)
	: states(states), alphabet(alphabet), transitionFunction(transitionFunction), startState(startState), endState(endState) {}

void NFA::PrintNFA()
{
	std::unordered_set<char> alphabetLambda = alphabet;
	alphabetLambda.insert(lambda);;
	std::cout << "   q    || ";
	for (auto const& it : alphabetLambda)
	{
		std::cout << it << "   ";
	}
	std::cout << "\n";
	std::cout << "--------++";
	for (int i = 0; i < alphabet.size() * 5; i++)
	{
		std::cout << "-";
	}
	std::cout << "\n";
	for (auto const& state : states)
	{
		if (state == startState)
		{
			std::cout << "->";
			std::cout << " " << state << "   || ";
		}
		else if(state==endState)
		{
			std::cout << " *";
			if (state.length() > 2)
			std::cout << " " << state << "  || ";
			else std::cout << " " << state << "   || ";
		}
		else {
			if(state.length()>2)
			std::cout << "   " << state << "  || ";
			else std::cout << "   " << state << "   || ";
		}
		
		
		for (char symbol : alphabetLambda) {
			auto it = transitionFunction.find({ state, symbol });
			if (it != transitionFunction.end()) {
				for (const auto& targetState : it->second) {
					std::cout << targetState << "  ";
				}
			}
			else {
				std::cout << "-" << "   ";
			}
		}
		std::cout << '\n';
	}

}

std::vector<char> NFA::GetPolishForm(std::string expression)
{
	return PolishForm(expression);
}

std::unordered_set<std::string> NFA::GetLambdaClosure(const std::unordered_set<std::string> states)
{
	return CalculateLambdaClosure(states);
}

std::unordered_set<std::string> NFA::GetReachablesStates(const std::unordered_set<std::string> states, char symbol)
{
	return ReachablesStates(states,symbol);
}

std::string NFA::GetStartState()
{
	return startState;
}

std::string NFA::GetEndState()
{
	return endState;
}

std::unordered_set<char> NFA::GetAlphabet()
{
	return alphabet;
}



std::vector<char> NFA::PolishForm(std::string expresion)
{
	std::vector<char> polishForm;
	std::stack<char> operatori;
	for (auto current : expresion)
	{
		if (isalpha(current) || isdigit(current))
		{
			polishForm.push_back(current);
		}
		else
		{
			if (current == '(')
			{
				operatori.push(current);
			}
			else
			{
				if (current == ')')
				{
					while (operatori.top() != '(' && !operatori.empty())
					{
						polishForm.push_back(operatori.top());
						operatori.pop();
					}
					if (!operatori.empty())
					{
						operatori.pop();
					}
				}

				else if (current == '|' || current == '.' || current == '*')
				{
					while (!operatori.empty() && OperatorPriority(operatori.top()) >= OperatorPriority(current))
					{
						polishForm.push_back(operatori.top());
						operatori.pop();
					}
					operatori.push(current);
				}
			}
		}
	}
	while (!operatori.empty())
	{
		polishForm.push_back(operatori.top());
		operatori.pop();
	}
	return polishForm;
}

int NFA::OperatorPriority(char op)
{
	if (op == '(' || op == ')')
	{
		return 0;
	}
	if (op == '|')
	{
		return 1;
	}
	else if (op == '.')
	{
		return 2;
	}
	else if (op == '*')
	{
		return 3;
	}
}

std::unordered_set<std::string> NFA::CalculateLambdaClosure(const std::unordered_set<std::string> states)
{
	//adaugam in setul de lambda inchideri setul de stari pentru care vrem sa calculam lambda inchiderea
	std::unordered_set<std::string> lambdaClosure = states;

	//starile din states care trebuiesc procesate pe rand
	std::deque<std::string> statesDeque(states.begin(), states.end());
	std::stack<std::string> statesInProcess(statesDeque);
	
	while (!statesInProcess.empty())
	{
		std::string currentState = statesInProcess.top();
		statesInProcess.pop();

		//verificam toate tranzitiile lambda din starea curenta
		auto transition = transitionFunction.find({ currentState,lambda });
		if (transition != transitionFunction.end()) {
			for (const std::string& state : transition->second) {
				if (lambdaClosure.find(state) == lambdaClosure.end()) { 
					lambdaClosure.insert(state);
					statesInProcess.push(state); 
				}
			}
		}
	
	}
	return lambdaClosure;
	
}

std::unordered_set<std::string> NFA::ReachablesStates(const std::unordered_set<std::string>& states, char symbol)
{
	//multimea de stari accesibile
	std::unordered_set<std::string> reachablesStates;

	//iteram prin fiecare stare din states
	for (const std::string& state : states)
	{
		//cautam tranzitiile pentru simbolul specificat si starea curenta
		auto transition = transitionFunction.find({ state,symbol });
		if (transition != transitionFunction.end())
		{
			//adaugam starea rezultata din tranzitie
			reachablesStates.insert(transition->second.begin(), transition->second.end());
		}

	}
	return reachablesStates;
}

