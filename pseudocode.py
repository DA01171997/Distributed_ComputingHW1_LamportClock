#n = number of processor
#m = number of events

#event object that hold each event from user input
#example event = s3
#m_type = s
#m_order = 3
class Event:
    m_type
    m_order
    m_LC
    def isSend:
        return True of m_type is a Send else False
    def isReceive:
        return True of m_type is a Receive else False
    def isNull
        return True of m_type is a Null else False
	def isInternal:
		return True of m_type is a Internal else False

#processor object that hold multiple event
#example p0= s1, s2, d
#Processor.m_Vector_Events = [Event(s1),Event(s2),Event(d)]
class Processor:
	m_Vector_Events =[Event0,...,-> Event m-1]
	
#program object that contains multiple processor
#example p0 = s1, s2, d
#        p1 = d, r1, r2
#Program.m_VECTOR_Processors = [Processor(p0), Processor(p1)]
#Program.m_num_processes = n
#Program.m_num_message = m
class Program:
	m_VECTOR_Processors = [Processor0,..., -> Processor m-1]
	m_num_processes
	m_num_message
	def calculateLC:
		lc_calculator(m_VECTOR_Processors)
		lc_calculator.calculateLC()
		lc_calcula.printLC()

#LC_Calculator take the program.m_VECTOR_Processors
#calculate LC value for all events in each processor
#then call print to print all LC value
#LC_Calculator.m_VECTOR_Processors = Program.m_VECTOR_Processors
class LC_Calculator:
	m_VECTOR_Processors = Program.m_VECTOR_Processors

	#cursor used to cycle through the vector of processors
	m_processor_cursor = number of processors
	#cursor used to move through each event of a process
	m_VECTOR_event_cursor = [0 for all value in range(m)]
	
	#where key = send event = Event.m_type+Event.m_order and value = LC Event.m_LC of send event 
	#example { s3:2, s4:2....}
	m_MAP_send_event ={}
	
	#simple map used to keep track of number processor left to calculate
	m_MAP_processors_to_process = {0:0,.....-> x:x where x in range(n)}
	
	def printLC:
		print lc of all the events in all process in progam 
	
	#while there are will processors to calculate it events LC values
	#calculate current event if event met criteria else break out and move
	#to next processor and cycle back later.
	def calculateLC:
		while m_MAP_processors_to_process is not empty:
		
			# i is index of where the cursor of each processor stopped, i all start at 0 at first
			for i in range (m_VECTOR_event_cursor[m_processor_cursor], m):
				event = LC_Calculator.m_VECTOR_Processors[m_processor_cursor].m_Vector_Events[i]
				
				if event is first and event isInternal() or event isSend():
					if event isSend:
						key = "s" + event.m_order
						if key is not in m_MAP_send_event:
							insert key:event.m_LC=1 into m_MAP_send_event
						sendcounter++
					event.m_LC = 1
					
				elif event is first and event isReceive():
					key = "s" + event.m_order
					if key is in m_MAP_send_event:
						event.m_LC= key.value +1
					else:
						keep timeout counter for receive++
						break out of for loop and move on to next process
						
				elif event is not first and event isInternal or event isSend:
					k_value = LC of the event before current event
					key = "s" + event.m_order
					if event isSend:
						if key is not in m_MAP_send_event:
							insert key:event.m_LC=k_value+1 into m_MAP_send_event
						else:
							update key.value =k_value+1 
						sendcounter++
					event.m_LC = k_value + 1
					
				elif even is not first and even isReceive:
					key = "s" + event.m_order
					if key is in m_MAP_send_event:
						k_value = k_value = LC of the event before current event
						k_value = k_value if k_value > key.value else k_value = key.value
						event.m_LC = k_value + 1
					else:
						keep timeout counter for receive++
						break out of for loop and move on to next process
				#
				#Update m_VECTOR_event_cursor[m_processor_cursor] to i, where event stopped for that processor
				m_VECTOR_event_cursor[m_processor_cursor]=i
			
			# if event at processor m_processor_cursor is at the last event
			# mean that that process is done calculating
			# remove the process from the m_MAP_processors_to_process
			if m_VECTOR_event_cursor[m_processor_cursor] is m-1:
				key =  m_processor_cursor
				if key is in m_MAP_processors_to_process:
					m_MAP_processors_to_process remove key
			
			#cycle the cursor so that it cycle through the vector of processors
			m_processor_cursor =  (m_processor_cursor+1) % n
			
		#couldn't find the matching receive for send
		if timeout counter for receive > timeout value:
			return false
		#too many send
		if sendcounter is > receive:
			return false
		return true
			
class LC_Verifier:
	location object
	receive = {}
	def verify():
		#first preprocess by looping through and determine which event is a receive
		#save those receive event, and saved those receive event with the increase order
		#of the k value to the dictionary
		for events in program:
			receive[k-value] = preprocess(event)
			
		#loop through the event again to find event where LC value is equal to the k-value
		#update that event type to receive and remove the k value from the dictionary
		for k-value in receive:
			for event in program:
				if events.m_LC = k-value
					update event.m_type = send
					remove k-value from receive
		
		#if all the receive is found then receive dictionary is empty
		#else meaning it failed to find corresponding send event 
		#mean the sequence of input is invalid.
		if receive.empty:
			return True
		return False
					
			
		
#get user option
def main:
	n = number of processor = input("how many processor?")
	m = number of events = input("how many event is each processor?")
	Program progam(n,m)
	for processor in range(n):
		events = input("what is sequence of event per processor")
		program.m_VECTOR_Processors.m_Vector_Events= events
	
	while(True):
		print("option1 = calculate")
		print("option2 = verify")
		option = input("which option?")
		if option =="1":
			progam.calculateLC()
		elif option =="2":
			program.verify()
		else:
			break
	
	
 
			
				
						
						
				
				
							
	