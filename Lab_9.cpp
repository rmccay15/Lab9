/*
 * lab9.cpp
 *
 *  Created on: Apr 19, 2017
 *      Author: rjmwc6
 */

/*
 * lab_9.cpp
 *
 *  Created on: Apr 19, 2017
 *      Author: rjmwc6
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

class message{

	public:

		vector <char> message_input;

		message();
		message(char* para_message);
		~message(){};
		void print_message();
};

message::message(char* para_message)
{
	unsigned i=0;
	while(i<strlen(para_message))
	{
		message_input.push_back(para_message[i]);
		i++;
	}

}

void message::print_message()
{
	unsigned i=0;
	while(i<message_input.size())
	{
		cout<<message_input[i];
		i++;
	}



}

message::message()
{
	char scan_message;
	cout<<"Enter Message Consisting of Uppercase/Lowercase Letters, Numbers, or Symbols Respresented in the Morse Code Alphabet, End With An Asterisk: "<<endl;
	while(cin>>scan_message && scan_message!='*')
	{
		message_input.push_back(scan_message);
	}

}


class morseCodeMessage:public message
{

	public:
		vector <string> morseMessage;
		void translate();
		void print_message();
		morseCodeMessage();
		morseCodeMessage(char* par_message);
		~morseCodeMessage(){};

};
morseCodeMessage::morseCodeMessage():message(){}
morseCodeMessage::morseCodeMessage(char* par_message):message(par_message){}

void morseCodeMessage::print_message()
{
	cout<<"\n*****************MESSAGE IN TEXT****************"<<endl;
	for(unsigned i=0;i<message_input.size();i++)
	{
		cout<<message_input[i];
	}
	cout<<endl;
	cout<<"\n*****************MESSAGE IN MORSE CODE*****************"<<endl;
	for(unsigned i=0;i<morseMessage.size();i++)
	{
		cout<<morseMessage[i]<<" ";
	}
}

void morseCodeMessage::translate()
{

	char text[68] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','1','2','3','4','5','6','7','8','9','0','.',',',' '};
	string morse[68] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.","--.-", ".-.", "...", "-", "..-", "...-", ".--","-..-", "-.--", "--..",".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",".----",
                    "..---","...--","....-",".....","-....","--...","---..","----.","-----",".-.-.-","--..--","/"};
	unsigned i=0;
	while(i<message_input.size())
	{

		for(int j=0;j<68;j++)
		{
			if(message_input[i] == text[j])
			{
				morseMessage.push_back(morse[j]);

			}
		}

		i++;
	}


}




class Stack{
	private:
		vector <morseCodeMessage> stack1;
	public:
		Stack(morseCodeMessage obj);
		int pop();
		int push();
		int print_stack(){

			int fd;		// for the file descriptor of the special file we need to open.
			unsigned long *BasePtr;		// base pointer, for the beginning of the memory page (mmap)
			unsigned long *PBDR, *PBDDR;	// pointers for port B DR/DDR

			fd = open("/dev/mem", O_RDWR|O_SYNC);	// open the special file /dem/mem
			if(fd == -1){
				printf("\n error\n");
			return(-1);  // failed open
			}

			// We need to map Address 0x80840000 (beginning of a memory page)
			// An entire memory page is mapped. We need to point to the beginning of the page.
			BasePtr = (unsigned long*)mmap(NULL,getpagesize(),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0x80840000);
			if(BasePtr == MAP_FAILED){
				printf("\n Unable to map memory space \n");
				return(-2);
			}  // failed mmap

			// To access other registers in the page, we need to offset the base pointer to reach the
			// corresponding addresses. Those can be found in the board's manual.
			PBDR = BasePtr + 1;		// Address of port B DR is 0x80840004
			PBDDR = BasePtr + 5;	// Address of port B DDR is 0x80840014

		// configures port B7 as output (Green LED)
			*PBDDR &= 0xFFFFFFFE;	// configures port B0 as input (first push button). You could use: &= ~0x01
			if(stack1.empty()){
				cout<<"Stack Is Too Empty To Print"<<endl;
			}

			for(unsigned j=0;j<stack1.size();j++){

				for(unsigned i=0;i<(stack1[j].morseMessage.size());i++){
					cout<<stack1[j].morseMessage[i]<<" ";
					for(unsigned k=0;k<stack1[j].morseMessage[i].size();k++){

						if(stack1[j].morseMessage[i][k]=='-'){
							*PBDDR |= 0x40;
							*PBDR |= 0x40;
							sleep(1);
							*PBDR &= ~0x40;
							sleep(1);
						}else if(stack1[j].morseMessage[i][k]=='.'){
							*PBDDR |= 0x20;
							*PBDR |= 0x20;
							sleep(1);
							*PBDR &= ~0x20;
							sleep(1);
						}else{
							*PBDDR |= 0x80;
							*PBDR |= 0x80;
							sleep(1);
							*PBDR &= ~0x80;

						}
					}

				}

				*PBDR |= 0x80;
				*PBDR |= 0x40;
				*PBDR |= 0x20;
				sleep(1);
				*PBDR &= ~0x80;
				*PBDR &= ~0x40;
				*PBDR &= ~0x20;
				sleep(1);
				cout<<endl;
				for(unsigned i=0;i<(stack1[j].morseMessage.size());i++){
					cout<<stack1[j].message_input[i];
				}

			}
			close(fd);
			return 0;
		}
};

Stack::Stack(morseCodeMessage obj)
{
	stack1.push_back(obj);
	int choice;
	cout<<endl;
	cout<<"\n==============MENU================\n1. Push()\n2. Pop()\n3. Print_Stack()\n0. Exit: "<<endl;
	cin>>choice;
	while(choice!=0)
	{
		switch(choice)
		{
			case 1:
				if(push()==0){

				}
				break;
			case 2:
				if(pop()==0){
					stack1.erase(stack1.end());
				}

				break;
			case 3:
				print_stack();
				break;
			default:
				break;
		}
		cout<<"\n==============MENU================\n1. Push()\n2. Pop()\n3. Print_Stack()\n0. Exit: "<<endl;
			cin>>choice;

	}

}
int Stack::push()
{
	morseCodeMessage s;
	s.translate();
	stack1.push_back(s);

	return 0;
}

int Stack::pop()
{
	if(stack1.empty()){
		cout<<"Stack is empty, cannot pop anymore!!!!"<<endl;
		return 1;
	}
	return 0;
}



int main(int argc,char* argv[])
{





	if(argc==1){
		message note;
		note.print_message();

	}else{
		char* help;
		help = argv[1];

		for(int i=2;i<argc;i++)
		{
			help = strcat(help," ");
			help = strcat(help,argv[i]);
		}
		help = strcat(help," ");
		morseCodeMessage mess1(help);

		mess1.translate();
		mess1.print_message();
		Stack s1(mess1);

	}
	return 0;
}


