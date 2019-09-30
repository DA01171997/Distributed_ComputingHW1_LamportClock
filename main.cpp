#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <regex>
#include <unordered_map>
#include <map>


//Simple object event that hold each process event
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
	Event(size_t lc) :
		m_LC(lc)
	{
	}
	// check if event is send
	bool isSend() const
	{
		return (m_type == "s") ? true : false;
	}
	// check if event is receive
	bool isReceive() const
	{
		return (m_type == "r") ? true : false;
	}
	// check if event is null
	bool isNULL() const
	{
		return (m_type == "NULL") ? true : false;
	}
	// check if event is internal
	bool isInternal() const
	{
		return ((m_type == "r") || (m_type == "s") || isNULL()) ? false : true;
	}
};

//Simple object processor that holds vector of events
struct Processor
{
	std::vector<Event> m_events;
};


//Simple calculator object used to calculate lamport clock value for each event
struct LC_Calculator
{
	size_t m_processor_cursor = 0;
	size_t m_num_process, m_num_message;
	std::vector<size_t> m_event_cursor;
	std::vector<Processor>& m_processors;
	std::unordered_map<std::string, size_t> m_unordered_map_send;
	std::unordered_map<size_t, size_t> m_unordered_map_processor;

	LC_Calculator(std::vector<Processor> & processors, size_t num_process, size_t num_message) : 
		m_processors(processors),
		m_num_process(num_process),
		m_num_message(num_message)
	{
		m_event_cursor.resize(num_process);
		for (size_t i = 0; i < num_process; i++) {
			m_unordered_map_processor.insert(std::make_pair(i, i));
		}
	}

	//while the unordered map that still has processors to calculate it will
	//cycle through each processor and event to find the approciate LC value
	//if even is a receive it will find the appropriate send event else it will 
	//break out of the loop and cycle again til it could find it.
	void calculate() {
		//cycle through each processor of the program
		while (!m_unordered_map_processor.empty())
		{
			//cycle through each event of the processor
			//checking each rules
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
						if (m_unordered_map_send.find(key) == m_unordered_map_send.end())
						{
							m_unordered_map_send.insert(std::make_pair(key, 1));
						}
					}
					event_pointer->m_LC = 1;
				}
				else if ((i == 0) && (event_pointer->isReceive()))
				{
					key = "s" + std::to_string(event_pointer->m_order);
					iter = m_unordered_map_send.find(key);
					if (iter != m_unordered_map_send.end()) {
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
					iter = m_unordered_map_send.find(key);
					if (event_pointer->isSend())
					{
						if (iter == m_unordered_map_send.end())
						{
							m_unordered_map_send.insert(std::make_pair(key, k + 1));
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
					iter = m_unordered_map_send.find(key);
					if (iter != m_unordered_map_send.end())
					{
						size_t k = m_processors[m_processor_cursor].m_events[i - 1].m_LC;
						k = (k > iter->second) ? k : iter->second;
						event_pointer->m_LC = k + 1;
					}
					else {
						break;
					}
				}
				m_event_cursor[m_processor_cursor] = i;
			}
			//check if event is the last of the current processor, if it is
			//then remove that processor from the unordermap because it is finished.
			if (m_event_cursor[m_processor_cursor]==m_num_message-1)
			{
				std::unordered_map<size_t, size_t>::iterator iter = m_unordered_map_processor.find(m_processor_cursor);
				if (iter != m_unordered_map_processor.end()) 
				{
					m_unordered_map_processor.erase(m_processor_cursor);
				}
			}
			m_processor_cursor = (m_processor_cursor + 1) % m_num_process;
		}
	}
};
struct LC_Verifier
{
	struct Location {
		size_t m_processor_index;
		size_t m_message_index;
		std::map<size_t, size_t> m_map_location;
		Location(size_t processor_index, size_t message_index) : 
			m_processor_index(processor_index),
			m_message_index(message_index)
		{
			m_map_location[processor_index] = message_index;
		}
		Location() {}
	};
	size_t m_num_process, m_num_message;
	size_t number_to_process_counter = 0;
	size_t m_processor_cursor = 0;
	size_t m_send_counter = 1;
	std::vector<size_t> m_event_cursor;
	std::vector<Processor>& m_processors;
	std::unordered_map<std::string, size_t> m_unordered_map_send;
	std::unordered_map<size_t, size_t> m_unordered_map_processor;
	std::map<size_t,Location> m_map_receive;

	LC_Verifier(std::vector<Processor> & processors, size_t num_process, size_t num_message) :
		m_processors(processors),
		m_num_process(num_process),
		m_num_message(num_message)
	{
		m_event_cursor.resize(num_process);
		for (size_t i = 0; i < num_process; i++) {
			m_unordered_map_processor.insert(std::make_pair(i, i));
		}
	}

	bool verify()
	{
		std::map<size_t, Location>::iterator iter;
		size_t number_to_process = 0;
		size_t receive_counter = 1;

		for (size_t i = 0; i < m_num_process; i++)
		{
			size_t clock_value_counter = 1;
			for (size_t j = 0; j < m_num_message; j++)
			{
				if (m_processors[i].m_events[j].m_LC == 0)
				{
					m_processors[i].m_events[j].m_type = "NULL";
				}
				else if (clock_value_counter == m_processors[i].m_events[j].m_LC)
				{
					m_processors[i].m_events[j].m_type = "i";
				}
				else if (clock_value_counter > m_processors[i].m_events[j].m_LC)
				{
					m_processors[i].m_events[j].m_type = "Error";
					return false;
				}
				else
				{
					m_processors[i].m_events[j].m_type = "r";
					clock_value_counter = m_processors[i].m_events[j].m_LC;
					size_t k_value = m_processors[i].m_events[j].m_LC - 1;
					if (j != 0)
					{
						size_t k_value_element_before = m_processors[i].m_events[j - 1].m_LC;
						k_value = (k_value > k_value_element_before) ? k_value : k_value_element_before;
					}
					iter = m_map_receive.find(k_value);
					if (iter == m_map_receive.end())
					{
						m_map_receive[k_value] = Location(i, j);
					}
					else
					{
						iter->second.m_map_location[i] = j;
					}
					
				}
				clock_value_counter++;
			}
		}
		number_to_process += m_map_receive.size();
		while (number_to_process_counter < number_to_process)
		{	
			for (size_t i = 0; i < m_num_process; i++)
			{
				for (size_t j = 0; j < m_num_message; j++)
				{
					iter = m_map_receive.begin();
					if (iter != m_map_receive.end()) {
						if (iter->first == m_processors[i].m_events[j].m_LC)
						{
							if (iter->second.m_processor_index != i)
							{
								m_processors[i].m_events[j].m_type = "s";
								m_processors[i].m_events[j].m_order = m_send_counter;
								m_send_counter++;

								std::map<size_t, size_t>::iterator iter2 = iter->second.m_map_location.begin();
								for (; iter2 != iter->second.m_map_location.end(); iter2++) 
								{
									m_processors[iter2->first].m_events[iter2->second].m_order = receive_counter;
								}
								receive_counter++;
								m_map_receive.erase(iter->first);
							}
						}
					}

				}
			}
			number_to_process_counter++;
		}
		if (m_map_receive.empty()) {
			return true;
		}
		return false;
	}
};

//Simple program object that used to hold vectors of processors
struct Program
{
	size_t m_num_process, m_num_message;
	std::vector<Processor> m_processors;
	Program(size_t num_process, size_t num_message) : 
		m_num_process(num_process),
		m_num_message(num_message)
	{
		m_processors.resize(num_process);
	}
	//create an  LC calculator object then calculate all events LC values.
	void calculateLC()
	{
		LC_Calculator lc_calculator(m_processors, m_num_process, m_num_message);
		lc_calculator.calculate();
	}
	//print function that used to print all LC value of each processor
	void print_LC() const
	{
		std::cout << "The Lamport Clock output of this program is: " << std::endl;
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

	bool verifyLC()
	{
		LC_Verifier lc_verifier(m_processors, m_num_process, m_num_message);
		if (!lc_verifier.verify()) {
			std::cout << "The output is INCORRECT!" << std::endl;
			return false;
		}
		else 
		{
			std::cout << "i=Internal Event" << std::endl;
			std::cout << "The possible output is: " << std::endl;
			return true;
		}
	}

};


//overloading "<<" function that used to print the object more easily 
//{
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
//}

//parser function that will parse out spaces between user input
//then insert the input into the given vector of event. 
void parser(std::string & input, std::vector<Event> & v, size_t num_message, std::string type)
{
	std::stringstream ss(input);
	std::string s;
	char delimiter = ' ';
	while (std::getline(ss, s, delimiter))
	{
		if (type == "LC")
		{
			v.push_back(s);
		}
		else if (type == "VERIFY")
		{
			v.push_back(std::stoi(s));
		}
	}

	if (v.size() < num_message)
	{
		if (type == "LC")
		{
			std::string n = "NULL";
			for (size_t i = v.size(); i < num_message; i++)
			{
				v.push_back(n);
			}
		}
		else if (type == "VERIFY")
		{
			v.push_back(0);
		}
	}
}

//print ouf instruction on how to use. 
void print_usage_message()
{
	std::cout << "***Please be careful when entering the input.                                     ***" << std::endl;
	std::cout << "***Only one space between each process event and no space at the end of each line.***" << std::endl;
	std::cout << "***Please end the input with NULL if no more event.                               ***" << std::endl;
	std::cout << "***   option 1: Calculate LC                                                      ***" << std::endl;
	std::cout << "***   option 2: Verify LC                                                         ***" << std::endl;
	std::cout << "***   option 3: Quit                                                              ***" << std::endl;
	std::cout << "***   Enter option:1                                                              ***" << std::endl;
	std::cout << "***                                                                               ***" << std::endl;
	std::cout << "***   Example:Calculate LC                                                        ***" << std::endl;
	std::cout << "***   Enter number of process:3<enter>                                            ***" << std::endl;
	std::cout << "***   Enter number of message:4<enter>                                            ***" << std::endl;
	std::cout << "***   Number of process-N=3                                                       ***" << std::endl;
	std::cout << "***   Number of message-M=4                                                       ***" << std::endl;
	std::cout << "***   Enter Process Events For p0:a<space>s1<space>r3<space>b<enter>              ***" << std::endl;
	std::cout << "***   Enter Process Events For p1:c<space>r2<space>s3<space>NULL<enter>           ***" << std::endl;
	std::cout << "***   Enter Process Events For p2:r1<space>d<space>s2<space>e<enter>              ***" << std::endl;
	std::cout << "***                                                                               ***" << std::endl;
	std::cout << "***   Example:Verify    LC                                                        ***" << std::endl;
	std::cout << "***   Enter number of process:3<enter>                                            ***" << std::endl;
	std::cout << "***   Enter number of message:4<enter>                                            ***" << std::endl;
	std::cout << "***   Number of process-N=3                                                       ***" << std::endl;
	std::cout << "***   Number of message-M=4                                                       ***" << std::endl;
	std::cout << "***   Enter Events LC For p0:1<space>2<space>r3<space>4<enter>                    ***" << std::endl;
	std::cout << "***   Enter Events LC For p1:1<space>2<space>r3<space>4<enter>                    ***" << std::endl;
	std::cout << "***   Enter Events LC For p2:1<space>2<space>r3<space>4<enter>                    ***" << std::endl;
	std::cout << "***                                                                               ***" << std::endl;
	std::cout << "***                                                                               ***" << std::endl;
	std::cout << "***                                                                               ***" << std::endl;
	std::cout << std::endl << std::endl;
}
void print_option()
{
	std::cout << std::endl;
	std::cout << "option 1: Calculate LC" << std::endl;
	std::cout << "option 2: Verify LC" << std::endl;
	std::cout << "option 3: Quit" << std::endl;
}

//ask for user option
//ask for user size of processor and event
//ask for input from each processor event
//calculate LC value for each event
//or verify LC value.

int main()
{
	size_t num_process = 0;
	size_t num_message = 0;
	size_t option;
	std::string input;	
	
	std::vector<std::string> vectS;
	print_usage_message();
	while (true)
	{
		print_option();
		std::cout << "Enter option:";
		std::cin >> option;
		if (option == 3) 
		{
			break;
		}
		std::cout << "Enter number of process:";
		std::cin >> num_process;
		std::cout << "Enter number of message:";
		std::cin >> num_message;
		std::cout << "Number of process-N=" << num_process << std::endl;
		std::cout << "Number of message-M=" << num_message << std::endl;
		std::cin.ignore();
		Program program(num_process, num_message);
		if (option == 1)
		{
			for (size_t i = 0; i < program.m_processors.size(); i++)
			{
				std::cout << "Enter Process Events For p" << i << ":";
				std::getline(std::cin, input);
				parser(input, program.m_processors[i].m_events, num_message, std::string("LC"));
			}
			std::cout << program << std::endl;
			program.calculateLC();
			program.print_LC();
		}
		else if (option == 2)
		{
			for (size_t i = 0; i < program.m_processors.size(); i++)
			{
				std::cout << "Enter Events LC For p" << i << ":";
				std::getline(std::cin, input);
				parser(input, program.m_processors[i].m_events, num_message, std::string("VERIFY"));
			}
			if (program.verifyLC())
			{
				std::cout << program << std::endl;
			}
		}	
	}
	system("PAUSE");
	return 0;
}
