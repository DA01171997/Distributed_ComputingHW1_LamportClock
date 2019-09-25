#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <regex>
#include <unordered_map>

struct Event
{
	std::string m_type;
	size_t m_order = 0;
	size_t m_LC = 0;
	Event(std::string input)
	{
		if (input == "NULL")
		{
			m_type = input;
		}
		else if (input.size() == 2)
		{
			m_type = input[0];
			m_order = std::atoi((&input[1]));
		}
		else if (input.size() == 1)
		{
			m_type = input[0];
		}
		else
		{
			throw std::invalid_argument("Not Supported Event Type");
		}
	}
	bool isSend() const
	{
		return (m_type == "s") ? true : false;
	}
	bool isReceive() const
	{
		return (m_type == "r") ? true : false;
	}
	bool isNULL() const
	{
		return (m_type == "NULL") ? true : false;
	}
	bool isInternal() const
	{
		return ((m_type == "r") || (m_type == "s") || isNULL()) ? false : true;
	}
};

struct Processor
{
	std::vector<Event> m_events;
	size_t getEventMessageSize() const
	{
		return m_events.size();
	}
};

struct LC_Calculator
{
	size_t m_processor_cursor = 0;
	size_t m_num_process, m_num_message;
	std::vector<size_t> m_event_cursor;
	std::vector<Processor>& m_processors;
	std::unordered_map<std::string, size_t> m_unordered_map;
	std::unordered_map<size_t, size_t> m_unordered_map_processor;

	LC_Calculator(std::vector<Processor> & processors, size_t num_process, size_t num_message) : m_processors(processors),
		m_num_process(num_process),
		m_num_message(num_message)
	{
		m_event_cursor.resize(num_process);
		for (size_t i = 0; i < num_process; i++) {
			m_unordered_map_processor.insert(std::make_pair(i, i));
		}
	}
	void calculate() {
		size_t temp = 0;
		bool done = false;
		while (!m_unordered_map_processor.empty())
		//while (!done)
		{
			for (size_t i = m_event_cursor[m_processor_cursor]; i < m_num_message; i++)
			{
				Event * event_pointer = &(m_processors[m_processor_cursor].m_events[i]);
				std::unordered_map<std::string, size_t>::iterator iter;
				std::string key;
				if (event_pointer->isNULL())
				{
				}
				else if ((i == 0) && ((event_pointer->isInternal()) || (event_pointer->isSend())))
				{
					if (event_pointer->isSend())
					{
						key = event_pointer->m_type + std::to_string(event_pointer->m_order);
						if (m_unordered_map.find(key) == m_unordered_map.end())
						{
							m_unordered_map.insert(std::make_pair(key, 1));
						}
					}
					event_pointer->m_LC = 1;
				}
				else if ((i == 0) && (event_pointer->isReceive()))
				{
					key = "s" + std::to_string(event_pointer->m_order);
					iter = m_unordered_map.find(key);
					if (iter != m_unordered_map.end()) {
						event_pointer->m_LC = iter->second + 1;
					}
					else {
						break;
					}
				}
				else if ((i != 0) && ((event_pointer->isInternal()) || (event_pointer->isSend())))
				{
					size_t k = m_processors[m_processor_cursor].m_events[i - 1].m_LC;
					key = event_pointer->m_type + std::to_string(event_pointer->m_order);
					iter = m_unordered_map.find(key);
					if (event_pointer->isSend())
					{
						if (iter == m_unordered_map.end())
						{
							m_unordered_map.insert(std::make_pair(key, k + 1));
						}
						else
						{
							iter->second = k + 1;
						}
					}
					event_pointer->m_LC = k + 1;
				}
				else if ((i != 0) && (event_pointer->isReceive()))
				{
					key = "s" + std::to_string(event_pointer->m_order);
					iter = m_unordered_map.find(key);
					if (iter != m_unordered_map.end())
					{
						size_t k = m_processors[m_processor_cursor].m_events[i - 1].m_LC;
						k = (k > iter->second) ? k : iter->second;
						event_pointer->m_LC = k + 1;
					}
					else {
						break;
					}
				}
				//std::cout << "_____-process cursor " << m_processor_cursor << " even cursor " << m_event_cursor[m_processor_cursor] << " i = " << i << std::endl;
				m_event_cursor[m_processor_cursor] = i;
				//std::cout << "process cursor " << m_processor_cursor << " even cursor " << m_event_cursor[m_processor_cursor] << " i = " << i << std::endl;
			}
			if (m_event_cursor[m_processor_cursor]==m_num_message-1)
			{
				std::unordered_map<size_t, size_t>::iterator iter = m_unordered_map_processor.find(m_processor_cursor);
				if (iter != m_unordered_map_processor.end()) 
				{
					m_unordered_map_processor.erase(m_processor_cursor);
				}
			}
			m_processor_cursor = (m_processor_cursor + 1) % m_num_process;
			/*temp++;
			if (temp > 30) {
				done = true;
			}*/
		}
		print_LC();
	}
	void print_LC() const
	{
		std::cout << "The Lamport Logical clock of this program is: " << std::endl;
		for (size_t i = 0; i < m_processors.size(); i++)
		{
			std::cout << "process " << i << ": ";
			for (size_t j = 0; j < m_processors[i].m_events.size(); j++)
			{
				std::cout << m_processors[i].m_events[j].m_LC << "\t";
			}
			std::cout << std::endl;
		}
		
	}
};

struct Program
{
	size_t m_num_process, m_num_message;
	std::vector<Processor> m_processors;
	Program(size_t num_process, size_t num_message) : m_num_process(num_process),
		m_num_message(num_message)
	{
		m_processors.resize(num_process);
	}
	void calculateLC()
	{
		LC_Calculator lc_calculator(m_processors, m_num_process, m_num_message);
		lc_calculator.calculate();
	}
};

std::ostream& operator << (std::ostream& stream, const Event& object)
{
	stream << "Event: " << object.m_type << object.m_order << std::endl;
	return stream;
}


std::ostream& operator << (std::ostream& stream, const Processor& object)
{
	for (size_t i = 0; i < object.m_events.size(); i++)
	{
		stream << object.m_events[i].m_type;
		if (object.m_events[i].m_order != 0)
		{
			stream << object.m_events[i].m_order;
		}
		stream << "\t";
	}
	return stream;
}

std::ostream& operator << (std::ostream& stream, const Program& object)
{
	for (size_t i = 0; i < object.m_processors.size(); i++)
	{
		stream << "process " << i << ": ";
		stream << object.m_processors[i] << std::endl;
	}
	return stream;
}

void parser(std::string & input, std::vector<Event> & v, size_t num_message)
{
	std::stringstream ss(input);
	std::string s;
	char delimiter = ' ';
	while (std::getline(ss, s, delimiter))
	{
		v.push_back(s);
	}

	if (v.size() < num_message)
	{
		std::string n = "NULL";
		for (size_t i = v.size(); i < num_message; i++)
		{
			v.push_back(n);
		}
	}
}

void print_usage_message()
{
	std::cout << "***Please be careful when entering the input.                                     ***" << std::endl;
	std::cout << "***Only one space between each process event and no space at the end of each line.***" << std::endl;
	std::cout << "***Please end the input with NULL if no more event.                               ***" << std::endl;
	std::cout << "***                                                                               ***" << std::endl;
	std::cout << "***   Example:                                                                    ***" << std::endl;
	std::cout << "***   Enter number of process:3<enter>                                            ***" << std::endl;
	std::cout << "***   Enter number of message:4<enter>                                            ***" << std::endl;
	std::cout << "***   Number of process-N=3                                                       ***" << std::endl;
	std::cout << "***   Number of message-M=4                                                       ***" << std::endl;
	std::cout << "***   Enter Process Events:a<space>s1<space>r3<space>b<enter>                     ***" << std::endl;
	std::cout << "***   Enter Process Events:c<space>r2<space>s3<space>NULL<enter>                  ***" << std::endl;
	std::cout << "***   Enter Process Events:r1<space>d<space>s2<space>e<enter>                     ***" << std::endl;
	std::cout << "***                                                                               ***" << std::endl;
	std::cout << "***                                                                               ***" << std::endl;
	std::cout << "***                                                                               ***" << std::endl;
	std::cout << std::endl << std::endl;
}

int main()
{

	size_t num_process = 0;
	size_t num_message = 0;
	std::string input;

	print_usage_message();
	std::cout << "Enter number of process:";
	std::cin >> num_process;
	std::cout << "Enter number of message:";
	std::cin >> num_message;
	std::cout << "Number of process-N=" << num_process << std::endl;
	std::cout << "Number of message-M=" << num_message << std::endl;

	/*
	cin.ignore();
	std::cout << "Enter Process Events: ";
	std::getline(std::cin, input);
	*/
	std::vector<std::string> vectS;
	std::string input1 = "a s1 r3 b s7";
	std::string input2 = "c r2 s3 NULL";
	std::string input3 = "r1 d s2 e";
	std::string input4 = "d s4 s5 NULL";
	std::string input5 = "r4 d r5 e r7 d";

	vectS.push_back(input1);
	vectS.push_back(input2);
	vectS.push_back(input3);
	vectS.push_back(input4);
	vectS.push_back(input5);

	Program program(num_process, num_message);
	for (size_t i = 0; i < program.m_processors.size(); i++)
	{
		parser(vectS[i], program.m_processors[i].m_events, num_message);
	}
	std::cout << program << std::endl;
	program.calculateLC();
	system("PAUSE");
	return 0;
}



